///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the geometry
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRYSERVICE_H
#define GEOMETRYSERVICE_H

#include "Geometry/Geometry.h"

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "TGeoManager.h"

namespace emph
{
  namespace geo
  {
    
    class GeometryService 
    {
    public:
      // Get a GeometryService instance here
      GeometryService(const fhicl::ParameterSet& pset,
			art::ActivityRegistry& reg);
      virtual ~GeometryService();
      
      void preBeginRun(const art::Run& run);

      emph::geo::Geometry* Geo() const { return fGeometry.get(); }
      emph::geo::Geometry* GeoRef() const { return fGeometryRef.get(); }

    private:
      std::unique_ptr<emph::geo::Geometry> fGeometry;
      std::unique_ptr<emph::geo::Geometry> fGeometryRef;
      unsigned int fRunNumber;
      bool fGetGDMLFromRunHistory;
      std::string fGDMLFile;
      std::string fGDMLFileRef;
      int fMoveStationNumber;
      float fMoveStationByX, fMoveStationByY, fMoveStationByZ;
      int fRotateStationNumber;
      float fRotateStationBydPhi;
    };
    
  }
}

DECLARE_ART_SERVICE(emph::geo::GeometryService, SHARED)

#endif
