///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "ChannelMap/service/ChannelMapService.h"
#include "RunHistory/service/RunHistoryService.h"

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
      
      art::ServiceHandle<runhist::RunHistoryService> rhs;

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
      fAbortIfFileNotFound = pset.get<bool>("AbortIfFileNotFound");
    }
    
    //----------------------------------------------------------
    void ChannelMapService::preBeginRun(const art::Run& )
    {
      fChannelMap->SetAbortIfFileNotFound(fAbortIfFileNotFound);      
      art::ServiceHandle<runhist::RunHistoryService> rhs;

      fChannelMap->LoadMap(rhs->RunHist()->ChanFile());
    }
    
  }
}

DEFINE_ART_SERVICE(emph::cmap::ChannelMapService)
