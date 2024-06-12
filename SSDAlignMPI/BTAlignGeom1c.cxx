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

#include "BTAlignGeom1c.h"

namespace emph {
  namespace rbal {
  
     BTAlignGeom1c::BTAlignGeom1c():
       fZCoordsMagnetCenter(987.645), fMagnetKick120GeV(-0.570e-3),
       fPitch(0.06),
       fWaferWidth(static_cast<int>(fNumStrips)*fPitch),
       fHalfWaferWidth(0.5*fWaferWidth), fIntegrationStepSize(0.),  
//       fZNomPosX{0.45, 281.45, 501.75, 615.75, 846.45, 1139.88, 1139.88, 1465.32, 1465.32},  // revised Sept  2023, by LinYan, picked up from the gdml file 
       fZNomPosX{0.45, 281.45, 501.75, 615.75, 846.45, 1143.51, 1143.51, 1465.32, 1465.32},  // Station 5 invesigation, Dec 14 2023 run 7o61
       fZNomPosY{-6.5, 274.5, 501.45, 615.45, 839.5, 1146.83, 1146.83, 1472.27, 1472.27}, 
       fZNomPosSt2and3{494.5, 608.5},
       fZNomPosSt4and5{1147.13, 1147.13, 1472.57, 1472.57}, // actually 5 and 6 in Phase1c
       fZDeltaPosX(fNumSensorsXorY, 0.),  fZDeltaPosY(fNumSensorsXorY, 0.), 
       fZDeltaPosSt2and3(fNumSensorsU, 0.), fZDeltaPosSt4and5(fNumSensorsW, 0.),
       fTrNomPosX{fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth,
                  0., 0., 0., 0.},
       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth,
                  0., 0., 0., 0.},    
       fTrNomPosSt2and3{-fHalfWaferWidth, -fHalfWaferWidth}, //  give shits of -15. ? Investigating..
//       fTrNomPosSt4and5{-fWaferWidth, -fWaferWidth, -fWaferWidth, -fWaferWidth},  // Weird...!... MC bug ???? 
       fTrNomPosSt4and5{0., 0., 0., 0.},  // Weird...!... MC bug ???? 
       fTrDeltaPosX(fNumSensorsXorY, 0.), fTrDeltaPosY(fNumSensorsXorY, 0.),  
       fTrDeltaPosSt2and3(fNumSensorsU, 0.), fTrDeltaPosSt4and5(fNumSensorsW, 0.), 
       fTrDeltaPitchX(fNumSensorsXorY, 0.), fTrDeltaPitchY(fNumSensorsXorY, 0.),  
       fTrDeltaPitchSt2and3(fNumSensorsU, 0.), fTrDeltaPitchSt4and5(fNumSensorsW, 0.), 
       fRollX(fNumSensorsXorY, 0.), fRollY(fNumSensorsXorY, 0.),  
       fRollSt2and3(fNumSensorsU, 0.), fRollSt4and5(fNumSensorsW, 0.),
       fRollXC(fNumSensorsXorY, 0.), fRollYC(fNumSensorsXorY, 0.),  
       fRollSt2and3C(fNumSensorsU, 0.), fRollSt4and5C(fNumSensorsW, 0.),
//       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.05067243, 0.05067243, 0.06630287, 0.06630287}, // At 120 GeV, with target
//       fMultScatUncertU{0.01948, 0.05067243}, 
//       fMultScatUncertW{0.05067243, 0.05067243, 0.06630287, 0.06630287},  
//   Seq 4 1 to l        
//       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.02, 0.02, 0.02, 0.02}, // At 120 GeV, with No target (guessed 
//       fMultScatUncertU{0.01948, 0.02}, 
//       fMultScatUncertW{0.05067243, 0.02, 0.02, 0.02},   // Weird!!! Over-weighting Station 5, probably...      
       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.024, 0.0283, 0.0283, 0.04, 0.04}, // At 120 GeV, with No target (guessed a bit!!!)  
       fMultScatUncertU{0.014, 0.0195}, 
       fMultScatUncertW{0.0283, 0.0283, 0.04, 0.04},   // Weird!!! Over-weighting Station 5, probably...      
       fUnknownUncertX(fNumSensorsXorY, 1.0e-8), fUnknownUncertY(fNumSensorsXorY, 1.0e-8),
       fUnknownUncertW(fNumSensorsW, 1.0e-8), fUnknownUncertU(fNumSensorsU, 1.0e-8),
       fZPosX(fNumSensorsXorY, 0.),  fZPosY(fNumSensorsXorY, 0.), 
       fZPosSt2and3(fNumSensorsU, 0.),fZPosSt4and5(fNumSensorsW, 0.),
       fTrPosX(fNumSensorsXorY, 0.),  fTrPosY(fNumSensorsXorY, 0.), 
       fTrPosSt2and3(fNumSensorsU, 0.),fTrPosSt4and5(fNumSensorsW, 0.)
     { 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosX[kSe] = fZNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosY[kSe] = fZNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsW; kSe++) { fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe]; } 

       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosX[kSe] = fTrNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosY[kSe] = fTrNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsW; kSe++) { fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe]; } 
       //
       // Attempting to understand problem at 30 GeV, kick is wrong.. Reconstructed momentum is 18.4, in average... 
       // This is with a fixed kick. 
       // fMagnetKick120GeV *= 30.0/18.4; 
     }
     //
     // Setters 
     //
     void BTAlignGeom1c::SetDeltaZ(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     fZDeltaPosX[kSe] = v; fZPosX[kSe] = fZNomPosX[kSe] + v;  break;  
	    } 
	 case 'Y' :  { fZDeltaPosY[kSe] = v; fZPosY[kSe] = fZNomPosY[kSe] + v; break;} 
	 case 'U' : { fZDeltaPosSt2and3[kSe] = v; fZPosSt2and3[kSe] = fZNomPosSt2and3[kSe] + v; break;} 
	 case 'V' : case 'W' :{ fZDeltaPosSt4and5[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetDeltaZ, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
//     
     void BTAlignGeom1c::SetDeltaZStation(char view,  size_t kSt, double v) {
       switch (view) {
     	 case 'X' : {
	     if (kSt < 5) { 
	       fZDeltaPosX[kSt] =  v; fZPosX[kSt] = fZNomPosX[kSt] + v; 
	     } else if (kSt == 5) {
	       for (size_t kSe=5; kSe != 7; kSe++) {
	          fZDeltaPosX[kSe] =  v; fZPosX[kSe] = fZNomPosX[kSe] + v; 
	       }
	     }  else if (kSt == 6) {
	       for (size_t kSe=7; kSe != 9; kSe++) {
	          fZDeltaPosX[kSe] =  v; fZPosX[kSe] = fZNomPosX[kSe] + v; 
	       }  
	    }
	    break;
	 }
     	 case 'Y' : {
	   if (kSt < 5) { 
	     fZDeltaPosY[kSt] =  v; fZPosY[kSt] = fZNomPosY[kSt] + v; 
	   } else if (kSt == 5) {
	     for (size_t kSe=5; kSe != 7; kSe++) {
	  	fZDeltaPosY[kSe] =  v; fZPosY[kSe] = fZNomPosY[kSe] + v; 
	     }
	   }  else if (kSt == 6) {
	     for (size_t kSe=7; kSe != 9; kSe++) {
	  	fZDeltaPosY[kSe] =  v; fZPosY[kSe] = fZNomPosY[kSe] + v; 
	     }  
	   }
	   break;
	 }	  
	 case 'U' :  { 
	    fZDeltaPosSt2and3[kSt-2] = v; fZPosSt2and3[kSt-2] = fZNomPosSt2and3[kSt-2] + v;
	  break;
	 } 
	 case 'V' : case 'W' : { 
	     for (size_t kkSe=0; kkSe != 2; kkSe++) { 
	       size_t kSe = kSt -5 +kkSe;
	       fZDeltaPosSt4and5[kSe] = v; fZPosSt4and5[kSe] = fZNomPosSt4and5[kSe] + v;
	     }
	 }
	 default : { 
	      std::cerr << " BTAlignGeom1c::ZDeltaZStation, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetDeltaTr(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
	     fTrDeltaPosX[kSe] = v;  fTrPosX[kSe] = fTrNomPosX[kSe] + v;  break;  
	    } 
	 case 'Y' :  { 
	    fTrDeltaPosY[kSe] = v; fTrPosY[kSe] = fTrNomPosY[kSe] + v; break;
	 } 
	 case 'U' : { fTrDeltaPosSt2and3[kSe] = v; fTrPosSt2and3[kSe] = fTrNomPosSt2and3[kSe] + v; break;} 
	 case 'W' :  case 'V' : { fTrDeltaPosSt4and5[kSe] = v; fTrPosSt4and5[kSe] = fTrNomPosSt4and5[kSe] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetDeltaTr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetValueTrShiftLastPlane(char view, double v) {
     
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fTrNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPosX[fNumSensorsXorY-1] = v;  fTrPosX[fNumSensorsXorY-1] = fTrNomPosX[fNumSensorsXorY-1] + v;  break;  
	    } 
	 case 'Y' :  { 
	    fTrDeltaPosY[fNumSensorsXorY-1] = v; fTrPosY[fNumSensorsXorY-1] = fTrNomPosY[fNumSensorsXorY-1] + v; break;
	 } 
	 case 'U' : { fTrDeltaPosSt2and3[fNumSensorsU-1] = v; fTrPosSt2and3[fNumSensorsU-1] = fTrNomPosSt2and3[fNumSensorsU-1] + v; break;} 
	 case 'W' : case 'V' : { fTrDeltaPosSt4and5[fNumSensorsW-1] = v; fTrPosSt4and5[fNumSensorsW-1] = fTrNomPosSt4and5[fNumSensorsW-1] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetValueTrShiftLastPlane, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  }
        } 
     }
     void BTAlignGeom1c::SetRoll(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollX[kSe] = v;     
//	     std::cerr << " BTAlignGeom1c::SetRoll ... X View for kSe " << kSe << " Setting Roll to " << v << std::endl; 
	     break;
	    } 
	 case 'Y' :  { fRollY[kSe] = v;  break;} 
	 case 'W' : case 'V' : { fRollSt4and5[kSe] = v;  break;} 
	 case 'U' :  { fRollSt2and3[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetRollCenter(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fRollXC[kSe] = v;   break;  
	    } 
	 case 'Y' :  { fRollYC[kSe] = v;  break;} 
	 case 'U' :  { fRollSt2and3C[kSe] = v;  break;} 
	 case 'V' :  case 'W' : { 
	       fRollSt4and5C[kSe] = v; 
	       break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetRollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetDeltaPitchCorr(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : {
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPitchX[kSe] = v;   break;  
	    } 
	 case 'Y' :  { fTrDeltaPitchY[kSe] = v;  break;} 
	 case 'U' : { fTrDeltaPitchSt2and3[kSe] = v;  break;} 
	 case 'W' :  case 'V' :  { fTrDeltaPitchSt4and5[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetDeltaPitchCorr, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetUnknownUncert(char view,  size_t kSe, double v) {
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
	      std::cerr << " BTAlignGeom1c::SetUnknownUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetUnknownUncert(char view,  double v) {
       switch (view) {
     	 case 'X'  :{
	     for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) fUnknownUncertX[kSe] = v;  
	     break;  
	    } 
     	 case 'Y' :{
	     for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) fUnknownUncertY[kSe] = v;  
	     break;  
	    } 
	 case 'W' :  case 'V' : { for (size_t kSe=0; kSe != fNumSensorsW; kSe++) fUnknownUncertW[kSe] = v;  
	                 break;
			 } 
	 case 'U' :  {  for (size_t kSe=0; kSe != fNumSensorsU; kSe++) fUnknownUncertU[kSe] = v; 
	     break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetUnknownUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom1c::SetMultScatUncert(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : case 'Y' :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fMultScatUncertXorY[kSe] = v;  break;  
	    } 
	 case 'U' :  { fMultScatUncertU[kSe] = v;  break;} 
	 case 'V' :  case 'W' : {  fMultScatUncertW[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom1c::SetMultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     }	
     void BTAlignGeom1c::SetUncertErrorOutOfPencilBeam() { 
       this->SetUnknownUncert('X', 4, 100.); this->SetUnknownUncert('Y', 4, 100.);
       this->SetUnknownUncert('X', 6, 100.); this->SetUnknownUncert('Y', 6, 100.);
//       this->SetUnknownUncert('U', 0, 100.); this->SetUnknownUncert('U', 2, 100.);
//       this->SetUnknownUncert('V', 100.);
     }
     void BTAlignGeom1c::MoveZPosOfXUVByY() {
       for (size_t kSt = 1; kSt != fNumStations; kSt++) { // Phase1b limits, the last station stay put.. 
         const double theZDeltaPosY = fZDeltaPosY[kSt]; 
	 // Phase1b : At station 4, we get the first double sensors at index 4, and the 5th sensor is at the same Z. 
	 this->SetDeltaZStation('X', kSt, theZDeltaPosY); 
	 if ((kSt == 2) || (kSt == 3)) this->SetDeltaZStation('U', kSt, theZDeltaPosY); 
	 if ((kSt == 5) || (kSt == 6)) this->SetDeltaZStation('V', kSt, theZDeltaPosY); 
       }
     }
     void BTAlignGeom1c::SetUnknownUncertForStations(const std::vector<int> sts) {
       for (size_t k=0; k != sts.size(); k++) {
         switch (sts[k]) {
	   case 2: 
	   case 3: 
	   {
             this->SetUnknownUncert('X', static_cast<size_t>( sts[k]), 5000.);
             this->SetUnknownUncert('Y', static_cast<size_t>( sts[k]), 5000.);
             this->SetUnknownUncert('U', static_cast<size_t>( sts[k]-2), 5000.);
	     break;
	   }
	   
	   case 4: 
	   {
	      this->SetUnknownUncert('X', 4, 5000.);
              this->SetUnknownUncert('Y', 4, 5000.);
 	      break;
           }
	   case 5: 
	   {
              this->SetUnknownUncert('V', 0, 5000.); this->SetUnknownUncert('V', 1, 5000.);
              this->SetUnknownUncert('X', 5, 5000.); this->SetUnknownUncert('X', 6, 5000.);
              this->SetUnknownUncert('Y', 5, 5000.); this->SetUnknownUncert('Y', 6, 5000.);
 	      break;
            }
	   case 6: 
	   {
              this->SetUnknownUncert('V', 2, 5000.); this->SetUnknownUncert('V', 3, 5000.);
              this->SetUnknownUncert('X', 7, 5000.); this->SetUnknownUncert('X', 8, 5000.);
              this->SetUnknownUncert('Y', 7, 5000.); this->SetUnknownUncert('Y', 8, 5000.);
 	      break;
            }
	 
	   default: break;
	 }
       }
     
     }
   } // namespace 
}  // namespace   
     
