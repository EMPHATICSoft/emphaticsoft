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
       fHalfWaferWidth(0.5*fWaferWidth),  
       fZNomPosX{0.75, 121.25, 363.15, 484.15, 985.75, 985.75, 1211.95, 1211.95}, 
       fZNomPosY{0.15, 120.65, 360.75, 481.75, 986.35, 986.35, 1212.55, 1212.55},
       fZNomPosU{360.15,  481.15}, fZNomPosV{988.75, 988.75, 1214.95, 1214.95},
       fZDeltaPosX(fNumSensorsXorY, 0.),  fZDeltaPosY(fNumSensorsXorY, 0.), 
       fZDeltaPosU(fNumSensorsU, 0.), fZDeltaPosV(fNumSensorsV, 0.),
       fTrNomPosX{fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},
       fTrNomPosY{-fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, -fHalfWaferWidth, 
                  fWaferWidth, fWaferWidth, fWaferWidth, fWaferWidth},      
       fTrNomPosU{-fHalfWaferWidth, -fHalfWaferWidth},
       fTrNomPosV{fWaferWidth, -fWaferWidth, -fWaferWidth, -fWaferWidth},  
       fTrDeltaPosX(fNumSensorsXorY, 0.), fTrDeltaPosY(fNumSensorsXorY, 0.),  
       fTrDeltaPosU(fNumSensorsU, 0.), fTrDeltaPosV(fNumSensorsV, 0.), 
       fTrDeltaPitchX(fNumSensorsXorY, 0.), fTrDeltaPitchY(fNumSensorsXorY, 0.),  
       fTrDeltaPitchU(fNumSensorsU, 0.), fTrDeltaPitchV(fNumSensorsV, 0.), 
       fRollX(fNumSensorsXorY, 0.), fRollY(fNumSensorsXorY, 0.),  
       fRollU(fNumSensorsU, 0.), fRollV(fNumSensorsV, 0.),
       fMultScatUncertXorY{0., 0.003830147, 0.01371613, 0.01947578, 0.05067243, 0.05067243, 0.06630287, 0.06630287},
       fMultScatUncertU{0.05067243, 0.05067243}, 
       fMultScatUncertV{0.05067243, 0.05067243, 0.06630287, 0.06630287},        
       fUnknownUncertXorY(fNumSensorsXorY, 0.5), fUnknownUncertU(fNumSensorsU, 0.5), fUnknownUncertV(fNumSensorsV, 0.5),
       fZPosX(fNumSensorsXorY, 0.),  fZPosY(fNumSensorsXorY, 0.), 
       fZPosU(fNumSensorsU, 0.),fZPosV(fNumSensorsV, 0.),
       fTrPosX(fNumSensorsXorY, 0.),  fTrPosY(fNumSensorsXorY, 0.), 
       fTrPosU(fNumSensorsU, 0.),fTrPosV(fNumSensorsV, 0.)
     { 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosX[kSe] = fZNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fZPosY[kSe] = fZNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fZPosU[kSe] = fZNomPosU[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsV; kSe++) { fZPosV[kSe] = fZNomPosV[kSe]; } 

       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosX[kSe] = fTrNomPosX[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsXorY; kSe++) { fTrPosY[kSe] = fTrNomPosY[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsU; kSe++) { fTrPosU[kSe] = fTrNomPosU[kSe]; } 
       for (size_t kSe=0; kSe != fNumSensorsV; kSe++) { fTrPosV[kSe] = fTrNomPosV[kSe]; } 
       
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
	 case 'U' :  { fZDeltaPosU[kSe] = v; fZPosU[kSe] = fZNomPosU[kSe] + v; break;} 
	 case 'V' : case 'W' : { fZDeltaPosV[kSe] = v; fZPosV[kSe] = fZNomPosV[kSe] + v; break;}
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
	 case 'U' :  { 
	  if (kSt == 4) { 
	    for (size_t kSe=0; kSe != 2; kSe++) { 
	      fZDeltaPosU[kSe] = v; fZPosU[kSe] = fZNomPosU[kSe] + v;
	    }
	  }
	  break;
	 } 
	 case 'V' : case 'W' : { 
	   if (kSt == 5) { 
	     for (size_t kSe=0; kSe != 4; kSe++) { 
	       fZDeltaPosV[kSe] = v; fZPosV[kSe] = fZNomPosV[kSe] + v;
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
//	     if (sensor >= fTrNomPosX.size()) { std::cerr .... No checks!. 
	     fTrDeltaPosX[kSe] = v;  fTrPosX[kSe] = fTrNomPosX[kSe] + v;  break;  
	    } 
	 case 'Y' :  { fTrDeltaPosY[kSe] = v; fTrPosY[kSe] = fTrNomPosY[kSe] + v; break;} 
	 case 'U' :  { fTrDeltaPosU[kSe] = v; fTrPosU[kSe] = fTrNomPosU[kSe] + v; break;} 
	 case 'V' : case 'W' : { fTrDeltaPosV[kSe] = v; fTrPosV[kSe] = fTrNomPosV[kSe] + v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaTr, unknown view " << view << " fatal, quit " << std::endl; 
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
	 case 'U' :  { fRollU[kSe] = v;  break;} 
	 case 'V' :  case 'W' : { fRollV[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaRoll, unknown view " << view << " fatal, quit " << std::endl; 
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
	 case 'U' :  { fTrDeltaPitchU[kSe] = v;  break;} 
	 case 'V' :  case 'W' : { fTrDeltaPitchV[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
     void BTAlignGeom::SetUnknwonUncert(char view,  size_t kSe, double v) {
       switch (view) {
     	 case 'X' : case 'Y' :{
//	     if (sensor >= fRollNomPosX.size()) { std::cerr .... No checks!. 
	     fUnknownUncertXorY[kSe] = v;  break;  
	    } 
	 case 'U' :  { fUnknownUncertU[kSe] = v;  break;} 
	 case 'V' :  case 'W' : {  fUnknownUncertV[kSe] = v; break;}
	 default : { 
	      std::cerr << " BTAlignGeom::SetDeltaRoll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	}
     } 
   } // namespace 
}  // namespace   
     
