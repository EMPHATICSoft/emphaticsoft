////////////////////////////////////////////////////////////////////////
/// \brief   3D aligner, U and V sensor offset fitting. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDALIGN3DUVYALGO1_H
#define SSDALIGN3DUVALGO1_H

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
 
    typedef enum tTrAlignType { NONE = 0, // unspecified. 
                                XYONLY = 10,
                                XONLY = 2,
				YONLY = 3,
				XYUCONF1 = 11,
				XYUCONF2 = 12,
				XYUCONF3 = 13,
				XYVCONF1 = 21,
				XYVCONF2 = 22,
				XYVCONF3 = 23,
				XYUVCONF1 = 121, // prelim... incomplete, too confusing.
				XYUVCONF2 = 122,
				XYUVCONF3 = 123,
			} TrAlignType;
				
    class SSDAlign3DUVAlgo1 {
    
       public:
      
	SSDAlign3DUVAlgo1(); // No args .. for now.. 
	SSDAlign3DUVAlgo1(char aView, int aStation, bool atl45); // U or V 
        ~SSDAlign3DUVAlgo1();
	
        private:
	  const size_t fNumStations = 6;
	  const size_t fNumStrips = 639; // Per wafer. 
	  const double fOneOverSqrt12; 
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
	  bool fFilesAreOpen;
	  char fView; 
	  int fStation; // the station & sensor to align 
	  bool fAlt45; // will dictated which sensor will be used. 
	  int fSensor;      
	  double fPitch;
	  double fHalfWaferWidth;
	  int fNumIterMax; // Maximum number of iteration 
	  double fChiSqCut;
	  double fChiSqCutXY;
	  std::string fTokenJob;
	  double fZCoordsMagnetCenter, fMagnetKick120GeV; 
	  std::vector<double> fZCoordXs, fZCoordYs,  fZCoordUs, fZCoordVs;
	  std::vector<double> fNominalOffsetsX, fNominalOffsetsY, fNominalOffsetsXAlt45, fNominalOffsetsYAlt45; // Nominal offsets not yet used.. 
	  std::vector<double> fNominalOffsetsU, fNominalOffsetsUAlt45; 
	  double fNominalOffsetsV, fNominalOffsetsVAlt45; // Only for station 5  
	  std::vector<double> fFittedResidualsX;// the meanvalue over a run..Or previously fitted..  
	  std::vector<double> fFittedResidualsY;// the meanvalue over a run..Or previously fitted..  
          // For future use..
	  std::vector<double> fPitchAngles;
	  std::vector<double> fPitchAnglesAlt; // for the sensor with fewer ptoton beam statistics. 
	  std::vector<double> fYawAngles;
	  std::vector<double> fYawAnglesAlt; // for the sensor with fewer ptoton beam statistics. 
	  std::vector<double> fRollAngles;
	  std::vector<double> fRollAnglesAlt; // for the sensor with fewer ptoton beam statistics. 
	  
// Additional cuts.. and variables. 
//	  
	  std::vector<double> fMinStrips; // to use only to pick the center of the beam.. !! Might be run dependant! 
	  std::vector<double> fMaxStrips;
	  std::vector<double> fMultScatUncert;
	  std::vector<double> fOtherUncert;
//
          TrAlignType fTrType;
          double fTrXOffset, fTrYOffset, fTrXSlope, fTrYSlope; // Assume straight track, neglect the magent kick, too small, given the large 
	                                                   // uncertainties .  Also, these are the slopes upstream of the magnet.   
          double fTrXOffsetErr, fTrYOffsetErr, fTrXSlopeErr, fTrYSlopeErr; 
//
          double fChiSqX, fChiSqY; 
	  std::vector<int> fNHitsXView, fNHitsYView; 
	  
	  
	  emph::ssdr::SSDAlignSimpleLinFit myLinFitX, myLinFitY; 

	  std::ofstream fFOutXY, fFOutXYU, fFOutXYV;
	  
	   
	public:
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetNumIterMax( int n) { fNumIterMax = n; }
	 inline void SetChiSqCut1 (double v) { fChiSqCut = v; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
//	 inline void SetZLocShifts(const std::vector<double> v) { fZLocShifts = v; } 
	 inline void SetOtherUncert(const std::vector<double> v) { fOtherUncert = v; } 
//	 inline void SetPitchAngles(const std::vector<double> v) { fPitchOrYawAngles = v; } 
//	 inline void SetRefPtForPitchOrYawAngle(double v) { fRefPointPitchOrYawAngle = v; }
	 inline void SetFittedResidualsForX(std::vector<double> v) { fFittedResidualsX = v;} 
	 inline void SetFittedResidualsForY(std::vector<double> v) { fFittedResidualsY = v;} 
	 inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; }
	 inline void SetChiSqCutXY(double v) { fChiSqCutXY = v; }
	 void InitializeCoords(bool lastIs4, const std::vector<double> &zCoordXs, const std::vector<double> &zCoordYs,
	                                     const std::vector<double> &zCoordUs, const std::vector<double> &zCoordVs);
	 inline void SetTheView(char aView) {
	   if ((aView != 'U') && (aView != 'V')) {
	     std::cerr << " SSDAlign3DUVAlgo1, setting an unknown view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	   }
	   fView = aView;
	 }
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 
	 void  alignIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
	 private:
	 
	 bool recoXY(rb::planeView view,  const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
	 bool checkUV(rb::planeView view, size_t kStation, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
	 double GetTsFromCluster(char aView, size_t kStation,  double strip) const;
	 
	 inline double GetTsUncertainty(size_t kSt, std::vector<rb::SSDCluster>::const_iterator itCl) const {
	  double aRMS = itCl->WgtRmsStrip();
	  double errMeasSq = (1.0/12.)*fPitch * fPitch * 1.0/(1.0 + aRMS*aRMS); // Very approximate, need a better model. 
	  return std::sqrt(errMeasSq + fOtherUncert[kSt]*fOtherUncert[kSt] + fMultScatUncert[kSt]*fMultScatUncert[kSt]);
	  
	 }
	 
//	 inline double correctTsForPitchOrYawAngle(size_t kStation, int sensor, double ts) {
	 inline double correctTsForPitchOrYawAngle(size_t , int, double ts) {
//	   return (ts + ts*fPitchOrYawAngles[kStation]);
	   return ts;
	 }
	 void initTrackParams();
	 void openOutputCsvFiles();
	 void dumpXYInfo(int nHitsT);
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDAlign3DUVAlgo1