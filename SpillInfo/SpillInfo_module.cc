////////////////////////////////////////////////////////////////////////
// Class:       SpillInfo
// Plugin Type: producer (Unknown Unknown)
// File:        SpillInfo_module.cc
//
// Generated at Thu Mar 24 15:29:24 2022 by Jonathan Paley using cetskelgen
// from cetlib version v3_13_03.
////////////////////////////////////////////////////////////////////////

//#include "SpillInfo.h"
#include "art/Framework/Core/ModuleMacros.h"

////////////////////////////////////////////////////////////////////////
/// \brief   This module reads IFDB and then stores MCenter spill info
/// \author lackey32@fnal.gov
////////////////////////////////////////////////////////////////////////

//C++ includes 
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <time.h> 
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RecoBase/Spill.h"

//IFDB includes
#include "ifbeam.h" 
//#include "Munits.h"
#include "IFBeam_service.h"

/// Information about the IFDB
namespace emph
{
  class SpillInfo : public art::EDProducer 
  {
  public:
    explicit SpillInfo(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor      
    virtual ~SpillInfo();                        
    
    void produce(art::Event& evt);
    void beginSubRun(art::SubRun &sr);
    void endSubRun(art::SubRun &sr);

  private:
    bool fNeedNewSpillInfo;

    std::unique_ptr<rb::Spill> fSpill;
    double fOffset;

    art::ServiceHandle<ifbeam_ns::IFBeam> ifbeam_handle;
    std::unique_ptr<BeamFolder> bfp;

  };
}


////////////////////////////////////////////////////////////////////////
namespace emph
{
  //..............................................................
  SpillInfo::SpillInfo(fhicl::ParameterSet const& pset) :
    EDProducer(pset),
    fOffset         (pset.get< double      >("Offset")         ),
    bfp( ifbeam_handle->getBeamFolder(pset.get< std::string >("Bundle"), pset.get< std::string >("URL"), pset.get< double >("TimeWindow")))
  {
    fNeedNewSpillInfo = true;

    // how close in time does the spill time have to be from the DAQ tims [seconds]
    // FTBF has one 4.2 second spill every minute.
    bfp->set_epsilon(5); 
    // tell the module what it is making as this is a EDProducer
    produces< rb::Spill, art::InSubRun >();
    // Be noisy to demonstrate what's happening
    
    mf::LogInfo("SpillInfo") << "In SpillInfo Constructor\n";
  
  }
  
  //......................................................................
  SpillInfo::~SpillInfo()
  {
  }
  
  //......................................................................
  void SpillInfo::beginSubRun(art::SubRun& )//sr)
  {
    fNeedNewSpillInfo = true;

  }

  //......................................................................
  void SpillInfo::endSubRun(art::SubRun& sr)
  {
    sr.put(std::move(fSpill));
    fNeedNewSpillInfo = true;
  }

  //......................................................................
  void SpillInfo::produce(art::Event& evt)
  {
    
    if(!evt.isRealData()){
      // don't have any beam sim to put in here      
      return;
    }
    if (fNeedNewSpillInfo) {
      fSpill.reset(new rb::Spill());
      fSpill->SetTimestamp(evt.time().timeHigh());

      // Query the database to get post-spill sample device info. 
      // We are querying $36, so an offset is added to the event time 
      // to match the db time. Epsilon for the beam folder is large enough
      // to encompass full spill
      try {
	double spillTime = (double)evt.time().timeHigh();

	float intensity;
	float momentum;

	bfp->GetNamedData(spillTime+fOffset,"F:MT6SC1,S:MTNRG", &intensity,&momentum);
	fSpill->SetIntensity(intensity);
	fSpill->SetMomentum(momentum);
	fNeedNewSpillInfo = false;
      }
      catch (const WebAPIException &we) {
	std::string err(we.what());
	if (!err.find("variable not found")) {
	  mf::LogError("SpillInfo") << "Exception encountered: " 
				    << we.what();
	  std::abort();
	}
      }      
    }
    
  }
  
} // end namespace emph
////////////////////////////////////////////////////////////////////////
DEFINE_ART_MODULE(emph::SpillInfo)
