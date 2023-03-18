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
    if (sigYP > 5.) {
      std::cerr << " EmphMisaligner::doSSDYawPitchOnStations sigYP greater than 5, unrelasitic, fatal " << std::endl;
      exit(2);
    }
    // Here we add the rotation matrices for the 6 stations. 
    // Search for the 1rst instance of "ABOVE IS FOR SSD"
    std::string blankStr(""); blankStr.clear();
    int nl = 0;
    bool didIt = false;
    std::vector<std::string> myNamesRot;
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
          for (size_t kSt=0; kSt != 6; kSt++) {
	    // need to cheat a little bit, Station is leaning on the magnet.. Or the RICH.. Reduce width.. 
	    const double sigYPR = (kSt > 3  ) ? 0.5*sigYP : sigYP;
	    const double thPitchDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	    const double thPitchDelta = thPitchDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigYPR; 
	    const double thYawDeltaSign = ((static_cast<double>(rand())/RAND_MAX ) < 0.5) ? -1. : 1.;
	    const double thYawchDelta = thYawDeltaSign * (static_cast<double>(rand())/RAND_MAX ) * sigYPR;
	    std::ostringstream lnStrStr; lnStrStr << "\"SSDStation_" << kSt << "_physMisRot\"";
	    std::string lnStr(lnStrStr.str()); myNamesRot.push_back(lnStr);
	    std::ostringstream llStrStr; llStrStr << "            " 
	        << "<rotation name=" << lnStr << " x=\"" << thPitchDelta << "\" y=\"" <<  thYawchDelta << "\" unit=\"deg\"/> ";
	    std::string llStr(llStrStr.str());
	    ilAdd = fLines.insert(ilAdd, llStr); ilAdd++;
	    	
	  }
	  ilAdd = fLines.insert(ilAdd, blankStr); ilAdd++;
	  didIt = true;
	  break; // done for this phase 
	}
        if (didIt) break; // should not be needed !
    }
    std::cerr << " .... Declare the Yaw/Pitch rotation matrices... all " << myNamesRot.size() << " of them.. " << std::endl;
    std::cerr << " ................ last " << myNamesRot[5] << std::endl;
    //
    // Now create a new mother volume such that we can place the rotated set of planes in a new local reference frame. 
    // start with Boxes 
    //
    std::vector<std::string>::iterator ilI1 =  fLines.begin(); 
    nl=0; 
    for (std::vector<std::string>::iterator il = fLines.begin(); il != fLines.end(); il++, nl++) {
      std::string aLine(*il);
      if (aLine.find("<box name=\"ssddouble_bkpln_box") != std::string::npos) { 
        ilI1  = il; ilI1++; ilI1++; nl+= 3; break;
      } 
    }
    //
    // We create three big boxes, emcompassing mother volumes of existing stations..  
    //  Volume is air, air to air, we do not add any materials.
    //  scale factor tune to the maximum yaw or Pitch angle of 5 degrees. Roughly! 
    //
    const double extraLengthSingle = 1.2 *( 1. + 150.0*(M_PI*sigYP/180.)); // 1.2 safety factor ??? 
    const double extraLengthRotate = 1.7 *( 1. + 200.0*(M_PI*sigYP/180.)); // 1.7 empirically determined for sigYP = 3.8 degrees. 
    const double extraLengthDouble = 1.2 * (1. + 300.0*(M_PI*0.5*sigYP/180.)); 
   
    std::ostringstream addBox1StrStr; addBox1StrStr << 
	"	  <box name=\"ssdStationsingleSup_box\" x=\"1.2*ssdStationsingleWidth\"" <<
	                 " y=\"1.2*ssdStationsingleHeight\" " << 
			 " z=\"" << extraLengthSingle << "+ssdStationsingleLength\" />"; 
    std::string addBox1Str(addBox1StrStr.str());
    ilI1 = fLines.insert(ilI1, addBox1Str); ilI1++; nl++;
    
    std::ostringstream addBox2StrStr; addBox2StrStr << 
	"	  <box name=\"ssdStationrotateSup_box\" x=\"1.2*ssdStationrotateWidth\"" <<
	                 " y=\"1.2*ssdStationrotateHeight\" " << 
		" z=\"" << extraLengthRotate << "+ssdStationrotateLength\" />"; 
    std::string addBox2Str(addBox2StrStr.str());
    ilI1 = fLines.insert(ilI1, addBox2Str); ilI1++; nl++;
    
     std::ostringstream addBox3StrStr; addBox3StrStr << 
	"	  <box name=\"ssdStationdoubleSup_box\" x=\"1.2*ssdStationdoubleWidth\"" <<
	          " y=\"1.2*ssdStationdoubleHeight\" " << 
		  " z=\"" << extraLengthDouble << "+ssdStationdoubleLength\" />"; 
    std::string addBox3Str(addBox3StrStr.str());
    ilI1 = fLines.insert(ilI1, addBox3Str); ilI1++; nl++;
    std::string blLine("          ");
    ilI1 = fLines.insert(ilI1, blLine); ilI1++;
//
// Now, after creating the stations, we insert these station in those bounding boxes. With the adhoc rotations
//      
    std::vector<std::string>::iterator ilI2 =  ilI1;
    nl=0;  
    for (std::vector<std::string>::iterator il = ilI1; il != fLines.end(); il++, nl++) {
      std::string aLine(*il);
      if (aLine.find("ssdStationdouble1_vol") != std::string::npos) {
        ilI2  = il; ilI2++; nl++;
       for (std::vector<std::string>::iterator il2 = ilI2; il2 != fLines.end(); il2++, nl++) {
          std::string aLine2(*il2);
          if (aLine2.find("</volume>") != std::string::npos) { 
             ilI2  = il2; ilI2++; ilI2++;
	     break;
	  }
	}
	break;
      } 
    }
    std::string aNewLine000("	  <volume name=\"ssdStationsingle0Sup_vol\">"); 
    std::string aNewLine001("	  <volume name=\"ssdStationsingle1Sup_vol\">"); 
    std::string aNewLine010("	  <volume name=\"ssdStationrotate0Sup_vol\">"); 
    std::string aNewLine011("	  <volume name=\"ssdStationrotate1Sup_vol\">"); 
    std::string aNewLine020("	  <volume name=\"ssdStationdouble0Sup_vol\">"); 
    std::string aNewLine021("	  <volume name=\"ssdStationdouble1Sup_vol\">"); 

    std::string aNewLineMat("	         <materialref ref=\"Air\"/>"); 

    std::string aNewLinebs("	         <solidref ref=\"ssdStationsingleSup_box\"/>"); 
    std::string aNewLinebr("	         <solidref ref=\"ssdStationrotateSup_box\"/>"); 
    std::string aNewLinebd("	         <solidref ref=\"ssdStationdoubleSup_box\"/>"); 
    
    std::string aNewLine100("	         <physvol name=\"ssdStationsingle0_phys\">"); 
    std::string aNewLine101("	         <physvol name=\"ssdStationsingle1_phys\">"); 
    std::string aNewLine110("	         <physvol name=\"ssdStationrotate0_phys\">"); 
    std::string aNewLine111("	         <physvol name=\"ssdStationrotate1_phys\">"); 
    std::string aNewLine120("	         <physvol name=\"ssdStationdouble0_phys\">"); 
    std::string aNewLine121("	         <physvol name=\"ssdStationdouble1_phys\">"); 
    
     
    std::string aNewLine200("	             <volumeref ref=\"ssdStationsingle0_vol\"/>"); 
    std::string aNewLine201("	             <volumeref ref=\"ssdStationsingle1_vol\"/>"); 
    std::string aNewLine210("	             <volumeref ref=\"ssdStationrotate0_vol\"/>"); 
    std::string aNewLine211("	             <volumeref ref=\"ssdStationrotate1_vol\"/>"); 
    std::string aNewLine220("	             <volumeref ref=\"ssdStationdouble0_vol\"/>"); 
    std::string aNewLine221("	             <volumeref ref=\"ssdStationdouble1_vol\"/>"); 

    std::string aNewLine2xx("	             <rotationref ref=");
    std::string aNewLine300(aNewLine2xx + myNamesRot[0] + std::string(" />")); 
    std::string aNewLine301(aNewLine2xx + myNamesRot[1] + std::string(" />")); 
    std::string aNewLine310(aNewLine2xx + myNamesRot[2] + std::string(" />")); 
    std::string aNewLine311(aNewLine2xx + myNamesRot[3] + std::string(" />")); 
    std::string aNewLine320(aNewLine2xx + myNamesRot[4] + std::string(" />")); 
    std::string aNewLine321(aNewLine2xx + myNamesRot[5] + std::string(" />"));
     
    std::string aNewLineCenter("	             <positionref ref=\"center\"/>"); 
    std::string aNewLineEP("	        </physvol>"); 
    std::string aNewLineEV("	  </volume>"); 
   
    ilI2 = fLines.insert(ilI2, aNewLine000); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineMat); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLinebs); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine100); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine200); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineCenter); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine300); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEP); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEV); ilI2++;
    ilI2 = fLines.insert(ilI2, blLine); ilI2++;
    
    ilI2 = fLines.insert(ilI2, aNewLine001); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineMat); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLinebs); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine101); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine201); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineCenter); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine301); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEP); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEV); ilI2++;
    ilI2 = fLines.insert(ilI2, blLine); ilI2++;
    
    ilI2 = fLines.insert(ilI2, aNewLine010); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineMat); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLinebr); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine110); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine210); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineCenter); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine310); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEP); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEV); ilI2++;
    ilI2 = fLines.insert(ilI2, blLine); ilI2++;
    
    ilI2 = fLines.insert(ilI2, aNewLine011); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineMat); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLinebr); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine111); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine211); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineCenter); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine311); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEP); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEV); ilI2++;
    ilI2 = fLines.insert(ilI2, blLine); ilI2++;
    
    ilI2 = fLines.insert(ilI2, aNewLine020); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineMat); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLinebd); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine120); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine220); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineCenter); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine320); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEP); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEV); ilI2++;
    ilI2 = fLines.insert(ilI2, blLine); ilI2++;
    
    ilI2 = fLines.insert(ilI2, aNewLine021); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineMat); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLinebd); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine121); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine221); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineCenter); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLine321); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEP); ilI2++;
    ilI2 = fLines.insert(ilI2, aNewLineEV); ilI2++;
    ilI2 = fLines.insert(ilI2, blLine); ilI2++;
   
    // Now we replace in the spectrometer volume the untilted stations with the one which contains the 
    // tilted volumes.  
    std::vector<std::string>::iterator ilI3 = ilI2;
    for (std::vector<std::string>::iterator il = ilI3; il != fLines.end(); il++) {
      std::string aLine(*il);
      if (aLine.find("ssdStation0_phys") != std::string::npos) { ilI3 = il; ilI3++; break; }
    }
    std::string aLineI3(*ilI3);
    std::string keyRepI3_0("ssdStationsingle0_vol");
    size_t iPosI3 = aLineI3.find(keyRepI3_0);
    if (iPosI3 == std::string::npos) {
      std::cerr << " EmphMisaligner::doSSDYawPitchOnStations, logic problem at line " << aLineI3 << " fatal " << std::endl; 
      exit(2);
    }
    aLineI3.replace(iPosI3, keyRepI3_0.length(), "ssdStationsingle0Sup_vol");
    *ilI3 = aLineI3; ilI3++;
    
    for (std::vector<std::string>::iterator il = ilI3; il != fLines.end(); il++) {
      std::string aLine(*il);
      if (aLine.find("ssdStation1_phys") != std::string::npos) { ilI3 = il; ilI3++; break; }
    }
    aLineI3 = (*ilI3);
    std::string keyRepI3_1("ssdStationsingle1_vol");
    iPosI3 = aLineI3.find(keyRepI3_1);
    aLineI3.replace(iPosI3, keyRepI3_1.length(), "ssdStationsingle1Sup_vol");
    *ilI3 = aLineI3; ilI3++;
    
    for (std::vector<std::string>::iterator il = ilI3; il != fLines.end(); il++) {
      std::string aLine(*il);
      if (aLine.find("ssdStation2_phys") != std::string::npos) { ilI3 = il; ilI3++; break; }
    }
    aLineI3 = (*ilI3);
    std::string keyRepI3_2("ssdStationrotate0_vol");
    iPosI3 = aLineI3.find(keyRepI3_2);
    aLineI3.replace(iPosI3, keyRepI3_2.length(), "ssdStationrotate0Sup_vol");
    *ilI3 = aLineI3; ilI3++;
    
    for (std::vector<std::string>::iterator il = ilI3; il != fLines.end(); il++) {
      std::string aLine(*il);
      if (aLine.find("ssdStation3_phys") != std::string::npos) { ilI3 = il; ilI3++; break; }
    }
    aLineI3 = (*ilI3);
    std::string keyRepI3_3("ssdStationrotate1_vol");
    iPosI3 = aLineI3.find(keyRepI3_3);
    aLineI3.replace(iPosI3, keyRepI3_3.length(), "ssdStationrotate1Sup_vol");
    *ilI3 = aLineI3; ilI3++;
    
     for (std::vector<std::string>::iterator il = ilI3; il != fLines.end(); il++) {
      std::string aLine(*il);
      if (aLine.find("ssdStation4_phys") != std::string::npos) { ilI3 = il; ilI3++; break; }
    }
    aLineI3 = (*ilI3);
    std::string keyRepI3_4("ssdStationdouble0_vol");
    iPosI3 = aLineI3.find(keyRepI3_4);
    aLineI3.replace(iPosI3, keyRepI3_4.length(), "ssdStationdouble0Sup_vol");
    *ilI3 = aLineI3; ilI3++;
    
     for (std::vector<std::string>::iterator il = ilI3; il != fLines.end(); il++) {
      std::string aLine(*il);
      if (aLine.find("ssdStation5_phys") != std::string::npos) { ilI3 = il; ilI3++; break; }
    }
    aLineI3 = (*ilI3);
    std::string keyRepI3_5("ssdStationdouble1_vol");
    iPosI3 = aLineI3.find(keyRepI3_5);
    aLineI3.replace(iPosI3, keyRepI3_5.length(), "ssdStationdouble1Sup_vol");
    *ilI3 = aLineI3; ilI3++;
    
   
    
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
