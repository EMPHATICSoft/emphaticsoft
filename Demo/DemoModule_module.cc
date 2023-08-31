////////////////////////////////////////////////////////////////////////
/// \brief   The simplest job module possible to illustrate the essential
///          features of a job module
/// \author  $Author: gsdavies $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// ROOT includes
#include "TH1F.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "RawData/RawDigit.h"
#include "RecoBase/SSDHit.h"

///package to illustrate how to write modules
namespace demo {
  class DemoModule : public art::EDProducer {
    public:
      explicit DemoModule(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor      
      ~DemoModule();                        
      
      // Optional, read/write access to event
      void produce(art::Event& evt); 

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      
    private:
      ///< Sample configurable module parameters
      int              fInt;    
      std::vector<int> fVecInt;
      float            fFloat;  
      double           fDouble; 
      std::string      fInputModuleLabel; ///> label for module that made the data products you need for this module 
      TH1F*            fHisto;            ///> a histogram 

    };
}

////////////////////////////////////////////////////////////////////////
// note it is convention to NOT declare ANY "using namespace xxx" here
// especially "using namespace std"
namespace demo
{
  //.......................................................................
  DemoModule::DemoModule(fhicl::ParameterSet const& pset) 
    : EDProducer(pset),
      fInt              (pset.get< int              >("MyInt")),
      fVecInt           (pset.get< std::vector<int> >("MyVectorInt")),
      fFloat            (pset.get< float            >("MyFloat")),
      fDouble           (pset.get< double           >("MyDouble")),
      fInputModuleLabel (pset.get< std::string      >("MyInputModuleLabel"))

  {
    //======================================================================
    // This is the constructor "nova" will use to create your module.
    //
    // Modules use configurations which have the same name as the
    // module. The variable "version" sets which version of this
    // configuration to use. Users are required to supply a configuration
    // called "default" but other versions ("test" eg.) are allowed.
    //
    // Which version to use is set at run time by the job description XML
    // file.
    //======================================================================

    // tell the module what it is making as this is a EDProducer
    produces< std::vector<int> >();

    // Be noisy to demonstrate what's happening
    mf::LogInfo("DemoModule") << " DemoModule::DemoModule()\n";
  }

  //......................................................................
  DemoModule::~DemoModule()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................
  void DemoModule::beginJob()
  {
    //
    // Book histograms, ntuples, initialize counts etc., etc., ...
    //
    art::ServiceHandle<art::TFileService> tfs;

    fHisto = tfs->make<TH1F>("fHisto",
			     "Histo Title;x-axis title (units);y-axis title (units)",
			     100,0.,1.);
    
  }

  //......................................................................
  void DemoModule::produce(art::Event& evt)
  {
    //======================================================================
    // Called for every event. 
    //======================================================================

    mf::LogInfo("DemoModule") << "DemoModule::Reco()"
			      << "\n"
			      << "New Reco Event "; 

    //  get rawdata::RawDigit from the event
    auto const& digit = evt.getProduct<std::vector<rawdata::RawDigit>>(fInputModuleLabel);

    // make the collection of ints we want to store
    auto ssdhits = std::make_unique<std::vector<rb::SSDHit>>();

    // fill a histogram with the size of each MCTruth
    for(unsigned int i = 0; i < digit.size(); ++i){
      rb::SSDHit hit;
      hit.SetPitch(60.);
      hit.SetAngle(0.);
      hit.SetStrip(double(digit[i].Channel())/4096.);
      ssdhits->push_back( hit );
      fHisto->Fill( hit.Strip() );
    }
    
    // put the collection of SSD hits into the event
    evt.put(std::move(ssdhits));

    // maybe I really wanted to still keep that collection around for later 
    // use in a const fashion. Then I could have done
    // art::OrphanHandle< std::vector<rb::SSDHit> > ssdhitsoh = evt.put(ssdhits);      
    
    return;
  }

} // end namespace demo
////////////////////////////////////////////////////////////////////////
namespace demo
{
  DEFINE_ART_MODULE(DemoModule)
}
