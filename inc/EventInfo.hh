//
// EventInfo: event-level information
// Dave Brown (LBNL)
//
#ifndef EventInfo_HH
#define EventInfo_HH
#include <string>
#include "Rtypes.h"
namespace mu2e
{
  struct EventInfo {
    int event = 0; // event number
    int run = 0; // run number
    int subrun = 0; // subrun number
    int nprotons = 0; // estimated # of protons on target for this microbunch
    float pbtime = 0.0; // estimated proton bunch time
    float  pbterr = 0.0; // estimated proton bunch time uncertainty
    ULong64_t ewt = 0; // event window tag from the EventHeader (0 if no EventHeader is found)
    void reset() {*this = EventInfo(); }
  };
}
#endif
