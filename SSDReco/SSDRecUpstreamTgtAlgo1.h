////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of the beam track upstream of the magnet
///          For Phase1b, no fitting at all, as we have on 4 plane, 2View. 
///          
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRECUSTREAMTGTALGO1_H
#define SSDRECUSTREAMTGTALGO1_H

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
#include "SSDReco/SSDAlignSimpleLinFit.h"
#include "RecoBase/BeamTrackAlgo1.h" 
#include "SSDReco/ConvertDigitToWCoordAlgo1.h"
#include "SSDReco/SSD3DTrackFitFCNAlgo1.h"
#include "SSDReco/VolatileAlignmentParams.h"

namespace emph { 
  namespace ssdr {
 				
    class SSDRecUpstreamTgtAlgo1 {
    
       typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    
       
       public:
      
	SSDRecUpstreamTgtAlgo1(); // No args .. for now.. 
        ~SSDRecUpstreamTgtAlgo1();
	
        private:
	  const size_t fNumStations = 2; // phase 1b and phase1c
	  const size_t fNumStrips = 640; // Note: previous version had one missing.  Makes no difference, really.. 
	  const double fOneOSqrt12; 
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
          bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. 
	  bool fFilesAreOpen;
	  double fPitch;
	  double fHalfWaferWidth;
	  double fNominalMomentum;
	  std::string fTokenJob;
	  art::ServiceHandle<emph::geo::GeometryService> fGeoService;
	  emph::geo::Geometry *fEmgeo;
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	  emph::ssdr::ConvertDigitToWCoordAlgo1 fConvertCoord;
//
          std::vector<myItCl> fDataForXYRec; // We store the pointer, and implictly assume that the list of SSDCluster, within an event, 
	  // stays the same. Protect module wide by constness..  
          std::vector<rb::BeamTrackAlgo1> fTrXYs;
	  
// No fit for Phase1b.... 	   

	  mutable std::ofstream fFOut3D;  // X and Y, but no confirming views for 
	  
	   
	public:
	 inline void SetEmGeometryPtr(emph::geo::Geometry *aEmgeo) {fEmgeo = aEmgeo; } 
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetTokenJob(const std::string &aT) { fTokenJob = aT; }
	 inline void SetNominalMomentum(double p = 120.) { fNominalMomentum = p; }
	 inline void ResetInputData() { fDataForXYRec.clear(); }
	 inline void Reset() { fTrXYs.clear(); } 
	 //
	 // Getter, only one output, the reconstructed track.. 
	 // 
	 inline rb::BeamTrackAlgo1 GetBeamTrack(size_t k) const {
	    if (k < fTrXYs.size() )  return fTrXYs[k]; 
	    rb::BeamTrackAlgo1 aTr; return aTr; // empty, type is tBeamTrType::NONE 
	  } // Deep copy, but small struct.. 
	  // for const access 
	 inline size_t Size() const { return fTrXYs.size(); } 
	 inline std::vector<rb::BeamTrackAlgo1>::const_iterator CBegin() const { return fTrXYs.cbegin(); } 
	 inline std::vector<rb::BeamTrackAlgo1>::const_iterator CEnd() const { return fTrXYs.cend(); } 
	  
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 
	 size_t recoXY(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr); 
	 //  Requires at least 4 hits, one of each view, station 0 and 1 .. Return the number of tracks. (per event!) 
	 void dumpXYInforR(int uFlag=INT_MAX) const;	 
	 
	 private:
	 
	 void openOutputCsvFiles();
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRecUpstreamTgtAlgo1
