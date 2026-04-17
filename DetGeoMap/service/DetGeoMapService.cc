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
#include "TGeoManager.h"

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
      // Confusing.. 
//      if (fUseGeometryRef) std::cerr 
//         << " DetGeoMapService::DetGeoMapService, we will be using the reference alignment file.. " << std::endl;
//      if (!fUseGeometryRef) std::cerr 
//         << " DetGeoMapService::DetGeoMapService, we will be using the modified alignment file.. " << std::endl;
      art::ServiceHandle<emph::AlignService> align;
      // This to force the creation of two distinct 
      fDetGeoMap = new DetGeoMap(); fDetGeoMap->SetReferenceFlag(false);
      fDetGeoMapRef = new DetGeoMap(); fDetGeoMapRef->SetReferenceFlag(true);
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
      std::cerr << " DetGeoMapService::preBeginRun... start.. " << std::endl;
      art::ServiceHandle<emph::geo::GeometryService> geo;
      art::ServiceHandle<emph::geo::GeometryService> geoRef;
      art::ServiceHandle<emph::AlignService> align;

      if ((geo->Geo() == nullptr) || (geo->Geo() == 0)) {
        std::cerr << "  DetGeoMapService::preBeginRun No master geometry defined, quit here and now " << std::endl; 
	exit(2);
      }
      fDetGeoMap->SetRun(run.run());
      fDetGeoMapRef->SetRun(run.run());
      // Checking the geometry file names 
      if (geo->Geo()->GDMLFile().empty()) {
        std::cerr << " No Geometry file uploaded.. Calling the service ? " << std::endl;
	geo->preBeginRun(run); 
      } 
      std::cerr << " DetGeoMapService::preBeginRun GDML geometry from file " << geo->Geo()->GDMLFile() << std::endl;
      std::cerr << " DetGeoMapService::preBeginRun GDML reference  geometry from file " << geoRef->GeoRef()->GDMLFile() << std::endl;
      fDetGeoMap->SetGeometry(geo->Geo());
      fDetGeoMapRef->SetGeometry(geoRef->GeoRef());
      if (fUseAlign) {
        fDetGeoMap->SetAlign(align->GetAlign());
	fDetGeoMapRef->SetAlign(align->GetAlign());
      }
    }
    
  }
}
