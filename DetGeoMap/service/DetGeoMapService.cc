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
      fUseGeometry (pset.get<bool>("UseGeometry")),
      fUseGeometryRef (pset.get<bool>("UseGeometryRef")),
      fUseAlign (pset.get<bool>("UseAlign"))
    {
      if (fUseAlign) std::cerr << " DetGeoMapService::DetGeoMapService, we will be using an alignment file.. " << std::endl;
      else std::cerr << " DetGeoMapService::DetGeoMapService, we will NOT be using an alignment file.. " << std::endl;
      art::ServiceHandle<emph::AlignService> align;

      fDetGeoMap = new DetGeoMap();
      fDetGeoMapRef = new DetGeoMap();
      fDetGeoMap->SetUseGeometry(fUseGeometry);
      fDetGeoMapRef->SetUseGeometry(fUseGeometryRef);

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
      art::ServiceHandle<emph::geo::GeometryService> geoRef;
      art::ServiceHandle<emph::AlignService> align;

      fDetGeoMap->SetRun(run.run());
      fDetGeoMapRef->SetRun(run.run());
      fDetGeoMap->SetGeometry(geo->Geo());
      fDetGeoMapRef->SetGeometry(geoRef->Geo());
      if (fUseAlign) {
        fDetGeoMap->SetAlign(align->GetAlign());
	fDetGeoMapRef->SetAlign(align->GetAlign());
      }
    }
    
  }
}
