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
#include "RecoBase/ArichID.h"
#include "Utilities/PMT.h"

// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"


namespace emph {  

  class ARICHReco : public art::EDProducer {
  public:
    explicit ARICHReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~ARICHReco();
    
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
    std::string fTrackLabel;  //for now using sim::Tracks
    
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
    bool fFitCircle;
    
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    

    std::vector<double> momenta;
    std::vector<TVector3> dir;
    std::vector<TVector3> pos;
    std::vector<double> LLs;
   
    std::vector<double> LL_PION,LL_KAON, LL_PROT;//, vals_pdf_pion, vals_pdf_kaon, vals_pdf_prot;
    std::vector<int> bins,vals;


   // bins_pdf_pion,bins_pdf_kaon,bins_pdf_prot, vals; 
    
    std::vector<int> blocks,MCT_PDG,unique_ids;
    int pdg_event;
	
};

  //.......................................................................
  
  emph::ARICHReco::ARICHReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::ArichID>>();
    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fFillTree   = bool(pset.get<bool>("FillTree"));
    fFitCircle = bool(pset.get<bool>("FitCircle"));
 

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
 
  emph::ARICHReco::~ARICHReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void emph::ARICHReco::beginJob()
  {    
    art::ServiceHandle<art::TFileService const> tfs;
    fARICHTree = tfs->make<TTree>("ARICHRECO","event");
    fARICHTree->Branch("TruthPDG", &MCT_PDG);
    fARICHTree->Branch("Blocks", &blocks);
    fARICHTree->Branch("Momenta", &momenta);
    fARICHTree->Branch("LL_pion", &LL_PION);
    fARICHTree->Branch("LL_kaon", &LL_KAON);
    fARICHTree->Branch("LL_prot", &LL_PROT);
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
  void emph::ARICHReco::beginRun(art::Run &run)
  {
 } 
    
//......................................................................

 void emph::ARICHReco::endJob()
  {
  }
  
//......................................................................

 void emph::ARICHReco::endRun(art::Run &run)
  {
  }

//......................................................................

void ARICHReco::produce(art::Event& evt)
  { 
      std::unique_ptr<std::vector<rb::ArichID>> ARICH_LL(new std::vector<rb::ArichID>);

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

//	       LLs = ArichUtils->IdentifyMultiParticle(event_hist, np, momenta, pos, dir); 
	

	 delete event_hist;
	}

	
	// TO BE CHANGED TO USE RECO TRACKS //
/*
	for(size_t l =0 ; l < (*TracksH).size(); l++){
	double track_z = (*TracksH)[l].GetZ();
	int track_id = (*TracksH)[l].GetTrackID();	
	  	
	if (1920. < track_z && track_z < 1940. ){  // aerogel positions in phase1C 
	  
	  float mom = sqrt(pow((*TracksH)[l].GetPx(),2) + pow((*TracksH)[l].GetPy(),2) + pow((*TracksH)[l].GetPz(),2));
		 
	    if(std::find(unique_ids.begin(), unique_ids.end(), track_id) == unique_ids.end()){  

		//std::cout << "track id: " << track_id << " pos: " << (*TracksH)[l].GetX()/10 << ", " << (*TracksH)[l].GetY()/10 << std::endl;	
		pdg_event = (*TracksH)[l].GetPId();	 	
		MCT_PDG.push_back(pdg_event);
		TVector3 dir_((*TracksH)[l].GetPx()/mom,(*TracksH)[l].GetPy()/mom,(*TracksH)[l].GetPz()/mom);
                TVector3 pos_((*TracksH)[l].GetX()/10,(*TracksH)[l].GetY()/10,0.);  //in cm
                momenta.push_back(mom/1000);   //Tracks[p].mom use GeV not MeV
		dir.push_back(dir_);      //Tracks[p].dir);     
                pos.push_back(pos_);      //Tracks[p].pos);
	    	dir_.Clear(); pos_.Clear();
		unique_ids.push_back(track_id); 	 
	     }
	   }
	}
*/
	// THINGS WITH TRACK INFO	
	// well probably need to consider the case when we have rb::Tracks and when we don't
	//
	// WITH TRACKS: LL with PDF based on dir and momentum [assuming it reconstructs the tracks well]
	// NO TRACKS: LL? HOUGH FITTER? ML NRings?

	
	int np = unique_ids.size();   //Tracks.size() 
	//-> Number of tracks == number of particles to find (check with direction if the particle crosses the aerogel, it may not) 
	unique_ids.clear();
	
	//std::cout << "Unique particles found " << np << std::endl;	

	if(np == 1){  //for now single tracks

  	 // LLs = ArichUtils->IdentifyMultiParticle(event_hist, np, momenta, pos, dir);		
	 
	  //for(double val : LLs)std::cout << val << " ";
	  //std::cout << std::endl; 
	  // IF WE WANT THE PDFs -> std::vector<std::vector<TH2Poly*>> pdfs = ArichUtils->GetPDFs(np, momenta, pos, dir);
	
	  // for sum_scaled LL
	  //double sum =  std::accumulate(mid.begin(),mid.end(),0.0);
  	  //std::transform(mid.begin(),mid.end(),mid.begin(),[sum](double value){return value/sum;});



	  if(fFillTree){ LL_PION.push_back(LLs[0]); LL_KAON.push_back(LLs[1]); LL_PROT.push_back(LLs[2]);}
	  LLs.clear();
 
	} // end if 1 track
	
	//delete event_hist; 
	//momenta.clear(); pos.clear(); dir.clear();
	
/*
        if(fFillTree){
	   fARICHTree->Fill();
	   LL_PION.clear();LL_KAON.clear();LL_PROT.clear();MCT_PDG.clear();
	     bins.clear(); vals.clear();
	//    bins_pdf_pion.clear(); vals_pdf_pion.clear();
	//    bins_pdf_kaon.clear(); vals_pdf_kaon.clear();
        //    bins_pdf_prot.clear(); vals_pdf_prot.clear();	
        }
	//momenta.clear(); pos.clear(); dir.clear();
        //blocks.clear();
*/	evt.put(std::move(ARICH_LL));	   
    
     } // end produce 

}
DEFINE_ART_MODULE(emph::ARICHReco)
