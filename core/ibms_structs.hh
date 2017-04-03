#ifndef IBMS_FE_CORE_IBMS_STRUCTS_HH_
#define IBMS_FE_CORE_IBMS_STRUCTS_HH_

/*===========================================================================*\

author: Matthias W. Smith
email:  mwsmith2@uw.edu
file:   ibms_constants.hh

about:  A header file for structs used across IBMS software.

\*===========================================================================*/

//--- external includes  ----------------------------------------------------//
#include "TROOT.h"

namespace g2 {

// IMBS literals
#define EVENTID_IBMS 0X1
#define IBMS_NUM_CHANNELS 80
#define IBMS_TRACE_LENGTH 1024
#define IBMS_NUM_TRIGGERS 12
#define IBMS_LVDS_BITS 16

// IBMS
constexpr int kIbmsNumChannels = IBMS_NUM_CHANNELS;
constexpr int kIbmsNumTriggers = IBMS_NUM_TRIGGERS;
constexpr int kIbmsTraceLength = IBMS_TRACE_LENGTH;

// IBMS  data struct
struct ibms_t {
  ULong64_t sys_clock[IBMS_NUM_CHANNELS];
  ULong64_t dev_clock[IBMS_NUM_CHANNELS];
  UShort_t trace[IBMS_NUM_CHANNELS][IBMS_TRACE_LENGTH];
  UShort_t trigger[IBMS_NUM_TRIGGERS][IBMS_TRACE_LENGTH];
  UShort_t lvds_bits[3];
};

#define MAKE_IBMS_STRING(name, ch, len, trg) IMBS_HELPER(name, ch, len, trg)
#define IMBS_HELPER(name, ch, len, trg) \
const char * const name = \
"sys_clock["#ch"]/l:dev_clock["#ch"]/l:trace["#ch"]["#len"]/s:" \
"trigger["#trg"]["#len"]/s:lvds_bits[3]/s";

MAKE_IBMS_STRING(ibms_str, IBMS_NUM_CHANNELS, IBMS_TRACE_LENGTH, IBMS_NUM_TRIGGERS);

} // ::g2

#endif
