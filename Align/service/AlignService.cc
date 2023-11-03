///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a channel map configured to the right 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "Align/service/AlignService.h"
#include "RunHistory/service/RunHistoryService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"

namespace emph
{
    
  //------------------------------------------------------------
  AlignService::AlignService(const fhicl::ParameterSet& pset,
			     art::ActivityRegistry & reg)
  {
    art::ServiceHandle<runhist::RunHistoryService> rhs;

    fAlign = new Align();

    reg.sPreBeginRun.watch(this, &AlignService::preBeginRun);

  }
    
  //----------------------------------------------------------
    
  AlignService::~AlignService()
  {
  }
        
  //----------------------------------------------------------
  void AlignService::preBeginRun(const art::Run& )
  {
    art::ServiceHandle<runhist::RunHistoryService> rhs;

    fAlign->LoadSSDConsts(rhs->RunHist()->SSDAlignFile());
  }
    
}
