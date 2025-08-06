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
#include "SSDReco/ConvertDigitToWCoordAutre.h"
#include "SSDReco/VolatileAlignmentParams.h" 

namespace emph { 
  namespace ssdr {
 
    class SSDAlign2DXYAutre {
    
       public:
      
	SSDAlign2DXYAutre(); // No args .. for now.. 
	SSDAlign2DXYAutre(char aView); // X or Y
        ~SSDAlign2DXYAutre();
	
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
	  //
	  // We keep those for now, but all transfered to the Convertion digit to global coord system class. 
	  //
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
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;	  
	  emph::ssdr::SSDAlignSimpleLinFit myLinFit; // no contructor argument. 
	  emph::ssdr::ConvertDigitToWCoordAutre myConvert; // View is the argument.. 
	  
	  std::ofstream fFOutA1, fFOutA1Dbg;
	  
	   
	public:
	 // 
	 // Some getter, for checking .. 
	 //
	 inline double AssumedResidual(size_t kSt) const { return fMeanResiduals[kSt]; } 
	 inline void SetDoAling0to4( bool lastIs4) { 
	   fAlign0to4 = lastIs4;        
	   fNumStationsEff = fAlign0to4 ? fNumStations-1 : fNumStations;
	   std::cerr << " SSDAlign2DXYAutre::SetDoAling0to4, fNumStationsEff " << fNumStationsEff << std::endl;
         }
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetNumIterMax( int n) { fNumIterMax = n; }
	 inline void SetChiSqCut1 (double v) { fChiSqCut = v; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
	 inline void SetZLocShifts(const std::vector<double> v) { fZLocShifts = v; myConvert.SetZLocShifts(v); } 
	 inline void SetOtherUncert(const std::vector<double> v) { fOtherUncert = v; myConvert.SetOtherUncert(v); } 
	 inline void SetPitchAngles(const std::vector<double> v) { fPitchOrYawAngles = v; myConvert.SetPitchAngles(v); } 
	 inline void SetFittedResiduals(std::vector<double> v) { fMeanResiduals = v; myConvert.SetFittedResiduals(v); } 
	 inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; myConvert.SetMagnetKick120GeV(v);}
	 void InitializeCoords(bool lastIs4, const std::vector<double> &zCoords);
	 inline void SetTheView(char aView) {
	   if ((aView != 'X') && (aView != 'Y')) {
	     std::cerr << " SSDAlign2DXYAutre, setting an unknown view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	   }
	   fView = aView;
	   myConvert.SetTheView(aView);
	 }
	 void SetForMomentum(double p); // Rescale the Magnet kick, deviation on the X-Z plane 
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 	 
	 void  alignIt(const art::Event &evt, const std::vector<rb::SSDCluster> &aSSDcls); 
	 void  alignItAlt45(const bool skipStation4, const art::Event &evt, 
	                                  const std::vector<rb::SSDCluster> &aSSDcls); // find the residuals for station 4 & 5, Sensor 2 (in Y). 
	 
	 private:
	 
	 void openOutputCsvFiles();
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDAlign2DXYAutre
