////////////////////////////////////////////////////////////////////////
// Class:       BadChannelFinder
// Plugin Type: analyzer (Unknown Unknown)
// File:        BadChannelFinder_module.cc
//
// Generated at Tue Dec 13 13:54:39 2022 by Teresa Lackey using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////

#include "TFile.h"
#include "TTree.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "ChannelMap/service/ChannelMapService.h"
#include "RawData/SSDRawDigit.h"

namespace emph {
  namespace dq {
    class BadChannelFinder;
  }
}

using namespace emph;

class emph::dq::BadChannelFinder : public art::EDAnalyzer {
public:
  explicit BadChannelFinder(fhicl::ParameterSet const& pset);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  BadChannelFinder(BadChannelFinder const&) = delete;
  BadChannelFinder(BadChannelFinder&&) = delete;
  BadChannelFinder& operator=(BadChannelFinder const&) = delete;
  BadChannelFinder& operator=(BadChannelFinder&&) = delete;

  void beginJob();
  void endSubRun(art::SubRun const& sr);

  // Required functions.
  void analyze(art::Event const& evt) override;

private:
  static const unsigned int nFER = 4;
  static const unsigned int nMod = 6;
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

  TTree *bcstats;
  std::string fSSDRawLabel; ///< Data label for SSD Raw Digits
  unsigned int run, subrun;
  unsigned int fer, mod, row;
  float nhit;
  unsigned int ntriggers;   ///< Number of triggers with SSD hits, to scale plots
  float Nhit[nFER][nMod][640]={{{0}}};
};


//--------------------------------------------------
emph::dq::BadChannelFinder::BadChannelFinder(fhicl::ParameterSet const& pset)
  : EDAnalyzer{pset},
  fSSDRawLabel (pset.get< std::string >("SSDRawLabel"))
  // More initializers here.
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
}

//--------------------------------------------------
void emph::dq::BadChannelFinder::beginJob()
{
  art::ServiceHandle<art::TFileService> tfs;
  bcstats = tfs->make<TTree>("bcstats","");
  bcstats->Branch("run",&run,"run/I");
  bcstats->Branch("subrun",&subrun,"subrun/I");
  bcstats->Branch("fer",&fer,"fer/I");
  bcstats->Branch("mod",&mod,"mod/I");
  bcstats->Branch("row",&row,"row/I");
  bcstats->Branch("nhit",&nhit,"nhit/F");

  ntriggers=0;
}

//--------------------------------------------------
void emph::dq::BadChannelFinder::endSubRun(const art::SubRun& sr)
{
  run = sr.run();
  subrun = sr.subRun();
  if (ntriggers>0){
    for(fer=0; fer<nFER; ++fer){
      for( mod=0; mod<nMod; ++mod){
	emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
	if (!cmap->IsValidEChan(echan))
	    continue;
	for( row=0; row<640; ++row){
	  nhit=Nhit[fer][mod][row]/ntriggers;
	  bcstats->Fill();
	  //std::cout<<fer<<":"<<mod<<":"<<row<<":"<<hitVec[3]<<std::endl;
	  Nhit[fer][mod][row]=0;
	}
      }
    }  
  }
}

//--------------------------------------------------
void emph::dq::BadChannelFinder::analyze(art::Event const& evt)
{
  run = evt.run();
  subrun = evt.subRun();

  auto ssdHandle = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fSSDRawLabel);
  if (ssdHandle) {
    // only count triggers with SSD Hits
    ntriggers++;
    for (size_t idx=0; idx<ssdHandle->size(); ++idx){
      art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssdHandle,idx);
      Nhit[ssdDig->FER()][ssdDig->Module()][ssdDig->Row()]++;
    }
  }
  
}

DEFINE_ART_MODULE(emph::dq::BadChannelFinder)
