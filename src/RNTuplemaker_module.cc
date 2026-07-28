#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"

#include "TFile.h"
#include "canvas/Utilities/InputTag.h"

// ROOT & RNTuple headers
#include "TError.h"
#include "ROOT/RNTuple.hxx"
#include "ROOT/RNTupleModel.hxx"
#include "ROOT/RNTupleWriter.hxx"

// Mu2e Data Product Headers
#include "Offline/MCDataProducts/inc/SimParticle.hh"

namespace mu2e {

class RNTuplemaker : public art::EDAnalyzer {
public:
  struct Config {
    using Name = fhicl::Name;
    using Comment = fhicl::Comment;

    fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("Diagnostic level"), 0};
    fhicl::Atom<std::string> outputFileName{
    Name("outputFileName"), 
    Comment("Dedicated output root file for RNTuple"), 
    "nts.owner.rntuple.version.root"};
    fhicl::Atom<art::InputTag> simsTag{
        Name("simsTag"), 
        Comment("InputTag for SimParticleCollection"), 
        art::InputTag("compressRecoMCs")};
  };

  typedef art::EDAnalyzer::Table<Config> Parameters;

  explicit RNTuplemaker(const Parameters& conf);
  virtual ~RNTuplemaker() {}

  void beginJob() override;
  void analyze(const art::Event& e) override;
  void endJob() override;

private:
  Config _conf;
  art::InputTag _simsTag;
  std::string _outputFileName;
  std::unique_ptr<ROOT::Experimental::RNTupleWriter> writer_;

  // Scalar Fields bound to RNTuple Model (assumes single entry per event)
  std::shared_ptr<int>   fldHasElec_;
  std::shared_ptr<int>   fldElecPdg_;
  std::shared_ptr<float> fldElecMom_;
  std::shared_ptr<float> fldElecTime_;
};

RNTuplemaker::RNTuplemaker(const Parameters& conf)
    : art::EDAnalyzer(conf),
      _conf(conf()),
      _simsTag(conf().simsTag()),
      _outputFileName(conf().outputFileName()) {
  gErrorIgnoreLevel = kError;
  SetErrorHandler(DefaultErrorHandler);
}

void RNTuplemaker::beginJob() {
  auto model = ROOT::Experimental::RNTupleModel::Create();
   //art::ServiceHandle<art::TFileService> tfs;

  fldHasElec_  = model->MakeField<int>("hasElec");
  fldElecPdg_   = model->MakeField<int>("elec_pdg");
  fldElecMom_   = model->MakeField<float>("elec_mom");
  fldElecTime_  = model->MakeField<float>("elec_time");

  //std::string filename = tfs->file().GetName();

  //tfs->file().Close();

  writer_ = ROOT::Experimental::RNTupleWriter::Recreate(
      std::move(model), 
      "myNtuple", 
      _outputFileName
  );
}

void RNTuplemaker::analyze(const art::Event& e) {

  *fldHasElec_  = 0;
  *fldElecPdg_   = 0;
  *fldElecMom_   = 0.0f;
  *fldElecTime_  = 0.0f;

  auto simsHandle = e.getValidHandle<SimParticleCollection>(_simsTag);

  const mu2e::SimParticle* signalElectron = nullptr;

  for (const auto& kv : *simsHandle) {
    const auto& particle = kv.second;

    if (particle.pdgId() == 11 && particle.isPrimary()) {
      signalElectron = &particle;
      break;
    }
  }

  // Populate scalar fields if electron is present (FIXME this is still a dummy)
  if (signalElectron != nullptr) {
    *fldHasElec_  = 1;
    *fldElecPdg_   = signalElectron->pdgId();
    *fldElecMom_   = static_cast<float>(signalElectron->startMomentum().vect().mag());
    *fldElecTime_  = static_cast<float>(signalElectron->startGlobalTime());
  }

  // Write ONE row per event
  writer_->Fill();
}

void RNTuplemaker::endJob() {
  writer_.reset();
}

} // namespace mu2e

DEFINE_ART_MODULE(mu2e::RNTuplemaker)