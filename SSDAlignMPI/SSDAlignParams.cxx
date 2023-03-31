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
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>

#include "SSDAlignParams.h"

namespace emph {
  namespace rbal {
  
     SSDAlignParams::SSDAlignParams():
       myGeo(emph::rbal::BTAlignGeom::getInstance()),
       fNumStations(myGeo->NumStations()), 
       fNumSensorsXorY(myGeo->NumSensorsXorY()), 
       fNumSensorsU(myGeo->NumSensorsU()), 
       fNumSensorsV(myGeo->NumSensorsV()),
       fMode("2DY"), // Currently, 2DX, 2DY, 3D Default is 2DY (no magnetic deflection, to 1rst order, so, easiest. 
       fMoveLongByStation(true),
       fUseSoftLimits(false), // irrelevant here, I would think.. 
       fStrictSt6(true)  
     { 
        this->ReLoad(); 
     }
     //
     // Setters 
     //
     void SSDAlignParams::ReLoad() {
       std::cerr << " SSDAlignParams::ReLoad, mode is " << fMode << std::endl;
       fDat.clear();
       int aMinNumber=0;
//        const double pitchCorrLimit = 3.0*0.005; // ~ 170 mRad. 
        const double pitchCorrLimit = 30.0*0.005; // ~ 170 mRad. 
//       const double pitchCorrLimit = 1.0*0.005; // ~ 100 mRad. 
       if (fMode == std::string("2DY") || fMode == std::string("3D")) { 
	 for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) {
//           if (fStrictSt6 && ((kSe == 4) || (kSe == 6))) continue;  // We skip all the parameters for send sensor of station 4 and 6.  
           // Not for MC.. 
	   SSDAlignParam aPar; 
	   aPar.SetView('Y'); aPar.SetSensor(kSe);
	   aPar.SetType(emph::rbal::TRSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-55., 55.0));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1)) { 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	   aPar.SetType(emph::rbal::ZSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-10., 10.0));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1)) { 
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
	   aPar.SetView('X'); aPar.SetSensor(kSe);
	   aPar.SetType(emph::rbal::TRSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-55., 55.0));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1)) {
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	   aPar.SetType(emph::rbal::PITCHCORR); 
	   aPar.SetLimits(std::pair<double, double>(-pitchCorrLimit, pitchCorrLimit)); // Should be always positiv, max. tilt of ~ 36 degrees. Way too much. 
	   // Readjust to something much smaller.. 
	   aPar.SetValue(1.0e-6);
	   aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); // deep copy.. I hope.. 
	   if (fMoveLongByStation) continue; 
	   aPar.SetType(emph::rbal::ZSHIFT); 
	   aPar.SetLimits(std::pair<double, double>(-25., 25.0));
	   aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	   if ((kSe > 0) && (kSe != fNumSensorsXorY-1)) {
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	   }
	 }
	 SSDAlignParam aPar2;
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
	     SSDAlignParam aPar; 
	     aPar.SetView(views[kV]); aPar.SetSensor(kSe);
	     aPar.SetType(emph::rbal::ROLL); 
	     aPar.SetLimits(std::pair<double, double>(-0.25, 0.25));
	     aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	   }
         }
	 // U views and V views 
	 std::vector<size_t> nums{fNumSensorsU, fNumSensorsV};
	 for (size_t kV = 2; kV !=4; kV++) { 
 	   for (size_t kSe=0; kSe != nums[kV-2]; kSe++) {
	     SSDAlignParam aPar; 
	     aPar.SetType(emph::rbal::TRSHIFT); 
	     aPar.SetView(views[kV]); aPar.SetSensor(kSe);
	     if (kV == 3) aPar.SetLimits(std::pair<double, double>(-150., 150.0)); // Not clear what the offsets are.. Tuning V views 
	     if ((kV == 3) && (kSe == 3))  aPar.SetLimits(std::pair<double, double>(-1500., 1500.0)); // Not clear what the offsets are.. Tuning V views 
	     if (kV == 2) aPar.SetLimits(std::pair<double, double>(-150., 150.0)); // Checked U , offsets are indeed small. 
	     aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); // deep copy.. I hope.. 
	     if (!fMoveLongByStation) {
	       aPar.SetType(emph::rbal::ZSHIFT); 
	       aPar.SetLimits(std::pair<double, double>(-20., 20.0));
	       aPar.SetValue(0.); // to be refined, once we align from data from Phase1b 
	       aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar);
	     }
	     aPar.SetType(emph::rbal::PITCHCORR); 
	     aPar.SetLimits(std::pair<double, double>(-pitchCorrLimit, pitchCorrLimit)); // always positiv, max. tilt of ~ 36 degrees. 
	     aPar.SetValue(1.0e-6);
	     aPar.CheckAndComposeName(); aPar.SetMinuitNumber(aMinNumber); aMinNumber++; fDat.push_back(aPar); 
	     aPar.SetType(emph::rbal::ROLL); 
	     aPar.SetLimits(std::pair<double, double>(-0.25, 0.25));
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
   } // namespace 
}  // namespace   
     
