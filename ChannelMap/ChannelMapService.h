///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef CHANNELMAPSERVICE_H
#define CHANNELMAPSERVICE_H

#include "ChannelMap/ChannelMap.h"

//Framework includes
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

namespace emph
{
  namespace cmap
  {
    
    class ChannelMapService : public ChannelMap
    {
    public:
      // Get a ChannelMapService instance here
      ChannelMapService(const fhicl::ParameterSet& pset,
			art::ActivityRegistry& reg);
      virtual ~ChannelMapService();
      
      void reconfigure(const fhicl::ParameterSet& pset);
    
      void preBeginRun(const art::Run& run);
      
    protected:
      std::string fMapFileName;
      
    };
    
  }
}

DECLARE_ART_SERVICE(emph::cmap::ChannelMapService, LEGACY)

#endif