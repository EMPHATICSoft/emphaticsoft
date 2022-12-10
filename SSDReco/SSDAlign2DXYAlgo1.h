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

namespace emph { 
  namespace ssdr {
 
    class SSDAlign2DXYAlgo1 {
    
      public:
      
      struct myLinFitResult { 
        public:
	  int ndgf; // number of degrees of freedom Other data member are self explicit.
	  double offset;
	  double slope;
	  double sigmaOffset;
	  double sigmaSlope;
	  double covOffsetSlope; 
	  double chiSq;
	  std::vector<double> resids; // The residuals
      };
           
      
	SSDAlign2DXYAlgo1(); // No args .. for now.. 
	SSDAlign2DXYAlgo1(char aView); // No args .. for now.. 
        ~SSDAlign2DXYAlgo1();
	
        private:
	  const size_t fNumStations = 6;
	  const size_t fNumStrips = 639; // Per wafer. 
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
	  bool fFilesAreOpen;
	  char fView;      
	  double fPitch;
	  double fHalfWaferWidth;
	  int fNumIterMax; // Maximum number of iteration 
	  double fChiSqCut;
	  std::string fTokenJob;
	  std::vector<double> fZCoords;
	  std::vector<double> fNominalOffsets;
	  std::vector<double> fResiduals;   // the current one, for the a specific event. 
	  std::vector<double> fMeanResiduals;// the meanvalue over a run.. 
	  std::vector<double> fRMSResiduals;
// 
// Additional cuts.. and variables. 
//	  
	  std::vector<int> fMinStrips; // to use only to pick the center of the beam.. !! Might be run dependant! 
	  std::vector<int> fMaxStrips;
	  std::ofstream fFOutA1;
	  
	   
	public:
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetNumIterMax( int n) { fNumIterMax = n; }
	 inline void SetChiSqCut1 (double v) { fChiSqCut = v; } 
	 void InitializeCoords(const std::vector<double> &zCoords);
	 inline void SetTheView(char aView) {
	   if ((aView != 'X') && (aView != 'Y')) {
	     std::cerr << " SSDAlign2DXYAlgo1, setting an uknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	   }
	   fView = aView;
	 }
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 
	 void  alignIt(const art::Event &evt, const std::vector<rb::SSDCluster> &aSSDcls); 
	 
	 private:
	 
	 inline double getTsFromCluster(size_t kStation, double strip) {
	   double val = strip*fPitch + fNominalOffsets[kStation] + fResiduals[kStation];
	   // depending of the view and sensor choice, flip the sign here..
	   return val; 
	 }
	 void openOutputCsvFiles();
         void  fitLin(const std::vector<double> &t, const std::vector<double> &sigT, myLinFitResult &fitRes ) const ; 
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDAlign2DXYAlgo1
