////////////////////////////////////////////////////////////////////////
/// \brief   2D aligner, X-Z view or Y-Z view, indepedently from each others. 
///          Algorithm one.  Could beong to SSDCalibration, but, this aligner 
///          requires some crude track reconstruction, as it is based on track residuals, i
///          of SSD strip that are on too often. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>
//

#include "SSDReco/SSDHotChannelList.h" 
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/ConvertDigitToWCoordAlgo1.h" 

 using namespace emph;

namespace emph {
  namespace ssdr {

     ConvertDigitToWCoordAlgo1::ConvertDigitToWCoordAlgo1(char aView) :
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fView(aView), fMomentumIsSet(false), fPitch(0.06), fHalfWaferHeight(0.5*static_cast<int>(fNumStrips)*fPitch), 
       fZCoords(fNumStations+2, 0.), fZLocShifts(fNumStations+2, 0.), 
       fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fNominalOffsets(fNumStations, 0.), fDownstreamGaps(2, 0.), fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0),
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.),
       fPitchOrYawAngles(fNumStations, 0.) 
     { 
        if (aView == 'W') aView = 'V';
        if ((aView != 'X') && (aView != 'Y') && (aView != 'U') && (aView != 'V')) {
	     std::cerr << " ConvertDigitToWCoordAlgo1, setting an unknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	}
     }
     void ConvertDigitToWCoordAlgo1::SetForMomentum(double p) {
       if (fMomentumIsSet) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::SetForMomentum, already called, skip!!! " << std::endl;
	 return;
       }
       const double pRatio = 120.0 / p;
       fMagnetKick120GeV *= pRatio;
       for (size_t k=0; k != fMultScatUncert.size(); k++) fMultScatUncert[k] *= std::abs(pRatio); 
       fMomentumIsSet = true;
     }
     void  ConvertDigitToWCoordAlgo1::InitializeAllCoords(const std::vector<double> &zCoords) {
       if (zCoords.size() != 8) {
         std::cerr << "  ConvertDigitToWCoordAlgo1::InitailizeCoords Unexpected number of ZCoords, " 
	           << zCoords.size() << " fatal, and that is it! " << std::endl; exit(2); 
       }
       for (size_t k=0; k != 5; k++) fZCoords[k] = zCoords[k]; 
       fZCoords[5] = zCoords[7]; 
       std::cerr << " ConvertDigitToWCoordAlgo1::InitializeCoords ";
       for (size_t kSt=0; kSt != fZCoords.size(); kSt++) std::cerr << " " << fZCoords[kSt];
        std::cerr << std::endl;
       for (size_t k=0; k != fNumStations; k++) {
        if (std::abs(fZLocShifts[k]) > 1.0e-10) {
	   std::cerr << " ... For Station  " << k <<  " Shifting the Z position by " << fZLocShifts[k] << " from " << fZCoords[k];
	  fZCoords[k] += fZLocShifts[k]; 
	  std::cerr << " to " << fZCoords[k] << std::endl;
	}
	if (std::abs(fOtherUncert[k]) > 1/0e-10) {
	  std::cerr << " ....  For Station " << k <<  " Adding (in quadrature) transverse position uncertainty of  " 
	  << fOtherUncert[k] << " from " << fZCoords[k] << std::endl;
	}
       }
       switch (fView) { 
         case 'X' :
           for (size_t k=0; k != 4; k++) { 
//             fNominalOffsets[k] = fHalfWaferHeight +  fPitch; // xcoord is proportional to - strip number..v2b 
//             fNominalOffsets[k] = fHalfWaferHeight + 2.0*fPitch; // xcoord is proportional to - strip number.. v2c
             fNominalOffsets[k] = fHalfWaferHeight - fPitch; // xcoord is proportional to - strip number.. v2c
	   }
//	   fNominalOffsets[4] = 2.0*fHalfWaferHeight - fPitch; // To be checked.. v2b
//	   fNominalOffsets[5] = 2.0*fHalfWaferHeight + fPitch; // To be checked.. 
//	   fNominalOffsets[4] = 2.0*fHalfWaferHeight + 2.0*fPitch; // To be checked.. v2c Seems O.K. on one event 
//	   fNominalOffsets[5] = 2.0*fHalfWaferHeight + 2.0*fPitch; // To be checked.. v2c No good, too low.. 
//	   fNominalOffsets[5] = 2.0*fHalfWaferHeight + 4.0*fPitch; // To be checked.. v2d ???? Weird.. high statistics need.  No field. 
           fNominalOffsets[4] = 2.0*fHalfWaferHeight; fNominalOffsets[5] = 2.0*fHalfWaferHeight;
	    break;
	 case 'Y' :
           for (size_t k=0; k != 4; k++) { 
             fNominalOffsets[k] = -fHalfWaferHeight; // ycoord is proportional to  strip number.. 
	   }
	   for (size_t k=4; k != 6; k++)  {
	     fNominalOffsets[k] = 2.0*fHalfWaferHeight; // Using the 2nd sensor. Called Sensor 3 for Y I think... Y coord to trip number is flipped. 
	   }
           // 
           // Min amd maximum window.. To be defined.. Based on histogramming cluster positions.. 
           //
	   // Comment this out, that was for data, trial and error.. We should use the methos Select by View in the main module.. 
	   // 
	   // fMinStrips[0] = 250.; fMaxStrips[0] = 500.; 
	  // fMinStrips[1] = 260.; fMaxStrips[1] = 510.; 
	  // fMinStrips[2] = 275.; fMaxStrips[2] = 525.; 
	  // fMinStrips[3] = 275.; fMaxStrips[3] = 525.; 
	  // fMinStrips[4] = 525.; fMaxStrips[4] = 700.; // There are dead channels at lower strip count, distribution of strip choppy. 
	  // fMinStrips[5] = 490.; fMaxStrips[5] = 700.; 
	   break;
	   //
	   // Need to implement U & V 
	 case 'U' : 
	   fNominalOffsets[2] = -fHalfWaferHeight; // Sign to be checked !  
 	   fNominalOffsets[3] = -fHalfWaferHeight; // Sign to be checked ! 
	   break;
	 case 'V' : 
	   for (size_t k=4; k != 6; k++)  {
	     fNominalOffsets[k] = 2.0*fHalfWaferHeight; // 
	   }
	   break;
	   
      }
       // Setting of the uncertainties.  Base on G4EMPH, see g4gen_jobC.fcl, Should be valid for X and Y  But it does includes the target.
       //
       // Obtained in Dec 2022, with target in. 
       //
       /*
       fMultScatUncert[1] =  0.003201263;   
       fMultScatUncert[2] =  0.02213214;   
       fMultScatUncert[3] =  0.03676218;   
       fMultScatUncert[4] =  0.1022451;   
       fMultScatUncert[5] =  0.1327402; 
       */ 
       fMultScatUncert[1] =  0.003830147;   
       fMultScatUncert[2] =  0.01371613;	
       fMultScatUncert[3] =  0.01947578;	
       fMultScatUncert[4] =  0.05067243;  
       fMultScatUncert[5] =  0.06630287;
       /*
       ** This is momentum dependent... Carefull..At 120 GeV, no target, one has: (after rejection of wide scattering event, presumably hadronic.) 
       */
       fMultScatUncert[1] =  0.003201263;   
       fMultScatUncert[2] =  0.00512;   
       fMultScatUncert[3] =  0.0092;   
       fMultScatUncert[4] =  0.0212;   
       fMultScatUncert[5] =  0.0264; 
        
     }
  } // ssdr
} // emph
