////////////////////////////////////////////////////////////////////////
/// \file  Misaligner.h
/// \brief Use Geant4 to run the LArSoft detector simulation
///
///   Valid only for the Phase1b.gdml!. 
/// \version $Id: Misaligner.cxx,v 1.20 2012-12-03 23:29:49 rhatcher Exp $
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
#include "G4Base/Misaligner.h"

namespace g4b{

  //------------------------------------------------
  // Constructor
  Misaligner::Misaligner(const std::string &aNameIn, unsigned int aSeed)
  : fNameIn(aNameIn), fDoOnlyYTrans(false), fDoOnlyXTrans(false),
  fModelNumber(0), fSeed(aSeed), fGapDoubleSSD(3.0), fSigZ(0.), fSigRoll(0.), fSigTr(0.), fSigYP(0.),
  fViews(22, '?'), fViewIndicesY(), fViewIndicesX(), fViewIndicesU(), fViewIndicesV(),
  fZShiftsRaw(6, 0.), fXTransShiftsRaw(22, 0.), fYTransShiftsRaw(22, 0.),
  fRollsRaw(22, 0.), fYawsRaw(22, 0.), fPitchesRaw(22, 0.)
  { 
   if ((aNameIn == std::string("")) || aNameIn.empty() || (aNameIn == std::string("phase1b"))) {
      const char *srcDir = std::getenv("CETPKG_SOURCE");
      if (srcDir == nullptr) {
        std::cerr << " Misaligner::Misaligner, build environment not set, give up right here and now " << std::endl; exit(2);
      }
      fNameIn = std::string(srcDir) + std::string("/Geometry/gdml/phase1b/generate_gdml.pl");
   } else if (aNameIn == std::string("phase1c")) {
      const char *srcDir = std::getenv("CETPKG_SOURCE");
      if (srcDir == nullptr) {
        std::cerr << " Misaligner::Misaligner, build environment not set, give up right here and now " << std::endl; exit(2);
      }
      fNameIn = std::string(srcDir) + std::string("/Geometry/gdml/phase1c/generate_gdml.pl");
   } 
   srand(aSeed);
  }
  //------------------------------------------------
  void Misaligner::readToModifyIt() 
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
//      if (nLines < 100) std::cerr << ll << std::endl;
      if (ll.find("@SSD_station_shift") == 0) {
        fLine_SSD_station_shift = ll;
      } else if (ll.find("@SSD_shift") == 0) {
        fLine_SSD_shift = ll;
      } else if (ll.find("@SSD_mount_rotation") == 0) {
        fLine_SSD_mount_rotation = ll;
      } else if (ll.find("@SSD_angle") == 0) {
        fLine_SSD_angle = ll;
	this->setViewIndices();
      } 
    }
    // Remove the last line, it is blank. 
    fIn.close();
    std::cerr << "g4b::EmphAligner, read " << fLines.size() << " from file " << std::string(fNameIn) << std::endl; 
    std::cerr << " Now, modify it.. " << std::endl;    
    this->doSSDZTranslationOnStations(); 
    this->doSSDTransOffsetOnPlanes();
    this->doSSDRolls();
    this->doSSDYawPitchOnMounts();
    // Now replace the lines, store the text..  
    fIn.open(fNameIn.c_str());
    nLines = 0;
    fLines.clear();
    while (fIn.good()) { 
      fIn.getline(aLine, 2048);
      nLines++;
      std::string ll(aLine);
//      if (nLines < 100) std::cerr << ll << std::endl;
      if (ll.find("@SSD_station_shift") == 0) {
	fLines.push_back(fLine_SSD_station_shift);
      } else if (ll.find("@SSD_shift") == 0) {
	fLines.push_back(fLine_SSD_shift);
      } else if (ll.find("@SSD_mount_rotation") == 0) {
	fLines.push_back(fLine_SSD_mount_rotation);
      } else if (ll.find("@SSD_angle") == 0) {
	fLines.push_back(fLine_SSD_angle);
      } else { 
        fLines.push_back(ll); 
      }
    }
    // Remove the last line, it is blank. 
    fIn.close();
    fLines.pop_back();
  }
  //
  void Misaligner::writeIt(const char* fName) const {
    std::ofstream fOut(fName);
    for (std::vector<std::string>::const_iterator it=fLines.cbegin(); it!=fLines.cend(); it++) {
      fOut << (*it) << std::endl;
    }
    fOut.close();
  }
  std::string Misaligner::runIt(const std::string &suffix) const { // return a complete Unix path name, the gdml file to use in the 
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
     std::string fNameOutTruth(mycwd); 
     fNameOutTruth += std::string("/TruthValues_") + suffixAll + std::string(".txt");
     this->dumpRawMisAlignParams(fNameOutTruth.c_str()); 
     
     return std::string(fNameOutGdml);
  }
  
  void Misaligner::dumpRawMisAlignParams(const char* fName) const {
    std::ofstream fOut(fName);
//    fOut << " kSeT TransShiftX TransShifty deltaRoll " << std::endl;
//    for (size_t kP=0; kP != fXTransShiftsRaw.size(); kP++) {
//      fOut << " " << kP << " " <<  fXTransShiftsRaw[kP] << " " <<  fYTransShiftsRaw[kP] << " "
//           << fRollsRaw[kP] << std::endl;
//    }
// 
//  For easy comparison of the result of the Algo1 SSDAlign MPI fits.. 
//  Skip the Tilts, a Yaw or Pitch causes G4 volume overlap, not a good idea.. 
//
    const double oneOverSqrt2 = 1.0 /std::sqrt(2);
    fOut << "Tilt_Y_0 1.0e-6 1.0e-6 " << std::endl; // made up error, does not matter
    for (size_t kY=1; kY != fViewIndicesY.size(); kY++) {
      const size_t iSensor = fViewIndicesY[kY];
      fOut << "TransShift_Y_" << kY << " " << fYTransShiftsRaw[iSensor] << " 1.0e-6 " << std::endl;
      std::cerr << " .... Dump Y Trans ..kY " << kY << " sensor " <<  iSensor  << " val  " << fYTransShiftsRaw[iSensor] << std::endl;
      size_t iStation = kY;
      if ((kY == 4) || kY == 5) iStation = 4; 
      if ((kY == 6) || kY == 7) iStation = 5; 
      if (iStation != 5) fOut << "LongShift_Y_" << kY << " " << fZShiftsRaw[iStation] << " 1.0e-6 " << std::endl; 
      fOut << "Tilt_Y_" << kY << " 1.0e-6 1.0e-6 " << std::endl; // made up error, does not matter
    }
    fOut << "Tilt_X_0 1.0e-6 1.0e-6 " << std::endl; // made up error, does not matter
    for (size_t kX=1; kX != fViewIndicesX.size(); kX++) {
      const size_t iSensor = fViewIndicesX[kX];
      fOut << "TransShift_X_" << kX << " " << fXTransShiftsRaw[iSensor] << " 1.0e-6 " << std::endl;
      std::cerr << " .... Dump X Trans ..kX " << kX << " sensor " <<  iSensor  << " val  " << fXTransShiftsRaw[iSensor] << std::endl;
      fOut << "Tilt_X_" << kX << " 1.0e-6 1.0e-6 " << std::endl; // made up error, does not matter
    }
    fOut << "LongMagC 757.7 76.77 " << std::endl;
    fOut << "KickMag -0.000612 3.06e-05 " << std::endl;
    //
    // Now the Rolls and Roll Center. X and then Y.. Historical/Hysterical ordering, due to progression in defining params. 
    for (size_t kX=0; kX != fViewIndicesX.size(); kX++) {
      const size_t iSensor = fViewIndicesX[kX];
      const size_t iSensorY = fViewIndicesY[kX];  // X and Y have same dimensions.. 
      fOut << "DeltaRoll_X_" << kX << " " << fRollsRaw[iSensor] << " 1.0e-6 " << std::endl;
      std::cerr << " .... Dump X Rolls ..kX " << kX << " sensor " <<  iSensor  << " Yval  " << fYTransShiftsRaw[iSensorY] << std::endl;
      fOut << "DeltaRollCenter_X_" << kX << " " << fYTransShiftsRaw[iSensorY] << " 1.0e-6 " << std::endl; 
    }
    for (size_t kY=1; kY != fViewIndicesY.size(); kY++) {
      const size_t iSensor = fViewIndicesX[kY];
      const size_t iSensorX = fViewIndicesY[kY];  // X and Y have same dimensions.. 
      fOut << "DeltaRoll_Y_" << kY << " " << fRollsRaw[iSensor] << " 1.0e-6 " << std::endl;
      std::cerr << " .... Dump Y Rolls ..kX " << kY << " sensor " <<  iSensor  << " Xval  " << fXTransShiftsRaw[iSensorX] << std::endl;
      fOut << "DeltaRollCenter_Y_" << kY << " " << fXTransShiftsRaw[iSensorX] << " 1.0e-6 " << std::endl; 
    }
     
    
    for (size_t kU=0; kU != fViewIndicesU.size(); kU++) {
      const size_t iSensor = fViewIndicesU[kU];
      const double deltaU = oneOverSqrt2 * (fXTransShiftsRaw[iSensor] - fYTransShiftsRaw[iSensor]); //phase1b, check signs. 
      const double deltaV = oneOverSqrt2 * (-fXTransShiftsRaw[iSensor] - fYTransShiftsRaw[iSensor]); //phase1b, check signs. 
      fOut << "TransShift_U_" << kU << " " << deltaU << " 1.0e-6 " << std::endl;
      const size_t iStation = kU + 4; // Phase1b only 
//      fOut << "LongShift_U_" << kU << " " << fZShiftsRaw[iStation] << " 1.0e-6 " << std::endl; 
// Not defined in the SSDAlignAlog1 mnimizer, use the shift from x or Y internally.. 
      fOut << "Tilt_U_" << kU << " 1.0e-6 1.0e-6 " << std::endl; // made up error, does not matter
      fOut << "DeltaRoll_U_" << kU << " " << fRollsRaw[iSensor] << " 1.0e-6 " << std::endl;
      fOut << "DeltaRollCenter_U_" << kU << " " << deltaV << " 1.0e-6 " << std::endl; 
    }
    for (size_t kV=0; kV != fViewIndicesV.size(); kV++) {
      const size_t iSensor = fViewIndicesV[kV];
      const double deltaU = oneOverSqrt2 * (fXTransShiftsRaw[iSensor] - fYTransShiftsRaw[iSensor]); //phase1b, check signs. 
      const double deltaV = oneOverSqrt2 * (-fXTransShiftsRaw[iSensor] - fYTransShiftsRaw[iSensor]); //phase1b, check signs. 
      fOut << "TransShift_V_" << kV << " " << deltaV << " 1.0e-6 " << std::endl;
      size_t iStation = 4 + kV/2; // Phase1b only 
      fOut << "Tilt_V_" << kV << " 1.0e-6 1.0e-6 " << std::endl; // made up error, does not matter
      fOut << "DeltaRoll_V_" << kV << " " << fRollsRaw[iSensor] << " 1.0e-6 " << std::endl;
      fOut << "DeltaRollCenter_V_" << kV << " " << deltaU << " 1.0e-6 " << std::endl; 
    }
    // Now the Rolls and Roll Center.. 
    
    fOut.close();
  }
  //------------------------------------------------
  void Misaligner::doIt(int aModelNum, double gapD) {
    fModelNumber = aModelNum;
    fGapDoubleSSD = gapD; 
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
    std::cerr << " Misaligner::doIt, sigZ " << fSigZ << " sigTr " << fSigTr 
              << " SigYP " << fSigYP << " sigRolls " << fSigRoll << std::endl;
	      
    this->readToModifyIt();
    
  }
  double Misaligner::getRandomShift(double sig) {
    const double sign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
    const double val = sign * (static_cast<double>(rand())/RAND_MAX ) * sig; 
    return val;
  }
  double Misaligner::getValue(const std::string &line, const std::string &key) { // Obsolete, I think.. 
     const size_t iPosValue = line.find(key);
    if (iPosValue == std::string::npos) {
      std::cerr << " Misaligner::getValue, internal error or corrupted Phase1.gdml file, at line "
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    const size_t iPosbV = iPosValue + 1 + key.length();
    const std::string valStrAll = line.substr(iPosbV);
    const size_t iPoseV = valStrAll.find("\"");
    if (iPoseV == std::string::npos) {
      std::cerr << " Misaligner::getValue, corrupted Phase1.gdml file, no good value at line   " 
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    std::string valStr = valStrAll.substr(0, iPoseV);
    double zz = 0.; std::istringstream valStrStr(valStr); valStrStr >> zz;
    return zz;
  }
  std::string Misaligner::getName(const std::string &line) { // Obsolete, I think.. 
     const size_t iPosName = line.find("name=");
    if (iPosName == std::string::npos) {
      std::cerr << " Misaligner::getName, internal error or corrupted Phase1.gdml file, at line "
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    const size_t iPosbV = iPosName + 6;
    const std::string nameStrAll = line.substr(iPosbV);
    const size_t iPoseV = nameStrAll.find("\"");
    if (iPoseV == std::string::npos) {
      std::cerr << " Misaligner::getName, corrupted Phase1.gdml file, no good value at line   " 
    		<<  line  << " fatal " << std::endl; exit(2);
    }
    std::string valStr = nameStrAll.substr(0, iPoseV);
    return valStr;
  }
  void Misaligner::doSSDZTranslationOnStations() {
    if (std::abs(fSigZ) < 1.0e-6) return;
    std::cerr << " Misaligner::doSSDZTranslationOnStations with longitudinal displacements, maximum " << fSigZ << std::endl;
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
  void Misaligner::setViewIndices() { // Written only for phase1b, could work on phase1c 
//    std::cerr << " Misaligner::setViewIndices...Nominal rolls " << fLine_SSD_angle << std::endl;
    size_t iPos=fLine_SSD_angle.find("("); iPos++;
    std::string remLine = fLine_SSD_angle.substr(iPos);
    bool isLastValue = false;
    size_t iSensor = 0; fViewIndicesY.clear(); fViewIndicesX.clear(); fViewIndicesU.clear();fViewIndicesV.clear();
    while(true) { 
      size_t iPosComma = remLine.find(","); 
      if (iPosComma == std::string::npos)  { 
         isLastValue = true; iPosComma = remLine.find(")"); 
      }
      std::string valString = remLine.substr(0, iPosComma); 
      std::istringstream valInStrStr(valString); double zz;  valInStrStr >> zz;
//      std::cerr << " ..... iSensor " << iSensor << " valString " << valString << std::endl;
      if ((std::abs(zz) < 0.1) || (std::abs(180.0 - zz) < 0.1)) { fViews[iSensor] = 'Y'; fViewIndicesY.push_back(iSensor); }
      if ((std::abs(90.0 - zz) < 0.1) || (std::abs(270.0 - zz) < 0.1)) { fViews[iSensor] = 'X'; fViewIndicesX.push_back(iSensor); }
      if ((std::abs(-45.0 - zz) < 0.1) || (std::abs(315.0 - zz) < 0.1)) { fViews[iSensor] = 'U'; fViewIndicesU.push_back(iSensor); } 
      if ((std::abs(45.0 - zz) < 0.1) || (std::abs(225.0 - zz) < 0.1)) { fViews[iSensor] = 'V'; fViewIndicesV.push_back(iSensor); }
      if (isLastValue) break;
      iPosComma++; 
      remLine = remLine.substr(iPosComma); iPos = iPosComma;
      iSensor++;
    }
    // Now rewrite the line..
    std::cerr << " fViews ";
    for (size_t k=0; k != fViews.size(); k++) std::cerr << " " << fViews[k];
    std::cerr << std::endl;
    std::cerr << " fViewsIndicesY ";
    for (size_t k=0; k != fViewIndicesY.size(); k++) std::cerr << " " << fViewIndicesY[k];
    std::cerr << std::endl;
    std::cerr << " fViewsIndicesX ";
    for (size_t k=0; k != fViewIndicesX.size(); k++) std::cerr << " " << fViewIndicesX[k];
    std::cerr << std::endl;
    std::cerr << " fViewsIndicesU ";
    for (size_t k=0; k != fViewIndicesU.size(); k++) std::cerr << " " << fViewIndicesU[k];
    std::cerr << std::endl;
    std::cerr << " fViewsIndicesX ";
    for (size_t k=0; k != fViewIndicesV.size(); k++) std::cerr << " " << fViewIndicesV[k];
    std::cerr << std::endl;
//    std::cerr << " ..... And quit for now.. " << std::endl; exit(2);
  }  
  
  void Misaligner::doSSDRolls() {
    if (std::abs(fSigRoll)  < 1.0e-6) return;
    //
    std::cerr << " Misaligner::doSSDRolls with sigRolls " << fSigRoll << std::endl;
    fRollsRaw.clear();
//    std::cerr << " ...Nominal rolls " << fLine_SSD_angle << std::endl;
    size_t iPos=fLine_SSD_angle.find("("); iPos++;
    std::string remLine = fLine_SSD_angle.substr(iPos);
    bool isLastValue = false;
    int iSensor = 0; 
    while(true) {
      size_t iPosComma = remLine.find(","); 
      if (iPosComma == std::string::npos)  {
        isLastValue = true;
	iPosComma = remLine.find(")");
      }
      std::string valString = remLine.substr(0, iPosComma); 
      std::istringstream valInStrStr(valString); double zz;  valInStrStr >> zz;
      // Flat distribution... 
      const double zRollDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      const double zRoll = zRollDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigRoll; 
      zz += zRoll; fRollsRaw.push_back(zz);
//      std::cerr << " Sensor " << iSensor << " New roll angle " << zz << std::endl;
      // Sensor 0, View Y, station zero has no rolls by definition 
      if (iSensor == 0) zz = 0.;
      if (isLastValue) break;
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
  void Misaligner::doSSDYawPitchOnMounts() {
    if (fSigYP < 1.0e-4) return;
    std::cerr << " Misaligner::doSSDYawPitchOnStations generate mount yaw and pitch, max val " 
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

  void Misaligner::doSSDTransOffsetOnPlanes() {
//    if (std::abs(sigmaTrShifts) <  1.0e-4) return;  // Now we always do it, there is the gap. 
// August 1 2023 : strip level geometry, 
// August 5 2023 : Simplify, edit a single line..     
    if (std::abs(fGapDoubleSSD) > 30.) return; // unrealistic 
    std::cerr << " Misaligner::doSSDTransOffsetOnPlanes, sigmaTrShifts " << fSigTr << " dGap " << fGapDoubleSSD << std::endl;
//    std::cerr << " ... begin line " <<  fLine_SSD_shift.substr(0, 60) << std::endl;
    size_t iPos=fLine_SSD_shift.find("("); iPos++;
    std::string remLine = fLine_SSD_shift.substr(iPos);
    iPos=remLine.find("["); iPos++;
    remLine = remLine.substr(iPos);
    fXTransShiftsRaw.clear();  fYTransShiftsRaw.clear();
    size_t iSensor = 0;
    std::vector<double> fXG4(22, 0.); std::vector<double> fYG4(22, 0.);
    while(true) {
      size_t iPosComma = remLine.find(","); 
      std::string valStringX = remLine.substr(0, iPosComma); 
      std::istringstream valInXStrStr(valStringX); double xx;  valInXStrStr >> xx;
      // Flat distribution... 
      double aGapDoubleSSD = 0.;
      if ((xx < 0.) && (std::abs(xx) > 10.))aGapDoubleSSD = -0.5*fGapDoubleSSD; // This should be in the nominal geometry.. 
      if ((xx > 0.) && (std::abs(xx) > 10.))aGapDoubleSSD = 0.5*fGapDoubleSSD; //  
      const double xPosDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      double xPosDelta = ((iSensor == fViewIndicesX[0]) || (iSensor == fViewIndicesY[0])) ?
                               aGapDoubleSSD : aGapDoubleSSD + xPosDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigTr; 
      if (fDoOnlyYTrans && (fViews[iSensor] != 'Y')) xPosDelta = aGapDoubleSSD;		       
      if (fDoOnlyXTrans && (fViews[iSensor] != 'X')) xPosDelta = aGapDoubleSSD;		       
      xx += xPosDelta; fXTransShiftsRaw.push_back(xPosDelta);
      if ((iSensor == 11) || (iSensor == 17)) *fXTransShiftsRaw.rbegin() *= -1.0;; // phase1b, due to flipping, 90 vs 270 degrees. 
      fXG4[iSensor] = xx;
      size_t iPosY = iPosComma + 1;
      remLine = remLine.substr(iPosY+1);
      size_t iPosClosePar = remLine.find("]");
      std::string valStringY = remLine.substr(0, iPosClosePar); 
      std::istringstream valInYStrStr(valStringY); double yy;  valInYStrStr >> yy;
      const double yPosDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
      double yPosDelta = ((iSensor == fViewIndicesX[0]) || (iSensor == fViewIndicesY[0]) || (iSensor == fViewIndicesY[7])) ?
                               0. : yPosDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * fSigTr; 
      if (fDoOnlyYTrans && (fViews[iSensor] != 'Y')) yPosDelta = 0.;		       
      if (fDoOnlyXTrans && (fViews[iSensor] != 'X')) yPosDelta = 0.;		       
      yy += yPosDelta; fYTransShiftsRaw.push_back(yPosDelta);
      if ((iSensor == 12) || (iSensor == 18)) *fYTransShiftsRaw.rbegin() *= -1.0;; // phase1b, due to flipping, 90 vs 270 degrees. 
      fYG4[iSensor] = yy;
      remLine = remLine.substr(iPosClosePar+1);
      std::cerr << " ... At Sensor " << iSensor << " View " << fViews[iSensor] << " X-delta and Y-delta,  last  " 
                << *fXTransShiftsRaw.rbegin() << ", " <<*fYTransShiftsRaw.rbegin() 
		<< " G4Vals " << fXG4[iSensor] << ", " <<  fYG4[iSensor] << std::endl; 
//      std::cerr << " .... set new vals x and y , remline length " << remLine.length() << std::endl;
      size_t iPosOpenPar = remLine.find("["); 
      if (iPosOpenPar == std::string::npos) break; 
      remLine = remLine.substr(iPosOpenPar+1);
      iPos = iPosOpenPar+1;
      iSensor++;
    }
    std::ostringstream strOut; strOut << "@SSD_shift = ("; 
    for (size_t kP = 0; kP != fXG4.size(); kP++) {
      strOut << "[" << fXG4[kP] << ", " << fYG4[kP] << "]";
      if (kP != (fXG4.size() - 1)) strOut << ", ";
      else strOut << ");";
    }
    fLine_SSD_shift = strOut.str();
    std::cerr <<  " ... fLine_SSD_shift " << fLine_SSD_shift << std::endl;
//    std::cerr << " ..... And quit for now.. " << std::endl; exit(2);
  }  
} // namespace
