///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Geometry
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "Geometry/service/GeometryService.h"
#include "RunHistory/service/RunHistoryService.h"

#include "TGeoManager.h"

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
      TGeoManager::LockDefaultUnits(0);
      TGeoManager::SetDefaultUnits(TGeoManager::EDefaultUnits::kRootUnits);
      TGeoManager::LockDefaultUnits(1);

      reconfigure(pset);
      
      art::ServiceHandle<runhist::RunHistoryService> rhs;

      reg.sPreBeginRun.watch(this, &GeometryService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    GeometryService::~GeometryService()
    {
    }
    
    //-----------------------------------------------------------
    void GeometryService::reconfigure(const fhicl::ParameterSet& )//pset)
    {

    }
    
    //----------------------------------------------------------
    // If we have run-dependent geometry, do something here to reload
    // the geometry if necessary
    //----------------------------------------------------------
    void GeometryService::preBeginRun(const art::Run& )
    {

      art::ServiceHandle<runhist::RunHistoryService> rhs;
      
      fGeometry.reset(new emph::geo::Geometry(rhs->RunHist()->GeoFile() ) );
      
    }
    
  }
}

DEFINE_ART_SERVICE(emph::geo::GeometryService)
