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
/// \brief   This module reads IFDB and then stores MTest spill info.
///          Code based on NOvA Testbeam
/// \author jpaley@fnal.gov, lackey32@fnal.gov
////////////////////////////////////////////////////////////////////////

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
//#include "ifbeam.h" 
//#include "Munits.h"
#include "ifdh_art/IFBeamService/IFBeam_service.h"


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

    // FTBF has one 4.2 second spill every minute.  Get the information from the nearest entry within a minute.
    bfp->set_epsilon(60); 
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
  void SpillInfo::beginSubRun(art::SubRun& sr)
  {
    std::cout << "%%%%% Subrun time: " << sr.beginTime().timeHigh() << std::endl;
    fSpill.reset(new rb::Spill());
    fSpill->SetTimestamp(sr.beginTime().timeHigh());
    
    // Query the database to get post-spill sample device info. 
    // We are querying $36, so an offset is added to the event time 
    // to match the db time. Epsilon for the beam folder is large enough
    // to encompass full spill
    try {
      double spillTime = (double)sr.beginTime().timeHigh();
      double recordTime = spillTime + fOffset;
      
      double intensity;
      //      double momentum;
      double mt5cpr;
      double mt6cpr;
      double mt5cp2;
      double mt6cp2;
      double tbccdi;
      double tbccdo;
      double mt6ca1;
      double mt6ca2;
      double mt6ca3;
      
      // JMP: the beam momentum recording appears to be broken.  Not sure
      // right now what to do about this.  Skipping momentum info for now.
      //      bfp->GetNamedData(recordTime,"S:MTNRG",&momentum);


      bfp->GetNamedData(recordTime,"F:MT6SC1",&intensity);
      bfp->GetNamedData(recordTime,"F:MT5CPR,F:MT6CPR,F:MT5CP2,F:MT6CP2",&mt5cpr,&mt6cpr,&mt5cp2,&mt6cp2);
      bfp->GetNamedData(recordTime,"F:TBCCDI,F:TBCCDO",&tbccdi,&tbccdo);
      bfp->GetNamedData(recordTime,"F:MT6CA1,F:MT6CA2,F:MT6CA3",&mt6ca1,&mt6ca2,&mt6ca3);
      
      fSpill->SetIntensity(intensity);
      //      fSpill->SetMomentum(momentum);
      fSpill->SetMT5CPR(mt5cpr);
      fSpill->SetMT6CPR(mt6cpr);
      fSpill->SetMT5CP2(mt5cp2);
      fSpill->SetMT6CP2(mt6cp2);
      fSpill->SetTBCCDI(tbccdi);
      fSpill->SetTBCCDO(tbccdo);
      fSpill->SetMT6CA1(mt6ca1);
      fSpill->SetMT6CA2(mt6ca2);
      fSpill->SetMT6CA3(mt6ca3);

      std::cout << "%%%%% IFBeam query results: %%%%%" << std::endl;
      std::cout << "Time: spillTime+fOffset = " << uint64_t(spillTime+fOffset) << std::endl;
      std::cout << "Intensity: " << intensity << std::endl;
      //      std::cout << "Momentum: " << momentum << std::endl;
      std::cout << "CkovMT5 Pressure: " << mt5cpr << std::endl;
      std::cout << "CkovMT6 Pressure: " << mt6cpr << std::endl;

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
  
  //......................................................................
  void SpillInfo::endSubRun(art::SubRun& sr)
  {
    sr.put(std::move(fSpill));
  }

  //......................................................................
  void SpillInfo::produce(art::Event& ) //evt)
  {
    
  }
  
} // end namespace emph
////////////////////////////////////////////////////////////////////////
DEFINE_ART_MODULE(emph::SpillInfo)
