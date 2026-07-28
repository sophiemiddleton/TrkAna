#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"

#include "TFile.h"
#include "ROOT/RNTuple.hxx"
#include "ROOT/RNTupleWriter.hxx"
#include "ROOT/RField.hxx"
#include "ROOT/RNTupleModel.hxx"

#include "TError.h" // Needed for gErrorIgnoreLevel and kError
namespace mu2e {
class RNTuplemaker : public art::EDAnalyzer {
public:
  struct Config {
        using Name=fhicl::Name;
        using Comment=fhicl::Comment;
        fhicl::Atom<int> diagLevel{Name("diagLevel"), Comment("diagLevel")};
        };

  typedef art::EDAnalyzer::Table<Config> Parameters;

      explicit RNTuplemaker(const Parameters& conf);
      virtual ~RNTuplemaker() { }

      void beginJob() override;
      //void beginSubRun(const art::SubRun & subrun ) override;
      void analyze(const art::Event& e) override;
      void endJob() override;
      

private:
  Config _conf;
  int _diagLevel;
  std::unique_ptr<ROOT::Experimental::RNTupleWriter> writer_;
  std::shared_ptr<float> fldEnergy_;
};

RNTuplemaker::RNTuplemaker(const Parameters& conf):
    art::EDAnalyzer(conf),
    _conf(conf()),
    _diagLevel(conf().diagLevel())
{
  gErrorIgnoreLevel = kError;
  SetErrorHandler(DefaultErrorHandler);
}

/*void RNTuplemaker::beginJob() {
  auto model = ROOT::Experimental::RNTupleModel::Create();
  fldEnergy_ = model->MakeField<float>("energy");
  
  writer_ = ROOT::Experimental::RNTupleWriter::Recreate(
      std::move(model), 
      "myNtuple",                        // Ntuple name
      "nts.owner.description.version.root" // Output file name
  );
}*/


void RNTuplemaker::beginJob() {
  art::ServiceHandle<art::TFileService> tfs;

  auto model = ROOT::Experimental::RNTupleModel::Create();
  fldEnergy_ = model->MakeField<float>("energy");

  std::string filename = tfs->file().GetName();

  tfs->file().Close();

  writer_ = ROOT::Experimental::RNTupleWriter::Recreate(
      std::move(model), 
      "myNtuple", 
      filename
  );
}

void RNTuplemaker::analyze(const art::Event& e){
  // Fill the energy field with dummy data
  *fldEnergy_ = 105.0f;
  
  // Write the entry to the ntuple
  writer_->Fill();
}

void RNTuplemaker::endJob() {
  // Explicitly destroy the writer to flush buffers & write the RNTuple footer
  writer_.reset(); 
}
}
DEFINE_ART_MODULE(mu2e::RNTuplemaker)