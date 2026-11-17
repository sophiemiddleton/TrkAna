//
// MuCaptureInfo: muon capture normalisation for one subrun, copied from the
// Offline mu2e::MuonCaptureNormalization SubRun product.
//
// nCap is what the CE analysis normalises to (R_mue = N_signal / (N_cap * eps_CE)).
// Sum it over the subruns an analysis selects; RooUtil::GetMuCaptures() returns the
// total over all subruns in the input files.
//
//   source 1 (MC truth): expected counts, sum over microbunches of true PBI x stops/POT x f_cap.
//                        nStop and pot are filled; nCapErrStat/nCapErrSyst are 0.
//   source 2-4 (STM, CAPHRI, combined): measured N_cap from the DB. There is no POT
//                        measurement in data, so nStop and pot are -1.
// Systematics common to all subruns are not stored per subrun.
// valid is false (and nCap -1) if the subrun has no MuonCaptureNormalization product.
//
#ifndef MuCaptureInfo_HH
#define MuCaptureInfo_HH

namespace mu2e
{
  struct MuCaptureInfo {
    bool   valid = false;       // product found in this subrun
    int    source = 0;          // MuonCaptureNormalization::Source: 0 unknown, 1 MCTruth, 2 STM, 3 CAPHRI, 4 Combined
    double nCap = -1.;          // muon captures in the stopping target
    double nCapErrStat = 0.;    // statistical uncertainty (independent between subruns)
    double nCapErrSyst = 0.;    // systematic uncertainty uncorrelated between subruns
    double nStop = -1.;         // stopped muons (MC truth only; -1 otherwise)
    double pot = -1.;           // protons on target (MC truth only; -1 otherwise)
    unsigned long long nMicrobunches = 0; // microbunches the counts cover
    void reset() { *this = MuCaptureInfo(); }
  };
}
#endif
