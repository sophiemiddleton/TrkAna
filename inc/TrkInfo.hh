//
// structs used to record per-track information in TTrees.
// All momenta are in units of MeV/c, time in nsec WRT when the proton bunch pulse peak hits the production target,
// positions are in mm WRT the center of the tracker.
// Dave Brown (LBNL)
//
#ifndef TrkInfo_HH
#define TrkInfo_HH
#include "Offline/DataProducts/inc/XYZVec.hh"
#include "TrkAna/inc/helixpar.hh"
#include "Offline/MCDataProducts/inc/MCRelationship.hh"
#include "Rtypes.h"
namespace mu2e
{
// information about the track fit at a particular place
  struct TrkFitInfo {
    Float_t _fitmom,_fitmomerr;
    helixpar _fitpar;
    helixpar _fitparerr;
    TrkFitInfo() { reset(); }
    void reset() { _fitmom=_fitmomerr=-1000.0; _fitpar.reset(); _fitparerr.reset(); }
    static std::string leafnames() { static std::string leaves; leaves =
      std::string("mom/F:momerr/F:")+helixpar::leafnames()+std::string(":d0err/F:p0err/F:omerr/F:z0err/F:tderr/F");
      return leaves;
    }
  };

// general information about a track
  struct TrkInfo {
    Int_t _status; // Kalman fit status
    Int_t _alg; // pat. rec. algorithm
    Int_t _pdg;   // PDG code of particle assumed in fit
    Int_t _nhits;     // # hits associated to this track
    Int_t _ndof;      // number of degrees of freedom in the fit
    Int_t _nactive;   // number of active hits (actually used in the fit)
    Int_t _ndouble,_ndactive; // number of double-hit panels, and active double-hit panels
    Int_t _nnullambig;  // number of hits without any ambiguity assigned
    Int_t _nmat, _nmatactive; // number materials (straw) assigned and used (active) to this fit
    Int_t _nseg;     // number of trajectory segments 
    Float_t _t0;      // time this particle was estimated to cross z=0
    Float_t _t0err;   // error on t0
    Float_t _chisq;   // Kalman fit chisquared
    Float_t _fitcon;  // Kalman fit chisqured consistency
    Float_t _radlen;  // total radiation length of (active) material crossed by this particle inside the tracker
    Float_t _firstflt, _lastflt;  // distance along the helix of the first and last hit
    Float_t _startvalid, _endvalid; // distance along the helix for which the fit is accurate
    Float_t _trkqual;  // track quality MVA output
    TrkInfo() { reset(); }
    void reset() { 
      _status = -1000;
      _alg=0;
      _pdg = 0;
      _nhits = _nactive = _ndouble = _ndactive = _nnullambig = _nmat = _nmatactive = _nseg = _ndof = -1;
      _t0 = _t0err = _chisq = _fitcon = _radlen = _firstflt = _lastflt = -1.0;
      _trkqual=-1000.0;
      _startvalid = _endvalid = -999999.0;
    }
    static std::string const& leafnames() { static const std::string leaves =
    std::string("status/I:alg/I:pdg/I:nhits/I:ndof/I:nactive/I:ndouble/I:ndactive/I:nnullambig/I:nmat/I:nmatactive/I:nseg/I:t0/F:t0err/F:chisq/F:fitcon/F:radlen/F:firstflt/F:lastflt/F:startvalid/F:endvalid/F");
     return leaves;
    }
  };

// general NC info about the particle which generated the majority of the hits used in this track
  struct TrkInfoMC {
    Int_t _ndigi; // number of true straw digitizations (both ends) generatedy by the primary particle
    Int_t  _ndigigood; // number of true straw digitizations where the primary particle (not a delta-ray) crossed threshold
    Int_t _nhits, _nactive; // number of hits on the track and active on the track generated by the primary particle
    Int_t _nambig; // number of true hits where the reconstruction assigned the correct left-right ambiguity
    Int_t _pdg, _gen, _proc; // true PDG code, generator code, and process code of the primary particle
    Float_t _otime;  // origin time
    XYZVec _opos;  // origin position
    Float_t _omom;   // origin momentum (scalar)
    Float_t _ocosth; // origin cos(theta), where theta is the angle between the particle's momentum and z-axis
    Float_t _ophi; // origin phi (azimuthal angle of particle's momentum vector)
    MCRelationship _prel; // relationship if this tracks primary particle to the event primary
 
    TrkInfoMC() { reset(); }
    void reset() { _ndigi = _ndigigood = _nactive = _nhits = _nambig = _pdg = _gen  = _proc= -1;  _otime=0.0;
      _prel = MCRelationship();
      _opos = XYZVec();
      _omom = -1, _ocosth = -1, _ophi=-1;
    }
    static std::string leafnames() { static std::string leaves; leaves =
      std::string("ndigi/I:ndigigood/I:nhits/I:nactive/I:nambig/I:pdg/I:gen/I:proc/I:otime/F:")
    + Geom::XYZnames("opos") + std::string(":omom/F:ocosth/F:ophi/F:prel/B:prem/B");

      return leaves;
    }
  };
//  MC information about a particle for a specific point/time
  struct TrkInfoMCStep {
    Float_t _time;  // time of this step
    Float_t _mom; // scalar momentum of particle at the start of this step
    Float_t _costh; // cos(theta) of momentum vector of particles at the start of this step (theta is angle between momentum vector and z-axis)
    Float_t _phi; // azimuthal angle of momentum vector
    XYZVec _pos;  // particle position at the start of this step
    helixpar _hpar; // helix parameters corresponding to the particle position and momentum assuming the nominal BField
    TrkInfoMCStep() { reset(); }
    void reset() { _time = -1; _mom = -1; _costh = -1; _phi = -1; _pos = XYZVec(); _hpar.reset(); }
    static std::string leafnames() { static std::string leaves; leaves =
      std::string("t0/F:mom/F:costh/F:phi/F:") + Geom::XYZnames("pos") + std::string(":")+helixpar::leafnames();
      return leaves;
    }
  };

  //MC information for DIOBrem study
  struct MCPart {
    Int_t _pdg;
    Float_t _time;
    Float_t _e;
    Float_t _posx;
    Float_t _posy;
    Float_t _posz;
    Float_t _momx;
    Float_t _momy;
    Float_t _momz;
    MCPart() { reset(); }
    void reset() { _pdg = _time = _e = _posx = _posy = _posz = _momx = _momy = _momz = -1; }
    static std::string leafnames() { static std::string leaves; leaves =
      std::string("pdg/I:t/F:e/F:posx/F:posy/F:posz/F:momx/F:momy/F:momz/F");
      return leaves;
    }
  };
}
#endif
