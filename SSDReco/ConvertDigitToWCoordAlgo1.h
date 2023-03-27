////////////////////////////////////////////////////////////////////////
/// \brief   2D aligner, X-Z view or Y-Z view, indepedently from each others. 
///          Algorithm one.  Could beong to SSDCalibration, but, this aligner 
///          requires some crude track reconstruction, as it is based on track residuals, i
///          of SSD strip that are on too often. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CONVERTDIGITTOWCOORDALGO1_H
#define CONVERTDIGITTOWCOORDALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "RecoBase/SSDCluster.h"

namespace emph { 
  namespace ssdr {
 
    class ConvertDigitToWCoordAlgo1 {
    
       public:
      
	explicit ConvertDigitToWCoordAlgo1(char aView); // X or Y, U or V
	
        private:
	  const size_t fNumStations = 6;
	  const size_t fNumStrips = 639; // Per wafer. 
	  const double fOneOverSqrt12; 
	  char fView;
	  bool fMomentumIsSet;      
	  double fPitch;
	  double fHalfWaferHeight;
	  std::vector<double> fZCoords;
	  std::vector<double> fZLocShifts;
	  double fZCoordsMagnetCenter, fMagnetKick120GeV; 
	  std::vector<double> fNominalOffsets; // for station 4 and 5, Y View Sensor 3, if run 1055
	  // if MC, or other lower momentum runs, we need 
	  std::vector<double> fDownstreamGaps; 
	  std::vector<double> fResiduals;   // the current one, for the a specific event. Actually, not used so far. 
	  std::vector<double> fMeanResiduals;// the meanvalue over a run..Or previously fitted..  
// 
// Additional variables for resolution studies,  chi square cuts and so forth. 
//	  
	  std::vector<double> fMultScatUncert;
	  std::vector<double> fOtherUncert;
	  std::vector<double> fPitchOrYawAngles;
	  
	public:
	 inline void SetOtherUncert(const std::vector<double> v) { fOtherUncert = v; } 
	 inline void SetPitchAngles(const std::vector<double> v) { fPitchOrYawAngles = v; } 
	 inline void SetFittedResiduals(std::vector<double> v) { fMeanResiduals = v;} 
	 inline void SetZLocShifts(std::vector<double> v) { fZLocShifts = v;} 
	 inline void SetTransientResiduals(std::vector<double> v) { fResiduals = v;} // Actually, not used yet.. 
	 inline void SetMultScatUncert(std::vector<double> v) {fMultScatUncert = v;} 
	 inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; }
	 inline void SetDownstreamGap(size_t aStation, double v) { 
	   if ((aStation != 4) && (aStation != 5)) {
	     std::cerr << " ConvertDigitToWCoordAlgo1, Station  " << aStation << " has no double sensor, fatal, quit here " << std::endl; 
	     exit(2);
	   } 
	   fDownstreamGaps[aStation-4] = v; 
	 }
	 inline void SetTheView(char aView) {
           if (aView == 'W') aView = 'V';
	   if ((aView != 'X') && (aView != 'Y') && (aView != 'U')  && (aView != 'V')) {
	     std::cerr << " ConvertDigitToWCoordAlgo1, setting an unknown view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	   }
	   fView = aView;
	 }
	 void InitializeAllCoords(const std::vector<double> &zCoords);
	 void SetForMomentum(double p); // Rescale the Magnet kick, deviation on the X-Z plane 
	 
	 inline double GetTsUncertainty(size_t kSt, std::vector<rb::SSDCluster>::const_iterator itCl) const {
	  double aRMS = itCl->WgtRmsStrip();
	  double errMeasSq = (1.0/12.)*fPitch * fPitch * 1.0/(1.0 + aRMS*aRMS); // Very approximate, need a better model. 
	  return std::sqrt(errMeasSq + fOtherUncert[kSt]*fOtherUncert[kSt] + fMultScatUncert[kSt]*fMultScatUncert[kSt]);
	  
	 }
	 inline double GetTsFromCluster(size_t kStation, size_t kPlane, double strip) {
	   switch (fView) { // see SSDCalibration/SSDCalibration_module 
	     case 'X' :
	     {
	       double aVal=0.;
	       if (kStation < 4) {
	         aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	       } else {
		    aVal =  strip*fPitch - fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation];
		    if (kPlane == 0) aVal *= -1.;
//		    else aVal += 2*fPitch; // To compensate for the wrong sign addition in the NominalOffset
		 // Momentum correction, for 120 GeV primary beam 
		    aVal += fMagnetKick120GeV * (fZCoords[kStation] - fZCoordsMagnetCenter);
		 }
		 // Yaw Correction 
	       return aVal;
	       break;
	     } 
	     case 'Y' : 
	      {
	       double aVal = 0.;
	       if (kStation < 4) {  
	         aVal =  (strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	       } else {
//
//   When using the Monte-Carlo, and possibly the data , we had a sign mistake, before March 23 2023 	       
		   aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
		   if (kPlane == 2) { aVal *= -1;  } // last correction, related fence counting.. MC based ! Guess.. to be checked..
//		   if (kPlane == 3) { aVal -= fPitch; } // last correction, related fence counting.. MC based ! Probably not the last one
	       
//	         if (!alternate45) aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	       } 
	       const double aValC = this->correctTsForPitchOrYawAngle(kStation, aVal);
	       return aValC;
	       break; 
	      }
	      case 'U' : // Station 2 and 3 
	      {
		 double aVal =  (strip*fPitch + fNominalOffsets[kStation]  + 1.0e-10);
	         return aVal;
	      } 
	      case 'V' :// Station 4 and 5, double sensors.  
	      {
	         if (kStation < 4) {
	            std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected station " << kStation 
		              << " for W view internal error, fatal " << std::endl; exit(2);
		 }
	         if (kPlane < 4) {
	            std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected sensor " << kPlane 
		              << " for W view internal error, fatal " << std::endl; exit(2);
		 }
		 double aVal =  (-strip*fPitch + fNominalOffsets[kStation]  + 1.0e-10);
		 if (kPlane == 4) aVal *= -1.;
	        return aVal;
	      } 
	      default :
	        std::cerr << " ConvertDigitToWCoordAlgo1::getTsFromCluster, unexpected view, "
		<< fView << " kStation " << kStation << " Sensor " << kPlane << 
		 " internal error, fatal " << std::endl; exit(2);
	   }
	   return 0.; // should not happen  
	 }
	 inline double correctTsForPitchOrYawAngle(size_t kStation, double ts) {
	   return (ts + ts*fPitchOrYawAngles[kStation]);
	 }
    };
  
  } // namespace ssdr
}// namespace emph

#endif // ConvertDigitToWCoordAlgo1
