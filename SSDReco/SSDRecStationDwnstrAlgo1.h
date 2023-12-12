////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of 3D Station points, downstream of the magnet. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRECSTATIONDWNSTRALGO1_H
#define SSDRECSTATIONDWNSTRALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <fstream>

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Geometry/service/GeometryService.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/LineSegment.h"
#include "SSDReco/ConvertDigitToWCoordAlgo1.h"
#include "SSDReco/VolatileAlignmentParams.h"
#include "RecoBase/SSDStationPtAlgo1.h" 

namespace emph { 
  namespace ssdr {
 				
    class SSDRecStationDwnstrAlgo1 {
    
       typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    
       
       public:
	 explicit SSDRecStationDwnstrAlgo1(size_t aStationnum); // No args .. for now.. 
        ~SSDRecStationDwnstrAlgo1();
	
        private:
	  static const double fSqrt2, fOneOverSqrt2;
          art::ServiceHandle<emph::geo::GeometryService> fGeoService;
          emph::geo::Geometry *fEmgeo;
          art::ServiceHandle<emph::dgmap::DetGeoMapService> fDetGeoMapService;
	  emph::dgmap::DetGeoMap *fDetGeoMap; 
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	  ssdr::ConvertDigitToWCoordAlgo1 fCoordConvert;
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fStationNum;
	  int fNEvents; // Incremental events count for a given job. 
	  bool fDebugIsOn; 
          bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. 
	  bool fDoFirstAndLastStrips;
	  bool fInterchangeStXandWStation6; 
	  bool fInterchangeStYandWStation6; 
	  double fChiSqCut, fChiSqCutPreArb;
	  mutable double fXWindowWidth, fYWindowWidth, fXWindowCenter, fYWindowCenter;
	  double fPrelimMomentum; // to compute multiple scattering uncertainty. 
	  std::string fTokenJob;
	  //
          std::vector<rb::SSDStationPtAlgo1> fStPoints; // the stuff this routine produces. 
	  mutable std::ofstream *fFOutSt; // A pointer, such that we can optionally instantiate it, needed if this class is within an stl vector. 
	  mutable std::ofstream *fFOutStYFirst; // A list of X coordinate for which the Y measuring strip number is 0 
	  mutable std::ofstream *fFOutStYLast; // A list of X coordinate for which the Y measuring strip number is 639 
	  mutable std::ofstream *fFOutStXFirst; // A list of Y coordinate for which the X measuring strip number is 0 
	  mutable std::ofstream *fFOutStXLast; // A list of Y coordinate for which the X measuring strip number is 639 
	  // Internal stuff.. 
	  int fIdStPtNow; // Identifying the SSDStationPtAlgo1 objects.
	  std::vector<int> fClUsages;
	  int fNxCls, fNyCls, fNuCls; // the last stands for U or W (we have a maximum of three views 	
	  double fInViewOverlapCut; // in strips units.  
	  
	public:
	 inline void SetDebugOn(bool v = true) { fDebugIsOn = v; }
         inline void SetStationNum(int aStNum) { fStationNum = aStNum; } 
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetChiSqCut (double v) { fChiSqCut = v; }
	 inline void SetChiSqCutPreArb (double v) { fChiSqCutPreArb = v; }
	 inline void SetXYWindowWidth( double vx, double vy ) { fXWindowWidth = vx; fYWindowWidth = vy;}
	 inline void SetXYWindowCenter( double vx, double vy ) { fXWindowCenter = vx; fYWindowCenter = vy;}
	 inline void SetForMC(bool v=true) { fIsMC=v; fCoordConvert.SetForMC();  }
	 inline void SetDoFirstAndLastStrips(bool v=true) { fDoFirstAndLastStrips = v; } 
	 inline void SetPreliminaryMomentum(double p) { fPrelimMomentum = p; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
	 inline void Clear() { fStPoints.clear(); }
	 bool HasUniqueYW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const; 
	    // Only relevant for Phase1b, station 4, where we had 3 dead readout chip (3*128 missing channel in the X view.. 
	 //
	 // Getter, only one output, the reconstructed track.. 
	 //
	 inline int Station() const { return fStationNum; }
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 inline bool DoFirstAndLastStrips() const { return fDoFirstAndLastStrips; }
	 inline size_t Size() const {return fStPoints.size(); }
	 inline std::vector<rb::SSDStationPtAlgo1>::const_iterator CBegin() const { return fStPoints.cbegin(); } 
	 inline std::vector<rb::SSDStationPtAlgo1>::const_iterator CEnd() const { return fStPoints.cend(); } 
	 inline rb::SSDStationPtAlgo1 GetStPoint(std::vector<rb::SSDStationPtAlgo1>::const_iterator it) const { return *it; } 
	 inline rb::SSDStationPtAlgo1 GetStPoint(size_t i) const { return fStPoints[i]; } 
	 // Deep copy, but small struct..
	 inline std::vector<rb::SSDStationPtAlgo1>::const_iterator GetStationPointPtr(int id) const { 
	   for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt = fStPoints.cbegin(); itSt != fStPoints.cend(); itSt++) { 
	     if (itSt->ID() == id) return itSt;
	   }
	   return fStPoints.cend(); 
	 } 
	 // 
	 // Reset Usage flags (to be called post reconstruction. 
	 //
	 inline void ResetUsage() const { // pseudo const, not part of this reconstruction. For tracking usage.. 
	   for (std::vector<rb::SSDStationPtAlgo1>::const_iterator it = fStPoints.cbegin(); it != fStPoints.cend(); it++) it->SetUserFlag(0);
	 }
	 	 
	 size_t RecIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 
	 void dumpInfoForR() const;
	 void dumpInfoForRXViewEdges(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const;
	 void dumpInfoForRYViewEdges(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const;
	 int NumTriplets() const  {
	   int n = 0;
	   for (std::vector<rb::SSDStationPtAlgo1>::const_iterator it = fStPoints.cbegin(); it != fStPoints.cend(); it++) {
	     if ((it->Type() ==  rb::STXYW) || (it->Type() ==  rb::STXYU)) n++;
	   }
	   return n;
	 }
	 
       private:
	 
	 size_t recoXYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); // 
	 
	 size_t  recoXY(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 size_t  recoXUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 size_t  recoYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 
	 bool flagDuplicateOverlapping (emph::geo::sensorView view,
	                                std::vector<rb::SSDStationPtAlgo1>::iterator itPt1, 
	                                std::vector<rb::SSDStationPtAlgo1>::iterator itPt2,
	                                  double inViewCut); // argument is the difference in average strip numbers. 
	 bool hasOverlappingClusters( std::vector<rb::SSDCluster>::const_iterator itPt1, 
	                              std::vector<rb::SSDCluster>::const_iterator itPt2,
	                                  double inViewCut); // assume the callers make sure they are on the same view
	 bool arbitrateOverlapping (double inViewCut); // loop over all non-ordered pair of point, and flag  
	 
	 void openOutputCsvFiles() const ;	// we only modify the output streamer  
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRecStationDwnstrAlgo1
