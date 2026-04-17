////////////////////////////////////////////////////////////////////////
/// \file    MilleRun.cxx
/// \brief   Class and contaner to save a Mille record..  This is mostly to avoid having too much code in 
///          in the SingleTrackAlignmentV2 module. 
/// \version 
/// \author  lebrun@fnal.gov, to create interface c++ ->c -> f90.. 
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <iomanip>
#include <cfloat>
#include "TVector3.h"
#include "Alignment/MilleRecord.h"
#include "Alignment/MilleRun.h"
#include "Alignment/MilleResult.h"
#include "millepede_ii/Mille.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h" 

emph::align::MilleRun::MilleRun(bool byStation, int mode, const std::string &token ) :
  byStations_(byStation), fixRotation0X_(false), fixRotation0Y_(false), fixRotation4X_(false), fixRotation4Y_(false), 
  maxStation_(999), mode_(mode), phase_(1), scaleErrors_(1.), fNameSteerFile_(""), 
  token_(token),
  fNameBinary_("m0043_") // prefix is purely arbitray. 
   {
  
    ; 
  
  }
  
bool emph::align::MilleRun::doIt(emph::align::MilleRecords &myMrs, 
                    const bool fixStationRotations,  const int fixAllStationPosButOne, const int fixAllStationPosRotButOne, 
                    const int fixAllFreeOneRotStation, int newMode)  { 
		    
   if (newMode != -1) mode_ = newMode;
   
   allResults_.clear();
   
   std::cerr << " emph::alignMilleRun::doIt, start with mode " << newMode << std::endl;
    
   std::string aNameSteer("./steer_");
   
   fNameBinary_ += token_+ std::string("_mode_") + std::to_string(mode_) + std::string(".bin");
  
   aNameSteer += token_ + std::string("_mode_") + std::to_string(mode_);
   if (mode_ == 10) aNameSteer += std::string("_phase1");
   aNameSteer += std::string(".txt");
    
   maxStation_ = 0;
   for(auto it=myMrs.CBegin(); it!= myMrs.CEnd(); it++)  maxStation_ = std::max(maxStation_, it->station_); 
   std::cerr << " .... maxStation is " << maxStation_ << std::endl;
   fNameSteerFile_ = aNameSteer;
   
   this->composeAndWriteSteerFile(aNameSteer, fixStationRotations, fixAllStationPosButOne, fixAllStationPosRotButOne, 
                                  fixAllFreeOneRotStation);
  // 
   if (mode_ == 1) {
     std::cerr << " ... Shifting the pulls...Saving the average values  " << std::endl; 
     roughPulls_.clear(); 
     for(short iSt=0; iSt != maxStation_+1; iSt++) {
      // waste of time, as many station don't have have many sensors.. but cleaner code..  
       for (short jPl=0; jPl !=3; jPl++) {
         for(short kS=0; kS !=2; kS++) { 
	   const double meanPull = myMrs.CenterPulls(iSt, jPl, kS); // shift the pulls as well 
	   if (meanPull != DBL_MIN) {
	    roughPulls_.insert(std::pair<short, double>((iSt*1000 + jPl*100 + 10*kS), meanPull)); 
	   }
	 }
       }
     }
   }
   myMrs.SaveIt(byStations_, fNameBinary_); 
          	// Run pede..
		
   bool didRun = this->runItNow(aNameSteer); 
    // 
   bool gotResNow = this->fillResult();
   
   if (!gotResNow) { std::cerr << "  No result file, we are done! " << std::endl; return false; } 
   
  //  check
  
   for ( auto it=allResults_[0].CBeginSt(); it != allResults_[0].CEndSt(); it++) 
      std::cerr << " Parameter " << it->glVar_ << " value " << it->val_ << " +- " << it->err_ << std::endl;
    
   if (mode_ > 9) { 
     std::cerr << " .... mode is " << mode_ << " must generate a new GDML, ro keep running.. "  << std::endl; 
     art::ServiceHandle<emph::geo::GeometryService> geo;
     std::string fNameOldRef =  geo->GeoRef()->GDMLFile();
     std::cerr << " Requiring further iterations, check fNameOldRef name " << fNameOldRef << std::endl;
     emph::geo::ModGDML myMod(fNameOldRef);
     std::ostringstream newGDMLFileNameStrStr; 
     newGDMLFileNameStrStr << "./phase_1c_" << token_ << "_mode" << mode_<< ".gdml";
     std::string newGDMLFileNameStr(newGDMLFileNameStrStr.str());
     std::string aNameMille("./millepede.res"); 
     myMod.generateAlignedGDML(aNameMille, newGDMLFileNameStr); 
    } 
    //
    // Create a new geometry file...and/or Analyze the fit results. 
    //
    return gotResNow;
  } 
  // 
  bool emph::align::MilleRun::runItNow(const std::string &aNameSteer) { 
    const char* env_PEDELib = std::getenv("MILLEPEDE_II_LIB"); // assume the find_package worked.. 
    std::string pathToPede(env_PEDELib); 
    std::string libStr("lib"); std::string binStr("bin");
    size_t iPosLib = pathToPede.find("lib");
    pathToPede.replace(iPosLib, 3, binStr, 0, 3);
    pathToPede += std::string("/pede");
   // 
    std::string cmdPede(pathToPede); cmdPede += std::string(" ") + aNameSteer;
    std::cerr << " .... Now trying to run pede, cmd is " << cmdPede << std::endl;
    int returnCodePede = std::system(cmdPede.c_str());
    std::cerr << " ... back emph::align::MilleRun::RunItNow, return code from pede " << returnCodePede << std::endl;
    // Do we 
    std::ifstream fRes("./millepede.res");
    bool gotResNow = fRes.good();
    fRes.close();
    return gotResNow;
  }
  //
  // The old algorithm.. 
 bool emph::align::MilleRun::doIt4Param( int newMode)  { 

   if (byStations_) {
     std::cerr << " Inconsistent definition of emph::align::MilleRun, 4Param case always by Sensors.. " << std::endl;
     exit(2);
   }
		    
   if (newMode != -1) mode_ = newMode;
   
   allResults_.clear();
   
   std::cerr << " emph::alignMilleRun::doIt4Param, start with mode " << newMode << std::endl;
   
   const std::string inputSteerDir = getenv("CETPKG_SOURCE");
   std::string aNameSteer(inputSteerDir); aNameSteer+= std::string("/Alignment/mp2/steer_");
   if (newMode == 400) aNameSteer += std::string("WithScaleError_all.txt");
   else if (newMode == 401) aNameSteer += std::string("WithScaleError_all.txt");
   else if (newMode == 402) aNameSteer += std::string("WithSmallScaleError_all.txt");
   else aNameSteer += std::string("NoScaleError_all.txt");
   
   
   fNameBinary_ = std::string("m004.bin");
  
          	// Run pede..
		
   bool didRun = this->runItNow(aNameSteer); 
    // 
   bool gotResNow = this->fillResult(true);
   
   if (!gotResNow) { std::cerr << "  No result file, we are done! " << std::endl; return false; } 
   
  //  check
  
   for ( auto it=allResults_[0].CBeginSe4P(); it != allResults_[0].CEndSe4P(); it++) 
      std::cerr << " Parameter " << it->glVar_ << " value " << it->val_ << " +- " << it->err_ << std::endl;
      
   if (mode_ > 9) { 
     std::cerr << " .... mode is " << mode_ << " must generate a new GDML, to keep running.. "  << std::endl; 
     art::ServiceHandle<emph::geo::GeometryService> geo;
     std::string fNameOldRef =  geo->Geo()->GDMLFile(); // the geometry on which the pulls is defined now becomes the reference geomtry. 
     std::cerr << " Requiring further iterations, check fNameOldRef name " << fNameOldRef << std::endl;
     emph::geo::ModGDML myMod(fNameOldRef);
     std::ostringstream newGDMLFileNameStrStr; 
     newGDMLFileNameStrStr << "./phase_1c_" << token_ << "_mode" << mode_<< ".gdml";
     std::string newGDMLFileNameStr(newGDMLFileNameStrStr.str());
     std::string aNameMille("./millepede.res"); 
     myMod.generateAlignedGDML(aNameMille, newGDMLFileNameStr); 
    } 
    //
    // Create a new geometry file...and/or Analyze the fit results. 
    //
    return gotResNow;
  } 
  // 
 
  void emph::align::MilleRun::composeAndWriteSteerFile(const std::string &aNewFNameSteer,   const bool fixStationRotations,  
                const int fixAllStationPosButOne, const int fixAllStationPosRotButOne, const int fixAllFreeOneRotStation) {
    std::cerr << " emph::align::MilleRun::composeAndWriteSteerFile, doing it by  ";
    if (byStations_) std::cerr << "Stations "; else std::cerr << " Sensors "; 
    std::cerr << std::endl;
    std::ofstream fOut(aNewFNameSteer.c_str());
    fOut << "Cfiles" << std::endl;
    fOut << fNameBinary_ << std::endl;
    fOut << " entries 50 50 " << std::endl;
    fOut << " method inversion 3000 6.! " << std::endl;
//     fOut << "method fullGMRES 30 0.5" << std::endl;
    fOut << " chisqcut 3000. 600. " << std::endl; 
    fOut << " printcounts " << std::endl; 
    fOut << " monitorresiduals " << std::endl;
    if (std::abs(scaleErrors_ - 1) > 0.0001)  fOut << " scaleerrors " << scaleErrors_ << std::endl;
    bool introFixing = false;
    if (fixStationRotations) { // valid only for station rotations..  
      introFixing = true;
      fOut << "parameter " << std::endl;
      for (short ii=1; ii < maxStation_+1; ii++){
         if (ii == 4) continue;
	 if (byStations_) { 
           fOut << (10*ii + 3) << "  0.   -1. " << std::endl; // To upgrade, for a fixed value... 
	 } else {
	   std::cerr << " emph::align::MillRun::composeAndWriteSteerFile, fix Station rotation for sensor not implemented yet, fatal " << std::endl;
	   exit(2);
	 }
     }
   } 
   if (fixAllStationPosButOne != -1) { // valid only for station rotations.. 
      if (!introFixing) { 
        fOut << "parameter " << std::endl;
        introFixing = true;
      }
      for (short ii=1; ii< maxStation_+1; ii++){
 	if (ii == 4) continue;
        if (fixAllStationPosButOne == ii) continue;
        for (int kDir=1; kDir !=3; kDir++) { 
	  if (byStations_) { 
	    fOut << (10*ii + kDir) << "  0.   -1. " << std::endl;
	  } else {
	    std::cerr << " emph::align::MillRun::composeAndWriteSteerFile, fix Station rotation for sensor not implemented yet, fatal " << std::endl;
	    exit(2);
	  }
        }
      }
    } 
    if (fixAllStationPosRotButOne != -1) { // valid only for station rotations.. 
      if (!introFixing) { 
        fOut << "parameter " << std::endl;
        introFixing = true;
      }
      for (short ii=1; ii < maxStation_+1; ii++){
 	if (ii == 4) continue;
        if (fixAllStationPosRotButOne == ii) continue;
        for (int kDir=1; kDir !=4; kDir++) { 
	  if (byStations_) { 
	    fOut << (10*ii + kDir) << "  0.   -1. " << std::endl;
	  } else {
	   std::cerr << " emph::align::MillRun::composeAndWriteSteerFile, fix Station rotation for sensor not implemented yet, fatal " << std::endl;
	   exit(2);
	  }
        }
      } 
    } 
    if (fixAllFreeOneRotStation != -1) { // valid only for station rotations.. 
      if (!introFixing) { 
        fOut << "parameter " << std::endl;
        introFixing = true;
      }
      for (short ii=1; ii< maxStation_+1; ii++){
 	if (ii == 4) continue;
        for (short kDir=1; kDir !=4; kDir++) { 
          if ((kDir == 3) && (fixAllFreeOneRotStation == ii)) continue;
	  if (byStations_) { 
	    fOut << (10*ii + kDir) << "  0.   -1. " << std::endl;
	  } else {
	   std::cerr << " emph::align::MillRun::composeAndWriteSteerFile, fix Station rotation for sensor not implemented yet, fatal " << std::endl;
	   exit(2);
	  }
        }
      }
    }
    if (!byStations_) {
    // Unmeasured parameters, might as well fix them.. 
      for (short iSt = 1; iSt != 8; iSt++) {
          if (iSt == 4) continue;
          if (!introFixing) { 
            fOut << "parameter " << std::endl;
            introFixing = true;
         }
	 int iPar1=0; int iPar2=0; int iPar3=0; int iPar4=0;
         switch (iSt) {
	   case 1 : { iPar1 = 1001; iPar2 = 1102; break;} 
	   case 2 : { iPar1 = 2101; iPar2 = 2202; break; } 
	   case 3 : { iPar1 = 3101; iPar2 = 3202; break;} 
	   case 5 : { iPar1 = 5002; iPar2 = 5012; iPar3 = 5101; iPar4 = 5111; break;} 
	   case 6 : { iPar1 = 6002; iPar2 = 6012; iPar3 = 6101; iPar4 = 6111; break;} 
	   case 7 : { iPar1 = 7002; iPar2 = 7012; iPar3 = 7101; iPar4 = 7111; break;} 
	 }
	 fOut << iPar1 << "  0.   -1. " << std::endl; 
	 fOut << iPar2 << "  0.   -1. " << std::endl;
	 if (iSt > 4) { 
	   fOut << iPar3 << "  0.   -1. " << std::endl; 
	   fOut << iPar4 << "  0.   -1. " << std::endl;
	 } 
      }
      // problems with Y derivatives ??? This should be not done.. Debugging needed 
      std::array<int, 8> supiciousParams = {2002, 3002, 5205, 5202, 5212, 6201, 6202, 6212}; 
      for (size_t k=0; k != supiciousParams.size(); k++)
         fOut << supiciousParams[k] << "  0.   -1. " << std::endl; 
      if (fixRotation0Y_) {
         if (!introFixing) { 
          fOut << "parameter " << std::endl;
          introFixing = true;
         }
         fOut << "3  0.   -1. " << std::endl;
      }
      if (fixRotation0X_) {
         if (!introFixing) { 
          fOut << "parameter " << std::endl;
          introFixing = true;
         }
         fOut << "103  0.   -1. " << std::endl;
      }
      if (fixRotation4Y_) {
         if (!introFixing) { 
          fOut << "parameter " << std::endl;
          introFixing = true;
         }
         fOut << "4003  0.   -1. " << std::endl;
      }
      if (fixRotation4X_) {
         if (!introFixing) { 
          fOut << "parameter " << std::endl;
          introFixing = true;
         }
         fOut << "4103  0.   -1. " << std::endl;
      }
    }
    fOut.close();	
  }
 //
 void emph::align::MilleRun::editSteerFile( const bool fixStationRotations,  
                 const int fixAllStationPosButOne, const int fixAllStationPosRotButOne, 
                const int fixAllFreeOneRotStation) {
		 // too many sed command sto compose.. simple to write the damm files from scratch... 
		 std::cerr << " emph::align::MillRun::editSteerFile, obsolete, quit here and now " << std::endl; exit(2);
}
// 
bool emph::align::MilleRun::fillResult(bool is4P) {  

  std::ifstream fIn("./millepede.res"); 
  if (!fIn.good()) return false; 
  fIn.close();

  std::string cmdCp1("cp ./millepede.res ");
  std::string aNameMille("./millepede_"); 
  aNameMille +=  token_ + std::string("mode_") + std::to_string(mode_) + 
                      std::string("_phase_") + std::to_string(phase_) + std::string(".res");
  cmdCp1 += aNameMille;
  std::system(cmdCp1.c_str());
  
  const emph::align::MilleResult aRes(byStations_, aNameMille, is4P); 
  
  allResults_.push_back(aRes);
  return true;  
 
} 
    
    
