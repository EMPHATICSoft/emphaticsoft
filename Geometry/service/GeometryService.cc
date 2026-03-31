///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Geometry (implementation)
/// \author jpaley@fnal.gov extended by Paul Lebrun, lebrun@fnal.gov, 
///  To support Geometry modifier.. 
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes

#include "Geometry/service/GeometryService.h"
#include "RunHistory/service/RunHistoryService.h"
#include "Geometry/ModAlign/ModGDML.h"

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
	fGDMLFile(""), fGDMLFileRef(""),
	fMoveStationNumber( pset.get< int >("MoveStationNumber", -1)),
	fMoveStationByX(pset.get< float >("MoveStationByX", 0.)),
	fMoveStationByY(pset.get< float >("MoveStationByY", 0.)),
	fMoveStationByZ(pset.get< float >("MoveStationByZ", 0.)),
	fRotateStationNumber( pset.get< int >("RotateStationNumber", -1)),
	fRotateStationBydPhi(pset.get< float >("RotateStationBydPhi", 0.))
	
    {
      std::cerr << " GeometryService::GeometryService, start ... " << std::endl;
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

      std::cerr << " GeometryService::preBeginRun, start... " << std::endl;

      fRunNumber = run.run();     



      if (fGetGDMLFromRunHistory) {
	std::cout << "GeometryService::preBeginRun" << std::endl;
	art::ServiceHandle<runhist::RunHistoryService> rhs;	
	fGeometry.reset(new emph::geo::Geometry(rhs->RunHist()->GeoFile() ) );
      }      
      else {
        ModGDML myMods(fGDMLFile); // a utility class to handle the edits of a Geometry file. 
        if ((fMoveStationNumber != -1) && (fMoveStationNumber != 999)) {
	  bool ok = myMods.TranslateAStation(fMoveStationNumber, fMoveStationByX, fMoveStationByY, fMoveStationByZ);
	  if (!ok) {
	    std::cerr << " GeometryService::preBeginRun logic problem in ModGDML, stop here " << std::endl;
	    exit(2);
	  }
	  size_t iPosPh = fGDMLFile.find("phase");
	  std::string tmpName0 = fGDMLFile.substr(iPosPh);
	  size_t iPosDot = tmpName0.find(".gdml");
	  std::string tmpName1 = tmpName0.substr(0, tmpName0.size()-5);
	  std::ostringstream newFileNameStrStr; 
	  newFileNameStrStr << "./" << tmpName1 << "_MvStation_" << fMoveStationNumber;
	  if (std::abs(fMoveStationByX) > 1.0e-6) 
	     newFileNameStrStr << "_XBy_" << static_cast<int>(1000.*fMoveStationByX); 
	   // in micron, avoid the use of floating pts in filename.   
	  if (std::abs(fMoveStationByY) > 1.0e-6) 
	     newFileNameStrStr << "_YBy" << static_cast<int>(1000.*fMoveStationByY); 
	  if (std::abs(fMoveStationByZ) > 1.0e-6) 
	     newFileNameStrStr << "_ZBy" << static_cast<int>(1.*fMoveStationByZ); // Z in mm.. Yeah, confusing... 
	  newFileNameStrStr << ".gdml";
	  fGDMLFileRef = fGDMLFile; // we keep the reference geometry..
	  fGDMLFile = newFileNameStrStr.str();
	  std::cerr << " GeometryService::preBeginRun New file name " << fGDMLFile << std::endl;
	  myMods.SaveIt(fGDMLFile);
     	  fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
	  std::cerr << " ... Perhaps, successful modification of the GDML file.." << std::endl;
//	std::cerr << "  and quit now for good " << std::endl; exit(2);
        } else if (fMoveStationNumber == 999) {
	  std::cerr << " ... GeometryService::preBeginRun Move all the stations. Not implemented yet " << std::endl;
	  exit(2);
	}
        if ((fRotateStationNumber != -1) && (fRotateStationNumber != 999)) {
	  bool ok = myMods.RotateAStation(fRotateStationNumber, fRotateStationBydPhi);
	  if (!ok) {
	    std::cerr << " GeometryService::preBeginRun logic problem in ModGDML, in rotations stop here " << std::endl;
	    exit(2);
	  }
	  size_t iPosPh = fGDMLFile.find("phase");
	  std::string tmpName0 = fGDMLFile.substr(iPosPh);
	  size_t iPosDot = tmpName0.find(".gdml");
	  std::string tmpName1 = tmpName0.substr(0, tmpName0.size()-5);
	  std::ostringstream newFileNameStrStr; 
	  newFileNameStrStr << "./" << tmpName1 << "_RotStation_" << fRotateStationNumber;
	  newFileNameStrStr << "_" << static_cast<int>(1.0e6*fRotateStationBydPhi); 
	   // in micro-degree, avoid the use of floating pts in filename.   
	  newFileNameStrStr << ".gdml";
	  fGDMLFileRef = fGDMLFile; // we keep the reference geometry..
	  fGDMLFile = newFileNameStrStr.str();
	  std::cerr << " GeometryService::preBeginRun New file name " << fGDMLFile << std::endl;
	  myMods.SaveIt(fGDMLFile);
     	  fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
	  std::cerr << " ... Perhaps, successful modification of the GDML file.." << std::endl;
//	std::cerr << "  and quit now for good " << std::endl; exit(2);
        } else if (fRotateStationNumber == 999) {
	  std::cerr << " ... GeometryService::preBeginRun Move all the stations. Not implemented yet " << std::endl;
	  exit(2);
	}
     // Assume the use case of simulation, or alignment task..  
     	fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) ); // That is the modified geometry..
	if (fGDMLFileRef.length() > 2) fGeometryRef.reset(new emph::geo::Geometry(fGDMLFileRef.c_str()) );
	else fGeometryRef.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
	//
	// We need the capability of not moving the SSD stations, or the planes, or the magnet.
	// In which case, we will have two exactly identical instance of the geometry.. 
      } 
      // Checking things.. 
      std::cerr << " GeometryService::preBeginRun, check GDML, modified file Name  " << fGeometry->GDMLFile() << std::endl; 
      if (fGeometryRef != nullptr) 
        std::cerr << " GeometryService::preBeginRun, check GDML, reference  file Name  " << fGeometryRef->GDMLFile() << std::endl; 
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
