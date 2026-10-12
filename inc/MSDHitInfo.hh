//
// MSDHitInfo: mobile synchronization detector (MSD) hit information
//
#ifndef MSDHitInfo_HH
#define MSDHitInfo_HH

namespace mu2e
{
  struct MSDHitInfo {
    int    channel = -1; // MSD channel ID
    float  time    = -1; // hit time (-1 if not in the payload)
    float  tot     = -1; // time-over-threshold (-1 if not in the payload)
    void reset() { *this = MSDHitInfo(); }
  };
}
#endif
