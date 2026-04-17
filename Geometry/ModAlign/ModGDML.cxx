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
#include <cfloat>
#include <cmath>
#include <cstdio>
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
      std::string xEqual("x="); std::string yEqual("y="); std::string zEqual("z=");
      bool didIt = false; 
      for (auto it=fData.begin(); it != fData.end(); it++) {
        if (it->find(keyPosStr) == std::string::npos) continue;
	float xOld, yOld, zOld;
	size_t iPosX = it->find(xEqual); iPosX +=3;
	std::istringstream xValStrStr(it->substr(iPosX)); xValStrStr >> xOld; 
	float xNew = xOld + x; 
	size_t iPosY = it->find(yEqual); iPosY +=3;
	std::istringstream yValStrStr(it->substr(iPosY)); yValStrStr >> yOld; 
	float yNew = yOld + y; 
	size_t iPosZ = it->find(xEqual); iPosX +=3;
	std::istringstream zValStrStr(it->substr(iPosZ)); zValStrStr >> zOld; 
	float zNew = zOld + z;
	std::cerr << " ModGDML::TranslateAStation, new position, X  " << xNew << " Y " << yNew << " Z " << zNew << std::endl;
	std::ostringstream newLineStrStr;
	newLineStrStr << "	<position name=\"ssdStation"<< iStation << "_pos\" x=\"" 
	              << xNew << "\" y=\"" << yNew << "\" z=\"" << zNew <<"+ssdStation" << iStation 
		      <<"_shift+ssdD0_thick-0.5*mount_thick\" unit=\"mm\" />";
	std::string newLineStr(newLineStrStr.str());
	*it = newLineStr; 
	didIt = true; 
      }
      return didIt;
    }
   bool  ModGDML::PushPullAStation(int iStation, float z) {
      float xS = 0.; float yS = 0.;
      return this->TranslateAStation(iStation, xS, yS, z);
    }
    
   bool ModGDML::TranslateASensor(int iStation, int iPlane, int iSensor, float x, float y) {
      std::cerr << " ModGDML::TranslateASensor, station " << iStation 
                << " iPlane " << iPlane << " sensor " << iSensor << " x " << x << " y " << y << std::endl;
      std::ostringstream keyPosStrStr;  
      keyPosStrStr<< "<position name=\"ssdsensor_" 
                                    << iStation << "_" << iPlane << "_" << iSensor << "_pos"; 
      std::string keyPosStr(keyPosStrStr.str());
      bool didIt = false; 
      for (auto it=fData.begin(); it != fData.end(); it++) {
        if (it->find(keyPosStr) == std::string::npos) continue;
	float xOrig= 0.; float yOrig=0.;
	size_t iPosX = it->find("x=") + 3; // one extra for the double quote 
	std::string  xValStr((*it).substr(iPosX)); 
	std::istringstream xValStrStr(xValStr); xValStrStr >> xOrig; 
	size_t iPosY = it->find("y=") + 3; // one extra for the double quote 
	std::string  yValStr(it->substr(iPosY)); 
	std::istringstream yValStrStr(yValStr); yValStrStr >> yOrig; 
	float xNew = x + xOrig; float yNew = y + yOrig;
	std::ostringstream newLineStrStr;
	size_t iPosZ = it->find(" z=\"");
	std::string endZStr(it->substr(iPosZ)); 
	newLineStrStr << "	<position name=\"ssdsensor_"<< iStation << "_" << iPlane << "_" << iSensor  
	              << "_pos\" x=\"" << xNew << "\" y=\"" << yNew << "\" " << endZStr; 
				    
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
	std::string oldValStr = it->substr(iPosZ + 3);
	std::istringstream oldValStrStr(oldValStr); float oldVal; oldValStrStr >> oldVal; 
	std::string preVal = it->substr(0, (iPosZ+3));
	float newVal = oldVal + phi;
//	std::cerr << " ... debugging oldval " << oldVal << " preval " << preVal << std::endl;
	newLineStrStr << preVal;
	newLineStrStr << std::setprecision(8) << newVal;
//	   std::cerr << " ... debugging new val " << newVal <<  " newLineStrStr "  << newLineStrStr.str() << std::endl;
	newLineStrStr << "\"  unit=\"deg\"/>";
	std::string newLineStr(newLineStrStr.str());
	*it = newLineStr; 
	didIt = true; 
      }
      return didIt;
    }
    bool ModGDML::RotateAStation(int iStation, float phi) {
      // valid only for phase1c.  We don't a geometry updated yet.. 
      std::array<int, 8> nPlanes = {2, 2, 3, 3, 2, 3, 3, 2};
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
   bool ModGDML::generateAlignedGDML(const std::string &fNameMilleRes, const std::string &newGDMLName) {
   
     std::ifstream fIn(fNameMilleRes.c_str()); 
     if (!fIn.is_open()) {
        std::cerr << " ModGDML::generateAlignedGDML file is not open, check file name " << std::endl;   
	return false;
     }
     int nl=0;
     fIn.seekg(0, std::ios::beg);// Should not be needed Logic error below.. but worth keeping arouns... 
     char aLineChar[4096];
     // first pass, we look at the highest param number.  I less than 101, the mode is "byStation" 
     // else
     int parNum=0; double parVal=DBL_MIN;
     int parNumMax = 0; 
     while (true) {
        fIn.getline(aLineChar, 4096);
        std::string aLineStr(aLineChar);
	if (nl == 0) {
	  if (aLineStr.find("Parameter") == std::string::npos) {
	    std::cerr << " ModGDML::generateAlignedGDML first line does not has the Parameter keyword." << std::endl; 
	    std::cerr << " ... File Name " <<  fNameMilleRes << std::endl;
	    std::cerr << " .... First line " << aLineStr << " so, failing.. " << std::endl; 
	    fIn.close();
	    return true;
	  }
	  nl++; continue;
	}
	if (!fIn.good()) break; // done 
        std::istringstream aLineStrStr(aLineStr); aLineStrStr >> parNum;
	parNumMax = std::max(parNum, parNumMax);
	nl++;
     }
     bool isByStation = (parNumMax < 101);
     //
     // Support also the original definition of parameters, by Jon and Robert.. 
     short iStationMax = parNumMax/1000; 
     short jPlaneMax = (parNumMax - iStationMax*1000)/100;
     short kSensorMax =  (parNumMax - iStationMax*1000 - jPlaneMax*100)/10;
     short lParamMax = (parNumMax - iStationMax*1000 - jPlaneMax*100 - kSensorMax*10);
     bool isZParam = (lParamMax == 4);
     std::cerr << " ModGDML::generateAlignedGDML ";
     if (isByStation) std::cerr << " byStation "; 
     else { if (isZParam) std::cerr << " BySensor, with Z Param included but ignored "; 
            else  std::cerr << " BySensor, three params only ";
	  }
     std::cerr << std::endl;
     fIn.close();
     fIn.open(fNameMilleRes.c_str());
     nl = 0;
     float dx=0.; float dy=0.; float dPhi=0.;
     int kPar, iSt, jPl, kSe; 
     while (true) {
        fIn.getline(aLineChar, 4096);
        std::string aLineStr(aLineChar);
	if (nl == 0) { nl++; continue; }
	if (!fIn.good()) break; // done 
        std::istringstream aLineStrStr(aLineStr); aLineStrStr >> parNum >> parVal;
	if (isByStation) {
	  iSt = (parNum)/10; kPar = ( parNum) - iSt*10;  
	} else {
	  iSt = parNum/1000; jPl = (parNum - iSt*1000)/100; 
	  kSe = (parNum - iSt*1000 - jPl*100)/10;  
	  kPar = parNum - iSt*1000 - jPl*100 - kSe*10; 
	}
	switch (kPar) {
	  case 1 :  { dx = parVal; break; }
	  case 2 :  { dy = parVal; break; }
	  case 3 :  { if (!isZParam ) dPhi = 180.0*parVal/M_PI; break; }
	  case 4 :  { dPhi = 180.0*parVal/M_PI; break; }
	  default : {
	    std::cerr << "  ModGDML::generateAlignedGDML  unexpected param number at line " 
	              << nl << " parNum " << parNum  << " line " << aLineStr << std::endl;
		      fIn.close(); 
		      return false; 
	  }
	}
	if (kPar != 3) { nl++; continue; } 
	if ((std::abs(dx) > 1.0e-4) || (std::abs(dy) > 1.0e-4)) { 
	  if (isByStation) this->TranslateAStation(iSt, dx, dy, 0.);
	  else this->TranslateASensor(iSt, jPl, kSe, dx, dy);
	}
	if (std::abs( dPhi) > 1.0e-7) {
	  if (isByStation) this->RotateAStation(iSt, dPhi);
	  else this->RotateASensor(iSt, jPl, kSe, dPhi);
	}
	nl++;
     }
     fIn.close();
     this->SaveIt(newGDMLName); 
     return true;
   } 
   
 } // name space geo 
} // name space emph 
