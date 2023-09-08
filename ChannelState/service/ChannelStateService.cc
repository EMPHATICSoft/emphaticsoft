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
					   art::ActivityRegistry & reg):
    fLoadSSDFromDB   (pset.get<bool>("LoadSSDFromDB")),
    fLoadARICHFromDB (pset.get<bool>("LoadARICHFromDB")),
    fCondbURL        (pset.get<std::string>("CondbURL"))

  {
    fChannelState->SetLoadSSDFromDB(fLoadSSDFromDB);
    fChannelState->SetLoadARICHFromDB(fLoadARICHFromDB);
    fChannelState->SetCondbURL(fCondbURL);

    fChannelState = new ChannelState();
    fChannelState->SetDataType("data");

    reg.sPreBeginSubRun.watch(this, &ChannelStateService::preBeginSubRun);
    
  }
  
  //----------------------------------------------------------
  
  ChannelStateService::~ChannelStateService()
  {
  }
  
  //----------------------------------------------------------
  void ChannelStateService::preBeginSubRun(const art::SubRun& sr)
  {
    fChannelState->SetRunSubrun(sr.run(),sr.subRun());
  }
  
}
