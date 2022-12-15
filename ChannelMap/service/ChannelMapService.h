///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef CHANNELMAPSERVICE_H
#define CHANNELMAPSERVICE_H

#include "ChannelMap/ChannelMap.h"

//Framework includes
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace art {
  class ActivityRegistry;
  class Run;
}

namespace fhicl {
  class ParameterSet;
}

namespace emph
{
  namespace cmap
  {
    
    class ChannelMapService 
    {
    public:
      // Get a ChannelMapService instance here
      ChannelMapService(const fhicl::ParameterSet& pset,
			art::ActivityRegistry& reg);
      virtual ~ChannelMapService();
      
      void reconfigure(const fhicl::ParameterSet& pset);
    
      void preBeginRun(const art::Run& run);

      DChannel DetChan(EChannel echan) { return fChannelMap->DetChan(echan); }
      
      EChannel ElectChan(DChannel dchan) { return fChannelMap->ElectChan(dchan); }
      
      bool IsValidEChan(EChannel& echan) {return fChannelMap->IsValidEChan(echan); }

    private:
      bool fAbortIfFileNotFound;
      ChannelMap* fChannelMap;

    };
    
  }
}

DECLARE_ART_SERVICE(emph::cmap::ChannelMapService, SHARED)

#endif
