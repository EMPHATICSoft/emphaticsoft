///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "ChannelMap/ChannelMapService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace emph
{
  namespace cmap
  {

    //------------------------------------------------------------
    ChannelMapService::ChannelMapService(const fhicl::ParameterSet& pset,
                                       art::ActivityRegistry &reg)
  {
    reconfigure(pset);
    reg.sPreBeginRun.watch(this, &ChannelMapService::preBeginRun);
  }

  //----------------------------------------------------------

  ChannelMapService::~ChannelMapService()
  {
  }

  //-----------------------------------------------------------
  void ChannelMapService::reconfigure(const fhicl::ParameterSet& pset)
  {
    fMapFileName            = pset.get< std::string >("MapFileName");
  }

  //----------------------------------------------------------
  void ChannelMapService::preBeginRun(const art::Run& run)
  {
    if (run.run() > 0)
      if (! this->LoadMap(fMapFileName))
	mf::LogError("Failed to load channel map!");
    
    return;
  }
    
    
    DEFINE_ART_SERVICE(ChannelMapService)
    
  }
}
