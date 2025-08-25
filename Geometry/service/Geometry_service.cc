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
      : fRunNumber(0),
	fGetGDMLFromRunHistory(true),
	fGDMLFile("")
    {
      TGeoManager::LockDefaultUnits(0);
      TGeoManager::SetDefaultUnits(TGeoManager::EDefaultUnits::kRootUnits);
      TGeoManager::LockDefaultUnits(1);
      
      art::ServiceHandle<runhist::RunHistoryService> rhs;
      fGetGDMLFromRunHistory = pset.get< bool >("GetGDMLFromRunHistory");
      fGDMLFile = pset.get< std::string >("GDMLFile");

      if (fGetGDMLFromRunHistory && !fGDMLFile.empty()) {
	MF_LOG_ERROR("GeometryService") 
	  << "Cannot use geometry both from RunHistory and a defined file.  Check your fhicl configuration for the Geometry service.!";
	abort();
      }

      if (!fGetGDMLFromRunHistory && fGDMLFile.empty()) {
	MF_LOG_ERROR("GeometryService") 
	  << "GDML file undefined in Geometry service fhicl!";
	abort();
      }

      reg.sPreBeginRun.watch(this, &GeometryService::preBeginRun);
    }
    
    //----------------------------------------------------------
    
    GeometryService::~GeometryService()
    {
    }
    
    //-----------------------------------------------------------
    // If we have run-dependent geometry, do something here to reload
    // the geometry if necessary
    //----------------------------------------------------------
    void GeometryService::preBeginRun(const art::Run& run)
    {
      fRunNumber = run.run();

      if (fGetGDMLFromRunHistory) {
	std::cout << "GeometryService::preBeginRun" << std::endl;
	art::ServiceHandle<runhist::RunHistoryService> rhs;	
	fGeometry.reset(new emph::geo::Geometry(rhs->RunHist()->GeoFile() ) );
      }      
      else
     	fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
      /*
      std::cout << "GeometryService::preBeginRun" << std::endl;
      // Check if geo has already been loaded for this run
      if(run.run() == fRunNumber) return;
      fRunNumber = run.run();

      art::ServiceHandle<runhist::RunHistoryService> rhs;

      const std::string newGeoFile = rhs->RunHist()->GeoFile();
      
      // Only load geometry if it has changed
      if (newGeoFile == fLoadedGeoFile){
	std::cout << "Geometry for run " << fRunNumber
		  << " unchanged from previous run." << std::endl;
	return;
      }
      
      fGeometry.reset(new emph::geo::Geometry(newGeoFile) );
      fLoadedGeoFile = newGeoFile;
      
      */
    }

  }
}

DEFINE_ART_SERVICE(emph::geo::GeometryService)
