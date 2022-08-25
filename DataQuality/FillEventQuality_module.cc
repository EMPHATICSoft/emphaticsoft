////////////////////////////////////////////////////////////////////////
// Class:       FillEventQuality
// Plugin Type: producer (Unknown Unknown)
// File:        FillEventQuality_module.cc
//
// Generated at Thu Aug 18 15:15:12 2022 by Teresa Lackey using cetskelgen
// from cetlib version v3_13_03.
////////////////////////////////////////////////////////////////////////

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C++ includes
#include <memory>

// emphaticsoft includes
#include "DataQuality/EventQuality.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"

using namespace emph;

namespace emph {
  namespace dq {

    class FillEventQuality : public art::EDProducer {
    public:
      explicit FillEventQuality(fhicl::ParameterSet const& pset);
      // The compiler-generated destructor is fine for non-base
      // classes without bare pointers or other resource use.

      // Plugins should not be copied or assigned.
      FillEventQuality(FillEventQuality const&) = delete;
      FillEventQuality(FillEventQuality&&) = delete;
      FillEventQuality& operator=(FillEventQuality const&) = delete;
      FillEventQuality& operator=(FillEventQuality&&) = delete;

      // Required functions.
      void produce(art::Event& evt) override;

    private:

      // Declare member data here.
      std::string fTriggerLabel;
      std::string fSSDDataLabel;
    };

    //.......................................................................

    emph::dq::FillEventQuality::FillEventQuality(fhicl::ParameterSet const& pset)
      : EDProducer(pset)  // ,
    // More initializers here.
    {
      fTriggerLabel = pset.get< std::string >("TriggerLabel");
      fSSDDataLabel = pset.get< std::string >("SSDDataLabel");
      // Call appropriate produces<>() functions here.
      // Call appropriate consumes<>() for any products to be retrieved by this module.
      produces< emph::dq::EventQuality >();
    }

    //.......................................................................

    void emph::dq::FillEventQuality::produce(art::Event& evt)
    {
      // Implementation of required member function here.
      std::unique_ptr<dq::EventQuality> eventqual(new dq::EventQuality);

      // get trigger waveforms
      art::Handle< std::vector<emph::rawdata::WaveForm> > trigHandle;
      try {
	evt.getByLabel(fTriggerLabel, trigHandle);
	if (!trigHandle->empty()){
	}
      }
      catch(...){
      }
      
      
      // look for SSD hits
      art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
      try {
	evt.getByLabel(fSSDDataLabel, ssdHandle);
	if (!ssdHandle->empty()) {
	  eventqual->hasSSDHits = true;
	}
      }
      catch(...) {
	eventqual->hasSSDHits = false;
      }
      

      // Place EventQuality object into event
      evt.put(std::move(eventqual));
    }

  }//end namespace dq
}//end namespace emph

DEFINE_ART_MODULE(emph::dq::FillEventQuality)
