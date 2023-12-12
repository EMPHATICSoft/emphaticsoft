////////////////////////////////////////////////////////////////////////
/// \brief  The Volatile Geometry for the SSD aligner December 2023: extend to Phase1c
///         Using the gEOM Package for nominal positions  
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

#include "VolatileAlignmentParams.h"

namespace emph {
  namespace ssdr {
  
  // 
  
     VolatileAlignmentParams::VolatileAlignmentParams():
       fIsPhase1c(false),
       fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(0.612e-3),
       fPitch(0.06),
       fWaferWidth(static_cast<int>(fNumStrips)*fPitch),
       fHalfWaferWidth(0.5*fWaferWidth),  
       fZNomPosX{0.75, 121.25, 363.15, 484.15, 985.75, 985.75, 1211.95, 1211.95, DBL_MAX, DBL_MAX}, 
       fZNomPosY{0.15, 120.65, 360.75, 481.75, 986.35, 986.35, 1212.55, 1212.55, DBL_MAX, DBL_MAX},
//       fZNomPosSt4and5{360.15,  481.15}, fZNomPosSt2and3{988.75, 988.75, 1214.95, 1214.95},
       fZNomPosSt2and3{360.15,  481.15, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, 
       fZNomPosSt4and5{988.75, 988.75, 1214.95, 1214.95, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX},    // Change of convention Aug. Sept 2023 
       fZNomPos1cSt2and3U(fNumSensorsU, DBL_MAX), 
       fZNomPos1cSt5and6W(fNumSensorsW, DBL_MAX), //Phase1c. for Stereo angle views.  
       fZDeltaPosX(fNumSensorsXorY, 0.),  fZDeltaPosY(fNumSensorsXorY, 0.), 
       fZDeltaPosSt2and3(fNumSensorsU, 0.), fZDeltaPosSt4and5(fNumSensorsV, 0.),
       fZDeltaPos1cSt2and3U(fNumSensorsU, 0.), fZDeltaPos1cSt5and6W(fNumSensorsV, 0.),
       fTrNomPosX{fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth, DBL_MAX, DBL_MAX, DBL_MAX},
//       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
//                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth}, 
// Sept 5, sorting negative Y  
//       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
//                  0., fWaferWidth, 0., fWaferWidth}, 
// Sept 9, back to where they were on Aug 30.  Density plot on run NoTgt31Gev_ClSept_A1e_1o1_c7 indicate Y flipped sensors, on data. 
// NoTgt31Gev_ClSept_A1e_1o1_c9
       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth, DBL_MAX, DBL_MAX, DBL_MAX},  
//            August 25 2023, flip the sign for Y4a, Y5a.. Following the GDML data. 		      
//       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
//                  -fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},   
// Aug 30 -31.. Sorting U and V ..  Spet 4 : back to old config   
//       fTrNomPosSt4and5{-fWaferWidth, -fWaferWidth, -fWaferWidth, -fWaferWidth},  // Weird...!... MC bug ???? 
// Sept 5       
//       fTrNomPosSt4and5{0., 0., 0., 0.},  //Trial.. 
// Sept 9, on real data, run 1274, must set those as prior to Aug 30 Option c9 
//
// December 1 Set for Phase1b.  See update method.. Extra dimensioned for Phase1b, definitly.. 
       fTrNomPosSt4and5{-fWaferWidth, -fWaferWidth,-fWaferWidth, -fWaferWidth, 0., 0., 0., 0.}, 
       fTrNomPosSt2and3{-fHalfWaferWidth, -fHalfWaferWidth}, //  give shifts of -15. ? Investigating..
       fTrNomPos1cSt2and3U(fNumSensorsU, -fHalfWaferWidth),  
       fTrNomPos1cSt5and6W(fNumSensorsV, -fWaferWidth), 
//            Sept 2 2023, flip the sign for Y4a, Y5a.. Following the GDML data. 		      
//       fTrNomPosSt4and5{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 0., 0.},  // Weird...!... MC bug ???? 
//       fTrNomPosSt2and3{-fWaferWidth, -fWaferWidth,-fWaferWidth, -fWaferWidth, 0., 0., 0., 0., 0., 0.}, //  give shits of -15. ? Investigating..
       fTrDeltaPosX(fNumSensorsXorY, 0.), fTrDeltaPosY(fNumSensorsXorY, 0.),  
       fTrDeltaPosSt2and3(fNumSensorsV, 0.), fTrDeltaPosSt4and5(fNumSensorsU, 0.), 
       fTrDeltaPos1cSt2and3U(fNumSensorsU, 0.), 
       fTrDeltaPos1cSt5and6W(fNumSensorsW, 0.), 
       fTrDeltaPitchX(fNumSensorsXorY, 0.), fTrDeltaPitchY(fNumSensorsXorY, 0.),  
       fTrDeltaPitchSt2and3(fNumSensorsV, 0.), fTrDeltaPitchSt4and5(fNumSensorsU, 0.), 
       // Delta Pitch not implemented for Phase1c. 
       fRollX(fNumSensorsXorY, 0.), fRollY(fNumSensorsXorY, 0.),  
       fRollSt2and3(fNumSensorsV, 0.), fRollSt4and5(fNumSensorsU, 0.),
       fRoll1cSt2and3U(fNumSensorsU, 0.), fRoll1cSt5and6W(fNumSensorsW, 0.),
       fRollXC(fNumSensorsXorY, 0.), fRollYC(fNumSensorsXorY, 0.),  
       fRollSt2and3C(fNumSensorsV, 0.), fRollSt4and5C(fNumSensorsU, 0.),
       fRoll1cSt2and3UC(fNumSensorsU, 0.), fRoll1cSt5and6WC(fNumSensorsW, 0.),
//       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.05067243, 0.05067243, 0.06630287, 0.06630287},
//       fMultScatUncertSt2and3{0.05067243, 0.05067243}, // Probably too big, to be revisited 
//       fMultScatUncertSt4and5{0.05067243, 0.05067243, 0.06630287, 0.06630287}, 
// Temporary test, we underestimate the multiple scattering error propagation in the SSDAlign package.. 
//  The above one are probably more realistic, if the target is installed... If no target...  
       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.0283, 0.0283, 0.04, 0.04, 0.05, 0.05},
       // Currently a guess for Phase1c, station 5 and 6. 
       fMultScatUncertSt2and3{0.014, 0.0195}, // Now consistent with SSDALignAlog1, Nov. 7 
       fMultScatUncertSt4and5{0.0283, 0.0283, 0.04, 0.04}, 
       fMultScatUncertSt2and31c{0.014, 0.0195},  // Might be different, need to run MC
       fMultScatUncertSt5and61c{0.0283, 0.0283, 0.04, 0.04, 0.05, 0.05},   // extend to possibly use Station 7  
       fUnknownUncertXorY(fNumSensorsXorY, 2.5e-8), fUnknownUncertSt2and3(fNumSensorsV, 2.5e-8), fUnknownUncertSt4and5(fNumSensorsU, 2.5e-8),
       fZPosX(fNumSensorsXorY, 0.),  fZPosY(fNumSensorsXorY, 0.), 
       fZPosSt4and5(fNumSensorsV, 0.),fZPosSt2and3(fNumSensorsU, 0.),
       fZPos1cSt5and6W(fNumSensorsV, 0.),fZPos1cSt2and3U(fNumSensorsU, 0.),
       fTrPosX(fNumSensorsXorY, 0.),  fTrPosY(fNumSensorsXorY, 0.), 
       fTrPosSt4and5(fNumSensorsV, 0.), fTrPosSt2and3(fNumSensorsU, 0.),
       fTrPos1cSt5and6W(fNumSensorsV, 0.), fTrPos1cSt2and3U(fNumSensorsU, 0.)
     { 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosX[kSe] = fZNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosY[kSe] = fZNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsV; kSe++) { fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe]; } 

       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosX[kSe] = fTrNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosY[kSe] = fTrNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsV; kSe++) { fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe]; } 
       
       // an assumption !! 
       
       fZPosErr = 5.0e-9;  // Turn this off, because it will affect only mometum scale (correlated errors) 
       
       // 
       
     }
     void VolatileAlignmentParams::UpdateNominalFromStandardGeom(emph::geo::Geometry *theGeo) {
       std::cerr << " VolatileAlignmentParams::UpdateNominalFromStandardGeom ....Dump values, for use in SSDAlign...   " << std::endl;
       std::cerr << " Number of SSD Stations " << theGeo->NSSDStations() << std::endl;
       size_t iSensX=0; size_t iSensY=0; size_t iSensU=0; size_t iSensV=0;
       std::vector<double> zPosSensorsX;  std::vector<double> zPosSensorsY; 
       std::vector<double> zPosSensorsW; std::vector<double> zPosSensorsU;
       if (fIsPhase1c) {
         fZNomPosX.clear(); fZPosX.clear(); fZNomPosY.clear(); fZPosY.clear();
         fZPos1cSt2and3U.clear();  fZPos1cSt5and6W.clear();
         fZNomPos1cSt2and3U.clear();  fZNomPos1cSt5and6W.clear();
       }
       for (int kSt=0; kSt != theGeo->NSSDStations(); kSt++) { 
         const emph::geo::SSDStation *aSt = theGeo->GetSSDStation(kSt);
	 const TVector3 aStPos = aSt->Pos();
	 std::cerr << " ... At Station " << aSt->Name() << " Position, X  " << aStPos.X() << " Y " << aStPos.Y() 
	           << " " << aStPos.Z() + aSt->Dz() << std::endl; 
         for (int kPlSe=0; kPlSe != aSt->NPlanes(); kPlSe++) {
	   const emph::geo::Plane *aPlane = aSt->GetPlane(kPlSe);
	   for (int kSe=0; kSe != aPlane->NSSDs(); kSe++) { 
	     const emph::geo::Detector *aSensor = aPlane->SSD(kSe); 
	     const TVector3 aSePos = aSensor->Pos();
	     std::cerr << " ... ... At Sensor " << aSensor->Name() << " View " << aSensor->View()  
	             << " Position, X  " << aSePos.X() << " Y " << aSePos.Y() << " Z " << aSePos.Z() << std::endl; 
             if (aSensor->View() == emph::geo::X_VIEW) {
	       iSensX++; 
	       zPosSensorsX.push_back(aSePos.Z() + aStPos.Z()); 
	       if (fIsPhase1c) { fZNomPosX.push_back(aSePos.Z() + aStPos.Z()); fZPosX.push_back(aSePos.Z() + aStPos.Z()); } 
	     }
             if (aSensor->View() == emph::geo::Y_VIEW)  {	   
	        iSensY++; 
	        zPosSensorsY.push_back(aSePos.Z() + aStPos.Z());
	        if (fIsPhase1c) { fZNomPosY.push_back(aSePos.Z() + aStPos.Z()); fZPosY.push_back(aSePos.Z() + aStPos.Z()); } 
	     } 
             if (aSensor->View() == emph::geo::U_VIEW) {
	        if (!fIsPhase1c) zPosSensorsU.push_back(aSePos.Z() + aStPos.Z());
	        iSensU++; 
	        if (fIsPhase1c) { 
		   if ((kSt == 2) || (kSt == 3)) {  
		     fZNomPos1cSt2and3U.push_back(aSePos.Z() + aStPos.Z());  
		     fZPos1cSt2and3U.push_back(aSePos.Z() + aStPos.Z()); 
		   } else {
		     std::cerr << " VolatileAlignmentParams::UpdateNominalFromStandardGeom, unpected station for U view " << kSt << std::endl;
		     std::cerr << "  ............... Fatal..quit here and now ....  " << std::endl; exit(2);
		   } 
		} 
	     }
             if (aSensor->View() == emph::geo::W_VIEW) {
	       iSensV++; 
	       if (!fIsPhase1c) zPosSensorsW.push_back(aSePos.Z() + aStPos.Z());
	       if (fIsPhase1c) { // No check on station number.... this time... Assume I have it right.. Dec. 1 2023
		   fZNomPos1cSt5and6W.push_back(aSePos.Z() + aStPos.Z());  
		   fZPos1cSt5and6W.push_back(aSePos.Z() + aStPos.Z()); 
	       }
	      }
	    } // on sensor, signle vs double.. 
	  } // On view planes 
	} // on stations. 
       std::cerr << std::endl;
       std::cerr << " Z position Views X ";
       for (std::vector<double>::const_iterator it=zPosSensorsX.cbegin(); it != zPosSensorsX.cend(); it++) std::cerr << " " << *it << ",";
       std::cerr << std::endl << std::endl;
       std::cerr << " Z position Views Y ";
       for (std::vector<double>::const_iterator it=zPosSensorsY.cbegin(); it != zPosSensorsY.cend(); it++) std::cerr << " " << *it << ",";
       std::cerr << std::endl << std::endl;
       if (!fIsPhase1c) { 
         std::cerr << " Z position Views U, phase1b  ";
         for (std::vector<double>::const_iterator it=zPosSensorsU.cbegin(); it != zPosSensorsU.cend(); it++) std::cerr << " " << *it << ",";
         std::cerr << std::endl << std::endl;
         std::cerr << " Z position Views W, phase1b  ";
         for (std::vector<double>::const_iterator it=zPosSensorsW.cbegin(); it != zPosSensorsW.cend(); it++) std::cerr << " " << *it << ",";
         std::cerr << std::endl << std::endl;
       } else { 
         std::cerr << " Z position Views U, phase1c ";
         for (std::vector<double>::const_iterator it=fZPos1cSt2and3U.cbegin(); it != fZPos1cSt2and3U.cend(); it++) 
	        { std::cerr << " " << *it << ","; } 
         std::cerr << std::endl << std::endl;
         std::cerr << " Z position Views W, phase1c  ";
         for (std::vector<double>::const_iterator it=fZPos1cSt5and6W.cbegin(); it != fZPos1cSt5and6W.cend(); it++) 
	  { std::cerr << " " << *it << ","; } 
         std::cerr << std::endl << std::endl;
       }
       
//
// We should upload these new values.. It will make small differences, negligible for now.. But not in the alignment procedure. 
//
// Did upload them for Phase1c.  See above.. 
//     
      if (fIsPhase1c) { 
        // Add Station 4, single sensor, X and Y view..  The fTrNomPosX and fTrPosX will be oversized, 
	// A bit ugly, but harmless (a few times 64 bit of wasted memory.. 
	std::cerr << " Check the fTrNomPosX..size " << fTrNomPosX.size() << " Before inserting  "; 
	for(size_t k=0; k != fTrNomPosX.size(); k++) { std::cerr << " " << fTrNomPosX[k]; }  std::cerr << std::endl;
	fTrPosX.insert(fTrPosX.begin() + 4, fHalfWaferWidth); fTrNomPosX.insert(fTrNomPosX.begin() + 4, fHalfWaferWidth);
	fTrPosY.insert(fTrPosY.begin() + 4, -fHalfWaferWidth); fTrNomPosY.insert(fTrNomPosY.begin() + 4, -fHalfWaferWidth);
	std::cerr << " Check the fTrNomPosX.. "; 
	for(size_t k=0; k != fTrNomPosX.size(); k++) { std::cerr << " " << fTrNomPosX[k]; }
	std::cerr << std::endl;
	std::cerr << " Check, fNumSensorsXorY " << fNumSensorsXorY 
	          << " Size fTrNomPosX " << fTrNomPosX.size() << " Size fTrPosX " << fTrPosX.size() << std::endl;
	fTrNomPosX[9] = fTrNomPosX[7]; fTrNomPosX[10] = fTrNomPosX[8];  // Station 7, although it is empty.. 
        for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosX[kSe] = fTrNomPosX[kSe]; } 
	std::cerr << " Check, fNumSensorsXorY " << fNumSensorsXorY 
	          << " Size fTrNomPosY " << fTrNomPosY.size() << " Size fTrPosY " << fTrPosY.size() << std::endl;
        for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosY[kSe] = fTrNomPosY[kSe]; } 
	std::cerr << " Check, fNumSensorsU, " << fNumSensorsU << 
	     " sizes  " << fTrPos1cSt2and3U.size() <<  " and nominal " << fTrNomPos1cSt2and3U.size() << std::endl;  
        for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fTrPos1cSt2and3U[kSe] = fTrNomPos1cSt2and3U[kSe]; }
	std::cerr << " Check, fNumSensorsW, " << fNumSensorsW << 
	  " sizes  " << fTrPos1cSt5and6W.size() <<  " and nominal " << fTrNomPos1cSt5and6W.size() << std::endl;  
         for (size_t kSe=0; kSe != fNumSensorsW; kSe++) { fTrPos1cSt5and6W[kSe] = fTrNomPos1cSt5and6W[kSe]; } 
      }
//      std::cerr << " And... And .. quit for now !!! " << std::endl; exit(2);
   } // UpdateNominalFromStandardGeom

//
// Setters 
//
     void VolatileAlignmentParams::SetDeltaZ(emph::geo::sensorView view, size_t kSe, double v) {
        std::cerr << " VolatileAlignmentParams::SetDeltaZ, assuming U view is for Station 4 and 5 !  " << std::endl;
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     fZDeltaPosX[kSe] =  v; fZPosX[kSe] = fZNomPosX[kSe] + v;  break;  
	    } 
	 case emph::geo::Y_VIEW :  { fZDeltaPosY[kSe] = v; fZPosY[kSe] = fZNomPosY[kSe] + v; break;} 
	 case emph::geo::U_VIEW :  {
	     if (!fIsPhase1c) {
	        fZDeltaPosSt2and3[kSe] = v; fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe] + v;
	     } else  {
	        fZDeltaPos1cSt2and3U[kSe] = v; fZPos1cSt2and3U[kSe] = fZNomPos1cSt2and3U[kSe] + v;
	     } 
	     break;
           } 
	 case emph::geo::W_VIEW : { 
	   if (!fIsPhase1c) {
	       fZDeltaPosSt4and5[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe] + v; 
	       
	   } else { 
	      fZDeltaPos1cSt5and6W[kSe-2] = v; fZPos1cSt5and6W[kSe-2] = fZNomPos1cSt5and6W[kSe-2] + v;  
	   } 
	   break;
	 }
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetDeltaZ, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
//     
     void VolatileAlignmentParams::SetDeltaZStation(emph::geo::sensorView view,  size_t kSt, double v) {
       
        std::cerr << " VolatileAlignmentParams::SetDeltaZStation, assuming U view is for Station 4 and 5 !  " << std::endl; 
	std::cerr << " Not ready, period, Fatal, quit here and now " << std::endl; exit(2);
       switch (view) {
     	 case emph::geo::X_VIEW : {
	     if (kSt < 4) { 
	       fZDeltaPosX[kSt] =  v; fZPosX[kSt] = fZNomPosX[kSt] + v; 
	     } else if (kSt == 4) {
	       for (size_t kSe=4; kSe != 6; kSe++) {
	          fZDeltaPosX[kSe] =  v; fZPosX[kSe] = fZNomPosX[kSe] + v; 
	       }
	     }  else if (kSt == 5) {
	       for (size_t kSe=6; kSe != 8; kSe++) {
	          fZDeltaPosX[kSe] =  v; fZPosX[kSe] = fZNomPosX[kSe] + v; 
	       }  
	    }
	    break;
	 }
     	 case emph::geo::Y_VIEW : {
	   if (kSt < 4) { 
	     fZDeltaPosY[kSt] =  v; fZPosY[kSt] = fZNomPosY[kSt] + v; 
	   } else if (kSt == 4) {
	     for (size_t kSe=4; kSe != 6; kSe++) {
	  	fZDeltaPosY[kSe] =  v; fZPosY[kSe] = fZNomPosY[kSe] + v; 
	     }
	   }  else if (kSt == 5) {
	     for (size_t kSe=6; kSe != 8; kSe++) {
	  	fZDeltaPosY[kSe] =  v; fZPosY[kSe] = fZNomPosY[kSe] + v; 
	     }  
	   }
	   break;
	 }
	 // Still trying to figure out the orientation U vs W.. Code conservatively.. 	  
	 case emph::geo::W_VIEW :   
	 case emph::geo::U_VIEW :  
	  if (kSt < 4) { 
	    for (size_t kSe=0; kSe != 2; kSe++) { 
	      fZDeltaPosSt2and3[kSe] = v; fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe] + v;
	    }
	  } else { 
	    for (size_t kSe=0; kSe != 4; kSe++) { 
	      fZDeltaPosSt4and5[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe] + v;
	    }
	  }
	  break;
	  
	 default : { 
	      std::cerr << " VolatileAlignmentParams::ZDeltaZStation, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetDeltaTr(emph::geo::sensorView view,  size_t kSe, double v) {
//        std::cerr << " VolatileAlignmentParams::SetDeltaTr, assuming U view is for Station 2 and 3 " << std::endl;
      if (!fIsPhase1c) {
        switch (view) {
     	   case emph::geo::X_VIEW : {
//	     if (sensor >= fTrNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPosX[kSe] = v;  fTrPosX[kSe] = fTrNomPosX[kSe] + v;  break;  
// flip sign ???? 
//	     fTrDeltaPosX[kSe] = -v;  fTrPosX[kSe] = fTrNomPosX[kSe] - v;  break;  
	    } 
	   case emph::geo::Y_VIEW :  { 
	      fTrDeltaPosY[kSe] = v; fTrPosY[kSe] = fTrNomPosY[kSe] + v; break;
//	    fTrDeltaPosY[kSe] = -v; fTrPosY[kSe] = fTrNomPosY[kSe] - v; break;
	   } 
	 // flip sign as well ????? .. 
	   case emph::geo::W_VIEW :  { fTrDeltaPosSt4and5[kSe] = v; fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe] + v; break;} 
	   case emph::geo::U_VIEW : { fTrDeltaPosSt2and3[kSe] = v; fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe] + v; break;}
	   default : { 
	      std::cerr << " VolatileAlignmentParams::SetDeltaTr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	} else { // Phase1c 
	
        switch (view) {
     	   case emph::geo::X_VIEW : {
	     fTrDeltaPosX[kSe] = v;  fTrPosX[kSe] = fTrNomPosX[kSe] + v;  break;  
	    } 
	   case emph::geo::Y_VIEW :  { 
	      fTrDeltaPosY[kSe] = v; fTrPosY[kSe] = fTrNomPosY[kSe] + v; break;
	   } 
	   case emph::geo::U_VIEW :  { 
	     fTrDeltaPos1cSt2and3U[kSe] = v; fTrPos1cSt2and3U[kSe] = fTrNomPos1cSt2and3U[kSe] + v; 
	     break;
	   } 
	   case emph::geo::W_VIEW : { 
	     fTrDeltaPos1cSt5and6W[kSe-2] = v; fTrPos1cSt5and6W[kSe-2] = fTrNomPos1cSt5and6W[kSe-2] + v;
	     break;
	   }
	   default : { 
	      std::cerr << " VolatileAlignmentParams::SetDeltaTr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	
	}
     } 
     void VolatileAlignmentParams::SetValueTrShiftLastPlane(emph::geo::sensorView view, double v) {

       std::cerr << " VolatileAlignmentParams::SetValueTrShiftLastPlane, assuming U view is for Station 4 and 5  " << std::endl; 
       std::cerr << " Obsolete, do not use, fatal, quit here and now ! ... " << std::endl; exit(2);
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fTrNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPosX[fNumSensorsXorY-1] = v;  fTrPosX[fNumSensorsXorY-1] = fTrNomPosX[fNumSensorsXorY-1] + v;  break;  
	    } 
	 case emph::geo::Y_VIEW :  { 
	    fTrDeltaPosY[fNumSensorsXorY-1] = v; fTrPosY[fNumSensorsXorY-1] = fTrNomPosY[fNumSensorsXorY-1] + v; break;
	 } 
	 case emph::geo::U_VIEW :  { fTrDeltaPosSt2and3[fNumSensorsU-1] = v; fTrPosSt2and3[fNumSensorsU-1] = fTrNomPosSt2and3[fNumSensorsU-1] + v; break;} 
	 case emph::geo::W_VIEW : { fTrDeltaPosSt4and5[fNumSensorsV-1] = v; fTrPosSt4and5[fNumSensorsV-1] = fTrNomPosSt4and5[fNumSensorsV-1] + v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetValueTrShiftLastPlane, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
        } 
     }
     void VolatileAlignmentParams::SetRoll(emph::geo::sensorView view,  size_t kSe, double v) {
//       std::cerr << " VolatileAlignmentParams::SetRoll, assuming U view is for Station 2 and 3  " << std::endl; 
       if (!fIsPhase1c) {
         switch (view) {
     	   case emph::geo::X_VIEW : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollX[kSe] = v;   break;  
	    } 
	   case emph::geo::Y_VIEW :  { fRollY[kSe] = v;  break;} 
	   case emph::geo::W_VIEW :  { fRollSt4and5[kSe] = v;  break;} 
	   case emph::geo::U_VIEW : { fRollSt2and3[kSe] = v; break;}
	   default : { 
	      std::cerr << " VolatileAlignmentParams::SetRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
	   }
        } else { // Phase1c 
          switch (view) {
     	    case emph::geo::X_VIEW : {
	     fRollX[kSe] = v;   break;  
	    } 
	    case emph::geo::Y_VIEW :  { fRollY[kSe] = v;  break;} 
	    case emph::geo::U_VIEW :  { 
	       fRoll1cSt2and3U[kSe] = v;  break;
	    } 
	    case emph::geo::W_VIEW : { 
	      fRoll1cSt5and6W[kSe-2] = v; 
	      break;
	    }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::SetRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
	 
	 }
	}
     } 
     void VolatileAlignmentParams::SetRollCenter(emph::geo::sensorView view,  size_t kSe, double v) {
//       std::cerr << " VolatileAlignmentParams::SetRollCenter, assuming U view is for Station 4 and 5  " << std::endl; 
       if (!fIsPhase1c) { 
         switch (view) {
     	   case emph::geo::X_VIEW : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollXC[kSe] = v;   break;  
	    } 
	   case emph::geo::Y_VIEW :  { fRollYC[kSe] = v;  break;} 
	   case emph::geo::W_VIEW :  { fRollSt4and5C[kSe] = v;  break;} 
	   case emph::geo::U_VIEW : { fRollSt2and3C[kSe] = v; break;}
	   default : { 
	      std::cerr << " VolatileAlignmentParams::SetRollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
       } else { 
          switch (view) {
     	    case emph::geo::X_VIEW : {
	     fRollXC[kSe] = v;   break;  
	    } 
	    case emph::geo::Y_VIEW :  { fRollYC[kSe] = v;  break;} 
	    case emph::geo::U_VIEW :  { 
	       fRoll1cSt2and3UC[kSe] = v;  break;
	    } 
	    case emph::geo::W_VIEW : { 
	      fRoll1cSt5and6WC[kSe-2] = v; 
	      break;
	    }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::SetRollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
	  } 
	}
     } 
     void VolatileAlignmentParams::SetDeltaPitchCorr(emph::geo::sensorView view,  size_t kSe, double v) {
       std::cerr << " VolatileAlignmentParams::SetDeltaPitchCorr, To be done...   " << std::endl; 
       std::cerr << " Do not use yet, fatal, quit here and now ! ... " << std::endl; exit(2);
//       std::cerr << " VolatileAlignmentParams::SetDeltaPitchCorr, assuming U view is for Station 4 and 5  " << std::endl; 
        switch (view) {
     	  case emph::geo::X_VIEW : {
//	    if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	    fTrDeltaPitchX[kSe] = v;   break;  
	   } 
	  case emph::geo::Y_VIEW :  { fTrDeltaPitchY[kSe] = v;  break;} 
	  case emph::geo::W_VIEW :  { fTrDeltaPitchSt4and5[kSe] = v;  break;} 
	  case emph::geo::U_VIEW : { fTrDeltaPitchSt2and3[kSe] = v; break;}
	  default : { 
	     std::cerr << " VolatileAlignmentParams::SetDeltaPitchCorr, unknown view " << view << " fatal, quit " << std::endl; 
	     exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetUnknwonUncert(emph::geo::sensorView view,  size_t kSe, double v) {
//       std::cerr << " VolatileAlignmentParams::SetUnknwonUncert, assuming U view is for Station 4 and 5  " << std::endl; 
       if (fIsPhase1c) {
         std::cerr << " VolatileAlignmentParams::SetUnknwonUncert, obsolete for Phase1c, do not invoke thi, fatal " << std::endl;
	 exit(2);
       }
       switch (view) {
     	 case emph::geo::X_VIEW : case emph::geo::Y_VIEW :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fUnknownUncertXorY[kSe] = v;  break;  
	    } 
	 case emph::geo::U_VIEW :  { fUnknownUncertSt2and3[kSe] = v;  break;} 
	 case emph::geo::W_VIEW : {  fUnknownUncertSt4and5[kSe] = v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetUnknwonUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetMultScatUncert(emph::geo::sensorView view,  size_t kSe, double v) {
//       std::cerr << " VolatileAlignmentParams::SetMultScatUncert, assuming U view is for Station 4 and 5  " << std::endl; 
       if (!fIsPhase1c) { 
         switch (view) {
     	   case emph::geo::X_VIEW : case emph::geo::Y_VIEW :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fMultScatUncertXorY[kSe] = v;  break;  
	    } 
	   case emph::geo::U_VIEW :  { fMultScatUncertSt2and3[kSe] = v;  break;} 
	   case emph::geo::W_VIEW : {  fMultScatUncertSt4and5[kSe] = v; break;}
	   default : { 
	      std::cerr << " VolatileAlignmentParams::SetMultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	} else { 
	  switch (view) {
     	   case emph::geo::X_VIEW : case emph::geo::Y_VIEW :{ 
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fMultScatUncertXorY[kSe] = v;  break;  
	    } 
	   case emph::geo::U_VIEW :  { fMultScatUncertSt2and31c[kSe] = v;  break;} 
	   case emph::geo::W_VIEW : {  
	      if (kSe > 2) fMultScatUncertSt5and61c[kSe-2] = v; 
	      else fMultScatUncertSt2and31c[kSe] = v;
	      break;
	   }
	   default : { 
	      std::cerr << " VolatileAlignmentParams::SetMultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);   
	   }
	 }
       }
     }
     void VolatileAlignmentParams::SetTransShiftFor4c5c6c(bool correctX57, double factBad) {
     //
     // See job Sim5c_8d1 (30 GeV) on my desk1 machine and 4c_g (120 GeV) on the wilson cluster. 
     // We take the 30 GeV.. Worst case.. 
     // 
        std::cerr << " VolatileAlignmentParams::SetTransShiftFor4c5c6c, in effect.. " << std::endl;
        this->SetDeltaTr(emph::geo::X_VIEW, 1, 1.30528);
        this->SetDeltaTr(emph::geo::X_VIEW, 2, 1.40432 );
        this->SetDeltaTr(emph::geo::X_VIEW, 3, 2.42377);
        this->SetDeltaTr(emph::geo::X_VIEW, 4, 4.3912 );
        this->SetDeltaTr(emph::geo::X_VIEW, 5, 2.39239);
        this->SetDeltaTr(emph::geo::X_VIEW, 6, 4.15928);
        this->SetDeltaTr(emph::geo::X_VIEW, 7,  3.0   );  // See parameter Dgap (double sensor gap)
	 
        this->SetDeltaTr(emph::geo::Y_VIEW, 1,  0.457939 );
        this->SetDeltaTr(emph::geo::Y_VIEW, 2,  -0.905573);
        this->SetDeltaTr(emph::geo::Y_VIEW, 3,  0.0796809);
        this->SetDeltaTr(emph::geo::Y_VIEW, 4,  2.94305  );
        this->SetDeltaTr(emph::geo::Y_VIEW, 5,  3.01591  );
        this->SetDeltaTr(emph::geo::Y_VIEW, 6,  2.8699   );
        this->SetDeltaTr(emph::geo::Y_VIEW, 7,  3.0   );  // See parameter Dgap (double sensor gap) 
//        this->SetDeltaTr(emph::geo::U_VIEW, 0,  0.726008 );
//        this->SetDeltaTr(emph::geo::U_VIEW, 1,  2.85987  );
//        this->SetDeltaTr(emph::geo::W_VIEW, 0,  -5.42706 );
//        this->SetDeltaTr(emph::geo::W_VIEW, 1,  -4.71627 );
//        this->SetDeltaTr(emph::geo::W_VIEW, 2,  -1.262   );
//        this->SetDeltaTr(emph::geo::W_VIEW, 3,  -1.15554 );
        this->SetDeltaTr(emph::geo::W_VIEW, 0,  0.726008 );
        this->SetDeltaTr(emph::geo::W_VIEW, 1,  2.85987  );
        this->SetDeltaTr(emph::geo::U_VIEW, 0,  -5.42706 );
        this->SetDeltaTr(emph::geo::U_VIEW, 1,  -4.71627 );
        this->SetDeltaTr(emph::geo::U_VIEW, 2,  -1.262   );
        this->SetDeltaTr(emph::geo::U_VIEW, 3,  -1.15554 );
	if (correctX57) {
           this->SetDeltaTr(emph::geo::X_VIEW, 7,  3.0 - factBad*0.5091 );  // The last term is the average of the residual from the alignment fit 5c_8d2
           this->SetDeltaTr(emph::geo::X_VIEW, 5,  2.39239 - factBad*0.150 );  // The last term is the average of the residual from the alignment fit 5c_8d2
	}    
     }
     void VolatileAlignmentParams::SetGeomFromSSDAlign(const std::string &fileName) {
       if (fIsPhase1c) {
         std::cerr << " VolatileAlignmentParams::SetGeomFromSSDAlign, not ready for prime time for Phase1c.. Fatal " << std::endl;
	 exit(2); 
       }
       std::ifstream fIn(fileName.c_str());
       if (!fIn.is_open()) {
           std::cerr << "VolatileAlignmentParams::SetGeomFromSSDAlign , failed to open " << fileName << " fatal, quit here.. " << std::endl; exit(2);
       }
       char aLine[1024];
       std::cerr << " VolatileAlignmentParams::SetGeomFromSSDAlign, uploading Params from file " << fileName << std::endl;
       std::string tokenTransShift("TransShift_X_");
       std::string tokenRoll("DeltaRoll_X_");
       std::string tokenRollCenter("DeltaRollCenter_X_"); // The length of the strings are the same for all Views. 
       while (fIn.good()) {
         fIn.getline(aLine, 1024);
         std::string aLStr(aLine);
         std::istringstream aLStrStr(aLine);
	 std::string aName; double aVal; double aErr; 
	 aLStrStr >> aName >> aVal >> aErr; 
	 // we skip the tilts, only stransverse shifts and rolls for now.. 
	 // Change only X_1 and Y_1 for now.. Test!. 
//	 if (aName.find("_1") == std::string::npos) continue;
//	 if (aName.find("U_")!= std::string::npos) continue;
//	 if (aName.find("V_")!= std::string::npos) continue;
	 if (aName.find("TransShift") != std::string::npos) {
	   std::string aSensStr=aName.substr(tokenTransShift.length(), 1);
	   size_t aSens = static_cast<size_t>(std::atoi(aSensStr.c_str()));
	   std::cerr << ".... Uploading TransShift, name " <<  aName << ", sensor " <<  aSens << " value " << aVal << std::endl;
	   if (aName.find("_X") != std::string::npos) this->SetDeltaTr(emph::geo::X_VIEW, aSens, aVal);
	   if (aName.find("_Y") != std::string::npos) this->SetDeltaTr(emph::geo::Y_VIEW, aSens, aVal);
	   if (aName.find("_U") != std::string::npos) this->SetDeltaTr(emph::geo::U_VIEW, aSens, aVal);
	   if (aName.find("_V") != std::string::npos) this->SetDeltaTr(emph::geo::W_VIEW, aSens, aVal);
	 }
	 if ((aName.find("DeltaRoll") != std::string::npos) && (aName.find("Center") == std::string::npos)) {
	   std::string aSensStr=aName.substr(tokenRoll.length(), 1);
	   size_t aSens = static_cast<size_t>(std::atoi(aSensStr.c_str()));
	   std::cerr << ".... Uploading Roll angle, name " <<  aName << ", sensor " <<  aSens << " value " << aVal << std::endl;
	   if (aName.find("_X") != std::string::npos) this->SetRoll(emph::geo::X_VIEW, aSens, aVal);
	   if (aName.find("_Y") != std::string::npos) this->SetRoll(emph::geo::Y_VIEW, aSens, aVal);
	   if (aName.find("_U") != std::string::npos) this->SetRoll(emph::geo::U_VIEW, aSens, aVal);
	   if (aName.find("_V") != std::string::npos) this->SetRoll(emph::geo::W_VIEW, aSens, aVal);
	 }
	 if (aName.find("DeltaRollCenter") != std::string::npos) {
	   std::string aSensStr=aName.substr(tokenRollCenter.length(), 1);
	   size_t aSens = static_cast<size_t>(std::atoi(aSensStr.c_str()));
	   std::cerr << ".... Uploading Roll Center name " <<  aName << ", sensor " <<  aSens << " value " << aVal << std::endl;
	   if (aName.find("_X") != std::string::npos) this->SetRollCenter(emph::geo::X_VIEW, aSens, aVal);
	   if (aName.find("_Y") != std::string::npos) this->SetRollCenter(emph::geo::Y_VIEW, aSens, aVal);
	   if (aName.find("_U") != std::string::npos) this->SetRollCenter(emph::geo::U_VIEW, aSens, aVal);
	   if (aName.find("_V") != std::string::npos) this->SetRollCenter(emph::geo::W_VIEW, aSens, aVal);
	 }
	 
       }
     } 
   } // namespace 
}  // namespace   
     
