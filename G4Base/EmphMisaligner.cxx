////////////////////////////////////////////////////////////////////////
/// \file  EmphMisaligner.h
/// \brief Use Geant4 to run the LArSoft detector simulation
///
///   Valid only for the Phase1b.gdml!. 
/// \version $Id: EmphMisaligner.cxx,v 1.20 2012-12-03 23:29:49 rhatcher Exp $
/// \author  seligman@nevis.columbia.edu, brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "G4Base/EmphMisaligner.h"

namespace g4b{

  //------------------------------------------------
  // Constructor
  EmphMisaligner::EmphMisaligner(const std::string &fNameIn, unsigned int aSeed)
  : fModelNumber(0), fGapDoubleSSD(3.0)
  {
   this->readIt(fNameIn.c_str());
   srand(aSeed);
  }
  //------------------------------------------------
  void EmphMisaligner::readIt(const char* fName) 
  {
    std::ifstream fIn(fName);
    if (!fIn.is_open()) {
       std::cerr << "g4b::EmphAligner, could not find and/or open file " << std::string(fName) 
                 << " fatal, quit here.. " << std::endl; exit(2);
   
    }
    int nLines = 0;
    char aLine[2048];
    while (fIn.good()) { 
      fIn.getline(aLine, 2048);
      nLines++;
      std::string ll(aLine);
      fLines.push_back(ll); // to avoid creating an extra blank line 
    }
    // Remove the last line, it is blank. 
    fLines.pop_back();
    std::cerr << "g4b::EmphAligner, read " << fLines.size() << " from file " << std::string(fName) << std::endl;     
    fIn.close();
  }
  //
  void EmphMisaligner::writeIt(const char* fName) const {
    std::ofstream fOut(fName);
    for (std::vector<std::string>::const_iterator it=fLines.cbegin(); it!=fLines.cend(); it++) {
      fOut << (*it) << std::endl;
    }
    fOut.close();
  }
  //------------------------------------------------
  void EmphMisaligner::doIt(int aModelNum, double gapD) {
    fModelNumber = aModelNum; 
    // implement stwitched based on a simple cnvention based on the model. 
    const long int iSigmaRolls =  fModelNumber/(100*100*100);
    const long int iSigma2 =  (fModelNumber - 100*100*100*iSigmaRolls);
    const long int iSigmaYawPitch =  iSigma2/(100*100);
    const long int iSigma3 =  (iSigma2 - 100*100*iSigmaYawPitch);
    const long int iSigmaZ =  iSigma3/(100);
    const long int iSigma4 =  iSigma3 - 100*iSigmaZ;
    const long int iSigmaTr =  iSigma4;
   
    const double sigZ =  static_cast<double>(iSigmaZ); // now in mm. Possible range from 0 to 99 mm, or ~10 cm offset (unlikely)
    // Note: we still do need to implement the gap, even if the translations are very small. 
    // The case where there is no gap and no translation is faulty, but unphysical 
    //
    const double sigTr = std::max(0.01, (0.1* static_cast<double>(iSigmaTr))); // maximum is 9.9 mm, sigma. 
    const double sigRolls =  0.1*static_cast<double>(iSigmaRolls); // now in degrees. 
    const double sigYP =  0.1*static_cast<double>(iSigmaYawPitch); // now in degrees. 
    std::cerr << " EmphMisaligner::doIt, sigZ " << sigZ << " sigTr " << sigTr 
              << " SigYP " << sigYP << " sigRolls " << sigRolls << std::endl;
    this->doSSDRolls(sigRolls);
    this->doSSDYawPitchOnStations(sigYP);
    this->doSSDZTranslationOnStations(sigZ); // on stations only
    this->doSSDTransOffsetOnPlanes(sigTr, gapD); // on individual planes. Could induce volume overlaps! 
  }
  double EmphMisaligner::getRandomShift(double sig) {
    const double sign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
    const double val = sign * (static_cast<double>(rand())/RAND_MAX ) * sig; 
    return val;
  }
  double EmphMisaligner::getValue(const std::string &line, const std::string &key) {
     const size_t iPosValue = line.find(key);
    if (iPosValue == std::string::npos) {
      std::cerr << " EmphMisaligner::getValue, internal error or corrupted Phase1.gdml file, at line "
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    const size_t iPosbV = iPosValue + 1 + key.length();
    const std::string valStrAll = line.substr(iPosbV);
    const size_t iPoseV = valStrAll.find("\"");
    if (iPoseV == std::string::npos) {
      std::cerr << " EmphMisaligner::getValue, corrupted Phase1.gdml file, no good value at line   " 
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    std::string valStr = valStrAll.substr(0, iPoseV);
    double zz = 0.; std::istringstream valStrStr(valStr); valStrStr >> zz;
    return zz;
  }
  std::string EmphMisaligner::getName(const std::string &line) {
     const size_t iPosName = line.find("name=");
    if (iPosName == std::string::npos) {
      std::cerr << " EmphMisaligner::getName, internal error or corrupted Phase1.gdml file, at line "
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    const size_t iPosbV = iPosName + 6;
    const std::string nameStrAll = line.substr(iPosbV);
    const size_t iPoseV = nameStrAll.find("\"");
    if (iPoseV == std::string::npos) {
      std::cerr << " EmphMisaligner::getName, corrupted Phase1.gdml file, no good value at line   " 
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    std::string valStr = nameStrAll.substr(0, iPoseV);
    return valStr;
  }
  void EmphMisaligner::doSSDZTranslationOnStations(double sigmaZShifts) {
    if (std::abs(sigmaZShifts) < 1.0e-6) return;
    for (size_t kSt=0; kSt != 5; kSt++) {
     std::ostringstream aNameStrStr; aNameStrStr << "ssdStation" << kSt << "_shift";
     const std::string aNameStr(aNameStrStr.str());
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
        std::string origStr(*il);
        const size_t iPosType = origStr.find("<quantity");
	if (iPosType == std::string::npos) continue;
        const size_t iPosName = origStr.find(aNameStr);
	if (iPosName == std::string::npos) continue;
	double zPos = this->getValue(origStr);
	const double zPosDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	const double zPosDelta = zPosDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigmaZShifts; 
	// Flat distribution, more realistic. 
	std::cerr << " ... For station " << kSt << " ZPos Orig " << zPos << " Shift will be " << zPosDelta << std::endl;
	zPos += zPosDelta;
	std::ostringstream lStrStr;
	lStrStr <<  origStr.substr(0, iPosType) << "<quantity name=\"" 
	                           << aNameStr << "\" value=\"" << zPos << "\" unit=\"mm\"/>";
	std::string lStr(lStrStr.str()); 
	*il = lStr;			   
     }
    } // on station. 
  
  }
  void EmphMisaligner::doSSDRolls(double sigRolls) {
    // temporary.. 
    if (std::abs(sigRolls)  < 1.0e-6) return;
    //
    std::cerr << " EmphMisaligner::doSSDRolls with sigRolls " << sigRolls << std::endl;
    const std::string zKey("z=");
    std::vector<std::string>::iterator ilTrueBeg = fLines.begin(); 
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
      std::string origStr(*il);
      if ((origStr.find("rotation name=\"ssdsingle") != std::string::npos) || 
          (origStr.find("rotation name=\"ssdrotate") != std::string::npos)) {
	ilTrueBeg = il;
//	std::cerr << " ... line " << origStr << std::endl;
        double nomRot = this->getValue(origStr, zKey);
	std::string name = this->getName(origStr);
	const double thRollDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	const double thRollDelta = thRollDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigRolls; 
	const double newRot = nomRot + thRollDelta;
	std::ostringstream llStrStr; llStrStr << "            " 
	        << "<rotation name=\"" << name << "\" z=\"" << newRot  << "\" unit=\"deg\"/> ";
	std::string llStr(llStrStr.str());
	*il = llStr;
      }
    }
    std::cerr << " .............2nd phase, double sensors " << std::endl;
     // Assume now the double is a bit more messy.. Tedious 
     // We will assume that the roll is identical for both sensor located at the same Z 
    for (std::vector<std::string>::iterator il0 = ilTrueBeg; il0 != fLines.end(); il0++) {
      const std::string orig0Str(*il0);
      if (orig0Str.find("rotation name=\"ssddouble") != std::string::npos) {
//	std::cerr << " ... loop 0 line " << orig0Str << std::endl;
        double nomRot0 = this->getValue(orig0Str, zKey);
	std::string name0 = this->getName(orig0Str);
	const double thRollDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	const double thRollDelta = thRollDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigRolls; 
	const double newRot = nomRot0 + thRollDelta;
	std::ostringstream ll0StrStr; ll0StrStr << "            " 
	        << "<rotation name=\"" << name0 << "\" z=\"" << newRot  << "\" unit=\"deg\"/> ";
	std::string ll0Str(ll0StrStr.str());
	*il0 = ll0Str;
	std::vector<std::string>::iterator ilNext = il0; ilNext++;
	// Assume here that the matching sensor is on the next line. !!! 
        const std::string orig1Str(*ilNext);
        if (orig0Str.find("rotation name=\"ssddouble") == std::string::npos) continue; // 
        double nomRot1 = this->getValue(orig1Str, zKey);
	const double newRot1 = nomRot1 + thRollDelta;
	std::string name1 = this->getName(orig1Str);
	std::ostringstream ll1StrStr; ll1StrStr << "            " 
	        << "<rotation name=\"" << name1 << "\" z=\"" << newRot1  << "\" unit=\"deg\"/> ";
	std::string ll1Str(ll1StrStr.str());
	*ilNext = ll1Str;
	il0++; // need to skip to the next one. 
	// 
      } // got name match on double0 
    }// on il0.   
  }
  void EmphMisaligner::doSSDYawPitchOnStations(double sigYP) {
    if (std::abs(sigYP)  < 1.0e-4) return;
    std::cerr << " EmphMisaligner::doSSDYawPitchOnStations Rotate the stations, Yaw and Pitch, isgYP  " 
              << sigYP << " degrees " <<  std::endl;
    // Here we add the rotation matrices for the 6 stations. 
    // Search for the 1rst instance of "ABOVE IS FOR SSD"
    std::string blankStr(""); blankStr.clear();
    int nl = 0;
    bool didIt = false;
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++, nl++) {
        std::string origStr(*il);
	if (origStr.find("ABOVE IS FOR SSD") != std::string::npos) {
	  std::cerr << " ... Found the place to add the new rotation matrices, line  " << nl << std::endl 
	            << " line " << origStr << std::endl;
	  std::vector<std::string>::iterator ilAdd = il; ilAdd++;
	  ilAdd = fLines.insert(ilAdd, blankStr); ilAdd++;
	  std::string aComment("          <!-- Not Quite, Adding Yaw and Pitch for station  -->");
	  ilAdd = fLines.insert(ilAdd, aComment); ilAdd++;
	  ilAdd = fLines.insert(ilAdd, blankStr); ilAdd++;
          for (size_t kSt=0; kSt != 5; kSt++) {
	    const double thPitchDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	    const double thPitchDelta = thPitchDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigYP; 
	    const double thYawDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	    const double thYawchDelta = thYawDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigYP; 
	    std::ostringstream llStrStr; llStrStr << "            " 
	        << "<rotation name=\"SSDStation_" << kSt << "_physMisRot\" x=\"" << thPitchDelta 
		<< "\" y=\"" <<  thYawchDelta << "\" unit=\"deg\"/> ";
	    std::string llStr(llStrStr.str());
	    ilAdd = fLines.insert(ilAdd, llStr); ilAdd++;
	    	
	  }
	  ilAdd = fLines.insert(ilAdd, blankStr); ilAdd++;
	  didIt = true;
	  break; // done for this phase 
	}
        if (didIt) break; // should not be needed !
    }
    // Now use these new quatities.
    for (int kSt=0; kSt != 5; kSt++) {  
      for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++, nl++) {
        std::string origStr(*il);
	std::ostringstream keyStrStr; keyStrStr << "positionref ref=\"ssdStation" << kSt << "_pos";
	std::string keyStr(keyStrStr.str());
	if (origStr.find(keyStr) != std::string::npos) {
	  std::vector<std::string>::iterator ilAdd = il; ilAdd++;
	  std::ostringstream keyIStrStr; keyIStrStr 
	    << "                         <rotationref ref=\"SSDStation_" << kSt << "_physMisRot\"/>";
	  std::string keyIStr(keyIStrStr.str());
	  ilAdd = fLines.insert(ilAdd, keyIStr); ilAdd++; 
          break;
        }
      }
    }
  }
  void EmphMisaligner::doSSDTransOffsetOnPlanes(double sigmaTrShifts, double dGap) {
//    if (std::abs(sigmaTrShifts) <  1.0e-4) return;  // No we always do it, there is the gap. 
    if (std::abs(dGap) > 30.) return; // unrealistic 
    std::cerr << " EmphMisaligner::doSSDTransOffsetOnPlanes, sigmaTrShifts " << sigmaTrShifts << " dGap " << dGap << std::endl;
    std::vector<std::string> keyPos; // to store the neame we will be replace in the definition of physical voume for complete station. 
    // First step, declare new positions. Tedious, one line at a time.. 
    // We keep the nominal line, We some time have to add line... Code bloat, ugly, but it should work.. 
    keyPos.push_back("ssdsingle00");
    std::string posPos("_pos");
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[0]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr;  aNew0StrStr 
	    << "                <position name=\"" << keyPos[0] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"0.\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       // Now we create a second instance, as we have two indepedent station with the same geometry. 
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr 
	    << "                <position name=\"" << keyPos[0] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"0.\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    std::cerr << " ...Done ssdsingle00 " << std::endl;
    // Shamelessly near cloning
     keyPos.push_back("ssdsingle10");
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[1]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr;  aNew0StrStr 
	    << "                <position name=\"" << keyPos[1] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr 
	    << "                <position name=\"" << keyPos[1] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    // That was for station ssdStationsingle0, and ssdStationsingle0 Now the rotated station 1 
     keyPos.push_back("ssdrotate00");
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[2]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr;  aNew0StrStr 
	    << "                <position name=\"" << keyPos[2] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"0.\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr 
	    << "                <position name=\"" << keyPos[2] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"0.\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    // again
     keyPos.push_back("ssdrotate10");
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[3]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr;  aNew0StrStr 
	    << "                <position name=\"" << keyPos[3] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr 
	    << "                <position name=\"" << keyPos[3] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
     // And again
     keyPos.push_back("ssdrotate20");
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[4]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr;  aNew0StrStr 
	    << "                <position name=\"" << keyPos[4] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"ssd3plane_shift\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr 
	    << "                <position name=\"" << keyPos[4] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"ssd3plane_shift\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    // Now, similar for double wafer, but must include the gap.   Twice as long..  
    keyPos.push_back("ssddouble00");
    double ssddouble00XShift0 = 0.; double ssddouble00XShift1 = 0.;
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[5]+posPos) == std::string::npos) continue;
       const double xShift0 = dGap + this->getRandomShift(sigmaTrShifts); ssddouble00XShift0 = xShift0 - dGap; 
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr; aNew0StrStr.setf(std::ios_base::showpos);  aNew0StrStr 
	    << "                <position name=\"" << keyPos[5] << "Mis0_pos\" x =\"0.5*ssdD0_height" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"0.\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = dGap + this->getRandomShift(sigmaTrShifts); ssddouble00XShift1 = xShift1 - dGap; 
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr.setf(std::ios_base::showpos); aNew1StrStr 
	    << "                <position name=\"" << keyPos[5] << "Mis1_pos\" x =\"0.5*ssdD0_height" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"0.\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    // ssddouble01
    keyPos.push_back("ssddouble01");
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[6]+posPos) == std::string::npos) continue;
       const double xShift0 = -1.0*dGap + ssddouble00XShift0;
       const double yShift0 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew0StrStr;  aNew0StrStr.setf(std::ios_base::showpos); aNew0StrStr 
	    << "                <position name=\"" << keyPos[6] << "Mis0_pos\" x =\"-0.5*ssdD0_height" 
	    << xShift0 << "\" y=\"" << yShift0 << "\" z=\"0.\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 =  -1.0*dGap + ssddouble00XShift1;
       const double yShift1 = this->getRandomShift(sigmaTrShifts);
       std::ostringstream aNew1StrStr;  aNew1StrStr.setf(std::ios_base::showpos); aNew1StrStr 
	    << "                <position name=\"" << keyPos[6] << "Mis1_pos\" x =\"-0.5*ssdD0_height" 
	    << xShift1 << "\" y=\"" << yShift1 << "\" z=\"0.\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
   // ssddouble10
    keyPos.push_back("ssddouble10");
    double ssddouble10YShift0 = 0.; double ssddouble10YShift1 = 0.;
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[7]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = -1.0*dGap + this->getRandomShift(sigmaTrShifts); ssddouble10YShift0 = yShift0 + dGap;
       std::ostringstream aNew0StrStr;  aNew0StrStr.setf(std::ios_base::showpos); aNew0StrStr 
	    << "                <position name=\"" << keyPos[7] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"-0.5*ssdD0_height" << yShift0 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = -1.0*dGap + this->getRandomShift(sigmaTrShifts); ssddouble10YShift1 = yShift1 + dGap;
       std::ostringstream aNew1StrStr;  aNew1StrStr.setf(std::ios_base::showpos); aNew1StrStr 
	    << "                <position name=\"" << keyPos[7] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"-0.5*ssdD0_height" << yShift1 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
   // ssddouble11 
    keyPos.push_back("ssddouble11");
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[8]+posPos) == std::string::npos) continue;
       const double xShift0 = this->getRandomShift(sigmaTrShifts);
       const double yShift0 = dGap + ssddouble10YShift0;
       std::ostringstream aNew0StrStr;  aNew0StrStr.setf(std::ios_base::showpos); aNew0StrStr 
	    << "                <position name=\"" << keyPos[8] << "Mis0_pos\" x =\"" 
	    << xShift0 << "\" y=\"0.5*ssdD0_height" << yShift0 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double xShift1 = this->getRandomShift(sigmaTrShifts);
       const double yShift1 = dGap + ssddouble10YShift1;
       std::ostringstream aNew1StrStr;  aNew1StrStr.setf(std::ios_base::showpos); aNew1StrStr 
	    << "                <position name=\"" << keyPos[8] << "Mis1_pos\" x =\"" 
	    << xShift1 << "\" y=\"0.5*ssdD0_height" << yShift1 << "\" z=\"ssdD0_thick+carbon_fiber_thick\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    // Finally 
   // ssddouble20  V planes, the 0.3536 is 0.5 /sqrt(2), assume nominal angle of 45 degrees.
    keyPos.push_back("ssddouble20");
    double ssddouble20VShift0 = 0.; double ssddouble20VShift1 = 0.;
    const double oneOSqrt2 = 1.0/std::sqrt(2.0);
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[9]+posPos) == std::string::npos) continue;
       ssddouble20VShift0 = this->getRandomShift(sigmaTrShifts);
       const double vShift0 = oneOSqrt2*(dGap + ssddouble20VShift0);
       std::ostringstream aNew0StrStr;  aNew0StrStr.setf(std::ios_base::showpos); aNew0StrStr 
	    << "                <position name=\"" << keyPos[9] << "Mis0_pos\" x =\"0.3536*ssdD0_height" 
	    << vShift0 << "\" y=\"0.3536*ssdD0_height" << vShift0 << "\" z=\"ssd3plane_shift\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       ssddouble20VShift1 = this->getRandomShift(sigmaTrShifts);
       const double vShift1 = oneOSqrt2*(dGap + ssddouble20VShift1);
       std::ostringstream aNew1StrStr;  aNew1StrStr.setf(std::ios_base::showpos); aNew1StrStr 
	    << "                <position name=\"" << keyPos[9] << "Mis1_pos\" x =\"0.3536*ssdD0_height" 
	    << vShift1 << "\" y=\"0.3536*ssdD0_height" << vShift1 << "\" z=\"ssd3plane_shift\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
   // ssddouble21
    keyPos.push_back("ssddouble21");
     std::cerr << " Check 21 " << " ssddouble20VShift0 " << ssddouble20VShift0 
                               << " ssddouble20VShift1 " <<  ssddouble20VShift1 << std::endl;
     for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++) {
       std::string origStr(*il);
       if (origStr.find(keyPos[10]+posPos) == std::string::npos) continue;
       const double vShift0 = -oneOSqrt2*(dGap + ssddouble20VShift0);
       std::ostringstream aNew0StrStr;  aNew0StrStr.setf(std::ios_base::showpos); aNew0StrStr 
	    << "                <position name=\"" << keyPos[10] << "Mis0_pos\" x =\"-0.3536*ssdD0_height" 
	    << vShift0 << "\" y=\"-0.3536*ssdD0_height" << vShift0 << "\" z=\"ssd3plane_shift\" />";
       
       const std::string aNew0Str(aNew0StrStr.str());
       std::vector<std::string>::iterator ilAdd = il; ilAdd++;
       ilAdd = fLines.insert(ilAdd, aNew0Str);
       ilAdd++;
       const double vShift1 = -oneOSqrt2*(dGap + ssddouble20VShift1);
       std::ostringstream aNew1StrStr;  aNew1StrStr.setf(std::ios_base::showpos); aNew1StrStr 
	    << "                <position name=\"" << keyPos[10] << "Mis1_pos\" x =\"-0.3536*ssdD0_height" 
	    << vShift1 << "\" y=\"-0.3536*ssdD0_height" << vShift1 << "\" z=\"ssd3plane_shift\" />";
       
       const std::string aNew1Str(aNew1StrStr.str());
       ilAdd = fLines.insert(ilAdd, aNew1Str);
       break;
    }
    std::cerr << " ...Done ssddouble21 " << std::endl;
    //
    // Now replace.. Start by finding the place wher such phyical volumes are defined.
    //
    std::cerr << " Now doing the replacement in physical volume declaration " << std::endl;
    std::vector<std::string>::iterator ilB = fLines.begin(); 
    std::vector<std::string>::iterator ilB2 = fLines.begin(); 
    for (std::vector<std::string>::iterator il = ilB; il != fLines.end(); il++) {
      std::string origStr(*il);
      if (origStr.find("<volume name=\"ssdStationsingle0_vol\">") != std::string::npos) { ilB=il; break; } 
    }
    std::vector<std::string> keyMisses; keyMisses.push_back("Mis0_pos"); keyMisses.push_back("Mis1_pos");
    for (size_t k=0; k != keyPos.size(); k++) {
       size_t nl=0;
       for (std::vector<std::string>::iterator il = ilB; il != fLines.end(); il++, nl++) {
         std::string origStr(*il);
	 size_t iPos0 = origStr.find(keyPos[k]);
	 if (iPos0 != std::string::npos) {
	   std::string newStr(origStr);
	   newStr.replace(iPos0, keyPos[k].length()+4, (keyPos[k]+keyMisses[0]));
	   *il= newStr;
//	   std::cerr << " Replaced " << keyPos[k] << " by " << keyPos[k]+keyMisses[0] << " at line number " << nl << std::endl;
	   ilB2 = il; ilB2++;
	   break;
	 }
       } 
       for (std::vector<std::string>::iterator il = ilB2; il != fLines.end(); il++, nl++) {
         std::string origStr(*il);
	 size_t iPos1 = origStr.find(keyPos[k]);
	 if (iPos1 != std::string::npos) {
	   std::string newStr(origStr);
	   newStr.replace(iPos1, keyPos[k].length()+4, (keyPos[k]+keyMisses[1]));
	   *il= newStr;
//	   std::cerr << " Replaced " << keyPos[k] << " by " << keyPos[k]+keyMisses[1] << " at line number " << nl << std::endl;
	   break;
	 }
       }
     }
  }
} // namespace
