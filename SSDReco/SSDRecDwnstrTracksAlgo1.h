////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of a set of Downstream tracks Phase 1b, assume 4 station downstream of the 
///          the target, two of them downstream of the magnet. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRECDWNSTRTRACKSALGO1_H
#define SSDRECDWNSTRTRACKSALGO1_H
#include <vector>
#include <stdint.h>
#include <iostream>

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"
#include "SSDReco/VolatileAlignmentParams.h"
#include "RecoBase/SSDStationPtAlgo1.h" 
#include "RecoBase/BeamTrackAlgo1.h" 
#include "RecoBase/DwnstrTrackAlgo1.h" 
#include "SSDReco/SSD3DTrackFitFCNAlgo1.h"
#include "SSDReco/SSDDwnstrTrackFitFCNAlgo1.h"
#include "SSDReco/SSD3DTrackKlmFitFCNAlgo1.h"
#include "SSDReco/SSDRecStationDwnstrAlgo1.h"
#include "Geometry/service/GeometryService.h"


namespace emph { 
  namespace ssdr {
  
    typedef std::vector<rb::SSDStationPtAlgo1>::const_iterator myItStPt; 
 				
    class SSDRecDwnstrTracksAlgo1 {
    
       public:
      
	SSDRecDwnstrTracksAlgo1(); // No args .. for now.. 
        ~SSDRecDwnstrTracksAlgo1();
	
        private:
	  static const double fSqrt2, fOneOverSqrt2;
          art::ServiceHandle<emph::geo::GeometryService> fGeoService;
          emph::geo::Geometry *fEmgeo;
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	  ssdr::ConvertDigitToWCoordAlgo1 fCoordConvert;   
	    //  This is also invoked in SSDRecStationPoints.. Along the volatile Alignment Params, should be an art service.  
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
	  bool fDebugIsOn;
	  bool fIsMC; // The usual Ugly flag.. 
	  bool fIsGoodForAlignment, fIsPerfectForAlignment;  
	  bool fDoMigrad; // set to true, unless we are really begging for CPU cycles.. or Migrad fails too often. 
	  bool fNoMagnet; // set once we know the geometry.. 
	  double fChiSqCut, fChiSqCutKlm;
	  double fPrelimMomentum; // to compute multiple scattering uncertainty. 
	  double fChiSqCutPreArb;
	  bool fDoUseUpstreamTrack; 
	  bool fHasUniqueYWSt4; // Special case for station 4.. missing read out chip. 
	  std::string fTokenJob;
	  ssdr::SSDDwnstrTrackFitFCNAlgo1 *fFitterFCN;
	  ssdr::SSD3DTrackFitFCNAlgo1 *fUpDownFitterFCN;
	  ssdr::SSD3DTrackKlmFitFCNAlgo1 *fFitterKlmFCN;
	  //
          ssdr::SSDRecStationDwnstrAlgo1 fInputSt2Pts, fInputSt3Pts, fInputSt4Pts, fInputSt5Pts, fInputSt6Pts ;
	  // Station 7 appear to be empty on run 2098, 2113..  
	  
	  std::vector<myItCl> fDataItClForFits; // for UpDown 3D fits.. 
	   
	  std::vector<rb::DwnstrTrackAlgo1> fTrs; // the tracks, produced here..  
	  std::vector<rb::DwnstrTrackAlgo1> fTrsKlm; // the tracks, produced here, with the Kalman filter (with one param, the momentum).   
	   
	  mutable std::ofstream fFOutTrs;
	  mutable std::ofstream fFOutCompact; 
	  mutable std::ofstream fFOutCompactInfo; 
	  mutable std::ofstream fFOutCmpBeamTracks; 
	  mutable std::ofstream fFOutCmpKlmTracks; 
	  // Internal stuff.. ???
	  double fPrelimFitMom;
	  double fPrelimFitChiSq;
	  mutable std::vector<rb::BeamTrackAlgo1>::const_iterator fItUpstrTr; // a set of Dangling pointer.  
	    // Use with caution. access protected by above boolean.
	  rb::BeamTrackAlgo1 fUpStrDwnStrTrack; // global fit, to compare accuracy or biases. 
	  
	public:
	 inline void SetDebugOn(bool v = true) { 
	   fDebugIsOn = v; fInputSt2Pts.SetDebugOn(v); 
	   fInputSt3Pts.SetDebugOn(v); fInputSt4Pts.SetDebugOn(v); fInputSt5Pts.SetDebugOn(v); fInputSt6Pts.SetDebugOn(v);
	 }
         inline void SetRun(int aRunNum) { 
	   fRunNum = aRunNum; fInputSt2Pts.SetRun(aRunNum);
	   fInputSt3Pts.SetRun(aRunNum); fInputSt4Pts.SetRun(aRunNum);fInputSt5Pts.SetRun(aRunNum); fInputSt6Pts.SetRun(aRunNum);
	 } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetChiSqCut (double v) { fChiSqCut = v; }
	 inline void SetChiSqCutKlm (double v) { fChiSqCutKlm = v; }
	 inline void SetChiSqCutPreArb (double v) { fChiSqCutPreArb = v; }
	 inline void SetItUpstreamTrack(std::vector<rb::BeamTrackAlgo1>::const_iterator it) { fDoUseUpstreamTrack = true; fItUpstrTr = it; }
	 inline void VoidItUpstreamTrack() { fDoUseUpstreamTrack = false; }
	 inline void SetPreliminaryMomentum(double p) { // For multiple scattering uncertainties.. 
	   fPrelimMomentum = p; 
	   fCoordConvert.SetForMomentum(p);
	   fInputSt2Pts.SetPreliminaryMomentum(p); 
	   fInputSt3Pts.SetPreliminaryMomentum(p); 
	   fInputSt4Pts.SetPreliminaryMomentum(p); 
	   fInputSt5Pts.SetPreliminaryMomentum(p); 
	   fInputSt6Pts.SetPreliminaryMomentum(p); 
	 } 
	 inline void SetForMC(bool v=true) { // For multiple scattering uncertainties.. // Obsolete..
	   fIsMC = v;
	   fCoordConvert.SetForMC(v);
	   fInputSt2Pts.SetForMC(v); 
	   fInputSt3Pts.SetForMC(v); 
	   fInputSt4Pts.SetForMC(v); 
	   fInputSt5Pts.SetForMC(v); 
	   fInputSt6Pts.SetForMC(v); 
	 }
	 inline void SetCoeffsAlignUncert(double x, double y) {
//	   std::cerr << " ... SSDRecDwnstrTracksAlgo1::SetCoeffsAlignUncert, x " << x << " y " << y << std::endl;
	   fCoordConvert.SetCoeffsAlignUncert(x, y); // We clearly should make this a singleton class.. or move it to a service. 
	   fInputSt2Pts.SetCoeffsAlignUncert(x,  y); 
	   fInputSt3Pts.SetCoeffsAlignUncert(x,  y); 
	   fInputSt4Pts.SetCoeffsAlignUncert(x,  y); 
	   fInputSt5Pts.SetCoeffsAlignUncert(x,  y); 
	   fInputSt6Pts.SetCoeffsAlignUncert(x,  y); 
	 } 
	 inline void SetTokenJob(const std::string &aT) { 
	   fTokenJob = aT; fInputSt2Pts.SetTokenJob(aT); 
	   fInputSt3Pts.SetTokenJob(aT); fInputSt4Pts.SetTokenJob(aT); fInputSt5Pts.SetTokenJob(aT); fInputSt6Pts.SetTokenJob(aT);
	 }
	 inline void Clear() { fTrs.clear(); fTrsKlm.clear();}
	 inline void SetChiSqCutRecStation(size_t kSt, double c) { 
	   switch (kSt) {
	    case 2 : { fInputSt2Pts.SetChiSqCut(c); return; } 
	    case 3 : { fInputSt3Pts.SetChiSqCut(c); return; } 
	    case 4 : { fInputSt4Pts.SetChiSqCut(1.5*c); return; } // We increase the cut a bit, alignment uncertainties are larger.. 
	    case 5 : { fInputSt5Pts.SetChiSqCut(2.0*c); return; }
	    case 6 : { fInputSt6Pts.SetChiSqCut(2.0*c); return; }
	    default : { return; }  
	   }
	 }
	 //
	 // Getter, only one output, the reconstructed track.. 
	 //
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 inline int EvtNum() const { return fEvtNum; }
	 inline rb::DwnstrTrackAlgo1 GetTr(std::vector<rb::DwnstrTrackAlgo1>::const_iterator it) const { return *it; } // Deep copy, but small struct.. 
	 inline size_t Size() const {return fTrs.size(); }
	 inline std::vector<rb::DwnstrTrackAlgo1>::const_iterator CBegin() const { return fTrs.cbegin(); } 
	 inline std::vector<rb::DwnstrTrackAlgo1>::const_iterator CEnd() const { return fTrs.cend(); } 
	 inline bool IsGoodForAlignment() const { return fIsGoodForAlignment; } 
	 inline bool IsPerfectForAlignment() const { return fIsPerfectForAlignment; } 
	 
	 size_t RecStation(size_t kSt, const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 
	 size_t RecAndFitIt(const art::Event &evt); 
	 void dumpInfoForR() const;
	 inline void dumpStInfoForR() const {
	   if (fInputSt2Pts.Size() > 0) fInputSt2Pts.dumpInfoForR(); 
	   if (fInputSt3Pts.Size() > 0) fInputSt3Pts.dumpInfoForR(); 
	   if (fInputSt4Pts.Size() > 0) fInputSt4Pts.dumpInfoForR(); 
	   if (fInputSt5Pts.Size() > 0) fInputSt5Pts.dumpInfoForR();
	   if (fInputSt6Pts.Size() > 0) fInputSt6Pts.dumpInfoForR();
	 }
	 inline int NumTripletsSt2and3() const {
	   return  (fInputSt2Pts.NumTriplets() + fInputSt3Pts.NumTriplets());
	 }
	 
	 void dumpCompactEvt(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr ); 
	 void dumpBeamTracksCmp(bool useKlm = false) const; // Compare the beam track parameters estimates.. 	 
	 void dumpBeamTracksCmpKlm() const; // Compare the beam track parameters estimates.. 	 
	 bool doUpDwn3DClFitAndStore( double pStart = 50.);
	 bool doDwn3DKlmFitAndStore(const std::vector<myItStPt> &dataIn,  double pStart = 50.);
	 void transferKlmToClFit(); 
	 
       private:
	 
	 size_t RecAndFitAll4Stations();
	 
	 size_t RecAndFitStation234();
	 size_t RecAndFitStation235();
	 bool doFitAndStore(rb::DwnstrTrType aType, double xStart, double yStart, double xSlopeStart, double ySlopeStart, double pStart = 50.);
	 bool doPrelimFit(rb::DwnstrTrType aType, double xStart, double yStart, double xSlopeStart, double ySlopeStart);
	 bool IsAlreadyFound(const rb::DwnstrTrackAlgo1 &aTr) const;	 
	 void openOutputCsvFiles() const;
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRecDwnstrTracksAlgo1
