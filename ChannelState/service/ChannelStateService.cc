///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel state map  
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "ChannelState/service/ChannelStateService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/SubRun.h"

namespace emph
{
  //------------------------------------------------------------
  ChannelStateService::ChannelStateService(const fhicl::ParameterSet& pset,
					   art::ActivityRegistry & reg)
  {
    reconfigure(pset);
    
    fChannelState = new ChannelState();
    
    reg.sPreBeginSubRun.watch(this, &ChannelStateService::preBeginSubRun);
    
  }
  
  //----------------------------------------------------------
  
  ChannelStateService::~ChannelStateService()
  {
  }
  
  //-----------------------------------------------------------
  void ChannelStateService::reconfigure(const fhicl::ParameterSet& pset)
  {
    fChannelState->SetLoadSSDFromDB(pset.get<bool>("LoadSSDFromDB"));
    fChannelState->SetLoadARICHFromDB(pset.get<bool>("LoadARICHFromDB"));
    fChannelState->SetCondbURL(pset.get<std::string>("CondbURL"));
  }
  
  //----------------------------------------------------------
  void ChannelStateService::preBeginSubRun(const art::SubRun& sr)
  {
    fChannelState->SetRunSubrun(sr.run(),sr.subRun());
  }
  
}
