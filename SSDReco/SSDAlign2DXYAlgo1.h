////////////////////////////////////////////////////////////////////////
/// \brief   2D aligner, X-Z view or Y-Z view, indepedently from each others. 
///          Algorithm one.  Could beong to SSDCalibration, but, this aligner 
///          requires some crude track reconstruction, as it is based on track residuals, i
///          of SSD strip that are on too often. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDALIGN2DXYALGO1_H
#define SSDALIGN2DXYALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"
#include "SSDReco/SSDAlignSimpleLinFit.h"

namespace emph { 
  namespace ssdr {
 
    class SSDAlign2DXYAlgo1 {
    
       public:
      
	SSDAlign2DXYAlgo1(); // No args .. for now.. 
	SSDAlign2DXYAlgo1(char aView); // X or Y
        ~SSDAlign2DXYAlgo1();
	
        private:
	  const size_t fNumStations = 6;
	  const size_t fNumStrips = 639; // Per wafer. 
	  const double fOneOverSqrt12; 
	  bool fAlign0to4;
	  size_t fNumStationsEff;
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
	  bool fMomentumIsSet; // a flag to make sure we don't set the momentum more than once in the same job. 
	  bool fFilesAreOpen;
	  char fView;      
	  double fPitch;
	  double fHalfWaferWidth;
	  int fNumIterMax; // Maximum number of iteration 
	  double fChiSqCut;
//	  double fRefPointPitchOrYawAngle;
	  std::string fTokenJob;
	  double fZCoordsMagnetCenter, fMagnetKick120GeV; 
	  std::vector<double> fZCoords;
	  std::vector<double> fNominalOffsets; // for station 4 and 5, Y View Sensor 3 
	  std::vector<double> fNominalOffsetsAlt45; // for station 4 and 5, Y View Sensor 2 
	  std::vector<double> fResiduals;   // the current one, for the a specific event. Actually, not used so far. 
	  std::vector<double> fMeanResiduals;// the meanvalue over a run..Or previously fitted..  
	  std::vector<double> fRMSResiduals;
// 
// Additional cuts.. and variables. 
//	  
	  std::vector<double> fMinStrips; // to use only to pick the center of the beam.. !! Might be run dependant! 
	  std::vector<double> fMaxStrips;
	  std::vector<double> fMultScatUncert;
	  std::vector<double> fOtherUncert;
	  std::vector<double> fZLocShifts;
	  std::vector<double> fPitchOrYawAngles;
	  
	  emph::ssdr::SSDAlignSimpleLinFit myLinFit; // no contructor argument. 
	  std::ofstream fFOutA1, fFOutA1Dbg;
	  
	   
	public:
	 inline void SetDoAling0to4( bool lastIs4) { 
	   fAlign0to4 = lastIs4;        
	   fNumStationsEff = fAlign0to4 ? fNumStations-1 : fNumStations;
	   std::cerr << " SSDAlign2DXYAlgo1::SetDoAling0to4, fNumStationsEff " << fNumStationsEff << std::endl;
         }
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetNumIterMax( int n) { fNumIterMax = n; }
	 inline void SetChiSqCut1 (double v) { fChiSqCut = v; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
	 inline void SetZLocShifts(const std::vector<double> v) { fZLocShifts = v; } 
	 inline void SetOtherUncert(const std::vector<double> v) { fOtherUncert = v; } 
	 inline void SetPitchAngles(const std::vector<double> v) { fPitchOrYawAngles = v; } 
//	 inline void SetRefPtForPitchOrYawAngle(double v) { fRefPointPitchOrYawAngle = v; }
	 inline void SetFittedResiduals(std::vector<double> v) { fMeanResiduals = v;} 
	 inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; }
	 void InitializeCoords(bool lastIs4, const std::vector<double> &zCoords);
	 inline void SetTheView(char aView) {
	   if ((aView != 'X') && (aView != 'Y')) {
	     std::cerr << " SSDAlign2DXYAlgo1, setting an unknown view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	   }
	   fView = aView;
	 }
	 void SetForMomentum(double p); // Rescale the Magnet kick, deviation on the X-Z plane 
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 
	 inline double GetTsUncertainty(size_t kSt, std::vector<rb::SSDCluster>::const_iterator itCl) const {
	  double aRMS = itCl->WgtRmsStrip();
	  double errMeasSq = (1.0/12.)*fPitch * fPitch * 1.0/(1.0 + aRMS*aRMS); // Very approximate, need a better model. 
	  return std::sqrt(errMeasSq + fOtherUncert[kSt]*fOtherUncert[kSt] + fMultScatUncert[kSt]*fMultScatUncert[kSt]);
	  
	 }
	 
	 void  alignIt(const art::Event &evt, const std::vector<rb::SSDCluster> &aSSDcls); 
	 void  alignItAlt45(const bool skipStation4, const art::Event &evt, 
	                                  const std::vector<rb::SSDCluster> &aSSDcls); // find the residuals for station 4 & 5, Sensor 2 (in Y). 
	 
	 private:
	 
	 inline double getTsFromCluster(size_t kStation, bool alternate45, double strip) {
	   switch (fView) { // see SSDCalibration/SSDCalibration_module 
	     case 'X' :
	     {
	       double aVal=0.;
	       if (kStation < 4) {
	         aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	       } else {
	         if (!alternate45) aVal =  strip*fPitch - fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation];
		 else aVal =  -1.0*strip*fPitch + fNominalOffsetsAlt45[kStation] + fResiduals[kStation] + fMeanResiduals[kStation];
		 // Momentum correction, for 120 GeV primary beam  
		 aVal += fMagnetKick120GeV * (fZCoords[kStation] - fZCoordsMagnetCenter);
		 // Yaw Correction 
	       }
	       return aVal;
	       break;
	     } 
	     case 'Y' : 
	      {
	       double aVal = 0.;
	       if (kStation < 4) {  
	         aVal =  (strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	       } else {
	         if (!alternate45) aVal =  ( -1.0*strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
		 else aVal =  ( strip*fPitch + fNominalOffsetsAlt45[kStation] + fResiduals[kStation] + fMeanResiduals[kStation]);
	       } 
	       const double aValC = this->correctTsForPitchOrYawAngle(kStation, aVal);
	       return aValC;
	       break; 
	      }
	      default :
	        std::cerr << " SSDAlign2DXYAlgo1::getTsFromCluster, unexpected view, " 
		<< fView << " kStation " << kStation << 
		 " internal error, fatal " << std::endl; exit(2);
	   }
	   return 0.; // should not happen  
	 }
	 inline double correctTsForPitchOrYawAngle(size_t kStation, double ts) {
	   return (ts + ts*fPitchOrYawAngles[kStation]);
	 }
	 void openOutputCsvFiles();
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDAlign2DXYAlgo1
