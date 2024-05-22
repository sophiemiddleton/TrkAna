//
// Namespace for collecting tools used in TrkDiag tree filling
// Original author: A. Edmonds (November 2018)
//
#include "TrkAna/inc/InfoStructHelper.hh"
#include "Offline/RecoDataProducts/inc/TrkStrawHitSeed.hh"
#include "KinKal/Trajectory/CentralHelix.hh"
#include "Offline/Mu2eKinKal/inc/WireHitState.hh"
#include <cmath>
#include <limits>

namespace mu2e {
  void InfoStructHelper::fillHitCount(StrawHitFlagCollection const& shfC, HitCount& hitcount) {
    hitcount.nsd = shfC.size();
    for(const auto& shf : shfC) {
      if(shf.hasAnyProperty(StrawHitFlag::energysel))++hitcount.nesel;
      if(shf.hasAnyProperty(StrawHitFlag::radsel))++hitcount.nrsel;
      if(shf.hasAnyProperty(StrawHitFlag::timesel))++hitcount.ntsel;
      if(shf.hasAnyProperty(StrawHitFlag::bkg))++hitcount.nbkg;
    }
  }

  void InfoStructHelper::fillHitCount(RecoCount const& nrec, HitCount& hitcount) {
    hitcount.nesel = nrec._nshfesel;
    hitcount.nrsel = nrec._nshfrsel;
    hitcount.ntsel = nrec._nshftsel;
    hitcount.nbkg = nrec._nshfbkg;
  }

  void InfoStructHelper::fillTrkInfo(const KalSeed& kseed,std::vector<TrkInfo>& trkinfos) {
    TrkInfo trkinfo;

    if(kseed.status().hasAnyProperty(TrkFitFlag::kalmanConverged))
      trkinfo.status = 1;
    else if(kseed.status().hasAnyProperty(TrkFitFlag::kalmanOK))
      trkinfo.status = 2;
    else
      trkinfo.status = -1;

    if(kseed.status().hasAnyProperty(TrkFitFlag::FitOK)){
      trkinfo.goodfit = 1;
    } else
      trkinfo.goodfit = 0;

    if(kseed.status().hasAnyProperty(TrkFitFlag::MPRHelix))
      trkinfo.seedalg = 3;
    else if(kseed.status().hasAnyProperty(TrkFitFlag::APRHelix))
      trkinfo.seedalg = 2;
    else if(kseed.status().hasAnyProperty(TrkFitFlag::CPRHelix))
      trkinfo.seedalg = 1;
    else if(kseed.status().hasAnyProperty(TrkFitFlag::TPRHelix))
      trkinfo.seedalg = 0;

    if(kseed.status().hasAnyProperty(TrkFitFlag::KKLoopHelix)){
      trkinfo.fitalg =1;
    } else if(kseed.status().hasAnyProperty(TrkFitFlag::KKCentralHelix))
      trkinfo.fitalg = 2;
    else if(kseed.status().hasAnyProperty(TrkFitFlag::KKLine))
      trkinfo.fitalg = 3;
    else
      trkinfo.fitalg = 0;

    trkinfo.pdg = kseed.particle();

    fillTrkInfoHits(kseed, trkinfo);

    trkinfo.chisq = kseed.chisquared();
    trkinfo.ndof  = kseed.nDOF();
    trkinfo.fitcon = kseed.fitConsistency();
    trkinfo.nseg = kseed.nTrajSegments();
    trkinfo.maxgap = kseed._maxgap;
    trkinfo.avggap = kseed._avggap;

    trkinfo.firsthit = kseed.hits().back()._ptoca;
    trkinfo.lasthit = kseed.hits().front()._ptoca;
    for(auto const& hit : kseed.hits()) {
      if(hit.flag().hasAnyProperty(StrawHitFlag::active)){
        if( trkinfo.firsthit > hit._ptoca)trkinfo.firsthit = hit._ptoca;
        if( trkinfo.lasthit < hit._ptoca)trkinfo.lasthit = hit._ptoca;
      }
    }

    fillTrkInfoStraws(kseed, trkinfo);

    trkinfos.push_back(trkinfo);
  }

  void InfoStructHelper::fillTrkFitInfo(const KalSeed& kseed, std::vector<std::vector<TrkFitInfo>>& all_tfis) {
    std::vector<TrkFitInfo> tfis;
    double tmin(std::numeric_limits<float>::max());
    double tmax(std::numeric_limits<float>::lowest());
    size_t imin(0), imax(0);
    for(size_t ikinter = 0; ikinter < kseed.intersections().size(); ++ikinter){
      auto const& kinter = kseed.intersections()[ikinter];
      // record earliest and latest intersections
      if(kinter.time() < tmin){
        tmin = kinter.time();
        imin = ikinter;
      }
      if(kinter.time() > tmax){
        tmax = kinter.time();
        imax = ikinter;
      }
      TrkFitInfo tfi;
      tfi.mom = kinter.momentum3();
      tfi.pos = kinter.position3();
      tfi.time = kinter.time();
      tfi.momerr = kinter.momerr();
      tfi.inbounds = kinter.inBounds();
      tfi.gap = kinter.gap();
      tfi.sid = kinter.surfid_.id().id();
      tfi.sindex = kinter.surfid_.index();
      tfis.push_back(tfi);
    }
    // now flag early and latest intersections
    if(tfis.size() > 0){
      tfis[imin].early = true;
      tfis[imax].late = true;
    }
    all_tfis.push_back(tfis);
  }

  void InfoStructHelper::fillLoopHelixInfo(const KalSeed& kseed, std::vector<std::vector<LoopHelixInfo>>& all_lhis) {
    std::vector<LoopHelixInfo> lhis;
    for(auto const& kinter : kseed.intersections()) {
      auto lh = kinter.loopHelix();
      LoopHelixInfo lhi;
      lhi.rad = lh.rad();
      lhi.lam = lh.lam();
      lhi.cx = lh.cx();
      lhi.cy = lh.cy();
      lhi.phi0 = lh.phi0();
      lhi.t0 = lh.t0();
      lhi.raderr = sqrt(lh.paramVar(KinKal::LoopHelix::rad_));
      lhi.lamerr = sqrt(lh.paramVar(KinKal::LoopHelix::lam_));
      lhi.cxerr = sqrt(lh.paramVar(KinKal::LoopHelix::cx_));
      lhi.cyerr = sqrt(lh.paramVar(KinKal::LoopHelix::cy_));
      lhi.phi0err = sqrt(lh.paramVar(KinKal::LoopHelix::phi0_));
      lhi.t0err = sqrt(lh.paramVar(KinKal::LoopHelix::t0_));
      // deprecated!
      lhi.maxr =sqrt(lh.cx()*lh.cx()+lh.cy()*lh.cy())+fabs(lh.rad());
      lhis.push_back(lhi);
    }
    all_lhis.push_back(lhis);
  }
  void InfoStructHelper::fillCentralHelixInfo(const KalSeed& kseed, std::vector<std::vector<CentralHelixInfo>>& all_chis) {
    std::vector<CentralHelixInfo> chis;
    for(auto const& kinter : kseed.intersections()) {
      auto ch = kinter.centralHelix();
      CentralHelixInfo chi;
      chi.d0 = ch.d0();
      chi.phi0 = ch.phi0();
      chi.omega = ch.omega();
      chi.z0 = ch.z0();
      chi.tanDip = ch.tanDip();
      chi.t0 = ch.t0();
      chi.d0err = sqrt(ch.paramVar(KinKal::CentralHelix::d0_));
      chi.phi0err = sqrt(ch.paramVar(KinKal::CentralHelix::phi0_));
      chi.omegaerr = sqrt(ch.paramVar(KinKal::CentralHelix::omega_));
      chi.z0err = sqrt(ch.paramVar(KinKal::CentralHelix::z0_));
      chi.tanDiperr = sqrt(ch.paramVar(KinKal::CentralHelix::tanDip_));
      chi.t0err = sqrt(ch.paramVar(KinKal::CentralHelix::t0_));
      // deprecated!
      chi.maxr = fabs(-1.0/ch.omega() - ch.d0());
      chis.push_back(chi);
    }
    all_chis.push_back(chis);
  }
  void InfoStructHelper::fillKinematicLineInfo(const KalSeed& kseed, std::vector<std::vector<KinematicLineInfo>>& all_klis) {
    std::vector<KinematicLineInfo> klis;
    for(auto const& kinter : kseed.intersections()) {
      auto kl = kinter.kinematicLine();
      KinematicLineInfo kli;
      kli.d0 = kl.d0();
      kli.phi0 = kl.phi0();
      kli.z0 = kl.z0();
      kli.theta = kl.theta();
      kli.mom = kl.mom();
      kli.t0 = kl.t0();
      kli.d0err = sqrt(kl.paramVar(KinKal::KinematicLine::d0_));
      kli.phi0err = sqrt(kl.paramVar(KinKal::KinematicLine::phi0_));
      kli.z0err = sqrt(kl.paramVar(KinKal::KinematicLine::z0_));
      kli.thetaerr = sqrt(kl.paramVar(KinKal::KinematicLine::theta_));
      kli.momerr = sqrt(kl.paramVar(KinKal::KinematicLine::mom_));
      kli.t0err = sqrt(kl.paramVar(KinKal::KinematicLine::t0_));
      klis.push_back(kli);
    }
    all_klis.push_back(klis);
 }

  void InfoStructHelper::fillTrkInfoHits(const KalSeed& kseed, TrkInfo& trkinfo) {
    static StrawHitFlag active(StrawHitFlag::active);
    std::set<unsigned> planes;
    uint16_t minplane(0), maxplane(0);
    static StrawHitFlag allsel("EnergySelection:TimeSelection:RadiusSelection");
    static StrawHitFlag allrej("Background:Dead:Noisy");

    for (auto ihit = kseed.hits().begin(); ihit != kseed.hits().end(); ++ihit) {
      ++trkinfo.nhits;
      if (ihit->strawHitState() > WireHitState::inactive){
        ++trkinfo.nactive;
        planes.insert(ihit->strawId().plane());
        minplane = std::min(minplane, ihit->strawId().plane());
        maxplane = std::max(maxplane, ihit->strawId().plane());
        if (ihit->strawHitState()==WireHitState::null) {
          ++trkinfo.nnullambig;
        }
        trkinfo.avgedep += ihit->_edep;
        // count active hits by flag state
        if(ihit->flag().hasAllProperties(StrawHitFlag::energysel))trkinfo.nesel++;
        if(ihit->flag().hasAllProperties(StrawHitFlag::radsel))trkinfo.nrsel++;
        if(ihit->flag().hasAllProperties(StrawHitFlag::timesel))trkinfo.ntsel++;
        if(ihit->flag().hasAllProperties(StrawHitFlag::bkg))trkinfo.nbkg++;
        if(ihit->flag().hasAllProperties(allsel) && (!ihit->flag().hasAnyProperty(allrej)))trkinfo.nsel++;
        // these variables are obsolete, use stereo hits instead TODO
        auto jhit = ihit; jhit++;
        if(jhit != kseed.hits().end() && ihit->strawId().uniquePanel() == jhit->strawId().uniquePanel()){
          ++trkinfo.ndouble;
          if(jhit->strawHitState()>WireHitState::inactive) { ++trkinfo.ndactive; }
        }
      }
      trkinfo.nplanes = planes.size();
      trkinfo.planespan = abs(maxplane-minplane);
    }
    trkinfo.avgedep /= trkinfo.nactive;
  }

  void InfoStructHelper::fillTrkInfoStraws(const KalSeed& kseed, TrkInfo& trkinfo) {
    trkinfo.nmat = 0; trkinfo.nmatactive = 0; trkinfo.radlen = 0.0;
    for (std::vector<TrkStraw>::const_iterator i_straw = kseed.straws().begin(); i_straw != kseed.straws().end(); ++i_straw) {
      ++trkinfo.nmat;
      if (i_straw->active()) {
        ++trkinfo.nmatactive;
        trkinfo.radlen += i_straw->radLen();
      }
    }
  }

  void InfoStructHelper::fillHitInfo(const KalSeed& kseed, std::vector<std::vector<TrkStrawHitInfo>>& all_tshinfos ) {
    std::vector<TrkStrawHitInfo> tshinfos;
    // loop over hits
    static StrawHitFlag active(StrawHitFlag::active);
    for(std::vector<TrkStrawHitSeed>::const_iterator ihit=kseed.hits().begin(); ihit != kseed.hits().end(); ++ihit) {
      TrkStrawHitInfo tshinfo;

      tshinfo.state = ihit->_ambig;
      tshinfo.usetot = ihit->_kkshflag.hasAnyProperty(KKSHFlag::tot);
      tshinfo.usedriftdt = ihit->_kkshflag.hasAnyProperty(KKSHFlag::driftdt);
      tshinfo.useabsdt = ihit->_kkshflag.hasAnyProperty(KKSHFlag::absdrift);
      tshinfo.usendvar = ihit->_kkshflag.hasAnyProperty(KKSHFlag::nhdrift);
      tshinfo.algo = ihit->_algo;
      tshinfo.frozen = ihit->_frozen;
      tshinfo.bkgqual = ihit->_bkgqual;
      tshinfo.signqual = ihit->_signqual;
      tshinfo.driftqual = ihit->_driftqual;
      tshinfo.chi2qual = ihit->_chi2qual;
      tshinfo.earlyend   = ihit->_eend.end();
      tshinfo.plane = ihit->strawId().plane();
      tshinfo.panel = ihit->strawId().panel();
      tshinfo.layer = ihit->strawId().layer();
      tshinfo.straw = ihit->strawId().straw();

      tshinfo.edep   = ihit->_edep;
      tshinfo.etime   = ihit->_etime;
      tshinfo.wdist   = ihit->_wdist;
      tshinfo.werr   = ihit->_werr;
      tshinfo.tottdrift = ihit->_tottdrift;
      tshinfo.tot = ihit->_tot;
      tshinfo.ptoca   = ihit->_ptoca;
      tshinfo.stoca   = ihit->_stoca;
      tshinfo.rdoca   = ihit->_rdoca;
      tshinfo.rdocavar   = ihit->_rdocavar;
      tshinfo.rdt   = ihit->_rdt;
      tshinfo.rtocavar   = ihit->_rtocavar;
      tshinfo.udoca   = ihit->_udoca;
      tshinfo.udocavar   = ihit->_udocavar;
      tshinfo.udt   = ihit->_udt;
      tshinfo.utocavar   = ihit->_utocavar;
      tshinfo.rupos   = ihit->_rupos;
      tshinfo.uupos   = ihit->_uupos;
      tshinfo.rdrift   = ihit->_rdrift;
      tshinfo.cdrift   = ihit->_cdrift;
      tshinfo.sderr   = ihit->_sderr;
      tshinfo.uderr   = ihit->_uderr;
      tshinfo.dvel   = ihit->_dvel;
      tshinfo.lang   = ihit->_lang;
      tshinfo.utresid   = ihit->_utresid;
      tshinfo.utresidmvar   = ihit->_utresidmvar;
      tshinfo.utresidpvar   = ihit->_utresidpvar;
      tshinfo.udresid   = ihit->_udresid;
      tshinfo.udresidmvar   = ihit->_udresidmvar;
      tshinfo.udresidpvar   = ihit->_udresidpvar;
      tshinfo.rtresid   = ihit->_rtresid;
      tshinfo.rtresidmvar   = ihit->_rtresidmvar;
      tshinfo.rtresidpvar   = ihit->_rtresidpvar;
      tshinfo.rdresid   = ihit->_rdresid;
      tshinfo.rdresidmvar   = ihit->_rdresidmvar;
      tshinfo.rdresidpvar   = ihit->_rdresidpvar;

      tshinfo.wdot = ihit->_wdot;
      tshinfo.poca = ihit->_upoca;
      tshinfo.ustrawdist = ihit->_ustrawdist;
      tshinfo.ustrawphi = ihit->_ustrawphi;
      tshinfo.uwirephi = ihit->_uwirephi;

      // count correlations with other TSH
      // OBSOLETE: replace this with a test for KinKal StrawHitClusters
      for(std::vector<TrkStrawHitSeed>::const_iterator jhit=kseed.hits().begin(); jhit != kseed.hits().end(); ++jhit) {
        if(jhit != ihit && ihit->strawId().plane() ==  jhit->strawId().plane() &&
            ihit->strawId().panel() == jhit->strawId().panel() ){
          tshinfo.dhit = true;
          if (jhit->flag().hasAllProperties(active)) {
            tshinfo.dactive = true;
            break;
          }
        }
      }
      tshinfos.push_back(tshinfo);
    }
    all_tshinfos.push_back(tshinfos);
  }

  void InfoStructHelper::fillMatInfo(const KalSeed& kseed, std::vector<std::vector<TrkStrawMatInfo>>& all_tminfos ) {
    std::vector<TrkStrawMatInfo> tminfos;
    // loop over sites, pick out the materials

    for(const auto& i_straw : kseed.straws()) {
      TrkStrawMatInfo tminfo;

      tminfo.plane = i_straw.straw().getPlane();
      tminfo.panel = i_straw.straw().getPanel();
      tminfo.layer = i_straw.straw().getLayer();
      tminfo.straw = i_straw.straw().getStraw();

      tminfo.active = i_straw.active();
      tminfo.dp = i_straw.pfrac();
      tminfo.radlen = i_straw.radLen();
      tminfo.doca = i_straw.doca();
      tminfo.tlen = i_straw.trkLen();

      tminfos.push_back(tminfo);
    }
    all_tminfos.push_back(tminfos);
  }

  void InfoStructHelper::fillCaloHitInfo(const KalSeed& kseed, std::vector<TrkCaloHitInfo>& all_tchinfos) {
    TrkCaloHitInfo tchinfo;
    if (kseed.hasCaloCluster()) {
      auto const& tch = kseed.caloHit();
      auto const& cc = tch.caloCluster();
      tchinfo.active = tch._flag.hasAllProperties(StrawHitFlag::active);
      tchinfo.did = cc->diskID();
      tchinfo.poca = tch._cpos;
      tchinfo.mom = tch._tmom;
      tchinfo.cdepth = tch._cdepth;
      tchinfo.trkdepth = tch._trkdepth;
      tchinfo.doca = tch._udoca;
      tchinfo.dt = tch._udt;
      tchinfo.ptoca = tch._uptoca;
      tchinfo.tocavar = tch._utocavar;
      tchinfo.tresid   = tch._tresid;
      tchinfo.tresidmvar   = tch._tresidmvar;
      tchinfo.tresidpvar   = tch._tresidpvar;
      tchinfo.ctime = cc->time();
      tchinfo.ctimeerr = cc->timeErr();
      tchinfo.csize = cc->size();
      tchinfo.edep = cc->energyDep();
      tchinfo.edeperr = cc->energyDepErr();
      // compute relative azimuth dot product
      auto rmomhat = XYZVectorF(tch._tmom.X(),tch._tmom.Y(),0.0).Unit();
      auto rhohat = XYZVectorF(tch._cpos.X(),tch._cpos.Y(),0.0).Unit();
      tchinfo.dphidot = rmomhat.Dot(rhohat);
    }
    all_tchinfos.push_back(tchinfo);
  }


  void InfoStructHelper::fillHelixInfo(art::Ptr<HelixSeed> const& hptr, HelixInfo& hinfo) {
    if(hptr.isNonnull()){
      // count hits, active and not
      for(size_t ihit=0;ihit < hptr->hits().size(); ihit++){
        auto const& hh = hptr->hits()[ihit];
        hinfo.nch++;
        hinfo.nsh += hh.nStrawHits();
        if(!hh.flag().hasAnyProperty(StrawHitFlag::outlier)){
          hinfo.ncha++;
          hinfo.nsha += hh.nStrawHits();
        }

        if(hptr->status().hasAnyProperty(TrkFitFlag::MPRHelix))
          hinfo.flag = 3;
        else if(hptr->status().hasAnyProperty(TrkFitFlag::APRHelix))
          hinfo.flag = 2;
        else if(hptr->status().hasAnyProperty(TrkFitFlag::CPRHelix))
          hinfo.flag = 1;
        else if(hptr->status().hasAnyProperty(TrkFitFlag::TPRHelix))
          hinfo.flag = 0;

        hinfo.t0err = hptr->t0().t0Err();
        hinfo.mom = 0.299792*hptr->helix().momentum()*_bz0; //FIXME!
        hinfo.chi2xy = hptr->helix().chi2dXY();
        hinfo.chi2fz = hptr->helix().chi2dZPhi();
        if(hptr->caloCluster().isNonnull())
          hinfo.ecalo  = hptr->caloCluster()->energyDep();
      }
    }
  }

  // this function won't work with KinKal fits and needs to be rewritten from scratch //FIXME
  void InfoStructHelper::fillTrkPIDInfo(const TrkCaloHitPID& tchp, const KalSeed& kseed, TrkPIDInfo& trkpidInfo) {
    mu2e::GeomHandle<mu2e::Calorimeter> calo;
    int n_trktchpid_vars = TrkCaloHitPID::n_vars;
    for (int i_trktchpid_var = 0; i_trktchpid_var < n_trktchpid_vars; ++i_trktchpid_var) {
      TrkCaloHitPID::MVA_varindex i_index = TrkCaloHitPID::MVA_varindex(i_trktchpid_var);
      trkpidInfo._tchpvars[i_trktchpid_var] = (float) tchp[i_index];
    }
    trkpidInfo._mvaout = tchp.MVAOutput();
    trkpidInfo._mvastat = tchp.status();
    // extrapolate the track to the calorimeter disk faces and record the transverse radius
    // Use the last segment
    auto const& trkhel = kseed.segments().back().helix();
    static const CLHEP::Hep3Vector origin;
    for(int idisk=0;idisk < 2; idisk++){
      auto ffpos = calo->geomUtil().mu2eToTracker(calo->geomUtil().diskFFToMu2e(idisk,origin));
      float flen = trkhel.zFlight(ffpos.z());
      float blen = trkhel.zFlight(ffpos.z()+207.5); // private communication B. Echenard FIXME!!!!
      XYZVectorF extpos;
      trkhel.position(flen,extpos);
      trkpidInfo._diskfrad[idisk] = sqrt(extpos.Perp2());
      trkhel.position(blen,extpos);
      trkpidInfo._diskbrad[idisk] = sqrt(extpos.Perp2());
    }
  }

}
