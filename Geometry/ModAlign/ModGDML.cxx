////////////////////////////////////////////////////////////////////////
/// \file    ModGDML.cxx
/// \brief   Class that save the GDML file into memory, implementation. 
/// \version 
/// \author  lebrun@fnal.gov, based on asuggestion of jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <array>
#include <iomanip>
#include "Geometry/ModAlign/ModGDML.h"

namespace emph {
  namespace geo {
    
    //----------------------------------------------------------------------
    
    ModGDML::ModGDML(const std::string &fileName) :
      fFileName(fileName) {
      
      if (fileName.length() < 2) return;
      std::ifstream fIn(fileName.c_str());
      if (!fIn.is_open()) {
        std::cerr << " ModGDML::ModGDML file is not open, check file name " << std::endl;   
	std::cerr << " ... should not happen! EMPHATIC can't run with a GDML file.. Quit here and now " << std::endl; 
	exit(2);
      }
      int nl=0;
      char aLineChar[4096];
      while (fIn.good()) {
       fIn.getline(aLineChar, 4096);
       std::string aLineStr(aLineChar);
       // Insert the full path name for the GDMLSchema
       if (nl == 1) {
         size_t iPos = aLineStr.find("GDMLSchema");
         if (iPos == std::string::npos) {
	   std::cerr << " ModGDML::ModGDML: The 2nd line does not contain the location of the GDMLSchema ? Fatal.. " << std::endl;
	   exit(2);
	 }
	 const char* env_p = std::getenv("CETPKG_SOURCE");
	 std::string envStr(env_p); envStr += std::string("/ConstBase/Geometry/");
	 aLineStr.insert(iPos,envStr); 
       }
       if (fIn.good()) fData.push_back(aLineStr);
       nl++;
      }
      fIn.close();   
   }
   //
   void ModGDML::SaveIt(const std::string &newFileName) const {
     std::ofstream fOut(newFileName.c_str());
     for (auto it = fData.cbegin(); it != fData.cend(); it++) fOut << (*it) << std::endl;
     fOut.close();
   }
   //
   bool  ModGDML::TranslateAStation(int iStation, float x, float y, float z) {
      std::ostringstream keyPosStrStr;  keyPosStrStr<< "<position name=\"ssdStation" << iStation << "_pos"; 
      std::string keyPosStr(keyPosStrStr.str());
      bool didIt = false; 
      for (auto it=fData.begin(); it != fData.end(); it++) {
        if (it->find(keyPosStr) == std::string::npos) continue;
	std::ostringstream newLineStrStr;
	newLineStrStr << "	<position name=\"ssdStation"<< iStation << "_pos\" x=\"" 
	              << x << "\" y=\"" << y << "\" z=\"" << z <<"+ssdStation" << iStation 
		      <<"_shift+ssdD0_thick-0.5*mount_thick\" unit=\"mm\" />";
	std::string newLineStr(newLineStrStr.str());
	*it = newLineStr; 
	didIt = true; 
      }
      return didIt;
    }
    ////
   bool  ModGDML::RotateASensor(int iStation, int iPlane, int iSensor, float phi) { // phi in degrees
      std::cerr << " ModGDML::RotateASensor, station " << iStation 
                << " iPlane " << iPlane << " phi " << phi << std::endl;
      std::ostringstream keyPosStrStr;  keyPosStrStr<< "<rotation name=\"ssdsensor_" 
       << iStation << "_" << iPlane << "_" << iSensor <<  "_rot"; 
      std::string keyPosStr(keyPosStrStr.str());
      bool didIt = false; 
      for (auto it=fData.begin(); it != fData.end(); it++) {
        size_t iPosKey = it->find(keyPosStr);
        if (iPosKey == std::string::npos) continue;
	std::ostringstream newLineStrStr; 
	size_t iPosZ = it->find(" z=\"") + 1; // + 1 for the double quote character. 
	std::string oldVal = it->substr(iPosZ);
	std::string preVal = it->substr(0, (iPosZ+3));
//	std::cerr << " ... debugging oldval " << oldVal << " preval " << preVal << std::endl;
	newLineStrStr << preVal;
	if (oldVal.find("0") == 3) newLineStrStr << std::setprecision(8) << phi;
	if (oldVal.find("45") == 3)  
	 { float newVal = 45.+ phi; newLineStrStr << std::setprecision(8) << newVal; }
	if (oldVal.find("90") == 3) 
	 { float newVal = 90. + phi; newLineStrStr << std::setprecision(8) << newVal; }
	if (oldVal.find("180") == 3) 
	 { float newVal = 180. + phi; newLineStrStr << std::setprecision(8) << newVal; }
	if (oldVal.find("225") == 3) 
	 { float newVal = 225. + phi; newLineStrStr << std::setprecision(8) << newVal; }
	if (oldVal.find("270") == 3) 
	 { float newVal = 270. + phi; newLineStrStr << std::setprecision(8) << newVal; }
	if (oldVal.find("315") == 3) 
	 { float newVal = 315. + phi;
	   newLineStrStr << std::setprecision(8) << newVal;
//	   std::cerr << " ... debugging new val " << newVal <<  " newLineStrStr "  << newLineStrStr.str() << std::endl;
	 }
	newLineStrStr << "\"  unit=\"deg\"/>";
	std::string newLineStr(newLineStrStr.str());
	*it = newLineStr; 
	didIt = true; 
      }
      return didIt;
    }
    bool ModGDML::RotateAStation(int iStation, float phi) {
      // valid only for phase1c.  We don't a geometry updated yet.. 
      std::array<int, 8> nPlanes = {2, 2, 3, 3, 2, 3, 3, 3};
      if (iStation < 5) { // one sensor per plane.. 
        for (int kPl = 0; kPl != nPlanes[iStation]; kPl++) {
	  if (!RotateASensor(iStation, kPl, 0, phi))  {
	    std::cerr << " ModGDML::RotateAStation, problem incorrect script .. need debugging.. fatal " << std::endl;
	    exit(2);
	  }
        }
      } else {
        for (int kPl = 0; kPl != nPlanes[iStation]; kPl++) {
	  for (int iSensor=0; iSensor !=2; iSensor++) { 
	    if (!RotateASensor(iStation, kPl, iSensor, phi))  {
	      std::cerr << " ModGDML::RotateAStation, problem incorrect script for station " 
	              << iStation << "... need debugging.. fatal " << std::endl;
	      exit(2);
	    }
	  }
       }
     }
     return true;
   }
 } // name space geo 
} // name space emph 
