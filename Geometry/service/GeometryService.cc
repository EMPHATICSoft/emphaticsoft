///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Geometry (implementation)
/// \author jpaley@fnal.gov extended by Paul Lebrun, lebrun@fnal.gov, 
///  To support Geometry modifier.. 
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
//
#include <uuid/uuid.h> // for randomly modifed geometries.. 
#include "TRandom3.h"
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
	fRotateStationBydPhi(pset.get< float >("RotateStationBydPhi", 0.)),
	fMoveStationByWidthX(pset.get< float >("MoveStationByWidthX")),
	fMoveStationByWidthY(pset.get< float >("MoveStationByWidthY")),
	fMoveStationByWidthZ(pset.get< float >("MoveStationByWidthZ")),
	fRotateStationByWidthdPhi(pset.get< float >("RotateStationByWidthdPhi")),
        fMisAlignByStation(pset.get<bool>("MisAlignByStation")), 
        fMoveAllBySensors(pset.get<bool>("MoveAllBySensors")), 
	fRandomSeed(pset.get<double>("RandomSeed")),
        fMoveSensorByWidthX(pset.get<float>("MoveSensorByWidthX")),
	fMoveSensorByWidthY(pset.get<float>("MoveSensorByWidthY")), 
	fRotateSensorByWidthdPhi(pset.get<float>("RotateSensorByWidthdPhi")),
        fRotateStation0XPlaneBydPhi(pset.get<float>("RotateStation0XPlaneBydPhi")),
	fRotateStation0YPlaneBydPhi(pset.get<float>("RotateStation0YPlaneBydPhi")),
        fRotateStation4XPlaneBydPhi(pset.get<float>("RotateStation4XPlaneBydPhi")),
        fRotateStation4YPlaneBydPhi(pset.get<float>("RotateStation4YPlaneBydPhi")),
        fNPlanes({ 2, 2, 3, 3, 2, 3, 3, 2})
	// Valid only for Phase1c, with 8 stations total 
    {
//      std::cerr << " GeometryService::GeometryService quit now!! " << std::endl; exit(2);
      size_t indexVarMille = 0;
      for (short iSt = 0; iSt != fNPlanes.size(); iSt++) {
        for (short jPl=0; jPl != fNPlanes[iSt]; jPl++) { 
	  short nSensors = (iSt < 5) ? 1 : 2;
	  for (short kSe=0; kSe != nSensors; kSe++) {
	    short  ii=(1000*iSt + 100*jPl + 10*kSe);
	    size_t iVarMille=static_cast<size_t>(1000*iSt + 100*jPl + 10*kSe);
	    mapBySensorToIndexX.insert(std::pair<short, size_t>(ii, iVarMille+1));  
	    mapBySensorToIndexY.insert(std::pair<short, size_t>(ii, iVarMille+2));  
	    mapBySensorToIndexPhi.insert(std::pair<short, size_t>(ii, iVarMille+3));  
	  }
	}
      }
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
      if ((fRunNumber == run.run()) && (run.run() != 2998)) return; // 2998 is reserved for modified Geometries, 
      // Systematic error studies.. 

      std::cerr << " GeometryService::preBeginRun, start... GDML file at onset  " << fGDMLFile << std::endl;

      fRunNumber = run.run();     



      if (fGetGDMLFromRunHistory) {
	std::cerr << "GeometryService::preBeginRun, getting the file from run history database " << std::endl;
	art::ServiceHandle<runhist::RunHistoryService> rhs;	
	fGeometry.reset(new emph::geo::Geometry(rhs->RunHist()->GeoFile() ) );
	return;
      }      
       else { //
      // modification to study systematic uncertainties 
	std::cerr << " ....Not from RunHistory..  somewhat complicated logic.. to  be improved.. " << std::endl; 
	if ((fMoveStationNumber != -1) ||(fRotateStationNumber != -1)) { // one or more moves or rotation will occur..
          ModGDML myMods(fGDMLFile); // a utility class to handle the edits of a Geometry file. 
          if (fMisAlignByStation) this->modifyByStation(myMods);
	  else this->modifyBySensor(myMods);
	  fGDMLFileRef = fGDMLFile; // we keep the reference geometry..
	  fGDMLFile = fNewFileNameStrStr.str();
	  std::cerr << " GeometryService::preBeginRun New file name " << fGDMLFile << std::endl;
	  std::cerr << " ... Perhaps, successful modification of the GDML file.." << std::endl;
	  myMods.SaveIt(fGDMLFile);
     	  fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
	  fGeometryRef.reset(new emph::geo::Geometry(fGDMLFileRef.c_str()) );
      // Checking things.. 
          std::cerr << " GeometryService::preBeginRun, check GDML, modified file Name  " << fGeometry->GDMLFile() << std::endl; 
          if (fGeometryRef != nullptr) 
          std::cerr << " GeometryService::preBeginRun, check GDML, reference  file Name  " << fGeometryRef->GDMLFile() << std::endl;
	  return; 
       } else { // the reference and master are identical.. 
         fGeometry.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
       }
       fGeometryRef.reset(new emph::geo::Geometry(fGDMLFile.c_str()) );
     }
	//
	// We need the capability of not moving the SSD stations, or the planes, or the magnet.
	// In which case, we will have two exactly identical instance of the geometry.. 
  } // 
  void GeometryService::modifyByStation(ModGDML &myMods) {     
      std::cerr << " GeometryService::modifyByStation, start... " << std::endl; 
      TRandom3 *myRand = new TRandom3(fRandomSeed);
      uuid_t uuid;
      uuid_generate(uuid);
      char uuidChar[37];
      uuid_unparse(uuid, uuidChar); // We might not use it, if only one station moves. 
      size_t iPosPh = fGDMLFile.find("phase");
      std::string tmpName0 = fGDMLFile.substr(iPosPh);
      size_t iPosDot = tmpName0.find(".gdml");
      std::string tmpName1 = tmpName0.substr(0, tmpName0.size()-5);
      fNewFileNameStrStr.str("");
      fNewFileNameStrStr.clear();
      fNewFileNameStrStr << "./" << tmpName1;  
      if ((fMoveStationNumber != -1) && (fMoveStationNumber != 999)) {
        bool ok = myMods.TranslateAStation(fMoveStationNumber, fMoveStationByX, fMoveStationByY, fMoveStationByZ);
        if (!ok) {
          std::cerr << " GeometryService::preBeginRun logic problem in ModGDML, stop here " << std::endl;
          exit(2);
        }
        fNewFileNameStrStr << "_MvStation_" << fMoveStationNumber;
        if (std::abs(fMoveStationByX) > 1.0e-6) 
           fNewFileNameStrStr << "_XBy_" << static_cast<int>(1000.*fMoveStationByX); 
       // in micron, avoid the use of floating pts in filename.   
        if (std::abs(fMoveStationByY) > 1.0e-6) 
         fNewFileNameStrStr << "_YBy" << static_cast<int>(1000.*fMoveStationByY); 
        if (std::abs(fMoveStationByZ) > 1.0e-6) 
         fNewFileNameStrStr << "_ZBy" << static_cast<int>(1.*fMoveStationByZ); // Z in mm.. Yeah, confusing... 
      } else if (fMoveStationNumber == 999) { 
        for (short kSt=0; kSt != 8; kSt++) { // Valid only for Phase1c, late run ( 8 stations total) 
          if ((kSt == 4) || (kSt == 0)) { 
            std::array<float, 3> shifts = {0., 0., 0.}; fNewCoordStations.push_back(shifts); continue; 
          }
          double dx = fMoveStationByWidthX*2.0*(myRand->Uniform()-0.5); 
          double dy = fMoveStationByWidthY*2.0*(myRand->Uniform()-0.5); 
          double dz = fMoveStationByWidthZ*2.0*(myRand->Uniform()-0.5);
          if (std::abs(dz) < 0.01) dz = 0.; // Root GDML does not like scientifc notation for float or double, it seems 
          bool ok = myMods.TranslateAStation(kSt, dx, dy, dz);
          std::array<float, 3> shifts = 
             {static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz)}; 
             fNewCoordStations.push_back(shifts); 
          if (!ok) {
            std::cerr << " GeometryService::preBeginRun logic problem in ModGDML, stop here " << std::endl;
            exit(2);
          }
          
        }
      }
      if ((fRotateStationNumber != -1) && (fRotateStationNumber != 999)) {
        bool ok = myMods.RotateAStation(fRotateStationNumber, fRotateStationBydPhi);
        if (!ok) {
          std::cerr << " GeometryService::preBeginRun logic problem in ModGDML, in rotations stop here " << std::endl;
          exit(2);
        }
        fNewFileNameStrStr << "_RotStation_" << fRotateStationNumber;
        fNewFileNameStrStr << "_" << static_cast<int>(1.0e3*fRotateStationBydPhi); 
         // in micro-degree, avoid the use of floating pts in filename.   
//    std::cerr << "  and quit now for good " << std::endl; exit(2);
      } else if (fRotateStationNumber == 999) {
//      std::cerr << " ... GeometryService::preBeginRun Move all the stations. Not implemented yet " << std::endl;
//      exit(2);
//      exit(2);
        double dPhi = 0.;
        for (short kSt=0; kSt != 8; kSt++) { // Valid only for Phase1c, late run ( 8 stations total) 
          if ((kSt == 4) || (kSt == 0)) dPhi = 0.;
          else dPhi = fRotateStationByWidthdPhi*2.0*(myRand->Uniform()-0.5); // in degrees. 
          bool ok = myMods.RotateAStation(kSt, dPhi);
          if (!ok) {
            std::cerr << " GeometryService::preBeginRun logic problem in ModGDML, in rotations stop here " << std::endl;
            exit(2);
          }
          fNewRotationStations.push_back(dPhi); 
        }
      }
        if ((fRotateStationNumber == 999) || (fMoveStationNumber == 999))
               fNewFileNameStrStr << "_" << std::string(uuidChar); 
        fNewFileNameStrStr << ".gdml";
        delete myRand;
    }
    void GeometryService::modifyBySensor(ModGDML &myMods) { 
    
      std::cerr << " GeometryService::modifyBySensor, start... " << std::endl; 
      TRandom3 *myRand = new TRandom3(fRandomSeed);
      
      uuid_t uuid;
      uuid_generate(uuid);
      char uuidChar[37];
      uuid_unparse(uuid, uuidChar); // We might not use it, if only one station moves. 
      size_t iPosPh = fGDMLFile.find("phase");
      std::string tmpName0 = fGDMLFile.substr(iPosPh);
      size_t iPosDot = tmpName0.find(".gdml");
      std::string tmpName1 = tmpName0.substr(0, tmpName0.size()-5);
      fNewFileNameStrStr.str("");
      fNewFileNameStrStr.clear();
      fNewFileNameStrStr << "./" << tmpName1;  
      for (size_t iSt=0; iSt != 8; iSt++) { // Valid only for Phase1c, late run ( 8 stations total)
        std::array<float, 2> shifts = {0., 0.};
	float dPhi = 0.; 
	  for (size_t jPl=0; jPl != fNPlanes[iSt]; jPl++) { 
	    short  nS = (iSt < 5) ? 1 : 2;
	    for (short kSe=0; kSe != nS; kSe++) { 
              if ((fMoveAllBySensors) || ((iSt != 4) && (iSt != 0))) { 
                shifts[0] = static_cast<float>(fMoveSensorByWidthX*2.0*(myRand->Uniform()-0.5)); 
                shifts[1] = static_cast<float>(fMoveSensorByWidthY*2.0*(myRand->Uniform()-0.5));
	        dPhi = static_cast<float>(fRotateSensorByWidthdPhi*2.0*(myRand->Uniform()-0.5));
		std::cerr << " .... station " << iSt << " Plane " << jPl << " sensor " << kSe << 
		             " shifts, X " << shifts[0] << " y " << shifts[1] << " dPhi " << dPhi << std::endl;
                bool ok = myMods.TranslateASensor(static_cast<short>(iSt), jPl, kSe, shifts[0], shifts[1]);
	        if (!ok) { std::cerr << " GeometryService::modifyBySensor failure to translate for station "
	  			 << iSt << " Plane " << jPl << " Sensor " << kSe << " ... fatal " << std::endl;
	        		 exit(2);
	              }
	        ok = myMods.RotateASensor(static_cast<short>(iSt), jPl, kSe, dPhi);
	        if (!ok) { std::cerr << " GeometryService::modifyBySensor failure to rotate for station "
	  			 << iSt << " Plane " << jPl << " Sensor " << kSe << " ... fatal " << std::endl;
	        		 exit(2);
	              }
	        }
                fNewCoordSensors.push_back(shifts);
	        fNewRotationSensors.push_back(dPhi);
	    } // on sensors 
	 } // on Planes 
      }// on Stations ..
      // Rotate the X sensor for station 0 ?  
      delete myRand;
    // Set the file name... 
      fNewFileNameStrStr << "_" << std::string(uuidChar); 
      fNewFileNameStrStr << ".gdml";
      std::cerr << " ... End of GeometryService::modifyBySensor, file name " << fNewFileNameStrStr.str() << std::endl;
    }
  }
}
