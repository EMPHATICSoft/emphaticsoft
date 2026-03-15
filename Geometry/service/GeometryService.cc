///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Geometry (implementation)
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "Geometry/service/GeometryService.h"
#include "RunHistory/service/RunHistoryService.h"
#include "Geometry/Persistency/SaveGDML.h"
#include "TGeoManager.h"
#include "TFile.h"
#include "TClass.h"
#include "TObject.h"
#include "TSystem.h"

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
	fGDMLFile(""),
	fTestSaveGDML(pset.get< bool >("TestSaveGDML", false))
    {
    
      TGeoManager::LockDefaultUnits(0);
      TGeoManager::SetDefaultUnits(TGeoManager::EDefaultUnits::kG4Units);
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
      if (fRunNumber == run.run()) return;

      fRunNumber = run.run();

      if (fGetGDMLFromRunHistory) {
	std::cerr << "GeometryService::preBeginRun, from Run History, run number " << fRunNumber << std::endl;
	art::ServiceHandle<runhist::RunHistoryService> rhs;	
	fGeometry.reset(new emph::geo::Geometry(rhs->RunHist()->GeoFile() ) );
      }      
      else {
        
        std::cerr << " GeometryService::preBeginRun, run " << fRunNumber 
	          << " GDML file " << fGDMLFile << std::endl; 
     	fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
      }
      SaveGDML saveUs(fGDMLFile); // done 
      
      if (fTestSaveGDML) {
        
	std::cerr << "GeometryService::preBeginRun, testing the " << 
	              "Provenance of the Geometry, currently in memory .. " << std::endl; 
	
	std::cerr << " ... Sniffing for the z position of station 6 " << std::endl;
	
	for (auto it= saveUs.cbegin(); it != saveUs.cend(); it++) {
	  if (it->find("ssdStation6_shift") != std::string::npos) { 
	    std::cerr << " ... The relevant line is " << (*it) << std::endl;
	    break;
	  }
	}
	//
	// Test the ASCII output..
	//
	saveUs.SaveAgain(std::string("./testSaveGDMLAgain_v0.gdml"));
	//
	// Test the ROOT i/o business. A lot more difficult.. Not finished !!!!
	//
	std::unique_ptr<TFile> myTFilePtr( TFile::Open("TestSaveGDML.root", "RECREATE") );
//	auto myClassTClass_0 = TClass::GetClass("SaveGDML"); //does not crash, but the root file is only 431 bytes long, can't be it.. 
//      We load nothing.. 
	std::cerr << " ... trying to save it in a root file.. ( I think,... hopefully! " << std::endl;
        gSystem->Load("libGeometry_Persistency_dict");
        myTFilePtr->Add((TObject*) &saveUs);  //does not crash, but the root file is only 431 bytes long, can't be it.. 
	std::cerr << " ... Saved in a root file.. ( I think,... hopefully!) Not the case, Solid crash... " << std::endl;
	myTFilePtr->Close();
	
        std::cerr << " ... End of testing SaveGDML, Provenance of the Geometry " << std::endl;
//        std::cerr << " ... And quit for now..  " << std::endl; exit(2);
      }
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
