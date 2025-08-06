////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of a set of Tracks produce by the brick located in front of the SSD Station 0 
///          Valid for only  run 1366, I am afraid..  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRecBrickTracksAutre_H
#define SSDRecBrickTracksAutre_H
#include <vector>
#include <stdint.h>
#include <iostream>

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Geometry/service/GeometryService.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"
#include "SSDReco/VolatileAlignmentParams.h"
#include "RecoBase/SSDStationPtAutre.h" 
#include "RecoBase/DwnstrTrackAutre.h" 
#include "SSDReco/SSDDwnstrTrackFitFCNAutre.h"
#include "SSDReco/SSDRecStationDwnstrAutre.h"
#include "SSDReco/ConvertDigitToWCoordAutre.h"

namespace emph { 
  namespace ssdr {
  
    class SSDTrPreArbitrationAutre { // Dubious usage here... We (possibly, but almost certainly) need to do 
      // artbitration. So we need to stash candidate tracks, in the 
      public: 
        explicit SSDTrPreArbitrationAutre(int itId, int ix0, int iy0, int ix1, int iy1, int i2, int i3, int i4, int i5); 
        int fTrId; // The unique track Di (as contructed in the class below.) 
        int fIdClX0, fIdClY0, fIdClX1, fIdClY1; // Clusters Unique ID within Station 0 and 1 (they have unique sensor in Phase1b )
        int fIdSt2, fIdSt3, fIdSt4, fIdSt5; // Station point Unique Ids 
    };
 				
    class SSDRecBrickTracksAutre {
    
       public:
      
	SSDRecBrickTracksAutre(); // No args .. for now.. 
        ~SSDRecBrickTracksAutre();
	
       private:
	  static const double fSqrt2, fOneOverSqrt2;
          art::ServiceHandle<emph::geo::GeometryService> fGeoService;
          emph::geo::Geometry *fEmgeo;
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	  ssdr::ConvertDigitToWCoordAutre fCoordConvert;
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNumCompactSaved;
	  bool fDebugIsOn; 
	  bool fNoMagnet; // set once we know the geometry.. 
	  double fDistFromBeamCenterCut, fBeamCenterX, fBeamCenterY;
	  double fTrackSlopeCut;
	  double fChiSqCut;
	  double fAssumedMomentum; // to compute multiple scattering uncertainty. 
	  //
	  // internal variables..
	  //
	  size_t fMaxNumTrComb;
	  size_t fMaxNumSpacePts;
	  std::vector<double>  fDeltaZX, fDeltaZY, fDeltaZSqX, fDeltaZSqY; 
	  std::string fTokenJob;
	  ssdr::SSDDwnstrTrackFitFCNAutre *fFitterFCN;
          std::vector<ssdr::SSDRecStationDwnstrAutre> fInputStPts; 
	  
	  //
          // Output data.. 
	  //
	  std::vector<rb::DwnstrTrackAutre> fTrs; // the tracks, produced here..Downstream is a misnomer.. 
	  std::vector<SSDTrPreArbitrationAutre> fTrsForArb; // for arbitration, and saving rb::Clusters.   
	   
	  mutable std::ofstream fFOutTrs; // For Analysis with R
	  mutable std::ofstream fFOutCompact; // For subsequent re-alignment, to be used in SSDAlign
	  
       public:
	 inline void SetDebugOn(bool v = true) { 
	   fDebugIsOn = v; 
	   for (size_t kSt=0; kSt != fInputStPts.size(); kSt++) fInputStPts[kSt].SetDebugOn(v); 
	 }
         inline void SetRun(int aRunNum) { 
	   fRunNum = aRunNum; 
	   for (size_t kSt=0; kSt != fInputStPts.size(); kSt++) fInputStPts[kSt].SetRun(aRunNum);
	 } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetDistFromBeamCenterCut( double v) { fDistFromBeamCenterCut = v; }
	 inline void SetMaxNumTrComb(size_t n) { fMaxNumTrComb = n; }
	 inline void SetMaxNumSpacePts(size_t n) { fMaxNumSpacePts = n; }
	 inline void SetBeamCenterX( double v) { fBeamCenterX = v; }
	 inline void SetBeamCenterY( double v) { fBeamCenterY = v; }
	 inline void SetTrackSlopeCut(double v) { fTrackSlopeCut = v; }  
	 inline void SetChiSqCut (double v) { fChiSqCut = v; }
	 inline void SetDoFirstAndLastStrips(bool v = true) {
	   for (size_t kSt=0; kSt != fInputStPts.size(); kSt++) fInputStPts[kSt].SetDoFirstAndLastStrips(v);

	 }
	 inline void SetAssumedMomentum(double p) { // For multiple scattering uncertainties.. 
	   fAssumedMomentum = p; // some kind of mean value. 
	   for (size_t kSt=0; kSt != fInputStPts.size(); kSt++) fInputStPts[kSt].SetPreliminaryMomentum(p);
	 } 
	 inline void SetTokenJob(const std::string &aT) { 
	   fTokenJob = aT; for (size_t kSt=0; kSt != fInputStPts.size(); kSt++) fInputStPts[kSt].SetTokenJob(aT); 
	 }
	 inline void Clear() { fTrs.clear(); }
	 inline void SetChiSqCutRecStation(size_t kSt, double c) {  
	   fInputStPts[kSt-2].SetChiSqCut(c); // assumed here that the caller uses the usual station numering. 
	 }
	 //
	 // Getter, only one output, the reconstructed track.. 
	 //
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 inline int EvtNum() const { return fEvtNum; }
	 inline rb::DwnstrTrackAutre GetTr(std::vector<rb::DwnstrTrackAutre>::const_iterator it) const { return *it; } // Deep copy, but small struct.. 
	 inline size_t Size() const {return fTrs.size(); }
	 inline std::vector<rb::DwnstrTrackAutre>::const_iterator CBegin() const { return fTrs.cbegin(); } 
	 inline std::vector<rb::DwnstrTrackAutre>::const_iterator CEnd() const { return fTrs.cend(); } 
	 inline std::vector<rb::DwnstrTrackAutre>::const_iterator GetTrackPtrConst(int id) const { 
	   for (std::vector<rb::DwnstrTrackAutre>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it++) 
	     if (it->ID() == id) return it;
	   return fTrs.cend();
	 }
	 inline std::vector<rb::DwnstrTrackAutre>::iterator GetTrackPtr(int id) { 
	   for (std::vector<rb::DwnstrTrackAutre>::iterator it = fTrs.begin(); it != fTrs.end(); it++) 
	     if (it->ID() == id) return it;
	   return fTrs.end();
	 }
	 size_t RecStation(size_t kSt, const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 
	 size_t RecAndFitIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 void dumpInfoForR() const;
	 inline void dumpStInfoForR(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const {
	   for (size_t kSt=0; kSt != fInputStPts.size(); kSt++) {
	      fInputStPts[kSt].dumpInfoForR();
	      if (fInputStPts[kSt].DoFirstAndLastStrips()) {
	        fInputStPts[kSt].dumpInfoForRXViewEdges(aSSDClsPtr);
	        fInputStPts[kSt].dumpInfoForRYViewEdges(aSSDClsPtr);
	      }
	   }
	 }
	 
       private:
	 
	 size_t RecAndFitAll6Stations(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 bool doFitAndStore(rb::DwnstrTrType aType, double xStart, double yStart, double xSlopeStart, double ySlopeStart);
	 
	 bool IsAlreadyFound(const rb::DwnstrTrackAutre &aTr) const;
	 bool isClusterIsolated(std::vector<rb::SSDCluster>::const_iterator itClSel, 
	                        const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr, double nStripCut);
	 size_t Arbitrate(); // could be used with an option flag. 	 
	 void openOutputCsvFiles() const;	 
         void dumpCompactEvt(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr ); 
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRecBrickTracksAutre
