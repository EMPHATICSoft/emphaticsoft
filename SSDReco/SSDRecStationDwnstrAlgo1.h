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
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"
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
          runhist::RunHistory *fRunHistory;
          emph::geo::Geometry *fEmgeo;
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
	  double fChiSqCut;
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
	  
	public:
	 inline void SetDebugOn(bool v = true) { fDebugIsOn = v; }
         inline void SetStationNum(int aStNum) { fStationNum = aStNum; } 
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetChiSqCut (double v) { fChiSqCut = v; }
	 inline void SetForMC(bool v=true) { fIsMC=v; fCoordConvert.SetForMC();  }
	 inline void SetDoFirstAndLastStrips(bool v=true) { fDoFirstAndLastStrips = v; } 
	 inline void SetPreliminaryMomentum(double p) { fPrelimMomentum = p; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
	 inline void Clear() { fStPoints.clear(); }
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
	 
       private:
	 
	 size_t recoXYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); // 
	 
	 size_t  recoXY(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 size_t  recoXUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 size_t  recoYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr);
	 
	 void openOutputCsvFiles() const ;	// we only modify the output streamer  
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRecStationDwnstrAlgo1
