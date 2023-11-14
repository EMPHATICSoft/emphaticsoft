///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a detector channel-to-position map 
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "Geometry/service/GeometryService.h"
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
				       art::ActivityRegistry & reg):
      fUseGeometry (pset.get<bool>("UseGeometry"))
    {
      art::ServiceHandle<emph::AlignService> align;

      fDetGeoMap = new DetGeoMap();
      fDetGeoMap->SetUseGeometry(fUseGeometry);

      reg.sPreBeginRun.watch(this, &DetGeoMapService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    DetGeoMapService::~DetGeoMapService()
    {
    }
    
    //----------------------------------------------------------
    void DetGeoMapService::preBeginRun(const art::Run& run)
    {
      art::ServiceHandle<emph::geo::GeometryService> geo;
      art::ServiceHandle<emph::AlignService> align;

      fDetGeoMap->SetRun(run.run());
      fDetGeoMap->SetGeometry(geo->Geo());
      fDetGeoMap->SetAlign(align->GetAlign());
    }
    
  }
}
