///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Geometry
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "Geometry/GeometryService.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace emph
{
  namespace geo
  {
    
    //------------------------------------------------------------
    GeometryService::GeometryService(const fhicl::ParameterSet& pset,
					 art::ActivityRegistry & reg)
    {
      reconfigure(pset);
      
      fGeometry = new emph::geo::Geometry(fGeoFileName);

      reg.sPreBeginRun.watch(this, &GeometryService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    GeometryService::~GeometryService()
    {
    }
    
    //-----------------------------------------------------------
    void GeometryService::reconfigure(const fhicl::ParameterSet& pset)
    {
      
      fGeoFileName = pset.get< std::string >("GeoFileName");
    }
    
    //----------------------------------------------------------
    // If we have run-dependent geometry, do something here to reload
    // the geometry if necessary
    //----------------------------------------------------------
    void GeometryService::preBeginRun(const art::Run& )
    {
      
    }
    
  }
}

DEFINE_ART_SERVICE(emph::geo::GeometryService)
