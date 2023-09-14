////////////////////////////////////////////////////////////////////////
/// \brief  The Volatile Geometry for the SSD aligner 
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
       fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(0.612e-3),
       fPitch(0.06),
       fWaferWidth(static_cast<int>(fNumStrips)*fPitch),
       fHalfWaferWidth(0.5*fWaferWidth),  
       fZNomPosX{0.75, 121.25, 363.15, 484.15, 985.75, 985.75, 1211.95, 1211.95}, 
       fZNomPosY{0.15, 120.65, 360.75, 481.75, 986.35, 986.35, 1212.55, 1212.55},
//       fZNomPosSt4and5{360.15,  481.15}, fZNomPosSt2and3{988.75, 988.75, 1214.95, 1214.95},
       fZNomPosSt2and3{360.15,  481.15, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX}, 
       fZNomPosSt4and5{988.75, 988.75, 1214.95, 1214.95, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX},    // Change of convention Aug. Sept 2023 
       fZDeltaPosX(fNumSensorsXorY, 0.),  fZDeltaPosY(fNumSensorsXorY, 0.), 
       fZDeltaPosSt2and3(fNumSensorsU, 0.), fZDeltaPosSt4and5(fNumSensorsV, 0.),
       fTrNomPosX{fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},
//       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
//                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth}, 
// Sept 5, sorting negative Y  
//       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
//                  0., fWaferWidth, 0., fWaferWidth}, 
// Sept 9, back to where they were on Aug 30.  Density plot on run NoTgt31Gev_ClSept_A1e_1o1_c7 indicate Y flipped sensors, on data. 
// NoTgt31Gev_ClSept_A1e_1o1_c9
       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},  
//            August 25 2023, flip the sign for Y4a, Y5a.. Following the GDML data. 		      
//       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
//                  -fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},   
// Aug 30 -31.. Sorting U and V ..  Spet 4 : back to old config   
//       fTrNomPosSt4and5{-fWaferWidth, -fWaferWidth, -fWaferWidth, -fWaferWidth},  // Weird...!... MC bug ???? 
// Sept 5       
//       fTrNomPosSt4and5{0., 0., 0., 0.},  //Trial.. 
// Sept 9, on real data, run 1274, must set those as prior to Aug 30 Option c9 
//
       fTrNomPosSt4and5{-fWaferWidth, -fWaferWidth,-fWaferWidth, -fWaferWidth, 0., 0., 0., 0.}, 
       fTrNomPosSt2and3{-fHalfWaferWidth, -fHalfWaferWidth}, //  give shits of -15. ? Investigating..
//            Sept 2 2023, flip the sign for Y4a, Y5a.. Following the GDML data. 		      
//       fTrNomPosSt4and5{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 0., 0.},  // Weird...!... MC bug ???? 
//       fTrNomPosSt2and3{-fWaferWidth, -fWaferWidth,-fWaferWidth, -fWaferWidth, 0., 0., 0., 0., 0., 0.}, //  give shits of -15. ? Investigating..
       fTrDeltaPosX(fNumSensorsXorY, 0.), fTrDeltaPosY(fNumSensorsXorY, 0.),  
       fTrDeltaPosSt2and3(fNumSensorsV, 0.), fTrDeltaPosSt4and5(fNumSensorsU, 0.), 
       fTrDeltaPitchX(fNumSensorsXorY, 0.), fTrDeltaPitchY(fNumSensorsXorY, 0.),  
       fTrDeltaPitchSt2and3(fNumSensorsV, 0.), fTrDeltaPitchSt4and5(fNumSensorsU, 0.), 
       fRollX(fNumSensorsXorY, 0.), fRollY(fNumSensorsXorY, 0.),  
       fRollSt2and3(fNumSensorsV, 0.), fRollSt4and5(fNumSensorsU, 0.),
       fRollXC(fNumSensorsXorY, 0.), fRollYC(fNumSensorsXorY, 0.),  
       fRollSt2and3C(fNumSensorsV, 0.), fRollSt4and5C(fNumSensorsU, 0.),
       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.05067243, 0.05067243, 0.06630287, 0.06630287},
       fMultScatUncertSt2and3{0.05067243, 0.05067243}, // Probably too big, to be revisited 
       fMultScatUncertSt4and5{0.05067243, 0.05067243, 0.06630287, 0.06630287},        
       fUnknownUncertXorY(fNumSensorsXorY, 0.0005), fUnknownUncertSt2and3(fNumSensorsV, 0.0005), fUnknownUncertSt4and5(fNumSensorsU, 0.0005),
       fZPosX(fNumSensorsXorY, 0.),  fZPosY(fNumSensorsXorY, 0.), 
       fZPosSt4and5(fNumSensorsV, 0.),fZPosSt2and3(fNumSensorsU, 0.),
       fTrPosX(fNumSensorsXorY, 0.),  fTrPosY(fNumSensorsXorY, 0.), 
       fTrPosSt4and5(fNumSensorsV, 0.),fTrPosSt2and3(fNumSensorsU, 0.)
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
       
     }
     void VolatileAlignmentParams::UpdateNominalFromStandardGeom(emph::geo::Geometry *theGeo) {
       std::cerr << " VolatileAlignmentParams::UpdateNominalFromStandardGeom ....Y View updates only....  " << std::endl;
       std::cerr << " Number of SSD Stations " << theGeo->NSSDStations() << std::endl;
       size_t iSensX=0; size_t iSensY=0; size_t iSensU=0; size_t iSensV=0;
       for (int kSt=0; kSt != theGeo->NSSDStations(); kSt++) { 
         const emph::geo::SSDStation aSt = theGeo->GetSSDStation(kSt);
	 const TVector3 aStPos = aSt.Pos();
	 std::cerr << " ... At Station " << aSt.Name() << " Position, X  " << aStPos.X() << " Y " << aStPos.Y() 
	           << " " << aStPos.Z() + aSt.Dz() << std::endl; 
         for (int kSe=0; kSe != aSt.NSSDs(); kSe++) {
	   const emph::geo::Detector aSensor = aSt.GetSSD(kSe); 
	   const TVector3 aSePos = aSensor.Pos();
	   std::cerr << " ... ... At Sensor " << aSensor.Name() << " View " << aSensor.View()  
	             << " Position, X  " << aSePos.X() << " Y " << aSePos.Y() << " Z " << aSePos.Z() << std::endl; 
	   if (aSensor.View() == emph::geo::Y_VIEW) {
	     if (aSensor.IsFlip()) 
	        std::cerr << " .... ... ... Compare with my old static data, fTrNomPosY " << fTrNomPosY[iSensY] 
		          << " vs, flipped,  " << aSePos.Y() <<  " iSensY " <<iSensY <<  std::endl;
	     else  std::cerr << " .... ... ... Compare with my old static data, fTrNomPosY " << fTrNomPosY[iSensY] 
		          << " vs, Not flipped,  " << aSePos.Y() << " iSensY " <<iSensY << std::endl;
	   }	     
           if (aSensor.View() == emph::geo::X_VIEW) iSensX++; 
           if (aSensor.View() == emph::geo::Y_VIEW) iSensY++; 
           if (aSensor.View() == emph::geo::U_VIEW) iSensU++; 
           if (aSensor.View() == emph::geo::W_VIEW) iSensV++; 
	 }
	 std::cerr << std::endl;
       }
//       std::cerr << " And... And .. quit for now !!! " << std::endl; exit(2);
     }
     //
     // Setters 
     //
     void VolatileAlignmentParams::SetDeltaZ(emph::geo::sensorView view, size_t kSe, double v) {
        std::cerr << " VolatileAlignmentParams::SetDeltaZ, assuming U view is for Station 4 and 5 !  " << std::endl;
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     fZDeltaPosX[kSe] =  fZPosX[kSe] = fZNomPosX[kSe] + v;  break;  
	    } 
	 case emph::geo::Y_VIEW :  { fZDeltaPosY[kSe] = v; fZPosY[kSe] = fZNomPosY[kSe] + v; break;} 
	 case emph::geo::U_VIEW :  { fZDeltaPosSt4and5[kSe] = v; fZPosSt2and3[kSe] = fZNomPosSt4and5[kSe] + v; break;} 
	 case emph::geo::W_VIEW : { fZDeltaPosSt2and3[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt2and3[kSe] + v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetDeltaZ, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
//     
     void VolatileAlignmentParams::SetDeltaZStation(emph::geo::sensorView view,  size_t kSt, double v) {
        std::cerr << " VolatileAlignmentParams::SetDeltaZStation, assuming U view is for Station 4 and 5 !  " << std::endl; 
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
        std::cerr << " VolatileAlignmentParams::SetDeltaTr, assuming U view is for Station 4 and 5 " << std::endl;
        if ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)) {
	  std::cerr << " .... Stereo Views...  Skip chenge for now, due to probalble mislabeling.. "  << std::endl;
	  return;
	}
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
	 case emph::geo::U_VIEW :  { fTrDeltaPosSt4and5[kSe] = v; fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe] + v; break;} 
	 case emph::geo::W_VIEW : { fTrDeltaPosSt2and3[kSe] = v; fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe] + v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetDeltaTr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetValueTrShiftLastPlane(emph::geo::sensorView view, double v) {

       std::cerr << " VolatileAlignmentParams::SetValueTrShiftLastPlane, assuming U view is for Station 4 and 5  " << std::endl; 
     
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fTrNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPosX[fNumSensorsXorY-1] = v;  fTrPosX[fNumSensorsXorY-1] = fTrNomPosX[fNumSensorsXorY-1] + v;  break;  
	    } 
	 case emph::geo::Y_VIEW :  { 
	    fTrDeltaPosY[fNumSensorsXorY-1] = v; fTrPosY[fNumSensorsXorY-1] = fTrNomPosY[fNumSensorsXorY-1] + v; break;
	 } 
	 case emph::geo::W_VIEW :  { fTrDeltaPosSt2and3[fNumSensorsU-1] = v; fTrPosSt2and3[fNumSensorsU-1] = fTrNomPosSt2and3[fNumSensorsU-1] + v; break;} 
	 case emph::geo::U_VIEW : { fTrDeltaPosSt4and5[fNumSensorsV-1] = v; fTrPosSt4and5[fNumSensorsV-1] = fTrNomPosSt4and5[fNumSensorsV-1] + v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetValueTrShiftLastPlane, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
        } 
     }
     void VolatileAlignmentParams::SetRoll(emph::geo::sensorView view,  size_t kSe, double v) {
       std::cerr << " VolatileAlignmentParams::SetRoll, assuming U view is for Station 4 and 5  " << std::endl; 
     
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollX[kSe] = v;   break;  
	    } 
	 case emph::geo::Y_VIEW :  { fRollY[kSe] = v;  break;} 
	 case emph::geo::U_VIEW :  { fRollSt4and5[kSe] = v;  break;} 
	 case emph::geo::W_VIEW : { fRollSt2and3[kSe] = v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetRollCenter(emph::geo::sensorView view,  size_t kSe, double v) {
       std::cerr << " VolatileAlignmentParams::SetRollCenter, assuming U view is for Station 4 and 5  " << std::endl; 
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollXC[kSe] = v;   break;  
	    } 
	 case emph::geo::Y_VIEW :  { fRollYC[kSe] = v;  break;} 
	 case emph::geo::U_VIEW :  { fRollSt4and5C[kSe] = v;  break;} 
	 case emph::geo::W_VIEW : { fRollSt2and3C[kSe] = v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetRollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetDeltaPitchCorr(emph::geo::sensorView view,  size_t kSe, double v) {
       std::cerr << " VolatileAlignmentParams::SetDeltaPitchCorr, assuming U view is for Station 4 and 5  " << std::endl; 
       switch (view) {
     	 case emph::geo::X_VIEW : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPitchX[kSe] = v;   break;  
	    } 
	 case emph::geo::Y_VIEW :  { fTrDeltaPitchY[kSe] = v;  break;} 
	 case emph::geo::U_VIEW :  { fTrDeltaPitchSt4and5[kSe] = v;  break;} 
	 case emph::geo::W_VIEW : { fTrDeltaPitchSt2and3[kSe] = v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetDeltaPitchCorr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetUnknwonUncert(emph::geo::sensorView view,  size_t kSe, double v) {
       std::cerr << " VolatileAlignmentParams::SetUnknwonUncert, assuming U view is for Station 4 and 5  " << std::endl; 
       switch (view) {
     	 case emph::geo::X_VIEW : case emph::geo::Y_VIEW :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fUnknownUncertXorY[kSe] = v;  break;  
	    } 
	 case emph::geo::U_VIEW :  { fUnknownUncertSt4and5[kSe] = v;  break;} 
	 case emph::geo::W_VIEW : {  fUnknownUncertSt2and3[kSe] = v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetUnknwonUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void VolatileAlignmentParams::SetMultScatUncert(emph::geo::sensorView view,  size_t kSe, double v) {
       std::cerr << " VolatileAlignmentParams::SetMultScatUncert, assuming U view is for Station 4 and 5  " << std::endl; 
       switch (view) {
     	 case emph::geo::X_VIEW : case emph::geo::Y_VIEW :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fMultScatUncertXorY[kSe] = v;  break;  
	    } 
	 case emph::geo::U_VIEW :  { fMultScatUncertSt4and5[kSe] = v;  break;} 
	 case emph::geo::W_VIEW : {  fMultScatUncertSt2and3[kSe] = v; break;}
	 default : { 
	      std::cerr << " VolatileAlignmentParams::SetMultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
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
	 if (aName.find("_1") == std::string::npos) continue;
	 if (aName.find("U_")!= std::string::npos) continue;
	 if (aName.find("V_")!= std::string::npos) continue;
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
     
