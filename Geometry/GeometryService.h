///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the geometry
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRYSERVICE_H
#define GEOMETRYSERVICE_H

#include "Geometry/Geometry.h"

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

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
      
      void reconfigure(const fhicl::ParameterSet& pset);
    
      void preBeginRun(const art::Run& run);

      emph::geo::Geometry* Geo() const { return fGeometry; }

      std::string GDMLFile() const { return fGeoFileName; }

    private:
      emph::geo::Geometry* fGeometry;
      std::string fGeoFileName;

    };
    
  }
}

DECLARE_ART_SERVICE(emph::geo::GeometryService, SHARED)

#endif
