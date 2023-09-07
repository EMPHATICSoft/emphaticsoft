///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel state map
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef CHANNELSTATESERVICE_H
#define CHANNELSTATESERVICE_H

#include "ChannelState/ChannelState.h"

//Framework includes
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace art {
  class ActivityRegistry;
  class Run;
  class SubRun;
}

namespace fhicl {
  class ParameterSet;
}

namespace emph
{
  class ChannelStateService 
  {
  public:
    // Get a ChannelStateService instance here
    ChannelStateService(const fhicl::ParameterSet& pset,
		      art::ActivityRegistry& reg);
    virtual ~ChannelStateService();
    
    void reconfigure(const fhicl::ParameterSet& pset);
    
    void preBeginSubRun(const art::SubRun& subrun);
    
    ChannelStateType State(emph::geo::DetectorType detId, int chanId) {
      return fChannelState->State(detId, chanId); }
    
  private:
    ChannelState* fChannelState;
    bool fLoadSSDFromDB;
    bool fLoadARICHFromDB;
    std::string fCondbURL;
    
  };
  
}

DECLARE_ART_SERVICE(emph::ChannelStateService, SHARED)

#endif
