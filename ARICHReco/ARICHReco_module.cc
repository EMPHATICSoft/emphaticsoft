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
#include "RecoBase/Track.h"
#include "RecoBase/ArichPID.h"
#include "StandardRecord/SRARing.h"
#include "Simulation/ARICHHit.h"
#include "Utilities/PMT.h"
#include "Simulation/Track.h"

// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"
#include "ARICHRecoUtils/HoughFitter.h"


namespace emph {  
  class ARICHReco;
}  

  class emph::ARICHReco : public art::EDProducer {
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
    std::map<int,double> GetRefenceTimes(std::vector<emph::rawdata::TRB3RawDigit> hits);
    std::vector<std::vector<std::tuple<float, int, int, int>>> splitVector( std::vector<std::tuple<float, int, int, int>> sortedVec, int threshold);

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

std::map<int,double> emph::ARICHReco::GetRefenceTimes(std::vector<emph::rawdata::TRB3RawDigit> hits){

   std::map<int,double> refTime;
   for (size_t idx=0; idx < hits.size(); ++idx) {

    const emph::rawdata::TRB3RawDigit& trb3 = (hits)[idx];

    if (trb3.GetChannel()==0) {
      int fpga = trb3.GetBoardId();
      if (refTime.find(fpga)==refTime.end()) {
        refTime[fpga] = trb3.GetFinalTime();
        }
       else {
           std::cout << "Reference time for fpga " << fpga << " already exists." << " Time difference "
            << (trb3.GetFinalTime()-refTime[fpga])/1e3 << " (ns)" << std::endl;
       	    }
       }
   } 

   return refTime;

}

std::vector<std::vector<std::tuple<float, int, int, int>>> emph::ARICHReco::splitVector( std::vector<std::tuple<float, int, int, int>> sortedVec, int threshold) {
    
    std::vector<std::vector<std::tuple<float, int, int, int>>> clusters;
    size_t i = 0;   
    while (i < sortedVec.size()) {
        std::vector<std::tuple<float, int, int, int>> cluster;
        int start = abs(std::get<0>(sortedVec[i]));
	

	 while (i < sortedVec.size() && abs(abs(std::get<0>(sortedVec[i])) - start) <= threshold) {
        
            cluster.push_back(sortedVec[i]);
            ++i;
        }

        clusters.push_back(cluster);
    }
    
    return clusters;
}
//......................................................................

void emph::ARICHReco::produce(art::Event& evt)
  { 
      std::unique_ptr<std::vector<rb::Track>> ARICH_TRACK(new std::vector<rb::Track>);

      //int eventID = evt.id().event();;
      art::Handle<std::vector<emph::rawdata::TRB3RawDigit>> arichH;	
 //     art::Handle<std::vector<sim::Track>> TracksH;

      evt.getByLabel(fARICHLabel,arichH);
      std::vector<emph::rawdata::TRB3RawDigit> ArichDigs(*arichH);
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


    if(arichH->size()!=0){ 

	std::map<int, double> refTime = GetRefenceTimes(ArichDigs);

	std::vector<std::tuple<float, int, int, int>> hits;
	std::map<emph::cmap::EChannel,std::vector<double>> leadTimesCh;
        std::map<emph::cmap::EChannel,std::vector<double>> trailTimesCh;

	for (size_t idx=0; idx < arichH->size(); ++idx) {

          const emph::rawdata::TRB3RawDigit& trb3 = (*arichH)[idx];
          // skip timing channel
          if (trb3.GetChannel()==0) continue;
          
          int fpga = trb3.GetBoardId();
	  int ech = trb3.GetChannel();
	  
	  emph::cmap::EChannel echan(emph::cmap::TRB3,fpga,ech);
          
	  double time = (trb3.GetFinalTime()-refTime[fpga])/1e3;//ns
         
	  if (trb3.IsLeading())  leadTimesCh[echan].push_back(time);
	  if (trb3.IsTrailing()) trailTimesCh[echan].push_back(time);
          
	}

//	std::cout << "Raw data size " << arichH->size() << std::endl;
	
	for (auto lCh=leadTimesCh.begin();lCh!=leadTimesCh.end();lCh++) {
	
	 auto tCh = trailTimesCh.find(lCh->first);
          if (tCh==trailTimesCh.end()) continue;

	  std::vector<double> leadTimes  = lCh->second;;
          std::vector<double> trailTimes = tCh->second;

	  std::sort(leadTimes.begin(),leadTimes.end());
          std::sort(trailTimes.begin(),trailTimes.end());

	 for (unsigned int l=0;l<leadTimes.size();l++) {

            double lead  = leadTimes[l];
            double lead_next  = l<leadTimes.size()-1 ? leadTimes[l+1] : 1000;
            std::vector<double> trail_found;
        
	    for (unsigned int t=0;t<trailTimes.size();t++) {
              if (trailTimes[t]>lead && trailTimes[t]<lead_next ) {
                trail_found.push_back(trailTimes[t]);
               }
             }
	   if (trail_found.size()>0) {
 	    for(int n_trail=0; n_trail < (int)trail_found.size(); n_trail++){
		   emph::cmap::EChannel echan = lCh->first;
		   emph::cmap::DChannel dchan = cmap->DetChan(echan);
		  std::tuple<float, int, int, int> time_board_channel = std::make_tuple(lead, echan.Board(), echan.Channel(), dchan.Station());
	  	  hits.push_back(time_board_channel);	
	     }
	   }
	 } //end loop over lead times
     } //end loop over channels 


	//Now the vector hits contains all the necessary info to cluster the hits in time and feed the clusters to the reco code
	
	stable_sort(hits.begin(), hits.end());

        int threshold = 15;
        std::vector<std::vector<std::tuple<float, int, int, int>>> clusters = splitVector(hits, threshold);	

	for(int u = 0; u < (int)clusters.size(); u++){
            
            std::cout << "cluster " << u << " size " << clusters[u].size() << std::endl;
	
	     if(clusters[u].size() < 4)continue;            

            std::vector<std::tuple<float, int, int, int>> cluster = clusters[u];

            TH2D* event_hist = ArichUtils->DigsToHist(cluster);	
	   
	    if(fFitCircle){

		arichreco::HoughFitter* fitter = new arichreco::HoughFitter(event_hist);  
	
		int to_find = 1;
		std::vector<std::tuple<int, int, double>> circles =  fitter->GetCirclesCenters(1); 

		for(int l =0; l < (int)circles.size(); l++){
		caf::SRARing ring;

		ring.nhit = (int)cluster.size();
		ring.center[0] = (float)std::get<0>(circles[l]);
		ring.center[1] = (float)std::get<1>(circles[l]); 
		ring.radius = std::get<2>(circles[l]);
		
		std::cout << Form("circle %i center (%i, %i), r %f",l, std::get<0>(circles[l]),std::get<1>(circles[l]), std::get<2>(circles[l])) << std::endl;
		}
		
		std::cout << "Center by mean (" << event_hist->GetMean(1) << " ," << event_hist->GetMean(2) << ")" << std::endl;
		delete fitter;

/*		std::vector<TGraph*> circleGraphs =  fitter->createCircleGraph(circles,360);
	
		event_hist->SetName(Form("event_%i_cluster_%i",evt.event(),u));
	
		TGraph* Mean_center = new TGraph(1);

		Mean_center->SetPoint(0,event_hist->GetMean(1),event_hist->GetMean(2));Mean_center->SetMarkerStyle(47);

	    TCanvas *c1 = new TCanvas();

	    event_hist->Draw("colz");
	    for(int i = 0; i < to_find; i++){
             circleGraphs[i]->SetLineWidth(3);
	     circleGraphs[i]->Draw("L SAME");
           }
	   Mean_center->Draw("P SAME");
	   c1->SaveAs(Form("histos/event_%i_cluster_%i.png",evt.event(),u));
	   delete c1;
	*/ 
	 }
	
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


DEFINE_ART_MODULE(emph::ARICHReco)
