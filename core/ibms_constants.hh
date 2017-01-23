#ifndef IBMS_DAQ_CORE_FIELD_CONSTANTS_HH_
#define IBMS_DAQ_CORE_FIELD_CONSTANTS_HH_

/*===========================================================================*\

author: Matthias W. Smith
email:  mwsmith2@uw.edu
file:   ibms_constants.hh

about:  A header file for constant parameters used in IBMS system.

\*===========================================================================*/

// Midas EventID for each equipment
#define EVENTID_IBMS 0X1

// IMBS literals
#define IBMS_NUM_CHANNELS 80
#define IBMS_TRACE_LENGTH 1024

namespace g2 {

// IBMS
constexpr int kIbmsNumChannels = IBMS_NUM_CHANNELS;
constexpr int kIbmsTraceLength = IBMS_TRACE_LENGTH;

} // ::g2

#endif
