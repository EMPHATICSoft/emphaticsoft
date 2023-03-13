///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a detector channel-to-position map 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "DetGeoMap/service/DetGeoMapService.h"
//#include "RunHistory/service/RunHistoryService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"


namespace emph
{
  namespace dgmap
  {
    
    //------------------------------------------------------------
    DetGeoMapService::DetGeoMapService(const fhicl::ParameterSet& pset,
					 art::ActivityRegistry & reg)
    {
      reconfigure(pset);
      
      //      art::ServiceHandle<runhist::RunHistoryService> rhs;

      fDetGeoMap = new DetGeoMap();

      reg.sPreBeginRun.watch(this, &DetGeoMapService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    DetGeoMapService::~DetGeoMapService()
    {
    }
    
    //-----------------------------------------------------------
    void DetGeoMapService::reconfigure(const fhicl::ParameterSet& )// pset)
    {
      //      fAbortIfFileNotFound = pset.get<bool>("AbortIfFileNotFound");
    }
    
    //----------------------------------------------------------
    void DetGeoMapService::preBeginRun(const art::Run& run)
    {
      //      fDetGeoMap->SetAbortIfFileNotFound(fAbortIfFileNotFound);      
      //      art::ServiceHandle<runhist::RunHistoryService> rhs;

      fDetGeoMap->SetRun(run.run());
    }
    
  }
}
