////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct a primary vertex from reco'd tracks
///       
/// \author  $Author: Jon Paley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraph2D.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// EMPHATICSoft includes
#include "RecoBase/Track.h"
#include "RecoBase/Vertex.h"
#include "VertexReco/PrimaryVertexAlgo.h"

///package to illustrate how to write modules
namespace emph {
  ///
  class MakePrimaryVertex : public art::EDProducer {
  public:
    explicit MakePrimaryVertex(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakePrimaryVertex() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
  private:
  
    PVAlgo pvA;
    std::string fTrkLabel;
    
  };

  //.......................................................................
  
  emph::MakePrimaryVertex::MakePrimaryVertex(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fTrkLabel       (pset.get< std::string >("TrkLabel"))
    {
      this->produces< std::vector<rb::Vertex> >();      
    }
  
  //......................................................................

  void emph::MakePrimaryVertex::produce(art::Event& evt)
  {

    std::unique_ptr< std::vector<rb::Vertex> > vtxv(new std::vector<rb::Vertex>);

    art::Handle< std::vector<rb::Track> > trkH;
    
    try {
      evt.getByLabel(fTrkLabel, trkH);
      if (!trkH->empty()){
	std::vector<rb::Track> trkV;
	std::cout << "trkH->size() = " << trkH->size() << std::endl;
	for (size_t idx=0; idx < trkH->size(); ++idx) {
	  auto trk = (*trkH)[idx];
	  std::cout << "trk " << idx << " p = (" << trk.mom << ")" 
		    << std::endl;
	  trkV.push_back(trk);
	}
	if (trkV.size() == 2) {
	  rb::Vertex vtx;
	  std::cout << "beamtrk vtx = (" << trkV[0].vtx << ")" << std::endl;
	  std::cout << "scndtrk vtx = (" << trkV[1].vtx << ")" << std::endl;
	  vtxv->push_back(vtx);
	}
	else if (trkV.size() > 1) {
	  rb::Vertex vtx;
	  std::cout << "Finding vertex..." << std::endl;
	  if (pvA.FindVertexDOCA(trkV,vtx))
	    vtxv->push_back(vtx);
	}
      }
    }
    catch(...) {
      
    }

    evt.put(std::move(vtxv));
  }
  
} // end namespace emph

DEFINE_ART_MODULE(emph::MakePrimaryVertex)
