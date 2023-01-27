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
#include "RecoBase/BeamTrackAlgo1.h" 

namespace emph { 
  namespace ssdr {
 				
    class SSDAlign3DUVAlgo1 {
    
       public:
      
	SSDAlign3DUVAlgo1(); // No args .. for now.. 
	SSDAlign3DUVAlgo1(char aView, int aStation, bool atl45); // U or V 
        ~SSDAlign3DUVAlgo1();
	
        private:
	  const size_t fNumStations = 6;
	  const size_t fNumStrips = 639; // Per wafer. 
	  const double fSqrt2, fOneOverSqrt2;
	  const double fOneOverSqrt12; 
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
	  int fNEvtsCompact;
	  bool fMomentumIsSet; // a flag to make sure we don't set the momentum more than once in the same job. 
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
	  std::vector<double> fNominalOffsetsU, fNominalOffsetsV; 
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
          rb::BeamTrackAlgo1 fTrXY;
	  std::vector<int> fNHitsXView, fNHitsYView; 
	  
	  
	  emph::ssdr::SSDAlignSimpleLinFit myLinFitX, myLinFitY; 

	  std::ofstream fFOutXY, fFOutXYU, fFOutXYV, fFOutCompact;
	  
	   
	public:
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetNumIterMax( int n) { fNumIterMax = n; }
	 inline void SetChiSqCut (double v) { fChiSqCut = v; } 
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
	 void SetForMomentum(double p); // Rescale the Magnet kick, deviation on the X-Z plane 
	 //
	 // Getter, only one output, the reconstructed track.. 
	 // 
	 inline rb::BeamTrackAlgo1 GetBeamTrack() const { return fTrXY; } // Deep copy, but small struct.. 
	 inline std::vector<int> GetNHitsXView () const { return fNHitsXView; }
	 inline std::vector<int> GetNHitsYView () const { return fNHitsYView; }
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 
	 void  alignIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 void dumpCompactEvt(int spill, int evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 
	 private:
	 
	 bool recoXY(rb::planeView view,  const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
	 bool checkUV(rb::planeView view, size_t kStation, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
//	 double GetTsFromCluster(char aView, size_t kStation,  double strip, bool getX=true) const;
	 double GetTsFromCluster(char aView, size_t kStation,  size_t sensor, double strip) const;
	 
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
	 void openOutputCsvFiles();
	 void dumpXYInfo(int nHitsT);
	 // 
	 // For the 2nd order fitter, write compact events.. and the nominal coordinates. 
	 //
	 void writeNominalCoords();
	 
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDAlign3DUVAlgo1
