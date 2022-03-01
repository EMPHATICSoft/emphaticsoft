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
					 art::ActivityRegistry &) //reg)
    {
      reconfigure(pset);
      //reg.sPreBeginRun.watch(this, &ChannelMapService::preBeginRun);
      std::cout << "ChannelMapService()" << std::endl;
    }
    
    //----------------------------------------------------------
    
    ChannelMapService::~ChannelMapService()
    {
    }
    
    //-----------------------------------------------------------
    void ChannelMapService::reconfigure(const fhicl::ParameterSet& pset)
    {
      
      this->SetMapFileName(pset.get< std::string >("MapFileName"));
      this->SetAbortIfFileNotFound(pset.get<bool>("AbortIfFileNotFound"));
    }
    
    //----------------------------------------------------------
    //    void ChannelMapService::preBeginRun(const art::Run& )
    //    {
    //    }
    
  }
}

DEFINE_ART_SERVICE(emph::cmap::ChannelMapService)
