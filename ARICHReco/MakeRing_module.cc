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


namespace emph {  

  class MakeRing : public art::EDProducer {
  public:
    explicit MakeRing(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeRing();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginRun(art::Run &run);
    void endRun(art::Run &run);
    void endJob();

  private:

    arichreco::ARICH_UTILS* ArichUtils;
    TTree* 	fARICHTree;    
 
    int         fEvtNum;
    std::string fARICHLabel;  
    
   // Aerogel parameters
    double up_n;
    double up_pos;
    double up_thick;
    double down_n;
    double down_pos;
    double down_thick;

    // Photodetector parameters
    double PDdarkrate;
    double PDwin;
    double PDfillfactor;
    double PDzpos;
    TString PDfile;
    bool fFillTree;
    
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    

    std::vector<double> momenta;
    std::vector<TVector3> dir;
    std::vector<TVector3> pos;
   
    std::vector<int> bins,vals;


   // bins_pdf_pion,bins_pdf_kaon,bins_pdf_prot, vals; 
    
    std::vector<int> blocks,MCT_PDG,unique_ids;
    int pdg_event;
	
};

  //.......................................................................
  
  emph::MakeRing::MakeRing(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::ARing>>();
    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fFillTree   = bool(pset.get<bool>("FillTree"));
 

    //ARICH RECO UTILS STUFF
    PDfile  =  std::string(pset.get< std::string >("PD_file"));
    up_n = double(pset.get<double>("RefractiveIndex_UpstreamAerogel"));
    up_pos = double(pset.get<double>("Position_UpstreamAerogel"));
    up_thick = double(pset.get<double>("Thinkness_UpstreamAerogel"));
    down_n = double(pset.get<double>("RefractiveIndex_DownstreamAerogel"));
    down_pos = double(pset.get<double>("Position_DownstreamAerogel"));
    down_thick = double(pset.get<double>("Thickness_DownstreamAerogel"));
    PDdarkrate = double(pset.get<double>("PD_Darkrate"));
    PDwin = double(pset.get<double>("Trigger_window"));
    PDfillfactor = double(pset.get<double>("PD_FillFactor"));
    PDzpos = double(pset.get<double>("PD_Position"));
    fEvtNum = 0;
    
  }	
  //......................................................................
 
  emph::MakeRing::~MakeRing()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void emph::MakeRing::beginJob()
  {    
    art::ServiceHandle<art::TFileService const> tfs;
    fARICHTree = tfs->make<TTree>("ARICHRECO","event");
    fARICHTree->Branch("TruthPDG", &MCT_PDG);
    fARICHTree->Branch("Blocks", &blocks);
    fARICHTree->Branch("Momenta", &momenta);
    fARICHTree->Branch("BINS", &bins);
    fARICHTree->Branch("VALS", &vals);
	

    ArichUtils = new arichreco::ARICH_UTILS();
    std::string source_path = getenv("CETPKG_SOURCE");
    TString PDfile_path = source_path + PDfile;
    ArichUtils->SetUpDet(PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile_path);
    ArichUtils->SetUpArich(up_n,down_n,up_pos,up_thick,down_pos,down_thick);


 /*  fARICHTree->Branch("BINS_PDF_pion", &bins_pdf_pion);
    fARICHTree->Branch("VALS_PDF_pion", &vals_pdf_pion);
    fARICHTree->Branch("BINS_PDF_kaon", &bins_pdf_kaon);
    fARICHTree->Branch("VALS_PDF_kaon", &vals_pdf_kaon);
    fARICHTree->Branch("BINS_PDF_prot", &bins_pdf_prot);
    fARICHTree->Branch("VALS_PDF_prot", &vals_pdf_prot);
*/
}

 //.......................................................................
  void emph::MakeRing::beginRun(art::Run &run)
  {
 } 
    
//......................................................................

 void emph::MakeRing::endJob()
  {
  }
  
//......................................................................

 void emph::MakeRing::endRun(art::Run &run)
  {
  }

//......................................................................

void MakeRing::produce(art::Event& evt)
  { 
      std::unique_ptr<std::vector<rb::ARing>> ARICH_RINGS(new std::vector<rb::ARing>);

      //int eventID = evt.id().event();;
      art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;	
 //     art::Handle<std::vector<sim::Track>> TracksH;

      evt.getByLabel(fARICHLabel,arich_clusters);
      //std::cout << "FOUND " << ArichDigs.size() << " TRB3 HITS" << std::endl;

      //evt.getByLabel(fTrackLabel,TracksH);      

//      art::Handle<std::vector<rb::Track>> RecoTracksH;
//      evt.getByLabel("makesingletracks", RecoTracksH);
      //std::vector<rb::Track> RecoTracks(*RecoTracksH);

     // std::cout << "FOUND " << TracksH->size() << " SIM TRACKS, ";
     // std::cout << "FOUND " << RecoTracksH->size() << " RECO TRACKS" << std::endl;    
	 
     // std::cout << std::endl;

//	float posx, posy, posz;

      //if(RecoTracksH->size() != 0 ){

/*	  rb::Track track = RecoTracksH->at(1);

	  std::cout << "Momenta :" << RecoTracksH->at(1).P()[0] << ", " <<  RecoTracksH->at(1).P()[1] << ", " <<  RecoTracksH->at(1).P()[2] << std::endl;
	
     	  posx = track.Vtx()[0];
	  posy = track.Vtx()[1];
	  posz = track.Vtx()[2];
	 
 	  std::cout << "initial pos: " << posx << ", " << posy << ", " << posz << std::endl;
	 
     	  float mom = sqrt(pow(track.P()[0],2) + (track.P()[1],2) + (track.P()[2],2));

	  float finalx = posx + (192.0 - posz) * track.P()[0]/mom;
	  float finaly = posy + (192.0 - posz) * track.P()[1]/mom; 	
	  
          std::cout << "Final pos: " << finalx << ", " << finaly << std::endl;
*/
//	}

	

	for(int u = 0; u < (int)arich_clusters->size(); u++){
            
            std::cout << "cluster " << u << " size " << arich_clusters->at(u).NDigits() << std::endl;
	
	     if(arich_clusters->at(u).NDigits() < 4)continue;            
	
    
	      std::vector<std::pair<int,int>> digs = arich_clusters->at(u).Digits();  	

	      TH2D* event_hist = ArichUtils->DigsToHist(digs);	

	      arichreco::HoughFitter* fitter = new arichreco::HoughFitter(event_hist);  
	
	      int to_find = 1; // number of rings to find, should be = n tracks 
	      std::vector<std::tuple<int, int, double>> circles =  fitter->GetCirclesCenters(to_find); 

	      for(int j =0; j < (int)circles.size();j++ ){
		
	       rb::ARing ring;		
	
	       ring.radius = std::get<2>(circles[j]);
	       ring.center[0] = std::get<0>(circles[j]);
	       ring.center[1] = std::get<1>(circles[j]);	
	 
		ARICH_RINGS->push_back(ring); 
	    
	     } 				
 
	     std::vector<TGraph*> circleGraphs =  fitter->createCircleGraph(circles,360);
	

	    TCanvas *c1 = new TCanvas();

	    event_hist->Draw("colz");
	    for(int i = 0; i < to_find; i++){
             circleGraphs[i]->SetLineWidth(3);
	     circleGraphs[i]->Draw("L SAME");
           }
	   c1->SaveAs(Form("histos/event_%i_cluster_%i.png",evt.event(),u));
	   delete c1;		
	
	      delete event_hist;
	}

	evt.put(std::move(ARICH_RINGS));	   
    
     } // end produce 

}
DEFINE_ART_MODULE(emph::MakeRing)
