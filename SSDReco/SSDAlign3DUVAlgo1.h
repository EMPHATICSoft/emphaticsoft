////////////////////////////////////////////////////////////////////////
/// \brief   3D aligner, U and V sensor offset fitting. 
///          And a 3D fit using the Manetic field integrator.  See SSD3DTrackFitAlgo1
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
#include "SSDReco/ConvertDigitToWCoordAlgo1.h"
#include "SSDReco/SSD3DTrackFitFCNAlgo1.h"
#include "SSDReco/VolatileAlignmentParams.h"

namespace emph { 
  namespace ssdr {
 				
    class SSDAlign3DUVAlgo1 {
    
       typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    
       
       public:
      
	SSDAlign3DUVAlgo1(); // No args .. for now.. 
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
	  bool fDo3DFit;
          bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. 
	  bool fMomentumIsSet; // a flag to make sure we don't set the momentum more than once in the same job. 
	  bool fFilesAreOpen;
	  char fView; 
	  int fStation; // the station & sensor to align 
	  int fSensor;      
	  double fPitch;
	  double fHalfWaferWidth;
	  int fNumIterMax; // Maximum number of iteration 
	  double fChiSqCut;
	  double fChiSqCutX, fChiSqCutY;
	  double fMomentumInit3DFit;
	  bool fNoMagnet;
	  std::string fTokenJob;
	  double fZCoordsMagnetCenter, fMagnetKick120GeV; 
	  std::vector<double> fMagShift;
	  std::vector<double> fZCoordXs, fZCoordYs,  fZCoordUs, fZCoordVs;
	  std::vector<double> fFittedResidualsX;// the meanvalue over a run..Or previously fitted.. // No longer used, (May 2023)  
	  std::vector<double> fFittedResidualsY;// the meanvalue over a run..Or previously fitted..  
          // For future use.. Or not.. I defined a new VolatileAligmmentParams class. 
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
          std::vector<myItCl> fDataFor3DFit;
          rb::BeamTrackAlgo1 fTrXY;
	  std::vector<int> fNHitsXView, fNHitsYView; 
	  
	  
	  emph::ssdr::SSDAlignSimpleLinFit myLinFitX, myLinFitY; 
	  emph::ssdr::SSD3DTrackFitFCNAlgo1 *myNonLin3DFitPtr;
	  // Obsolete... I think..  
	  emph::ssdr::ConvertDigitToWCoordAlgo1 myConvertX; // View is the argument.. 
	  emph::ssdr::ConvertDigitToWCoordAlgo1 myConvertY; // One instance for each view.. Not a waste of memory, maximum dims are 8, not 22 
	  emph::ssdr::ConvertDigitToWCoordAlgo1 myConvertU; // For sake of uniformity. 
	  emph::ssdr::ConvertDigitToWCoordAlgo1 myConvertV; //
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	  
	   

	  std::ofstream fFOutXY, fFOutXYU, fFOutXYV, fFOut3DFit, fFOutCompact;
	  
	   
	public:
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetNumIterMax( int n) { fNumIterMax = n; }
	 inline void SetChiSqCut (double v) { fChiSqCut = v; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
	 inline void SetDo3DFit(bool b=true) { fDo3DFit = b; }
	 inline void SetMagnetShift(std::vector<double> v) { fMagShift = v; }
//	 inline void SetZLocShifts(const std::vector<double> v) { fZLocShifts = v; } 
	 inline void SetOtherUncert(const std::vector<double> v) { 
	    fOtherUncert = v; myConvertX.SetOtherUncert(v); myConvertY.SetOtherUncert(v); 
	    myConvertU.SetOtherUncert(v); myConvertV.SetOtherUncert(v);} 
//	 inline void SetPitchAngles(const std::vector<double> v) { fPitchOrYawAngles = v; } 
//	 inline void SetRefPtForPitchOrYawAngle(double v) { fRefPointPitchOrYawAngle = v; }
	 inline void SetFittedResidualsForX(std::vector<double> v) { fFittedResidualsX = v; myConvertX.SetFittedResiduals(v); } 
	 inline void SetFittedResidualsForY(std::vector<double> v) { fFittedResidualsY = v; myConvertY.SetFittedResiduals(v); } 
	 inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; myConvertX.SetMagnetKick120GeV(v);}
	 inline void SetChiSqCutX(double v) { fChiSqCutX = v; }
	 inline void SetChiSqCutY(double v) { fChiSqCutY = v; }
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
	 inline int GetNEvtsCompact() const { return fNEvtsCompact; } 
	 inline void Reset3DFitInputData() { fDataFor3DFit.clear(); } 
	 
	 void  alignIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 void dumpCompactEvt(int spill, int evt, bool strictY6, bool strictX6, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 // strictY6:  Requires 6 hits, and no hits from the little (or no ) exposure sensor in station 4 and 5, for the Y view.. 
	 
	 private:
	 
	 bool recoXY(emph::geo::sensorView view,  const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
	 int recoUV(emph::geo::sensorView view, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 
	 bool checkUV(emph::geo::sensorView view, size_t kStation, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 	 	 
	 bool fit3D(size_t minNumHits); 
	 
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
