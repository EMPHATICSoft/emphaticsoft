///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the run history
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "RunHistory/service/RunHistoryService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Run.h"

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
  void RunHistoryService::reconfigure(const fhicl::ParameterSet& pset)
  {
    fLoadFromDB = pset.get<bool>("LoadFromDB");
    if (fLoadFromDB)
      fQEURL = pset.get<std::string>("QEURL");
    
  }
  
  //----------------------------------------------------------
  // Set new run, which will force reloading of new data upon demand.
  //----------------------------------------------------------
  void RunHistoryService::preBeginRun(const art::Run& run)
  {
    fRunHistory.reset(new runhist::RunHistory(run.run()));
    if (fLoadFromDB) {
      fRunHistory->SetQEURL(fQEURL);
    }
  }
  
}
