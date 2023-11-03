///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide alignment constants
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef ALIGNSERVICE_H
#define ALIGNSERVICE_H

#include "Align/Align.h"

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
  class AlignService 
  {
  public:
    // Get a AlignService instance here
    AlignService(const fhicl::ParameterSet& pset,
		 art::ActivityRegistry& reg);
    virtual ~AlignService();
      
    void preBeginRun(const art::Run& run);

    Align* GetAlign() const { return fAlign; }

  private:
    bool fAbortIfFileNotFound;
    Align* fAlign;

  };
    
}

DECLARE_ART_SERVICE(emph::AlignService, SHARED)

#endif
