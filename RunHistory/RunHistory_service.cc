///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the run history
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "RunHistory/RunHistoryService.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace runhist
{
  //------------------------------------------------------------
  RunHistoryService::RunHistoryService(const fhicl::ParameterSet& pset,
				       art::ActivityRegistry & reg)
  {
    reconfigure(pset);

    reg.sPreBeginRun.watch(this, &RunHistoryService::preBeginRun);
    
  }
  
  //----------------------------------------------------------
  
  RunHistoryService::~RunHistoryService()
  {
  }
  
  //-----------------------------------------------------------
  void RunHistoryService::reconfigure(const fhicl::ParameterSet& ) // pset)
  {
    
  }
  
  //----------------------------------------------------------
  // Set new run, which will force reloading of new data upon demand.
  //----------------------------------------------------------
  void RunHistoryService::preBeginRun(const art::Run& run)
  {
    fRunHistory.reset(new runhist::RunHistory(run.run()));
  }
  
}

DEFINE_ART_SERVICE(runhist::RunHistoryService)
