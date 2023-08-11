////////////////////////////////////////////////////////////////////////
/// \file  EmphMisaligner.h
/// \brief Use Geant4 to run the LArSoft detector simulation
///
///   Valid only for the Phase1b.gdml!. 
/// \version $Id: EmphMisaligner.cxx,v 1.20 2012-12-03 23:29:49 rhatcher Exp $
/// \author  seligman@nevis.columbia.edu, brebel@fnal.gov
///  August 1 2023:  Upgrade the geometry 
///  August 5 2023:  simplify the code, modify perl variable instead of the the gdml file. 
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "G4Base/EmphMisaligner.h"

namespace g4b{

  //------------------------------------------------
  // Constructor
  EmphMisaligner::EmphMisaligner(const std::string &aNameIn, unsigned int aSeed)
  : fNameIn(aNameIn), fModelNumber(0), fSeed(aSeed), fGapDoubleSSD(3.0), fSigZ(0.), fSigRoll(0.), fSigTr(0.), fSigYP(0.),
  fXTransShiftsRaw(22, 0.), fYTransShiftsRaw(22, 0.),
  fRollsRaw(22, 0.), fYawsRaw(22, 0.), fPitchesRaw(22, 0.)
  { 
   if ((aNameIn == std::string("")) || aNameIn.empty() || (aNameIn == std::string("phase1b"))) {
      const char *srcDir = std::getenv("CETPKG_SOURCE");
      if (srcDir == nullptr) {
        std::cerr << " EmphMisaligner::EmphMisaligner, build environment not set, give up right here and now " << std::endl; exit(2);
      }
      fNameIn = std::string(srcDir) + std::string("/Geometry/gdml/phase1b/generate_gdml.pl");
   } else if (aNameIn == std::string("phase1c")) {
      const char *srcDir = std::getenv("CETPKG_SOURCE");
      if (srcDir == nullptr) {
        std::cerr << " EmphMisaligner::EmphMisaligner, build environment not set, give up right here and now " << std::endl; exit(2);
      }
      fNameIn = std::string(srcDir) + std::string("/Geometry/gdml/phase1c/generate_gdml.pl");
   } 
   srand(aSeed);
  }
  //------------------------------------------------
  void EmphMisaligner::readAndModifyIt() 
  {
    std::ifstream fIn(fNameIn.c_str());
    if (!fIn.is_open()) {
       std::cerr << "g4b::EmphAligner, could not find and/or open file " << std::string(fNameIn) 
                 << " fatal, quit here.. " << std::endl; exit(2);
   
    }
    int nLines = 0;
    char aLine[2048];
    while (fIn.good()) { 
      fIn.getline(aLine, 2048);
      nLines++;
      std::string ll(aLine);
      if (ll.find("@SSD_station_shift") == 0) {
        fLine_SSD_station_shift = ll;
        this->doSSDZTranslationOnStations(); 
	fLines.push_back(fLine_SSD_station_shift);
      } else if (ll.find("@SSD_shift") == 0) {
        fLine_SSD_shift = ll;
	this->doSSDTransOffsetOnPlanes();
	fLines.push_back(fLine_SSD_shift);
      } else if (ll.find("@SSD_mount_rotation") == 0) {
        fLine_SSD_mount_rotation = ll;
	this->doSSDYawPitchOnMounts();
	fLines.push_back(fLine_SSD_mount_rotation);
      } else if (ll.find("@SSD_angle") == 0) {
        fLine_SSD_angle = ll;
	this->doSSDRolls();
	fLines.push_back(fLine_SSD_angle);
      } else { 
        fLines.push_back(ll); 
      }
    }
    // Remove the last line, it is blank. 
    fLines.pop_back();
    std::cerr << "g4b::EmphAligner, read " << fLines.size() << " from file " << std::string(fNameIn) << std::endl;     
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
  std::string EmphMisaligner::runIt(const std::string &suffix) const { // return a complete Unix path name, the gdml file to use in the 
     char cwdBuff[2048];
     const char *mycwd = getcwd(cwdBuff, 2048);
     const char *srcDir = std::getenv("CETPKG_SOURCE");
     std::string theWkDirTop(srcDir); theWkDirTop += std::string("/Geometry/gdml/");
     std::string theWkDirBot(theWkDirTop); theWkDirBot += std::string("phase1b/");  // To be extend do phase1c, if we get to it..
     std::ostringstream suffixAllStrStr; suffixAllStrStr << suffix << "_" << fModelNumber << "_" << fSeed << "_";
     std::string suffixAll(suffixAllStrStr.str());
     std::string fNamePerlScript(theWkDirBot); 
     fNamePerlScript += std::string("generate_gdml_") + suffixAll + std::string(".pl");
     this->writeIt(fNamePerlScript.c_str());
     pid_t aPid = getpid(); std::ostringstream aPiStrStr;  aPiStrStr << "_" << aPid;
     std::string fNameOutGdmlTmp(theWkDirBot); 
     fNameOutGdmlTmp += std::string("phase1bTmp_") + suffixAll +  aPiStrStr.str() + std::string(".gdml"); 
     std::string cmd0("chmod +x "); 
     cmd0 += fNamePerlScript;
     system(cmd0.c_str());
     std::string cmd1(fNamePerlScript); 
     cmd1 += std::string(" -o ") + fNameOutGdmlTmp;
     cmd1 += std::string(" -m 0 -t 0 -suffix ") + suffixAll;
     system(cmd1.c_str());
     //
     std::string fNameOutGdml(mycwd); 
     fNameOutGdml += std::string("/phase1b_") + suffixAll + std::string(".gdml"); 
     std::string cmd2(theWkDirTop); 
     cmd2 += std::string("make_gdml.pl -i ") +  fNameOutGdmlTmp + std::string(" -o ") +  fNameOutGdml;
     system(cmd2.c_str());
     // 
     // clean up 
     std::string cmd3("/bin/rm ./phase1_*.gdml "); cmd3 += fNameOutGdmlTmp;
     system(cmd3.c_str());
     return std::string(fNameOutGdml);
  }
  void EmphMisaligner::dumpRawMisAlignParams(const char* fName) const {
    std::ofstream fOut(fName);
    fOut << " kSeT TransShiftX TransShifty deltaRoll " << std::endl;
    for (size_t kP=0; kP != fXTransShiftsRaw.size(); kP++) {
      fOut << " " << kP << " " <<  fXTransShiftsRaw[kP] << " " <<  fXTransShiftsRaw[kP] << " "
           << fRollsRaw[kP] << std::endl;
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
   
    fSigZ =  static_cast<double>(iSigmaZ); // now in mm. Possible range from 0 to 99 mm, or ~10 cm offset (unlikely)
    // Note: we still do need to implement the gap, even if the translations are very small. 
    // The case where there is no gap and no translation is faulty, but unphysical 
    //
    fSigTr = std::max(0.01, (0.1* static_cast<double>(iSigmaTr))); // maximum is 9.9 mm, sigma. 
    fSigRoll =  0.1*static_cast<double>(iSigmaRolls); // now in degrees. 
    fSigYP =  0.1*static_cast<double>(iSigmaYawPitch); // now in degrees. 
    std::cerr << " EmphMisaligner::doIt, sigZ " << fSigZ << " sigTr " << fSigTr 
              << " SigYP " << fSigYP << " sigRolls " << fSigRoll << std::endl;
	      
    this->readAndModifyIt();
    
  }
  double EmphMisaligner::getRandomShift(double sig) {
    const double sign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
    const double val = sign * (static_cast<double>(rand())/RAND_MAX ) * sig; 
    return val;
  }
  double EmphMisaligner::getValue(const std::string &line, const std::string &key) { // Obsolete, I think.. 
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
  std::string EmphMisaligner::getName(const std::string &line) { // Obsolete, I think.. 
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
  void EmphMisaligner::doSSDZTranslationOnStations() {
    if (std::abs(fSigZ) < 1.0e-6) return;
    std::cerr << " EmphMisaligner::doSSDZTranslationOnStations with longitudinal displacements, maximum " << fSigZ << std::endl;
    fZShiftsRaw.clear();
    size_t iPos=fLine_SSD_station_shift.find("("); iPos++;
    std::string remLine = fLine_SSD_station_shift.substr(iPos);
    bool isLastValue = false;
    while((!isLastValue)) {
      size_t iPosComma = remLine.find(","); 
      if (iPosComma == std::string::npos)  isLastValue = true;
      std::string valString; 
      if (!isLastValue) valString = remLine.substr(0, iPosComma);
      else {
        size_t iPosEnd = remLine.find(")");
	 valString = remLine.substr(0, iPosEnd);
      } 
      std::istringstream valInStrStr(valString); double zz;  valInStrStr >> zz;
      // Flat distribution... 
      const double zPosDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      const double zPosDelta = zPosDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigZ; 
      zz += zPosDelta; fZShiftsRaw.push_back(zz);
      iPosComma++; 
      if (!isLastValue) remLine = remLine.substr(iPosComma);
    }
    // Now rewrite the line.. 
    std::ostringstream lineOut; lineOut << "@SSD_station_shift = ("; 
    for (size_t k=0;  k != fZShiftsRaw.size()-1; k++) lineOut << fZShiftsRaw[k] << ", ";
    lineOut << fZShiftsRaw[fZShiftsRaw.size()-1] << ");"; 
    fLine_SSD_station_shift = lineOut.str();  
//    std::cerr <<  " ... fLine_SSD_station_shift " << fLine_SSD_station_shift << std::endl;
//    std::cerr << " ..... And quit for now.. " << std::endl; exit(2);
  }
  
  void EmphMisaligner::doSSDRolls() {
    if (std::abs(fSigRoll)  < 1.0e-6) return;
    //
    std::cerr << " EmphMisaligner::doSSDRolls with sigRolls " << fSigRoll << std::endl;
    fRollsRaw.clear();
//    std::cerr << " ...Nominal rools " << fLine_SSD_angle << std::endl;
    size_t iPos=fLine_SSD_angle.find("("); iPos++;
    std::string remLine = fLine_SSD_angle.substr(iPos);
    bool isLastValue = false;
    int iSensor = 0; 
    while((iPos < remLine.length()) && (!isLastValue)) {
      size_t iPosComma = remLine.find(","); 
      if (iPosComma == std::string::npos)  isLastValue = true;
      std::string valString = remLine.substr(0, iPosComma); 
      std::istringstream valInStrStr(valString); double zz;  valInStrStr >> zz;
      // Flat distribution... 
      const double zRollDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      const double zRoll = zRollDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigRoll; 
      zz += zRoll; fRollsRaw.push_back(zz);
//      std::cerr << " Sensor " << iSensor << " New roll angle " << zz << std::endl;
      iPosComma++; 
      remLine = remLine.substr(iPosComma); iPos = iPosComma;
//      iSensor++;
    }
    // Now rewrite the line.. 
    std::ostringstream lineOut; lineOut << "@SSD_angle = ("; 
    for (size_t k=0;  k != fRollsRaw.size()-1; k++) lineOut << fRollsRaw[k] << ", ";
    lineOut << fRollsRaw[fRollsRaw.size()-1] << ");"; 
    fLine_SSD_angle = lineOut.str(); 
//    std::cerr <<  " ... fLine_SSD_angle " << fLine_SSD_angle << std::endl;
//    std::cerr << " ..... And quit for now.. " << std::endl; exit(2);
  }  
  void EmphMisaligner::doSSDYawPitchOnMounts() {
    if (fSigYP < 1.0e-4) return;
    std::cerr << " EmphMisaligner::doSSDYawPitchOnStations generate mount yaw and pitch, max val " 
              << fSigYP << std::endl << " .............. Nefarious Volume overlaps might be generated.. Please check.. " << std::endl; 
    // Start by counting the number of double elements 
    size_t nPars = 0; fPitchesRaw.clear(); fYawsRaw.clear();
    for (size_t k=0; k != fLine_SSD_mount_rotation.length(); k++) if (fLine_SSD_mount_rotation[k] == '[') nPars++; 
    std::cerr << " ...... Num of mounts " << nPars << std::endl;; 
    for (size_t k=0; k != nPars; k++) {
      const double thPitchDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      fPitchesRaw.push_back(thPitchDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigYP); 
      const double thYawDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      fYawsRaw.push_back(thYawDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigYP);
    } 
    // Assume they are all zero to start with.. 
    std::ostringstream strOut; strOut << "@SSD_mount_rotation = ("; 
    for (size_t kP = 0; kP != fPitchesRaw.size(); kP++) {
      strOut << "[" << fYawsRaw[kP] << ", " << fPitchesRaw[kP] << "]";
      if (kP != nPars-1) strOut <<", ";
      else strOut << ");";
    }
    fLine_SSD_mount_rotation = strOut.str();
//    std::cerr <<  " ... fLine_SSD_mount_rotation " << fLine_SSD_mount_rotation << std::endl;
//    std::cerr << " ..... And quit for now.. " << std::endl; exit(2);
  }

  void EmphMisaligner::doSSDTransOffsetOnPlanes() {
//    if (std::abs(sigmaTrShifts) <  1.0e-4) return;  // Now we always do it, there is the gap. 
// August 1 2023 : strip level geometry, 
// August 5 2023 : Simplify, edit a single line..     
    if (std::abs(fGapDoubleSSD) > 30.) return; // unrealistic 
    std::cerr << " EmphMisaligner::doSSDTransOffsetOnPlanes, sigmaTrShifts " << fSigTr << " dGap " << fGapDoubleSSD << std::endl;
//    std::cerr << " ... begin line " <<  fLine_SSD_shift.substr(0, 60) << std::endl;
    size_t iPos=fLine_SSD_shift.find("("); iPos++;
    std::string remLine = fLine_SSD_shift.substr(iPos);
    iPos=remLine.find("["); iPos++;
    remLine = remLine.substr(iPos);
    bool isLastValue = false; fXTransShiftsRaw.clear();  fYTransShiftsRaw.clear();
//    int iSensor = 0;
    while((iPos < remLine.length()) && (!isLastValue)) {
      size_t iPosComma = remLine.find(","); 
      if (iPosComma == std::string::npos)  isLastValue = true;
      std::string valStringX = remLine.substr(0, iPosComma); 
      std::istringstream valInXStrStr(valStringX); double xx;  valInXStrStr >> xx;
      // Flat distribution... 
      if ((xx < 0.) && (std::abs(xx) > 10.)) xx -= 0.5*fGapDoubleSSD; // This should be in the nominal geometry.. 
      if ((xx > 0.) && (std::abs(xx) > 10.)) xx += 0.5*fGapDoubleSSD; //  
      const double xPosDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      const double xPosDelta = xPosDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigTr; 
      xx += xPosDelta; fXTransShiftsRaw.push_back(xx);
      size_t iPosY = iPosComma + 1;
      remLine = remLine.substr(iPosY+1);
      size_t iPosClosePar = remLine.find("]");
      std::string valStringY = remLine.substr(0, iPosClosePar); 
      std::istringstream valInYStrStr(valStringY); double yy;  valInYStrStr >> yy;
      const double yPosDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      const double yPosDelta = yPosDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigTr; 
      yy += yPosDelta; fYTransShiftsRaw.push_back(yy);
      if (isLastValue)  break; 
      remLine = remLine.substr(iPosClosePar+1);
//      std::cerr << " .... set new vals x and y , remline length " << remLine.length() << std::endl;
      size_t iPosOpenPar = remLine.find("["); 
      if (iPosOpenPar == std::string::npos) break;  
      remLine = remLine.substr(iPosOpenPar+1);
      iPos = iPosOpenPar+1;
//      std::cerr << " ... At Sensor " << iSensor << " iPos " << iPos << " remLine length " << remLine.length() << std::endl; 
//      iSensor++;
    }
    std::ostringstream strOut; strOut << "@SSD_shift = ("; 
    for (size_t kP = 0; kP != fXTransShiftsRaw.size(); kP++) {
      strOut << "[" << fXTransShiftsRaw[kP] << ", " << fYTransShiftsRaw[kP] << "]";
      if (kP != (fXTransShiftsRaw.size() - 1)) strOut << ", ";
      else strOut << ");";
    }
    fLine_SSD_shift = strOut.str();
//    std::cerr <<  " ... fLine_SSD_shift " << fLine_SSD_shift << std::endl;
//    std::cerr << " ..... And quit for now.. " << std::endl; exit(2);
  }  
} // namespace
