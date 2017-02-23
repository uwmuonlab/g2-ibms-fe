/*****************************************************************************\

Name:   sim_ibms.cxx
Author: Matthias W. Smith
Email:  mwsmith2@uw.edu

About:  Implement a simple MIDAS frontend that simulates reading out 3
        caen 1742 and aggregates the data for the IBMS system.

\*****************************************************************************/

//--- std includes ----------------------------------------------------------//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <ctime>
#include <random>
using std::string;

//--- other includes --------------------------------------------------------//
#include "midas.h"
#include "TFile.h"
#include "TTree.h"

//--- project includes ------------------------------------------------------//
#include "frontend_utils.hh"
#include "g2/ibms_structs.hh"
#include "g2field/caen1742.hh"
#include "g2field/wfd_container.hh"

//--- globals ---------------------------------------------------------------//
#define FRONTEND_NAME "IBMS Detector"

extern "C" {

  // The frontend name (client name) as seen by other MIDAS clients
  char *frontend_name = (char *)FRONTEND_NAME;

  // The frontend file name, don't change it.
  char *frontend_file_name = (char *)__FILE__;

  // frontend_loop is called periodically if this variable is TRUE
  BOOL frontend_call_loop = FALSE;

  // A frontend status page is displayed with this frequency in ms.
  INT display_period = 1000;

  // maximum event size produced by this frontend
  INT max_event_size = 0x800000; // 8 MB

  // maximum event size for fragmented events (EQ_FRAGMENTED)
  INT max_event_size_frag = 0x100000;

  // buffer size to hold events
  INT event_buffer_size = 0x1000000; // 64 MB

  // Function declarations
  INT frontend_init();
  INT frontend_exit();
  INT begin_of_run(INT run_number, char *error);
  INT end_of_run(INT run_number, char *error);
  INT pause_run(INT run_number, char *error);
  INT resume_run(INT run_number, char *error);

  INT frontend_loop();
  INT read_ibms_event(char *pevent, INT off);
  INT poll_event(INT source, INT count, BOOL test);
  INT interrupt_configure(INT cmd, INT source, POINTER_T adr);

  // Equipment list

  EQUIPMENT equipment[] =
    {
      {FRONTEND_NAME,  // equipment name
       {
	 EVENTID_IBMS, // event ID
	 0x1,          // trigger mask
         "SYSTEM",     // event buffer (use to be SYSTEM)
         EQ_POLLED,    // equipment type
         0,            // not used
         "MIDAS",      // format
         TRUE,         // enabled
         RO_RUNNING,   // read only when running
         10,           // poll for 10ms
         0,            // stop run after this event limit
         0,            // number of sub events
         0,            // don't log history
         "", "", "",
       },
       read_ibms_event,      // readout routine
      },

      {""}
    };

} //extern C

RUNINFO runinfo;

// Anonymous namespace for my "globals"
namespace {

int trigger_count = 0;
int event_number = 0;
bool write_root = false;
bool write_midas = true;

TFile *pf;
TTree *pt;

std::map<int, std::array<int, 32>> channel_map;
std::atomic<bool> run_in_progress;
g2::ibms_t data;

const double kSuperCyclePeriod = 83.33;
std::default_random_engine gen;
std::normal_distribution<double> norm_dist(0.0, 5.0);

const char *const mbank_name = (char *)"IBMS";
}

int load_devices(boost::property_tree::ptree pt)
{
  return SUCCESS;
}

//--- Frontend Init ----------------------------------------------------------//
INT frontend_init()
{
  boost::property_tree::ptree conf;
  INT rc = load_settings(frontend_name, conf);

  if (rc != SUCCESS) {
    return rc;
  }

  rc = load_devices(conf);

  if (rc != SUCCESS) {
    return rc;
  }

  int dev_idx = 0;

  // Load the channel maps
  for (auto &dev: conf.get_child("devices")) {

    int i = 0;

    std::array<int, 32> channels;
    auto pt = conf.get_child("channel_maps");
    
    for (auto &v : pt.get_child(dev.first)) {
      channels[i++] = v.second.get_value<int>();
    }
   
    channel_map[dev_idx] = channels;
 
    dev_idx++;
  }
      
  run_in_progress = false;

  cm_msg(MINFO, "init", FRONTEND_NAME" initialization complete");

  return SUCCESS;
}

//--- Frontend Exit ---------------------------------------------------------//
INT frontend_exit()
{
  run_in_progress = false;

  cm_msg(MINFO, "exit", FRONTEND_NAME" teardown complete");

  return SUCCESS;
}

//--- Begin of Run --------------------------------------------------//
INT begin_of_run(INT run_number, char *error)
{
  boost::property_tree::ptree conf;
  INT rc = load_settings(frontend_name, conf);

  if (rc != SUCCESS) {
    return rc;
  }

  rc = load_devices(conf);

  if (rc != SUCCESS) {
    return rc;
  }

   // ODB parameters
  HNDLE hDB, hkey;
  char str[256];
  int size;
  BOOL flag;

  // Set up the data.
  std::string datadir;
  std::string filename;

  // Grab the database handle.
  cm_get_experiment_database(&hDB, NULL);

  // Get the run info out of the ODB.
  db_find_key(hDB, 0, "/Runinfo", &hkey);
  if (db_open_record(hDB, hkey, &runinfo, sizeof(runinfo), MODE_READ,
		     NULL, NULL) != DB_SUCCESS) {
    cm_msg(MERROR, "begin_of_run", "Can't open \"/Runinfo\" in ODB");
    return CM_DB_ERROR;
  }

  // Get the data directory from the ODB.
  snprintf(str, sizeof(str), "/Logger/Data dir");
  db_find_key(hDB, 0, str, &hkey);

  if (hkey) {
    size = sizeof(str);
    db_get_data(hDB, hkey, str, &size, TID_STRING);
    datadir = std::string(str);
  }

  // Set the filename
  snprintf(str, sizeof(str), "root/sim_ibms_run_%05d.root", runinfo.run_number);

  // Join the directory and filename using boost filesystem.
  filename = (boost::filesystem::path(datadir) / boost::filesystem::path(str)).string();

  write_root = conf.get<bool>("write_root");

  // Set up the ROOT data output.
  if (write_root) {

    pf = new TFile(filename.c_str(), "recreate");
    pt = new TTree("t_ibms", "IBMS Detector Data");

    pt->SetAutoSave(5);
    pt->SetAutoFlush(20);

    std::string br_name("ibms");

    pt->Branch(br_name.c_str(), &data.sys_clock[0], g2::ibms_str);
  }

  // Run variables.
  event_number = 0;
  run_in_progress = true;

  cm_msg(MLOG, "begin_of_run", "Completed successfully");

  return SUCCESS;
}

//--- End of Run ----------------------------------------------------*/
INT end_of_run(INT run_number, char *error)
{
  // Make sure we write the ROOT data.
  if (run_in_progress && write_root) {

    pt->Write();
    pf->Write();
    pf->Close();

    delete pf;
  }

  run_in_progress = false;

  cm_msg(MLOG, "end_of_run", "Completed successfully");

  return SUCCESS;
}

//--- Pause Run -----------------------------------------------------*/
INT pause_run(INT run_number, char *error)
{
  return SUCCESS;
}

//--- Resuem Run ----------------------------------------------------*/
INT resume_run(INT run_number, char *error)
{
  return SUCCESS;
}

//--- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
  // If frontend_call_loop is true, this routine gets called when
  // the frontend is idle or once between every event
  return SUCCESS;
}

//-------------------------------------------------------------------*/

/********************************************************************\

  Readout routines for different events

\********************************************************************/

//--- Trigger event routines ----------------------------------------*/

INT poll_event(INT source, INT count, BOOL test) 
{
  // fake calibration
  if (test) {
    for (int i = 0; i < count; i++) {
      usleep(10);
    }
    return 0;
  }

  static auto t0 = hw::steadyclock_us();
  auto t1 = hw::steadyclock_us();

  // Only read events at the expected fill rate.
  if (t1 - t0 > kSuperCyclePeriod) {
    
    t0 = t1;
    return 1;

  } else {

    return 0;
  }
}

//--- Interrupt configuration ---------------------------------------*/

INT interrupt_configure(INT cmd, INT source, PTYPE adr)
{
  switch (cmd) {
  case CMD_INTERRUPT_ENABLE:
    break;
  case CMD_INTERRUPT_DISABLE:
    break;
  case CMD_INTERRUPT_ATTACH:
    break;
  case CMD_INTERRUPT_DETACH:
    break;
  }
  return SUCCESS;
}

//--- Event readout -------------------------------------------------*/

INT read_ibms_event(char *pevent, INT off)
{
  static bool triggered = false;
  static unsigned long long num_events;
  static unsigned long long events_written;

  // Variable which contains all wfd data.
  hw::event_data_t wfd_data;
  const int caen_ch = 32;

  int count = 0;
  char bk_name[10];
  DWORD *pdata;

  cm_msg(MDEBUG, "read_ibms_event", "data readout started");

  for (int dev_idx = 0; dev_idx < 3; ++dev_idx) {

    // Track wfd index.
    int j = 0;

    // Match up the device index with the mapped data index.
    for (int &ch : channel_map[dev_idx]) {

      std::cout << ch << ", " << j << std::endl;

      // Skip unused channels
      if (ch < 0) {
	continue; 
      }

      // Copy wfd data to ibms data.
      data.sys_clock[ch] = hw::steadyclock_us();
      data.dev_clock[ch] = hw::steadyclock_us();

      for (int i = 0; i < g2::kIbmsTraceLength; ++i) {
	data.trace[j][i] = 1000 * dev_idx + ch * 10 + norm_dist(gen);
      }
    }
    
    j++;
    dev_idx++;
  }

  if (write_root && run_in_progress) {

    cm_msg(MINFO, "read_ibms_event", "Filling TTree");

    pt->Fill();
    num_events++;

    if (num_events % 20 == 1) {

      cm_msg(MINFO, frontend_name, "flushing TTree.");
      pt->AutoSave("SaveSelf,FlushBaskets");
      pf->Flush();
    }
  }

  // And MIDAS output.
  bk_init32(pevent);

  if (write_midas) {

    cm_msg(MINFO, "read_ibms_event", "Shipping MIDAS event");

    // Copy the shimming trolley data.
    bk_create(pevent, mbank_name, TID_DWORD, &pdata);

    memcpy(pdata, &data, sizeof(data));
    pdata += sizeof(data) / sizeof(DWORD);

    bk_close(pevent, pdata);
  }

  cm_msg(MINFO, "read_ibms_event", "Finished reading event");

  return bk_size(pevent);
}
