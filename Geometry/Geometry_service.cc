///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Geometry
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>

// EMPHATIC includes
#include "Geometry/GeometryService.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"

namespace emph
{
  namespace geo
  {
    
    //------------------------------------------------------------
    GeometryService::GeometryService(const fhicl::ParameterSet& pset,
					 art::ActivityRegistry & reg)
    {
      reconfigure(pset);
      
      cet::search_path sp("CETPKG_SOURCE");
      
      std::string fFileName = fGeoFileName;
      sp.find_file(fFileName,fGeoFileName);
      struct stat sb;
      if ( fGeoFileName.empty() || stat(fGeoFileName.c_str(), &sb)!=0 ) {
	// failed to resolve the file name
	throw cet::exception("NoGDMLFile")
	  << "Geometry GDML file " << fGeoFileName << " not found!\n"
	  << __FILE__ << ":" << __LINE__ << "\n";
      }
      
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
