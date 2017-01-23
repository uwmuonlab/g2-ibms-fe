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

// IBMS
constexpr int kIbmsNumChannels = IBMS_NUM_CHANNELS;
constexpr int kIbmsTraceLength = IBMS_TRACE_LENGTH;

// IBMS  data struct
struct ibms_t {
  ULong64_t sys_clock[IBMS_NUM_CHANNELS];
  ULong64_t dev_clock[IBMS_NUM_CHANNELS];
  UShort_t trace[IBMS_NUM_CHANNELS][IBMS_TRACE_LENGTH];
};

#define MAKE_IBMS_STRING(name, ch, len) IMBS_HELPER(name, ch, len)
#define IMBS_HELPER(name, ch, len) \
const char * const name = \
"sys_clock["#ch"]/l:dev_clock["#ch"]/l:trace["#ch"]["#len"]/s"

MAKE_IBMS_STRING(ibms_str, IBMS_NUM_CHANNELS, IBMS_TRACE_LENGTH);

} // ::g2

#endif
