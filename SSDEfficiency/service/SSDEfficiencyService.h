///////////////////////////////////////////////////////////////////////////
/// \brief Simple service to provide SSD efficiencies
/// \author vo17@illinois.edu
///////////////////////////////////////////////////////////////////////////

#ifndef SSDEFFICIENCYSERVICE_H
#define SSDEFFICIENCYSERVICE_H

#include "SSDEfficiency/SSDEfficiency.h"

// Framework includes
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace art {
  class ActivityRegistry;
  class Run;
}

namespace fhicl {
  class ParameterSet;
}

namespace emph {
  class SSDEfficiencyService {
    public:
      SSDEfficiencyService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg); // Constructor
      virtual ~SSDEfficiencyService() {}; // Destructor

      void preBeginRun(const art::Run& run);
      SSDEfficiency* GetSSDEfficiency() const { return fSSDEfficiency; }

    private:
      bool fAbortIfFileNotFound;
      SSDEfficiency* fSSDEfficiency;
  };
}

DECLARE_ART_SERVICE(emph::SSDEfficiencyService, SHARED)

#endif
