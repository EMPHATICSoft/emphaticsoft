///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a detector channel-to-position map 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "DetGeoMap/service/DetGeoMapService.h"

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
      fDetGeoMap = new DetGeoMap();
      reconfigure(pset);
      
      reg.sPreBeginRun.watch(this, &DetGeoMapService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    DetGeoMapService::~DetGeoMapService()
    {
    }
    
    //-----------------------------------------------------------
    void DetGeoMapService::reconfigure(const fhicl::ParameterSet& pset)
    {
      fDetGeoMap->SetUseGeometry(pset.get<bool>("UseGeometry"));
    }
    
    //----------------------------------------------------------
    void DetGeoMapService::preBeginRun(const art::Run& run)
    {
      fDetGeoMap->SetRun(run.run());
      fDetGeoMap->Reset();
    }
    
  }
}
