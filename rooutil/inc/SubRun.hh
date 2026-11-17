#ifndef SubRun_hh_
#define SubRun_hh_

#include "EventNtuple/inc/SubRunInfo.hh"
#include "EventNtuple/inc/MuCaptureInfo.hh"
#include "EventNtuple/rooutil/inc/BranchUtils.hh"

namespace rooutil {
  struct SubRun {
    explicit SubRun(TChain* ntuple) {
      has_srinfo = CheckForBranch(ntuple, "srinfo", &srinfo);
      has_gen_event_count = CheckForBranch(ntuple, "genEventCount", &genEventCount);
      has_proc_event_count = CheckForBranch(ntuple, "procEventCount", &procEventCount);
      has_cosmic_livetime = CheckForBranch(ntuple, "cosmicLivetime", &cosmicLivetime);
      has_mu_capture = CheckForBranch(ntuple, "muCapture", &muCapture);
    }

    mu2e::SubRunInfo* srinfo = nullptr;
    long genEventCount = 0;
    long procEventCount = 0;
    float cosmicLivetime = 0.0;
    mu2e::MuCaptureInfo* muCapture = nullptr;

    bool has_srinfo = false;
    bool has_gen_event_count = false;
    bool has_proc_event_count = false;
    bool has_cosmic_livetime = false;
    bool has_mu_capture = false;
  };
} // namespace rooutil
#endif
