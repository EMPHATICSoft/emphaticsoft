///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "ChannelMap/ChannelMapService.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace emph
{
  namespace cmap
  {
    
    //------------------------------------------------------------
    ChannelMapService::ChannelMapService(const fhicl::ParameterSet& pset,
					 art::ActivityRegistry & reg)
    {
      reconfigure(pset);
      
      fChannelMap = new ChannelMap();

      reg.sPreBeginRun.watch(this, &ChannelMapService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    ChannelMapService::~ChannelMapService()
    {
    }
    
    //-----------------------------------------------------------
    void ChannelMapService::reconfigure(const fhicl::ParameterSet& pset)
    {
      
      fMapFileName = pset.get< std::string >("MapFileName");
      fAbortIfFileNotFound = pset.get<bool>("AbortIfFileNotFound");
    }
    
    //----------------------------------------------------------
    void ChannelMapService::preBeginRun(const art::Run& )
    {
      //fChannelMap->SetMapFileName(fMapFileName);
      fChannelMap->SetAbortIfFileNotFound(fAbortIfFileNotFound);      
    }
    
  }
}

DEFINE_ART_SERVICE(emph::cmap::ChannelMapService)
