////////////////////////////////////////////////////////////////////////
/// \brief   Convertion of a SSDCluster strip average and RMS to a coordinate.  
///          Algorithm one.  Could beong to SSDCalibration... 
///          The first version uses its private 2D alignment based figures. 
///          The most recent version uses the volaltile 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CONVERTDIGITTOWCOORDALGO1_H
#define CONVERTDIGITTOWCOORDALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "RecoBase/SSDCluster.h"
#include "SSDReco/VolatileAlignmentParams.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "DetGeoMap/DetGeoMap.h"

namespace emph { 
  namespace ssdr {
 
    class ConvertDigitToWCoordAlgo1 {
    
       public:
      
	explicit ConvertDigitToWCoordAlgo1(char aView); // X or Y, U or V,  or All of them... 
	explicit ConvertDigitToWCoordAlgo1(int aRunNum, char aView); // To support both Phase1b and Phase1c 
	//   X or Y, U or V,  or All of them... 
	
        private:
	  static const size_t fNumStations = 8; // Upgrade to Phase1c, from 6 to 8.. 
	  static const size_t fNumStrips = 640; // Per wafer. 5 readout chips, 128 channel each Was 639 Back in Nov. 2023.. 
	  static const double fOneOverSqrt12;
	  static const double fOneOverSqrt2;
	  //
	  // New access to the standard geometry..(strip based geom..)  
	  //
          art::ServiceHandle<emph::dgmap::DetGeoMapService> fDetGeoMapService;
	  emph::dgmap::DetGeoMap *fDetGeoMap; 
	  art::ServiceHandle<emph::geo::GeometryService> fGeoService;
	  emph::geo::Geometry *fEmgeo;

	  bool fIsPhase1c; // by default, phase1b.   
	  bool fIsMC; // Ugly!. But Obsolete.. we hope... Stereo angle use in GeoDetMap  
	  bool fIsReadyToGo; // Can't fully initialize everything in the constructor.. This should be amn ar service!!! 
	  char fView;
	  bool fDebugIsOn;
	  bool fMomentumIsSet;
	  double fEffMomentum;      
	  double fPitch;
	  double fHalfWaferHeight;
	  // This is mostly obsolete... need clean-up
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
	  std::vector<double> fMultScatUncert120; // at 120 GeV, does not include the target! 
	  std::vector<double> fMultScatUncert;  // Possibly rescaled at a given momentum. 
	  std::vector<double> fOtherUncert;
	  std::vector<double> fPitchOrYawAngles;
//
// This was the old version (Winter 2023..) now.... 
//
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;

	  
	public:
	 inline void SetPhase1X( int aRunNum) { 
	    fIsPhase1c = (aRunNum > 1999);
	    if (fIsPhase1c) this->SetForPhase1X();  
	 }  
	 inline void SetDebugOn( bool v=true) { fDebugIsOn = v; } 
	 inline void SetForMC(bool v=true) { fIsMC = v;} // Obsolete.. we hope !   
	 inline void SetOtherUncert(const std::vector<double> v) { fOtherUncert = v; } // Obsolete.. 
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
	 inline bool IsReadyToGo() const {return fIsReadyToGo; } // This class should be cleaned, with Volatile, moved as Geometry service. 
	 inline double GetMultScatUncert(size_t kSt) const { return fMultScatUncert[kSt]; } // no checks.. Explit args in SSDRecDwnnstTr
	 inline double GetMultScatUncert120(size_t kSt) const { return fMultScatUncert120[kSt]; }
	 inline double GetTsUncertainty(size_t kSt, std::vector<rb::SSDCluster>::const_iterator itCl) const {
	  double aRMS = itCl->WgtRmsStrip();
	  double errMeasSq = (1.0/12.)*fPitch * fPitch * 1.0/(1.0 + aRMS*aRMS); // Very approximate, need a better model. 
	  return std::sqrt(errMeasSq + fOtherUncert[kSt]*fOtherUncert[kSt] + fMultScatUncert[kSt]*fMultScatUncert[kSt]);
	  
	 }
	 inline double GetTsFromCluster(size_t kStation, size_t kPlane, double strip) {
	   std::cerr << " ConvertDigitToWCoordAlgo1::GetTsFromCluster Obsolete coordinate setting, quit here and now !! " << std::endl; exit(2);
	   switch (fView) { // see SSDCalibration/SSDCalibration_module 
	     case 'X' :
	     {
	       double aVal=0.;
	       if (!fIsPhase1c) { 
	         if (kStation < 4) {
	           aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	         } else {
		    aVal =  strip*fPitch - fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation];
		    if (kPlane == 0) aVal *= -1.;
//		    else aVal += 2*fPitch; // To compensate for the wrong sign addition in the NominalOffset
		 // Momentum correction, for 120 GeV primary beam 
		    aVal += fMagnetKick120GeV * (fZCoords[kStation] - fZCoordsMagnetCenter);
		 }
		 // Yaw Correction ? 
	       } else { // Phase1c To be checked.. 
	         if (kStation < 5) {
	           aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	         } else {
		    aVal =  strip*fPitch - fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation];
		    if (kPlane == 0) aVal *= -1.;
//		    else aVal += 2*fPitch; // To compensate for the wrong sign addition in the NominalOffset
		 // Momentum correction, for 120 GeV primary beam 
		    aVal += fMagnetKick120GeV * (fZCoords[kStation] - fZCoordsMagnetCenter);
	          }
		} 
	       return aVal;
	       break;
	     } 
	     case 'Y' : 
	      {
	       double aVal = 0.;
	       if (!fIsPhase1c) {
	         if (kStation < 4) {  
	           aVal =  (strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	         } else {
//
//   When using the Monte-Carlo, and possibly the data , we had a sign mistake, before March 23 2023 	       
		   aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
		   if (kPlane == 2) { aVal *= -1;  } // last correction, related fence counting.. MC based ! Guess.. to be checked..
	       
	         } 
		} else { // Phase1c To commissioned.. 
	          if (kStation < 4) {  
	            aVal =  (strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	          } else {
//
//   When using the Monte-Carlo, and possibly the data , we had a sign mistake, before March 23 2023 	       
		   aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
		   if (kPlane == 2) { aVal *= -1;  } // last correction, related fence counting.. MC based ! Guess.. to be checked..
	         } 
		
		}
	       const double aValC = this->correctTsForPitchOrYawAngle(kStation, aVal); // Nov. 2023: still a place holder.. 
	       return aValC;
	       break; 
	      }
	      case 'U' : // Station 2 and 3 .  For Phase1c, 2 3 and 4 
	      {
		 double aVal =  (strip*fPitch + fNominalOffsets[kStation]  + 1.0e-10);
	         return aVal;
	      } 
	      case 'V' :// Station 4 and 5, double sensors.  
	      {
	        if (!fIsPhase1c) {
	          if (kStation < 4) {
	            std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected station " << kStation 
		              << " for W view internal error, fatal " << std::endl; exit(2);
		  }
	          if (kPlane < 4) {
	            std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected sensor " << kPlane 
		              << " for W view internal error, fatal " << std::endl; exit(2);
		  }
		} else { 
		 // 
		 // Phase1c : No checks.. 
		 //  
		}
		double aVal =  (-strip*fPitch + fNominalOffsets[kStation]  + 1.0e-10);
		if ((!fIsPhase1c) && (kPlane == 4)) aVal *= -1.;
		// 
	        return aVal;
	      } 
	      default :
	        std::cerr << " ConvertDigitToWCoordAlgo1::getTsFromCluster, unexpected view, "
		<< fView << " kStation " << kStation << " Sensor " << kPlane << 
		 " internal error, fatal " << std::endl; exit(2);
	   } //switch view 
	   return 0.; // should not happen  
	 }
	 inline double correctTsForPitchOrYawAngle(size_t kStation, double ts) {
	   return (ts + ts*fPitchOrYawAngles[kStation]);
	 }
//
// New version... To be included in SSD3DTrackFitFCNAlgo1, at some point... Or, this entire class could go away.. 
	 
	 std::pair<double, double> getTrCoord(std::vector<rb::SSDCluster>::const_iterator it, double pMomMultScatter) const; 
	 // the 2nd argument is the presumed momentum, to compute the multiple scattering uncertainty. Target not included.  
	 // the pair is (coord, corrdErrSquared)  
	 std::pair<double, double> getTrCoord1c(std::vector<rb::SSDCluster>::const_iterator it, double pMomMultScatter) const; 
	 double getTrCoordRoot(std::vector<rb::SSDCluster>::const_iterator it); // Via the Root based geometry, as in the event display..  
	 // Checked on for Phase1b, and actually found it problematic, Oct-Nov 2023 
	
      private: 
        void SetForPhase1X(); // Place holder.. 	

	 
    }; // this class 
  
  } // namespace ssdr
}// namespace emph

#endif // ConvertDigitToWCoordAlgo1
