///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the run history
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef RUNHISTORYSERVICE_H
#define RUNHISTORYSERVICE_H

#include "RunHistory/RunHistory.h"

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

#include <memory>

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
