////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of the vertex. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRECVERTEXAUTRE_H
#define SSDRECVERTEXAUTRE_H
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
#include "SSDReco/experimental/VolatileAlignmentParams.h"
#include "RecoBase/BeamTrack.h" 
#include "SSDReco/experimental/SSDRecDwnstrTracksAutre.h"
#include "RecoBase/DwnstrTrack.h" 
#include "RecoBase/Vertex.h" 
#include "SSDReco/experimental/SSDVertexFitFCNAutre.h"


namespace emph { 
  namespace ssdr {
  
    class SSDRecVertexAutre {
    
       public:
      
	SSDRecVertexAutre(); // No args .. for now.. 
        ~SSDRecVertexAutre();
	
        private:
	  static const double fSqrt2, fOneOverSqrt2;
          art::ServiceHandle<emph::geo::GeometryService> fGeoService; // in case we need it. 
          emph::geo::Geometry *fEmgeo;
          emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
	    //  This is also invoked in SSDRecStationPoints.. Along the volatile Alignment Params, should be an art service.  
	  int fRunNum;  // The usual Ids for a art::event 
	  int fSubRunNum;
	  int fEvtNum;
//	  int fNEvents; // Incremental events count for a given job. No longer used.. Use art framework to get event id. 
	  bool fDebugIsOn;
	  double fChiSqCut; // on the downstream tracks. 
	  std::string fTokenJob;
	  ssdr::SSDVertexFitFCNAutre fFitterFCN; // We don't need to know the run number to create an instance, 
	  // so intantiate this in the constructor. 
	  // And the result..
	  double fChiFinal;
	  rbex::Vertex fVert;
	   
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
	                   std::vector<rb::BeamTrack>::const_iterator itBeam,
			   const ssdr::SSDRecDwnstrTracksAutre &dwnstr);
	 void dumpInfoForR() const;
	 
       private:
	 
	 void openOutputCsvFiles() const;	 
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDRECVERTEXAUTRE
