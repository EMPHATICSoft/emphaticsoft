///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "ChannelMap/service/ChannelMapService.h"
#include "RunHistory/service/RunHistoryService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"


namespace emph
{
  namespace cmap
  {
    
    //------------------------------------------------------------
    ChannelMapService::ChannelMapService(const fhicl::ParameterSet& pset,
					 art::ActivityRegistry & reg):
      fAbortIfFileNotFound (pset.get<bool>("AbortIfFileNotFound"))      
    {
      art::ServiceHandle<runhist::RunHistoryService> rhs;

      fChannelMap = new ChannelMap();

      reg.sPreBeginRun.watch(this, &ChannelMapService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    ChannelMapService::~ChannelMapService()
    {
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
