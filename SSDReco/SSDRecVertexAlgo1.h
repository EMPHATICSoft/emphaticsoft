////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of the vertex. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRECVERTEXALGO1_H
#define SSDRECVERTEXALGO1_H
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
#include "RecoBase/BeamTrackAlgo1.h" 
#include "SSDReco/SSDRecDwnstrTracksAlgo1.h"
#include "RecoBase/DwnstrTrackAlgo1.h" 
#include "RecoBase/VertexAlgo1.h" 
#include "SSDReco/SSDVertexFitFCNAlgo1.h"


namespace emph { 
  namespace ssdr {
  
    class SSDRecVertexAlgo1 {
    
       public:
      
	SSDRecVertexAlgo1(); // No args .. for now.. 
        ~SSDRecVertexAlgo1();
	
        private:
	  static const double fSqrt2, fOneOverSqrt2;
          art::ServiceHandle<emph::geo::GeometryService> fGeoService; // in case we need it. 
          emph::geo::Geometry *fEmgeo;
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	    //  This is also invoked in SSDRecStationPoints.. Along the volatile Alignment Params, should be an art service.  
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
	  int fNEvents; // Incremental events count for a given job. 
	  bool fDebugIsOn;
	  double fChiSqCut; // on the downstream tracks. 
	  std::string fTokenJob;
	  ssdr::SSDVertexFitFCNAlgo1 fFitterFCN; // We don't need to know the run number to create an instance, 
	  // so intantiate this in the constructor. 
	  // And the result..
	  double fChiFinal;
	  rb::VertexAlgo1 fVert;
	   
	  mutable std::ofstream fFOutVert;
	  // Internal stuff.. ???
	  
	public:
	 inline void SetDebugOn(bool v = true) { 
	   fDebugIsOn = v;  fFitterFCN.SetDebugOn(v);
	 }
         inline void SetRun(int aRunNum) { 
	   fRunNum = aRunNum;  
	 } 
         inline void SetSubRun(int aSubR) { fSubRunNum = aSubR; } 
	 inline void SetEvtNum(int aEvt) { fEvtNum = aEvt; } 
	 inline void SetChiSqCut (double v) { fChiSqCut = v; }
	 inline void SetTokenJob(const std::string &aT) { 
	   fTokenJob = aT; 
	 }
	 //
	 // Getter, only one output, the reconstructed track.. 
	 //
	 inline int RunNum() const { return fRunNum; }
	 inline int SubRunNum() const { return fSubRunNum; }
	 inline int EvtNum() const { return fEvtNum; }
	 bool RecAndFitIt(const art::Event &evt, 
	                   std::vector<rb::BeamTrackAlgo1>::const_iterator itBeam,
			   const ssdr::SSDRecDwnstrTracksAlgo1 &dwnstr);
	 void dumpInfoForR() const;
	 
       private:
	 
	 void openOutputCsvFiles() const;	 
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRECVERTEXALGO1
