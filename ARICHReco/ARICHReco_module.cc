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
#include "RecoBase/Track.h"
#include "RecoBase/ArichPID.h"
#include "Simulation/ARICHHit.h"
#include "Utilities/PMT.h"
#include "Simulation/Track.h"

// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"
#include "ARICHRecoUtils/HoughFitter.h"

using namespace emph;


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
    cmap::FEBoardType boardType = cmap::TRB3;    


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
  
  ARICHReco::ARICHReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::Track>>();
    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fTrackLabel = std::string(pset.get<std::string >("LabelTracks"));
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
    
    ArichUtils = new arichreco::ARICH_UTILS();
    std::string source_path = getenv("CETPKG_SOURCE");
    TString PDfile_path = source_path + PDfile;
    ArichUtils->SetUpDet(PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile_path);
    ArichUtils->SetUpArich(up_n,down_n,up_pos,up_thick,down_pos,down_thick);
  }	
  //......................................................................
 
  ARICHReco::~ARICHReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void ARICHReco::beginJob()
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
  /*  fARICHTree->Branch("BINS_PDF_pion", &bins_pdf_pion);
    fARICHTree->Branch("VALS_PDF_pion", &vals_pdf_pion);
    fARICHTree->Branch("BINS_PDF_kaon", &bins_pdf_kaon);
    fARICHTree->Branch("VALS_PDF_kaon", &vals_pdf_kaon);
    fARICHTree->Branch("BINS_PDF_prot", &bins_pdf_prot);
    fARICHTree->Branch("VALS_PDF_prot", &vals_pdf_prot);
*/
}

 //.......................................................................
  void ARICHReco::beginRun(art::Run &run)
  {
 } 
    
//......................................................................

 void ARICHReco::endJob()
  {
	
  }
  
//......................................................................

 void ARICHReco::endRun(art::Run &run)
  {
  }

//......................................................................

void ARICHReco::produce(art::Event& evt)
  { 
      std::unique_ptr<std::vector<rb::Track>> ARICH_TRACK(new std::vector<rb::Track>);

      //int eventID = evt.id().event();;
      art::Handle<std::vector<rawdata::TRB3RawDigit>> arichH;	
      art::Handle<std::vector<sim::Track>> TracksH;

      evt.getByLabel(fARICHLabel,arichH);
      //std::vector<rawdata::TRB3RawDigit> ArichDigs(*arichH);
      //std::cout << "FOUND " << ArichDigs.size() << " TRB3 HITS" << std::endl;

      evt.getByLabel(fTrackLabel,TracksH);      

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


    if(arichH->size()!=0){ 
        
	 for (size_t idx=0; idx< arichH->size(); ++idx){
 		cmap::EChannel* echan = new cmap::EChannel();
		echan->SetBoardType(boardType);
		echan->SetBoard((*arichH)[idx].GetBoardId());	
	 	echan->SetChannel((*arichH)[idx].GetDetChan());

		cmap::DChannel dchan = cmap->DetChan(*echan);
		blocks.push_back(dchan.Station());
		delete echan; 
	} 
	 TH2D* event_hist = ArichUtils->DigsToHist(blocks);	

/*	for(int k = 0; k < event_hist->GetNcells(); k++){
	  if(event_hist->GetBinContent(k) == 0)continue;
	  std::cout << k << " " << event_hist->GetBinContent(k) <<std::endl;
	  bins.push_back(k);
	  vals.push_back(event_hist->GetBinContent(k)); 
	}
*/
	// if(fFitCircle){ arichreco::HoughFitter* fitter = new arichreco::HoughFitter(event_hist);}
	// BLOCKS WILL BE USED FOR CIRCLE FITTING
	
	// TO BE CHANGED TO USE RECO TRACKS //

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

  	  LLs = ArichUtils->IdentifyMultiParticle(event_hist, np, momenta, pos, dir);		
	 
	  //for(double val : LLs)std::cout << val << " ";
	  //std::cout << std::endl; 
	  // IF WE WANT THE PDFs -> std::vector<std::vector<TH2Poly*>> pdfs = ArichUtils->GetPDFs(np, momenta, pos, dir);
	
	  // for sum_scaled LL
	  //double sum =  std::accumulate(mid.begin(),mid.end(),0.0);
  	  //std::transform(mid.begin(),mid.end(),mid.begin(),[sum](double value){return value/sum;});

	  rb::ArichPID* arich_pid = new rb::ArichPID(LLs);	
	  rb::Track* arich_track = new rb::Track(*arich_pid);  

	  ARICH_TRACK->push_back(*arich_track);	  
	  delete arich_pid;
	  delete arich_track;

	  if(fFillTree){ LL_PION.push_back(LLs[0]); LL_KAON.push_back(LLs[1]); LL_PROT.push_back(LLs[2]);}
	  LLs.clear();
 
	} // end if 1 track
	
	delete event_hist; 
	//momenta.clear(); pos.clear(); dir.clear();
	
       } //end some TRB3 hits 

        if(fFillTree){
	   fARICHTree->Fill();
	   LL_PION.clear();LL_KAON.clear();LL_PROT.clear();MCT_PDG.clear();
	     bins.clear(); vals.clear();
	//    bins_pdf_pion.clear(); vals_pdf_pion.clear();
	//    bins_pdf_kaon.clear(); vals_pdf_kaon.clear();
        //    bins_pdf_prot.clear(); vals_pdf_prot.clear();	
        }
	momenta.clear(); pos.clear(); dir.clear();
        blocks.clear();
      
	 evt.put(std::move(ARICH_TRACK));	   
    
     } // end produce 

} // end namespace emph

DEFINE_ART_MODULE(emph::ARICHReco)
