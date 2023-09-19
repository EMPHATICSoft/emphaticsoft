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

#include "BTAlignGeom.h"

namespace emph {
  namespace rbal {
  
     BTAlignGeom::BTAlignGeom():
       fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3),
       fPitch(0.06),
       fWaferWidth(static_cast<int>(fNumStrips)*fPitch),
       fHalfWaferWidth(0.5*fWaferWidth), fIntegrationStepSize(0.),  
//       fZNomPosX{0.75, 121.25, 363.15, 484.15, 985.75, 985.75, 1211.95, 1211.95}, 
//       fZNomPosX{3.775, 124.275, 367.125, 488.125, 982.725, 982.725, 1208.92, 1208.92},  // revised July August 2023, by LinYan, picked up from the gdml file 
       fZNomPosX{0.45, 120.95, 360.75, 481.75, 979.1, 979.1, 1205.3, 1205.3},  // revised Sept  2023, by LinYan, picked up from the gdml file 
//       fZNomPosY{0.15, 120.65, 360.75, 481.75, 986.35, 986.35, 1212.55, 1212.55},
//       fZNomPosY{-2.875, 117.625, 363.775, 484.775, 989.375, 989.375, 1215.58, 1215.58},
       fZNomPosY{-6.5, 114, 360.45, 481.45, 986.05, 986.05, 1212.25, 1212.25}, // revised Sept  2023, by LinYan, picked up from the gdml file 
//       fZNomPosSt2and3{360.15,  481.15}, 
//       fZNomPosSt2and3{357.125, 478.125}, 
       fZNomPosSt2and3{353.5, 474.5},
//       fZNomPosSt4and5{988.75, 988.75, 1214.95, 1214.95},
//       fZNomPosSt4and5{992.725, 992.725, 1218.92, 1218.92},
       fZNomPosSt4and5{986.35, 986.35, 1212.55, 1212.55},
       fZDeltaPosX(fNumSensorsXorY, 0.),  fZDeltaPosY(fNumSensorsXorY, 0.), 
       fZDeltaPosSt2and3(fNumSensorsW, 0.), fZDeltaPosSt4and5(fNumSensorsU, 0.),
       fTrNomPosX{fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},
       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},    
       fTrNomPosSt2and3{-fHalfWaferWidth, -fHalfWaferWidth}, //  give shits of -15. ? Investigating..
       fTrNomPosSt4and5{-fWaferWidth, -fWaferWidth, -fWaferWidth, -fWaferWidth},  // Weird...!... MC bug ???? 
       fTrDeltaPosX(fNumSensorsXorY, 0.), fTrDeltaPosY(fNumSensorsXorY, 0.),  
       fTrDeltaPosSt2and3(fNumSensorsW, 0.), fTrDeltaPosSt4and5(fNumSensorsU, 0.), 
       fTrDeltaPitchX(fNumSensorsXorY, 0.), fTrDeltaPitchY(fNumSensorsXorY, 0.),  
       fTrDeltaPitchSt2and3(fNumSensorsW, 0.), fTrDeltaPitchSt4and5(fNumSensorsU, 0.), 
       fRollX(fNumSensorsXorY, 0.), fRollY(fNumSensorsXorY, 0.),  
       fRollSt2and3(fNumSensorsW, 0.), fRollSt4and5(fNumSensorsU, 0.),
       fRollXC(fNumSensorsXorY, 0.), fRollYC(fNumSensorsXorY, 0.),  
       fRollSt2and3C(fNumSensorsW, 0.), fRollSt4and5C(fNumSensorsU, 0.),
       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.05067243, 0.05067243, 0.06630287, 0.06630287}, // At 120 GeV, with target
       fMultScatUncertW{0.01948, 0.05067243}, 
       fMultScatUncertU{0.05067243, 0.05067243, 0.06630287, 0.06630287},        
       fUnknownUncertX(fNumSensorsXorY, 1.0e-4), fUnknownUncertY(fNumSensorsXorY, 1.0e-4),
       fUnknownUncertW(fNumSensorsW, 1.0e-4), fUnknownUncertU(fNumSensorsU, 1.0e-4),
       fZPosX(fNumSensorsXorY, 0.),  fZPosY(fNumSensorsXorY, 0.), 
       fZPosSt2and3(fNumSensorsW, 0.),fZPosSt4and5(fNumSensorsU, 0.),
       fTrPosX(fNumSensorsXorY, 0.),  fTrPosY(fNumSensorsXorY, 0.), 
       fTrPosSt2and3(fNumSensorsW, 0.),fTrPosSt4and5(fNumSensorsU, 0.)
     { 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosX[kSe] = fZNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosY[kSe] = fZNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsW; kSe++) { fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe]; } 

       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosX[kSe] = fTrNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosY[kSe] = fTrNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsW; kSe++) { fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe]; } 
       //
       // Attempting to understand problem at 30 GeV, kick is wrong.. Reconstructed momentum is 18.4, in average... 
       // This is with a fixed kick. 
       // fMagnetKick120GeV *= 30.0/18.4; 
     }
     //
     // Setters 
     //
     void BTAlignGeom::SetDeltaZ(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     fZDeltaPosX[kSe] =  fZPosX[kSe] = fZNomPosX[kSe] + v;  break;  
	    } 
	 case 'Y' :  { fZDeltaPosY[kSe] = v; fZPosY[kSe] = fZNomPosY[kSe] + v; break;} 
	 case 'W' : case 'V' : { fZDeltaPosSt2and3[kSe] = v; fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe] + v; break;} 
	 case 'U' : { fZDeltaPosSt4and5[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaZ, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
//     
     void BTAlignGeom::SetDeltaZStation(char view,  size_t kSt, double v) {
       switch (view) {
     	 case 'X' : {
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
     	 case 'Y' : {
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
	 case 'W' :  case 'V' : { 
	  if (kSt == 2) { 
	    for (size_t kSe=0; kSe != 2; kSe++) { 
	      fZDeltaPosSt2and3[kSe] = v; fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe] + v;
	    }
	  }
	  break;
	 } 
	 case 'U' : { 
	   if (kSt == 5) { 
	     for (size_t kSe=0; kSe != 4; kSe++) { 
	       fZDeltaPosSt4and5[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe] + v;
	     }
	   }
	 }
	 default : { 
	      std::cerr << " BTAlignGeom::ZDeltaZStation, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetDeltaTr(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
	     fTrDeltaPosX[kSe] = v;  fTrPosX[kSe] = fTrNomPosX[kSe] + v;  break;  
	    } 
	 case 'Y' :  { 
	    fTrDeltaPosY[kSe] = v; fTrPosY[kSe] = fTrNomPosY[kSe] + v; break;
	 } 
	 case 'W' : case 'V' : { fTrDeltaPosSt2and3[kSe] = v; fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe] + v; break;} 
	 case 'U' :  { fTrDeltaPosSt4and5[kSe] = v; fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaTr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetValueTrShiftLastPlane(char view, double v) {
     
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fTrNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPosX[fNumSensorsXorY-1] = v;  fTrPosX[fNumSensorsXorY-1] = fTrNomPosX[fNumSensorsXorY-1] + v;  break;  
	    } 
	 case 'Y' :  { 
	    fTrDeltaPosY[fNumSensorsXorY-1] = v; fTrPosY[fNumSensorsXorY-1] = fTrNomPosY[fNumSensorsXorY-1] + v; break;
	 } 
	 case 'W' : case 'V' : { fTrDeltaPosSt2and3[fNumSensorsW-1] = v; fTrPosSt2and3[fNumSensorsW-1] = fTrNomPosSt2and3[fNumSensorsW-1] + v; break;} 
	 case 'U' : { fTrDeltaPosSt4and5[fNumSensorsU-1] = v; fTrPosSt4and5[fNumSensorsU-1] = fTrNomPosSt4and5[fNumSensorsU-1] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetValueTrShiftLastPlane, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
        } 
     }
     void BTAlignGeom::SetRoll(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollX[kSe] = v;   break;  
	    } 
	 case 'Y' :  { fRollY[kSe] = v;  break;} 
	 case 'U' :  { fRollSt4and5[kSe] = v;  break;} 
	 case 'V' :  case 'W' : { fRollSt2and3[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetRollCenter(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollXC[kSe] = v;   break;  
	    } 
	 case 'Y' :  { fRollYC[kSe] = v;  break;} 
	 case 'U' :  { fRollSt4and5C[kSe] = v;  break;} 
	 case 'V' :  case 'W' : { 
	       fRollSt2and3C[kSe] = v; 
	       break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetRollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetDeltaPitchCorr(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPitchX[kSe] = v;   break;  
	    } 
	 case 'Y' :  { fTrDeltaPitchY[kSe] = v;  break;} 
	 case 'W' : case 'V' : { fTrDeltaPitchSt2and3[kSe] = v;  break;} 
	 case 'U' :  { fTrDeltaPitchSt4and5[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaPitchCorr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetUnknownUncert(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X'  :{
	     fUnknownUncertX[kSe] = v;  break;  
	    } 
     	 case 'Y' :{
	     fUnknownUncertY[kSe] = v;  break;  
	    } 
	 case 'U' :  { fUnknownUncertU[kSe] = v;  break;} 
	 case 'V' :  case 'W' : {  fUnknownUncertW[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetUnknownUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetUnknownUncert(char view,  double v) {
       switch (view) {
     	 case 'X'  :{
	     for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) fUnknownUncertX[kSe] = v;  
	     break;  
	    } 
     	 case 'Y' :{
	     for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) fUnknownUncertY[kSe] = v;  
	     break;  
	    } 
	 case 'U' :  { for (size_t kSe=0; kSe != fNumSensorsU; kSe++) fUnknownUncertU[kSe] = v;  
	                 break;
			 } 
	 case 'V' :  case 'W' : {  for (size_t kSe=0; kSe != fNumSensorsW; kSe++) fUnknownUncertW[kSe] = v; 
	     break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetUnknownUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetMultScatUncert(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : case 'Y' :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fMultScatUncertXorY[kSe] = v;  break;  
	    } 
	 case 'U' :  { fMultScatUncertU[kSe] = v;  break;} 
	 case 'V' :  case 'W' : {  fMultScatUncertW[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetMultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     }	
     void BTAlignGeom::SetUncertErrorOutOfPencilBeam() { 
       this->SetUnknownUncert('X', 4, 100.); this->SetUnknownUncert('Y', 4, 100.);
       this->SetUnknownUncert('X', 6, 100.); this->SetUnknownUncert('Y', 6, 100.);
       this->SetUnknownUncert('U', 0, 100.); this->SetUnknownUncert('U', 2, 100.);
//       this->SetUnknownUncert('V', 100.);
     }
     void BTAlignGeom::MoveZPosOfXUVByY() {
       for (size_t kSt = 1; kSt != fNumStations; kSt++) { // Phase1b limits, the last station stay put.. 
         const double theZDeltaPosY = fZDeltaPosY[kSt]; 
	 // Phase1b : At station 4, we get the first double sensors at index 4, and the 5th sensor is at the same Z. 
	 this->SetDeltaZStation('X', kSt, theZDeltaPosY); 
	 if (kSt == 4) this->SetDeltaZStation('U', 4, theZDeltaPosY); 
       }
     }
   } // namespace 
}  // namespace   
     
