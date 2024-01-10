////////////////////////////////////////////////////////////////////////
/// \brief  The Geometry for the SSD aligner 
///          Used by main SSD Aligner Algo1 package.   
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include "SSDAlignParams.h"
#include "myMPIUtils.h"

namespace emph {
  namespace rbal {
  
     SSDAlignParams::SSDAlignParams():
       fIsPhase1c(false),
       myGeo(emph::rbal::BTAlignGeom::getInstance()),
       myGeo1c(emph::rbal::BTAlignGeom1c::getInstance()),  
       fNumStations(myGeo->NumStations()), 
       fNumSensorsXorY(myGeo->NumSensorsXorY()), 
       fNumSensorsU(myGeo->NumSensorsU()), 
       fNumSensorsV(myGeo->NumSensorsV()),
       fMode("2DY"), // Currently, 2DX, 2DY, 3D Default is 2DY (no magnetic deflection, to 1rst order, so, easiest. 
       fMoveLongByStation(true),
       fUseSoftLimits(false), // irrelevant here, I would think.. 
       fStrictSt6(true),
       fSpecificView('A'),
       fSpecificSensor(INT_MAX)
     {
         // The choice of the option code is rather confusing, 
	 // I simply did not know ahead of time how much trial & error were needed.  
        fSubTypeDirectory.push_back(std::string("NoFixes")); // not advised! 
        fSubTypeDirectory.push_back(std::string("TrShift"));
        fSubTypeDirectory.push_back(std::string("Tilt"));
	fSubTypeDirectory.push_back(std::string("TrShiftMagnetKick")); 
        fSubTypeDirectory.push_back(std::string("TrTiltShift"));
	fSubTypeDirectory.push_back(std::string("TrShiftMagnetKick"));
	fSubTypeDirectory.push_back(std::string("MagnetKick"));
        fSubTypeDirectory.push_back(std::string("TrRollCenterShift"));
	fSubTypeDirectory.push_back(std::string("TrRollCenterShiftXYOnly"));
	fSubTypeDirectory.push_back(std::string("TrRollShift"));
	fSubTypeDirectory.push_back(std::string("TrTiltRollShift"));
	fSubTypeDirectory.push_back(std::string("TrShiftX456"));
	fSubTypeDirectory.push_back(std::string("TrShiftXYOnly"));
	fSubTypeDirectory.push_back(std::string("TrShiftXYWOnly"));
	fSubTypeDirectory.push_back(std::string("TrShiftXYUOnly"));
	fSubTypeDirectory.push_back(std::string("TrShiftXOnly"));
	fSubTypeDirectory.push_back(std::string("TrShiftYOnly"));
	fSubTypeDirectory.push_back(std::string("TrZShift"));
	fSubTypeDirectory.push_back(std::string("ZShift"));
	fSubTypeDirectory.push_back(std::string("PitchCorr"));
	fSubTypeDirectory.push_back(std::string("TrTiltShift"));
	fSubTypeDirectory.push_back(std::string("TrTiltRollShift"));
	fSubTypeDirectory.push_back(std::string("DeltaRoll"));
	fSubTypeDirectory.push_back(std::string("DeltaRollCenter"));
        this->ReLoad(); 
     }
     //
     // Setters 
     //
     void SSDAlignParams::ReLoad() {
       std::cerr << " SSDAlignParams::ReLoad, mode is " << fMode << " NumSensor X or Y " << fNumSensorsXorY << std::endl;
       fDat.clear();
       int aMinNumber=0;
//        const double pitchCorrLimit = 3.0*0.005; // ~ 170 mRad. 
//        const double rollCorrLimit = 0.18; // ~ 10 degrees.  Large
//        const double rollCorrLimit = 0.09; // ~ 5 degrees.  More reasnable
        const double rollCorrLimit = fLimRolls; // December 6 2023.. 
        const double pitchCorrLimit = 0.09; // ~ 5 degrees.  
	const double rollCenterLimit= 250.; // for real data... 
//	const double rollCenterLimit= 25.; // for testing 
//       const double pitchCorrLimit = 1.0*0.005; // ~ 100 mRad. 
       if (fMode == std::string("2DY") || fMode == std::string("3D")) { 
	 for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) {
//           if (fStrictSt6 && ((kSe == 4) || (kSe == 6))) continue;  // We skip all the parameters for send sensor of station 4 and 6.  
           // Not for MC.. 
	   SSDAlignParam aPar;
	   aPar.SetForPhase1c(fIsPhase1c); 
	   aPar.SetView('Y'); aPar.SetSensor(kSe);
	   aPar.SetType(emph::rbal::TRSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-20., 20.0)); // Phase1c... Disgusting... 
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1)) { 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	   aPar.SetType(emph::rbal::ZSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-50., 50.0));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1) && (kSe != fNumSensorsXorY-2)) { 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	   aPar.SetType(emph::rbal::PITCHCORR); 
	   aPar.SetLimits(std::pair<double, double>(-pitchCorrLimit, pitchCorrLimit)); // Should be always positiv, max. tilt of ~ 36 degrees.
	   // For station 0, Y view, we find that the fit prefers negative values.. ==> strip pitch is a bit wider than 60 microns?  
	   // re-scale that, way too large..  
	   aPar.SetValue(1.0e-6);
	   aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); // deep copy.. I hope.. 
	 }
	 if (fMode == std::string("2DY")) return;
       }
       if (fMode == std::string("2DX") || fMode == std::string("3D")) { 
	 for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) {
//           if (fStrictSt6 && ((kSe == 4) || (kSe == 6))) continue;  // We skip all the parameters for send sensor of station 4 and 6.  
	   SSDAlignParam aPar; 
	   aPar.SetForPhase1c(fIsPhase1c); 
	   aPar.SetView('X'); aPar.SetSensor(kSe);
	   aPar.SetType(emph::rbal::TRSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-7.5, 7.5));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
//	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1)) { // May 20 2023: we now define the coordinate system base on a Z axis 
//             defince by view Y, first plane in Station 0, and station 5, 2nd sensor (Y5b)
//             We let the position of X5b vary.., as we fix the Roll of Y0 fixed at 0 degrees. 
	   if (kSe > 0) {
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	   aPar.SetType(emph::rbal::PITCHCORR); 
	   aPar.SetLimits(std::pair<double, double>(-pitchCorrLimit, pitchCorrLimit)); // Should be always positiv, max. tilt of ~ 36 degrees. Way too much. 
	   // Readjust to something much smaller.. 
	   aPar.SetValue(1.0e-6);
	   aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); // deep copy.. I hope.. 
	   if (fMoveLongByStation) continue; 
	   aPar.SetType(emph::rbal::ZSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-50., 50.0));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1) && (kSe != fNumSensorsXorY-2)) {
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	 }
	 SSDAlignParam aPar2;
	 aPar2.SetForPhase1c(fIsPhase1c); 
	 aPar2.SetType(emph::rbal::ZMAGC); 
	 aPar2.SetView('X'); aPar2.SetSensor(0);
	 aPar2.SetValue(myGeo->ZCoordsMagnetCenter()); // to be refined, once we align from data from Phase1b 
	 aPar2.SetLimits(std::pair<double, double>(myGeo->ZCoordsMagnetCenter()-10., myGeo->ZCoordsMagnetCenter() + 10.0));
	 aPar2.CheckAndComposeName(); aPar2.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar2);
	 aPar2.SetType(emph::rbal::KICKMAGN); 
	 const double kick = myGeo->MagnetKick120GeV(); aPar2.SetValue(kick);
	 if (kick > 0.) aPar2.SetLimits(std::pair<double, double>(0.5*kick, 2.0*kick));
	 else  aPar2.SetLimits(std::pair<double, double>(2.0*kick, 0.5*kick));
	 aPar2.CheckAndComposeName();aPar2.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar2);
	 if (fMode == std::string("2DX")) return;
      }
      if (fMode == std::string("3D")) {  // Adding U, V views .  But first, Rolls and Pitch size for X and Y views. 
         std::vector<char> views{'X', 'Y', 'U', 'V'};
	 for (size_t kV = 0; kV !=2; kV++) { 
 	   for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { // Reference frame is defined, station 0 & station 5 could have tilts and Rolls
	     // May 20 2023:  Nop, we define the reference frame by fixing the roll of Y0 to zero. 
	     if ((kV == 1) && (kSe == 0)) continue;
	     SSDAlignParam aPar; 
	     aPar.SetForPhase1c(fIsPhase1c); 
	     aPar.SetView(views[kV]); aPar.SetSensor(kSe);
	     aPar.SetType(emph::rbal::ROLL); 
	     aPar.SetLimits(std::pair<double, double>(-rollCorrLimit, rollCorrLimit));
//	     aPar.SetValue(-0.25*rollCorrLimit); // test.. Should be 0. by default.. 
	     aPar.SetValue(0.); // test.. Should be 0. by default.. 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	     aPar.SetType(emph::rbal::ROLLC); 
	     aPar.SetLimits(std::pair<double, double>(-rollCenterLimit, rollCenterLimit));
	     aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	   }
         }
	 // U views and V views 
	 std::vector<size_t> nums{fNumSensorsU, fNumSensorsV};
	 for (size_t kV = 2; kV !=4; kV++) { 
 	   for (size_t kSe=0; kSe != nums[kV-2]; kSe++) {
	     SSDAlignParam aPar; 
	     aPar.SetForPhase1c(fIsPhase1c); 
	     aPar.SetType(emph::rbal::TRSHIFT);
	     aPar.SetView(views[kV]); aPar.SetSensor(kSe);
	     // for MC.. To study for data.. (sign convention problem.. ) 
//	     if (kV == 3) aPar.SetLimits(std::pair<double, double>(-15., 15.)); // Not clear what the offsets are.. Tuning V views 
//	     if ((kV == 3) && (kSe == 3))  aPar.SetLimits(std::pair<double, double>(-15., 15.0)); // Not clear what the offsets are.. Tuning V views 
//	     if (kV == 2) aPar.SetLimits(std::pair<double, double>(-15., 15.0)); // Checked U , offsets are indeed small. 
             aPar.SetLimits(std::pair<double, double>(-20., 20.0)); 
	     aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); // deep copy.. I hope.. 
	     if (!fMoveLongByStation) {
	       aPar.SetType(emph::rbal::ZSHIFT); 
	       aPar.SetLimits(std::pair<double, double>(-50., 50.0));
	       aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	       aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	     }
	     aPar.SetType(emph::rbal::PITCHCORR); 
	     aPar.SetLimits(std::pair<double, double>(-pitchCorrLimit, pitchCorrLimit)); // always positiv, max. tilt of ~ 36 degrees. 
	     aPar.SetValue(1.0e-6);
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	     aPar.SetType(emph::rbal::ROLL); 
	     aPar.SetLimits(std::pair<double, double>(-rollCorrLimit, rollCorrLimit));
	     aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	     aPar.SetType(emph::rbal::ROLLC); 
	     aPar.SetLimits(std::pair<double, double>(-rollCenterLimit, rollCenterLimit));
	     aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	  } // on Sensors  
        } // on Views 
      } // 3D
       
//      std::string aTokenTmp("FromReload");
//      this->DumpTable(aTokenTmp);
    } // Reload
    
    void SSDAlignParams::DumpTable(const std::string &token) const {
      std::string fName("./AlignParams_"); fName += token; fName += std::string("_"); fName += fMode; 
      fName += std::string("_V1.txt");
      std::ofstream fOut(fName.c_str());
      fOut << " name number value DownLimit UpLimit " << std::endl;
      for (std::vector<SSDAlignParam>::const_iterator it=fDat.cbegin(); it!=fDat.cend(); it++) { 
        fOut << " " << it->Name() << " " << it->MinuitNumber() << " " 
	     << it->Value() << " " << it->Limits().first << " " << it->Limits().second << std::endl;
      }
      fOut.close();
    } 
//    
    void SSDAlignParams::LoadValueFromPreviousRun(const std::string token, bool isSimple) {
      // Get the rank of the process
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      // a Place holder.  Only rank 0 reads the file. 
      std::vector<double> allVals(fDat.size(), 0.);
        // Works only for 10 iterations.. 
      if (myRank == 0) {
        std::string cIterNum = token.substr(token.length()-1, 1);
        int aPrevIterNum = std::atoi(cIterNum.c_str()) - 1;
        std::ostringstream fNameStrStr;
	if (isSimple) {  
          fNameStrStr << "./MinValues_Simplex_OK_" << token.substr(0, token.length()-1) << aPrevIterNum << ".txt";
	} else { // old code, when I thought Migrad was better.. 
          fNameStrStr << "./MinValues_Migrad_OK_" << token.substr(0, token.length()-1) << aPrevIterNum << ".txt";
	}
        std::string fNameStr(fNameStrStr.str());
        std::cerr << " SSDAlignParams::LoadValueFromPreviousRun, current token is " 
	          << token << " filename is "  << fNameStr << std::endl;
        std::ifstream fIn(fNameStr.c_str());
        if (!fIn.is_open()) {
           std::cerr << "SSDAlignParams::LoadValueFromPreviousRun , failed to open " << fNameStr << " Not fatal, Use the NotOK file " << std::endl; 
           std::ostringstream fNameStrStr2;
	   if (isSimple) {  
             fNameStrStr2 << "./MinValues_Simplex_NotOK_" << token.substr(0, token.length()-1) << aPrevIterNum << ".txt";
	   } else { // old code, when I thought Migrad was better.. 
             fNameStrStr2 << "./MinValues_Migrad_NotOK_" << token.substr(0, token.length()-1) << aPrevIterNum << ".txt";
	   }
           std::string fNameStr2(fNameStrStr2.str());
           std::ifstream fIn2(fNameStr2.c_str());
           if (!fIn2.is_open()) {
             std::cerr << "SSDAlignParams::LoadValueFromPreviousRun , failed to open " << fNameStr << " Not even NotOK, fatal quit here " << std::endl; exit(2);
	   }
	   fIn2.close();
	   fIn.open(fNameStr2.c_str());
	}
	std::vector<SSDAlignParam>::iterator it=fDat.begin();
	char aLine[1024]; size_t nLines=0;
	while (fIn.good()) {
	   fIn.getline(aLine, 1024);
           std::string aLStr(aLine);
           std::istringstream aLStrStr(aLine);
	   std::string aName; double aVal; double aErr; 
	   aLStrStr >> aName >> aVal >> aErr; 
	   // exception for TransShift_Y_7, which we introduce to do consistent 3D in art.. 
	   if ((!fIsPhase1c) && aName.find("TransShift_Y_7") != std::string::npos) continue; // Reference frame fixing, no such paramter.. 
	   if ((fIsPhase1c) && aName.find("TransShift_Y_8") != std::string::npos) continue;
	   if ((aName.find(it->Name()) == std::string::npos) && (it->Name().find(aName) == std::string::npos)) {
	     std::cerr << "SSDAlignParams::LoadValueFromPreviousRun, out of order param " << aName 
	               << " already loaded " << it->Name() << " fatal, quit here and now " << std::endl; exit(2);
	   }
	   allVals[nLines] = aVal;
	   it->SetValue(aVal); 
	   it++;
	   nLines++;
	   if (it == fDat.end()) break;
	}
	this->CheatWithTruthValues(); 
	
      }
      emph::rbal::broadcastFCNParams(allVals);
    }
    void SSDAlignParams::LoadValueFromPreviousFCNHistory(const std::string token, int reqNCalls) {
      // Get the rank of the process
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      // a Place holder.  Only rank 0 reads the file. 
      std::vector<double> allVals(fDat.size(), 0.);
      if (myRank == 0) {
        std::string cIterNum = token.substr(token.length()-1, 1);
        int aPrevIterNum = std::atoi(cIterNum.c_str()) - 1;
        std::ostringstream fNameStrStr;
        fNameStrStr << "./BeamTrackSSDAlignFCN_" << token.substr(0, token.length()-1) << aPrevIterNum << "_V1.txt";
        std::string fNameStr(fNameStrStr.str());
        std::cerr << " SSDAlignParams::LoadValueFromPreviousFCNHistory, current token is " 
	          << token << " filename is "  << fNameStr << std::endl;
        std::ifstream fIn(fNameStr.c_str());
        if (!fIn.is_open()) {
           std::cerr << "SSDAlignParams::LoadValueFromPreviousFCNHistory , failed to open " << fNameStr << " fatal, quit here.. " << std::endl; exit(2);
	}
	std::vector<SSDAlignParam>::iterator it=fDat.begin();
	char aLine[4096]; size_t nLines=0;
        std::string lastLine;
	std::string headerLine;
	int nCallsPrev = 0;
	std::string reqCallLine;
	int nCC;
	while (fIn.good()) {
	   fIn.getline(aLine, 4096);
           std::string aLStr(aLine);
	   if (nLines == 0) {
	     std::string hh(" nCalls chi2 ");
	     headerLine = aLStr.substr(hh.length(), std::string::npos); 
	   } else {
	     if (aLStr.length() > 10) {
                std::istringstream aVStr(aLStr);
		aVStr >> nCC; 
		if (nCC == reqNCalls) reqCallLine = aLStr;
	        lastLine = aLStr;
	     }
	     nCallsPrev++;
	   }
	   nLines++;
	 }
	 std::cerr << " SSDAlignParams::LoadValueFromPreviousFCNHistory , number of FCN calls found " << nCallsPrev << std::endl;
         std::istringstream aHStr(headerLine);
	 if (reqCallLine.length() > 10) lastLine = reqCallLine;
         std::istringstream aVStr(lastLine);
	 double nCalls; double chi2;
	 aVStr >> nCalls >> chi2; 
	 double aVal;
	 std::string aName; 
	 for(std::vector<SSDAlignParam>::iterator it=fDat.begin(); it!=fDat.end(); it++) { 
	   aHStr >> aName;
	   aVStr >> aVal;
	   std::string aNameP = it->Name(); 
	   if (aNameP.find(aName) == std::string::npos) { 
	     std::cerr << "SSDAlignParams::LoadValueFromPreviousFCNHistory, out of order param " << aNameP 
	               << " defined " << it->Name() << " fatal, quit here and now " << std::endl; exit(2);
	   }
	   std::cerr << " ..... Setting parameter " << aName << " to " << aVal << std::endl;
	   it->SetValue(aVal);
	 }
       }
       emph::rbal::broadcastFCNParams(allVals);
    
    }
    void SSDAlignParams::RandomizeRollsAndRollCenters(double rollW, double rollCW) {
      for(std::vector<SSDAlignParam>::iterator it=fDat.begin(); it!=fDat.end(); it++) { 
        switch (it->Type()) {
          case emph::rbal::ROLL : case emph::rbal::ROLLC : {  
	    const double fact = (it->Type() == emph::rbal::ROLL) ? rollW : rollCW;
	    const double range = fact * std::abs(it->UpLimit() - it->DownLimit());
	    int i1 = std::rand(); int i2 = std::rand(); 
	    const double aSign = (i1 < RAND_MAX/2) ? -1.0 : 1.0; 
	    const double newVDelta = aSign * range * static_cast<double>(i2)/static_cast<double>(RAND_MAX);
	    const double aPrevVal = it->Value(); 
	    it->SetValue(newVDelta+aPrevVal);
	  }
	  default: { continue; } 
	}
      }
    }
    void SSDAlignParams::CheatWithTruthValues() {
     // Back door, should not be called..Debugging truth table... 
     
    //  this->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.38);
    //  this->SetValue(emph::rbal::TRSHIFT, 'X', 2, 1.37 );
    
    }
    void SSDAlignParams::SetParamsForG4EMPHRun5c() {  // volatile, we will keep updating by hand.. Could use Load from previous run
      /*
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 1, 0.449707991);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 2, -0.937927699);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 3, 0.05472351318);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 4, 3.03039755);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 5, 2.896809396);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 6, 2.898251805);	
      // from  Try3D_Sim5c_8b7_1 
      this->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.38);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 2, 1.37 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 3, 2.42 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 4, 4.03 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 5, 2.63 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 6, 3.78 );	       
     // from  Try3D_Sim5c_8c1_1  similar.. 
     // Raise the Edm to get success.  Try3D_Sim5c_8c2_1 TrShiftXYOnly. 
      this->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.37);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 2, 1.31 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 3, 2.37 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 4, 3.91 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 5, 2.35 );
      this->SetValue(emph::rbal::TRSHIFT, 'X', 6, 3.91 );
     // from  Try3D_Sim5c_8d1_1  similar.. 
     // Same high Edm, but with the U and V planes: 
     */
      this->SetValue(emph::rbal::TRSHIFT, 'X', 1,  1.30528);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 2,  1.40432);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 3,  2.42377);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 4,  4.3912);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 5,  2.39239);
      this->SetValue(emph::rbal::TRSHIFT, 'X', 6,  4.15928);  	    
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  0.457939);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -0.905573);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  0.0796809);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 4,  2.94305);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 5,  3.01591);
      this->SetValue(emph::rbal::TRSHIFT, 'Y', 6,  2.8699);  
      this->SetValue(emph::rbal::TRSHIFT, 'U', 0,  0.726008);
      this->SetValue(emph::rbal::TRSHIFT, 'U', 1,  2.85987);
      this->SetValue(emph::rbal::TRSHIFT, 'V', 0,  -5.42706);
      this->SetValue(emph::rbal::TRSHIFT, 'V', 1,  -4.71627);
      this->SetValue(emph::rbal::TRSHIFT, 'V', 2,  -1.262);
      this->SetValue(emph::rbal::TRSHIFT, 'V', 3,  -1.15554);
      //
     } 	       
     void SSDAlignParams::SetParamsForG4EMPHRun6g() {  // volatile, we will keep updating by hand.. Could use Load from previous run
      this->SetValue(std::string("TransShift_Y_1"), 1.61217   );
      this->SetValue(std::string("TransShift_Y_2"), -0.77719  );
      this->SetValue(std::string("TransShift_Y_3"), 2.26884   );
      this->SetValue(std::string("TransShift_Y_4"), 2.88277   );
      this->SetValue(std::string("TransShift_Y_5"), 2.65709   );
      this->SetValue(std::string("TransShift_Y_6"), 1.36515   );	  
      this->SetValue(std::string("TransShift_X_1"), 0.449348  );
      this->SetValue(std::string("TransShift_X_2"), 0.363069  );
      this->SetValue(std::string("TransShift_X_3"), -0.32250  );
      this->SetValue(std::string("TransShift_X_4"), 1.19214   );
      this->SetValue(std::string("TransShift_X_5"), 2.59275   );
      this->SetValue(std::string("TransShift_X_6"), -1.61661  );  
      this->SetValue(std::string("TransShift_U_0"), 2.98277   );
      this->SetValue(std::string("TransShift_U_1"), 1.78986   );
      this->SetValue(std::string("TransShift_V_0"), -3.21657  );
      this->SetValue(std::string("TransShift_V_1"), -6.94465  );
      this->SetValue(std::string("TransShift_V_2"), -2.06705  );
      this->SetValue(std::string("TransShift_V_3"), -6.05039  );
    }
     void SSDAlignParams::SetParamsForG4EMPHRun6h() {  // volatile, we will keep updating by hand.. Could use Load from previous run
      this->SetValue(std::string("TransShift_Y_1"), 1.96   );
      this->SetValue(std::string("TransShift_Y_2"), -0.94  );
      this->SetValue(std::string("TransShift_Y_3"), 2.26884   );
      this->SetValue(std::string("TransShift_Y_4"), 3.66   );
      this->SetValue(std::string("TransShift_Y_5"), 2.55   );
      this->SetValue(std::string("TransShift_Y_6"), 2.66   );		  
    }	
    
    void SSDAlignParams::SetMinuitParamFixes(const std::string &fitSubType, int pencilBeamMode) {
      if (!this->isSubFitTypeValid(fitSubType)) {
        std::cerr << " SSDAlignParams::SetMinuitParamFixes Invalid fitSubType, stop here and now " << std::endl; exit(2); 
        // MPI won't be happy, but, moving on.. 
      }       
      if (fitSubType == std::string("NoFixes")) {
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) it->SetFixedInMinuit(false);
	return;
      }
//      std::cerr << " SSDAlignParams::SetMinuitParamFixes, fitSubType " << fitSubType 
//               << " pencilBeamMode " << pencilBeamMode <<  " Specific Sensor " 
//		<< fSpecificSensor << " spec. view " << fSpecificView << std::endl;
// 
      for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
        it->SetFixedInMinuit(true); // by default, nothing moves. 
	const std::string aName(it->Name());
	// Confusing nomenclature.. Sorry about that.. 
//	 std::cerr << " ..... At Name " << aName << std::endl;
	if ((aName.find("TransShift") == 0) && (fitSubType.find("Tr") != std::string::npos) 
	                                  && (fitSubType.find("Shift") != std::string::npos)) it->SetFixedInMinuit(false);
	if ((aName.substr(0,9) == std::string("DeltaRoll")) && (aName.find("Center") == std::string::npos) && 
	    (fitSubType.find("Roll") != std::string::npos))  it->SetFixedInMinuit(false);
	if ((aName.substr(0,15) == std::string("DeltaRollCenter")) && (fitSubType.find("Center") != std::string::npos))  it->SetFixedInMinuit(false);
	if ((aName.substr(0,9) == std::string("LongShift")) && (fitSubType.find("ZShift") != std::string::npos))  it->SetFixedInMinuit(false);
	if ((aName.substr(0,4) == std::string("Tilt")) && (fitSubType.find("Tilt") != std::string::npos))  it->SetFixedInMinuit(false);
      // Magnet related stuff.. 
        
//	if (!it->isFixedInMinuit()) std::cerr <<  " ... Really Preliminary... We will vay " << aName << std::endl; 
	if (((fitSubType == std::string("MagnetZPos")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("LongMagC") == 0)) it->SetFixedInMinuit(false);   
	if (((fitSubType == std::string("MagnetKick")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("KickMag") == 0)) it->SetFixedInMinuit(false);
	if (((fitSubType == std::string("TrShiftMagnetKick")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("KickMag") == 0)) it->SetFixedInMinuit(false);
	 // try tilt and KickMag    
	if (((fitSubType == std::string("TrTiltShiftMagnetKick"))) &&
	      (aName.find("KickMag") == 0)) it->SetFixedInMinuit(false);
	 //
	 // decided... not quite ... 
	 //
//	 if (!it->isFixedInMinuit()) std::cerr <<  " ... Preliminary... We will vay " << aName << std::endl; 
	 if ((pencilBeamMode == 1) && (it->isOutOfPencilBeam())) it->SetFixedInMinuit(true);
	 if ((pencilBeamMode == -1) && (!(it->isOutOfPencilBeam()))) it->SetFixedInMinuit(true);
      
	// Now turn back, if we ask for a specific sensor and/or view 				  
        if ((fSpecificSensor != INT_MAX) && (fSpecificSensor != it->SensorI())) it->SetFixedInMinuit(true);   
        if ((fSpecificView != 'A') && (fSpecificView != it->View())) it->SetFixedInMinuit(true);  
//	if (!it->isFixedInMinuit()) std::cerr <<  " ... Final We will vay " << aName << std::endl; 
      }
      // Special cases, transverse shift. 
      if (fitSubType == std::string("TrShiftX456")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  if (aName.find("TransShift") != 0) continue; 
	  if (it->SensorI() > 3) it->SetFixedInMinuit(false);
	}
      } 
      if (fitSubType == std::string("TrShiftXOnly")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  if (aName.find("TransShift") != 0) continue; 
	  if (it->View() == 'X') it->SetFixedInMinuit(false);
	}
      } 
      if (fitSubType == std::string("TrShiftYOnly")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  if (aName.find("TransShift") != 0) continue; 
	  if (it->View() == 'Y') it->SetFixedInMinuit(false);
	}
      } 
      if (fitSubType == std::string("TrShiftXYOnly")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  if (aName.find("TransShift") != 0) continue; 
	  if ((it->View() == 'X') || (it->View() == 'Y')) it->SetFixedInMinuit(false);
	}
      }
      if (fitSubType == std::string("TrShiftXYWOnly")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  if (aName.find("TransShift") != 0) continue; 
	  if ((it->View() == 'X') || (it->View() == 'Y') || (it->View() == 'V')) it->SetFixedInMinuit(false);
	}
      }
      if (fitSubType == std::string("TrShiftXYUOnly")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  if (aName.find("TransShift") != 0) continue; 
	  if ((it->View() == 'X') || (it->View() == 'Y') || (it->View() == 'U')) it->SetFixedInMinuit(false);
	}
      }
      if (fitSubType == std::string("TrRollCenterShiftXYOnly")) { // Phase1b 
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
          it->SetFixedInMinuit(true); // by default, nothing moves. 
	  const std::string aName(it->Name());
	  const bool varying =  ((aName.find("TransShift") == 0) || (aName.find("DeltaRoll") == 0)); // include the DeltaRollCenter_X_2
	  if (!varying) continue;
	  if ((it->View() == 'X') || (it->View() == 'Y')) it->SetFixedInMinuit(false);
	  if ((it->View() == 'X') || (it->View() == 'Y')) it->SetFixedInMinuit(false);
	}
      }
      
      //
      // No good EDM for TransShifts, with no rolls.. And simulated data!  Then, indeed, it could make sens to fix Station 5 sensors. All of  them..      
      //
//      for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
        // August 7 & 9  : fix a new reference frame.. Test on one signle TransShift (Y_1)  (after bug fix in distributeEvents.., August 9) 
//        if ((it->Name() == std::string("TransShift_X_6")) || 
//	    (it->Name() == std::string("TransShift_X_7")) || 
//	    (it->Name() == std::string("TransShift_Y_6")) || 
//	    (it->Name() == std::string("TransShift_Y_7")) )   it->SetFixedInMinuit(true); 
// August 8 : make sure we have a vail Simplex minimum with ~ 13900 tr
//            if (it->Name().find("TransShift") != 0) continue; // already set to fix if we use TransShift submode. 
//	    if (it->Name() == std::string("TransShift_Y_1")) continue;
//	    it->SetFixedInMinuit(true); 
//      }
//      std::cerr << " ..... And quit for now!... " << std::endl; exit(2);
      return;
    } 
    void SSDAlignParams::FixParamsForView(const char aView, bool isTrue, const std::string &paramName) {
      for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
        if (isTrue) {
          if (it->isFixedInMinuit()) continue;
	  if (it->View() == aView) it->SetFixedInMinuit(true);
	} else {
	  if ((paramName == std::string("")) && (it->View() == aView)) it->SetFixedInMinuit(false);
	  if ((paramName == it->Name()) && (it->View() == aView)) it->SetFixedInMinuit(false);
	}
      }  
    }
    void SSDAlignParams::FixParamsForViewLastStation(const char aView, bool isTrue) {
      if ((aView == 'A') && (!fIsPhase1c)) return; 
      if (!isTrue) return; // That argument is not really usefull, as we do not plan to revert the decision along the way..  
      if (aView == 'A') {
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	  if (it->Name().find ("_X_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	  if (it->Name().find ("_X_8") != std::string::npos) it->SetFixedInMinuit(true); 
  	  if (it->Name().find ("_Y_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	  if (it->Name().find ("_Y_8") != std::string::npos) it->SetFixedInMinuit(true); 
   	  if (it->Name().find ("_V_2") != std::string::npos) it->SetFixedInMinuit(true); 
   	  if (it->Name().find ("_V_3") != std::string::npos) it->SetFixedInMinuit(true);
        }
      } else {
        for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
           if (it->isFixedInMinuit()) continue;
	   if (it->View() != aView) continue;
	   if (it->SensorI() < 2) continue; // Valid for both Phase1b and Phase1c 
	   if (it->Type() == emph::rbal::ZMAGC) continue;
	   if (it->Type() == emph::rbal:: KICKMAGN) continue;
	   it->SetFixedInMinuit(true);
        }
      } // All views..   
    } // FixParamsForViewLastStation
    void SSDAlignParams::FixParamsForViewAtStation(const size_t kSt, const char aView) {
      if  (!fIsPhase1c) return; 
      if (kSt < 5) { std::cerr << " SSDAlignParams::FixParamsForViewAtStation not yet implemented...Fatal  " << std::endl; exit(2); } 
      for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
        if (it->isFixedInMinuit()) continue;
        if (it->View() != aView) continue;
	switch (kSt) {
           case 5: {
	    if (it->Name().find ("_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_6") != std::string::npos) it->SetFixedInMinuit(true); 
            if ((aView == 'V') && (it->Name().find("V_0") != std::string::npos))  it->SetFixedInMinuit(true); 
            if ((aView == 'V') && (it->Name().find("V_1") != std::string::npos))  it->SetFixedInMinuit(true); 
	    break;
	  }
           case 6: {
	    if (it->Name().find ("_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_8") != std::string::npos) it->SetFixedInMinuit(true); 
            if ((aView == 'V') && (it->Name().find("V_2") != std::string::npos))  it->SetFixedInMinuit(true); 
            if ((aView == 'V') && (it->Name().find("V_3") != std::string::npos))  it->SetFixedInMinuit(true);
	    break; 
	  }
	  
	}
      } 
    }
    void SSDAlignParams::FixAllButStationView(const size_t kSt, const char aView) {
      if  (!fIsPhase1c) return;
      if (kSt < 5) { std::cerr << " SSDAlignParams::FixAllButStationView not yet implemented...Fatal  " << std::endl; exit(2); } 
      for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
        if (it->isFixedInMinuit()) continue;
	it->SetFixedInMinuit(true);
      } 
      for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
        if (it->View() != aView )  continue;
	if (it->Name().find("Tilt") != std::string::npos) continue;
	switch (kSt) {
           case 5: {
	    if (it->Name().find ("_5") != std::string::npos) it->SetFixedInMinuit(false); 
  	    if (it->Name().find ("_6") != std::string::npos) it->SetFixedInMinuit(false); 
            if ((aView == 'V') && (it->Name().find("V_0") != std::string::npos))  it->SetFixedInMinuit(false); 
            if ((aView == 'V') && (it->Name().find("V_1") != std::string::npos))  it->SetFixedInMinuit(false); 
	    break;
	   }
           case 6: {
	    if (it->Name().find ("_7") != std::string::npos) it->SetFixedInMinuit(false); 
  	    if (it->Name().find ("_8") != std::string::npos) it->SetFixedInMinuit(false); 
            if ((aView == 'V') && (it->Name().find("V_2") != std::string::npos))  it->SetFixedInMinuit(false); 
            if ((aView == 'V') && (it->Name().find("V_3") != std::string::npos))  it->SetFixedInMinuit(false); 
	    break;
	   }
	}
      } 
    }
    void SSDAlignParams::FixParamsForAllViewsAtStation(const int kSt, bool isTrue) {
      if (!fIsPhase1c) return; 
      if (!isTrue) return; // That argument is not really usefull, as we do not plan to revert the decision along the way..  
      switch (kSt) { 
       case 6: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_X_8") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_8") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find ("_V_2") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find ("_V_3") != std::string::npos) it->SetFixedInMinuit(true);
          }
	  break;
       }
       case 5: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_X_6") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_6") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find ("_V_0") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find ("_V_1") != std::string::npos) it->SetFixedInMinuit(true);
          }
	  break;
       }
       case 4: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_4") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_4") != std::string::npos) it->SetFixedInMinuit(true); 
          }
	  break;
       }
       case 3: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_3") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_3") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_U_1") != std::string::npos) it->SetFixedInMinuit(true); 
          }
	  break;
       }
       case 2: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_2") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_2") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_U_0") != std::string::npos) it->SetFixedInMinuit(true); 
          }
	  break;
       }
       case 1: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_1") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_1") != std::string::npos) it->SetFixedInMinuit(true); 
          }
	  break;
       }
       case 0: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find ("_X_0") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find ("_Y_0") != std::string::npos) it->SetFixedInMinuit(true); 
          }
	  break;
       }
       default: break;   
      } // Switch kSt   
    } // FixParamsForAllViewsAtStation
    void SSDAlignParams::FixRollAndRollCenterForStation(const int kSt) {
      if (!fIsPhase1c) return; 
      switch (kSt) { 
       case 6: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_X_8") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_8") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRoll_V_2") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRoll_V_3") != std::string::npos) it->SetFixedInMinuit(true);
 	    if (it->Name().find("DeltaRollCenter_X_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_X_8") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_7") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_8") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRollCenter_V_2") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRollCenter_V_3") != std::string::npos) it->SetFixedInMinuit(true);
          }
	  break;
       }
       case 5: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_X_6") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_6") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRoll_V_0") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRoll_V_1") != std::string::npos) it->SetFixedInMinuit(true);
 	    if (it->Name().find("DeltaRollCenter_X_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_X_6") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_5") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_6") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRollCenter_V_0") != std::string::npos) it->SetFixedInMinuit(true); 
   	    if (it->Name().find("DeltaRollCenter_V_1") != std::string::npos) it->SetFixedInMinuit(true);
          }
	  break;
       }
       case 4: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_4") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_4") != std::string::npos) it->SetFixedInMinuit(true); 
 	    if (it->Name().find("DeltaRollCenter_X_4") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_4") != std::string::npos) it->SetFixedInMinuit(true); 
          }
	  break;
       }
       case 3: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_3") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_3") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_U_1") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_X_3") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_3") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_U_1") != std::string::npos) it->SetFixedInMinuit(true); 
         }
	  break;
       }
       case 2: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_2") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_2") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_U_0") != std::string::npos) it->SetFixedInMinuit(true); 
 	    if (it->Name().find("DeltaRollCenter_X_2") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_2") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_U_0") != std::string::npos) it->SetFixedInMinuit(true); 
          }
       }
       case 1: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_1") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRoll_Y_1") != std::string::npos) it->SetFixedInMinuit(true); 
 	    if (it->Name().find("DeltaRollCenter_X_1") != std::string::npos) it->SetFixedInMinuit(true); 
  	    if (it->Name().find("DeltaRollCenter_Y_1") != std::string::npos) it->SetFixedInMinuit(true); 
          }
       }
       case 0: {
          for (std::vector<SSDAlignParam>::iterator it=fDat.begin(); it != fDat.end(); it++) {
 	    if (it->Name().find("DeltaRoll_X_0") != std::string::npos) it->SetFixedInMinuit(true); 
 	    if (it->Name().find("DeltaRollCenter_X_0") != std::string::npos) it->SetFixedInMinuit(true); 
          }
       }
       default: break;   
      } // Switch kSt   
    } // FixParamsForAllViewsAtStation
    void SSDAlignParams::FixParamsUpstreamMagnet() {
      if (!fIsPhase1c) return;
      for (size_t kSt=0; kSt !=5; kSt++) {
         this->FixParamsForAllViewsAtStation(kSt);
      }
    }
   } // namespace 
}  // namespace   
     
