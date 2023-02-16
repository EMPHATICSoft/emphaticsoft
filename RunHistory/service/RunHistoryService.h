///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the run history
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef RUNHISTORYSERVICE_H
#define RUNHISTORYSERVICE_H

#include "RunHistory/RunHistory.h"

//Framework includes
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

#include <memory>

namespace art {
  class ActivityRegistry;
  class Run;
}

namespace fhicl {
  class ParameterSet;
}

namespace runhist
{
  class RunHistoryService 
  {
  public:
    // Get a GeometryService instance here
    RunHistoryService(const fhicl::ParameterSet& pset,
		      art::ActivityRegistry& reg);
    virtual ~RunHistoryService();
    
    void reconfigure(const fhicl::ParameterSet& pset);
    
    void preBeginRun(const art::Run& run);
    
    runhist::RunHistory* RunHist() const { return fRunHistory.get(); }
    
  private:
    std::unique_ptr<runhist::RunHistory> fRunHistory;
    
  };
  
}

DECLARE_ART_SERVICE(runhist::RunHistoryService, SHARED)

#endif
