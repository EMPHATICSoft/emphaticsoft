////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create reco vectors from raw digits and
///          store them in the art output file
/// \author  $Author: mdallolio $
////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include "stdlib.h"
#include <map>
#include <numeric>
// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TGraph2D.h"
#include "TVector3.h"
#include "TTree.h"
#include "TCanvas.h"
// Framework includes
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "Geometry/service/GeometryService.h"
#include "DetGeoMap/service/DetGeoMapService.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/ARICHCluster.h"
#include "RecoBase/ARing.h"
#include "Utilities/PMT.h"

// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"
#include "ARICHRecoUtils/HoughFitter.h"


using namespace emph;

namespace emph
{

  class MakeRing : public art::EDProducer
  {
  public:
    explicit MakeRing(fhicl::ParameterSet const &pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeRing();

    // Optional, read/write access to event
    void produce(art::Event &evt);
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void endJob();
 
 private:
    arichreco::ARICH_UTILS *ArichUtils;
    TTree *fARICHTree;

    int fEvtNum;
    std::string fARICHLabel;

    bool fFillTree;

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;

    double fitX;
    double fitY;
    double fitR;



    TH2D event_hist;
  
    TGraph2D plot3d;
  };

  //.......................................................................

   MakeRing::MakeRing(fhicl::ParameterSet const &pset)
      : EDProducer(pset)
  {

    this->produces<std::vector<rb::ARing>>();
    fARICHLabel = std::string(pset.get<std::string>("LabelHits"));
    fFillTree = bool(pset.get<bool>("FillTree"));
    // ARICH RECO UTILS STUFF
    fEvtNum = 0;
  }
  //......................................................................

  MakeRing::~MakeRing()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void MakeRing::beginJob()
  {
   if(fFillTree){
     art::ServiceHandle<art::TFileService const> tfs;
     fARICHTree = tfs->make<TTree>("MAKERING","event");
     fARICHTree->Branch("event_hist", &event_hist);
     fARICHTree->Branch("fitX", &fitX);	
     fARICHTree->Branch("fitY", &fitY);  
     fARICHTree->Branch("fitR", &fitR);   
     fARICHTree->Branch("plot3D", &plot3d);
   }

     ArichUtils = new arichreco::ARICH_UTILS();

  } 
  void MakeRing::endJob()
  { 
  }

  //......................................................................

  void MakeRing::produce(art::Event &evt)
  {
    std::unique_ptr<std::vector<rb::ARing>> ARICH_RINGS(new std::vector<rb::ARing>);

    art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;

    evt.getByLabel(fARICHLabel, arich_clusters);

    rb::ARing ring;


    int max_cluster=-1;
    int max_size = 0;


    for(int u = 0; u < (int)arich_clusters->size(); u++){
 
     int size = arich_clusters->at(u).NDigits();
     if(size > max_size){
  	max_size = size;
	max_cluster = u;
	}
   }

//	for(int u = 0; u < (int)arich_clusters->size(); u++){
            
	
//	     if(arich_clusters->at(u).NDigits() < 4)continue;            
	
	if(arich_clusters->size() != 0 && arich_clusters->at(max_cluster).NDigits() > 4){    

	      std::vector<std::pair<int,int>> digs = arich_clusters->at(max_cluster).Digits();  	
	     
	      std::vector<float> times = arich_clusters->at(max_cluster).Times();

	      event_hist = *ArichUtils->DigsToHist(digs);	

	      plot3d = *ArichUtils->DigsToHist(digs,times);	      

	      arichreco::HoughFitter* fitter = new arichreco::HoughFitter(&event_hist);  
	
	      int to_find = 1; // number of rings to find, should be = n tracks 
	
	      std::vector<std::tuple<int, int, double>> circles =  fitter->GetCirclesCenters(to_find); 

	      //for(int j =0; j < (int)circles.size();j++ ){
		
		rb::ARing ring;		
		
		ring.SetRadius(std::get<2>(circles[0]));
		float center[3] = {float(std::get<0>(circles[0])),float(std::get<1>(circles[0])),0};

//		std::cout << "fit values " << center[0] << " " << center[1] << " " << std::get<2>(circles[0]) << std::endl;

		ring.SetCenter(center);
		ring.SetNHits(arich_clusters->at(max_cluster).NDigits()); 
		
		if(fFillTree){
		fitX = float(std::get<0>(circles[0])); 
		fitY = float(std::get<1>(circles[0])); 
		fitR = float(std::get<2>(circles[0]));	
		}	
	/*	std::cout << "	radius " << ring.Radius() << std::endl;
		double theta = atan(ring.Radius()/178.9); //mm
	 	std::cout << "	thetaC " << theta << " rad " << std::endl;
		double beta = 1/(1.028*cos(theta)); 
		std::cout << "	beta " << beta << std::endl;
		std::cout << "Prot p " << ArichUtils->calcP(0.9383,beta) << std::endl;
		std::cout << "Pion p " << ArichUtils->calcP(0.1395,beta) << std::endl;	
 	}	
	*/
		ARICH_RINGS->push_back(ring); 
		if(fFillTree)fARICHTree->Fill();   
//	     }	
	
		delete fitter;
	}

    evt.put(std::move(ARICH_RINGS));

  } // end produce

}
DEFINE_ART_MODULE(emph::MakeRing)
