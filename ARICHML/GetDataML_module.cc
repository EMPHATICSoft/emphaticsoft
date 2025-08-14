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
#include "art/Framework/Core/EDAnalyzer.h" 

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
#include "Geometry/service/GeometryService.h"
#include "RawData/TRB3RawDigit.h"
#include "Simulation/Track.h"
#include "RecoBase/ARICHCluster.h"
// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"


namespace emph {  

  class GetDataML : public art::EDAnalyzer {
  public:
    explicit GetDataML(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~GetDataML();
    
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void endJob();
	

  private:

    arichreco::ARICH_UTILS* ArichUtils;
    TTree* 	fARICHTree;    
 
    int         fEvtNum;
    std::string fARICHLabel;  
    std::string fSimTrackLabel;    

    bool fFillTree;
     
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
 
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    
    art::ServiceHandle<emph::geo::GeometryService> geom;

    std::vector<double> momenta;
    std::vector<double> pos;
    std::vector<double> LL;
    int pdg;

    TH2D histo; 

	
};

  //.......................................................................
  
  emph::GetDataML::GetDataML(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
 { 

    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fSimTrackLabel = std::string(pset.get<std::string >("SimTrackLabel"));
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
 
  emph::GetDataML::~GetDataML()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void emph::GetDataML::beginJob()
  {    
    if (fFillTree){
    art::ServiceHandle<art::TFileService const> tfs;
    fARICHTree = tfs->make<TTree>("ARICHRECO","event");
    fARICHTree->Branch("Momenta", &momenta);
    fARICHTree->Branch("Position", &pos);
    fARICHTree->Branch("histo", &histo);
    fARICHTree->Branch("pdg", &pdg);
    fARICHTree->Branch("LogLikelihood", &LL);  
   }
    ArichUtils = new arichreco::ARICH_UTILS();
   
    std::string source_path = getenv("CETPKG_SOURCE");
    TString PDfile_path = source_path + PDfile;
    ArichUtils->SetUpDet(PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile_path);
    ArichUtils->SetUpArich(up_n,down_n,up_pos,up_thick,down_pos,down_thick);


}
    
//......................................................................
void emph::GetDataML::endJob()
{
  fARICHTree->Fill();
}

//......................................................................
void emph::GetDataML::analyze(const art::Event& evt)
 { 

      art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;	

      evt.getByLabel(fARICHLabel,arich_clusters);


	art::Handle<std::vector<sim::Track>> sim_tracks;
	evt.getByLabel(fSimTrackLabel,sim_tracks);
	
	//std::cout << "Target USZ "<< geom->Geo()->TargetUSZPos() << std::endl;
	//std::cout << "Target DSZ "<< geom->Geo()->TargetDSZPos() << std::endl;

	//from phace1c.gdml, aerogels are between 1915 - 1955 mm 
	std::vector<std::tuple<int,sim::Track>> trackid_track;

	for(int h=0; h < (int)sim_tracks->size(); h++){

	sim::Track track = sim_tracks->at(h);

	if(track.GetZ() < 1915  || track.GetZ() > 1955)continue;
//	std::cout << "track id " << track.GetTrackID() << " son of " << track.GetParentTrackID() << " pdg " <<  track.GetPId() << " mom " << track.GetPz() << " pos_Z " << track.GetZ() << std::endl;

	auto it = std::find_if(trackid_track.begin(), trackid_track.end(),
   	[track](const auto element){ return std::get<0>(element) == track.GetTrackID();	});
	if(it == trackid_track.end())trackid_track.push_back(std::make_tuple(track.GetTrackID(), track));
	else continue;	
	}
	if((int)arich_clusters->size() == 0)return;
	for(int u = 0; u < (int)arich_clusters->size(); u++){
           	
	     if(arich_clusters->at(u).NDigits() < 4)continue;            
	
	      std::vector<std::pair<int,int>> digs = arich_clusters->at(u).Digits();  	
	      TH2D* event_hist = ArichUtils->DigsToHist(digs);	
	//if(trackid_pdg_mom.size() == 0) {     std::cout << "Zero sim track crossing aerogels but " << event_hist->Integral() << " hits" << std::endl;
	  //     for(auto track: *sim_tracks)std::cout << "track id " << track.GetTrackID() << " son of " << track.GetParentTrackID() << " pdg " <<  track.GetPId() << " mom " << track.GetPz() << " pos_Z " << track.GetZ() << std::endl;
	    // }  
	     histo = *event_hist;
	     delete event_hist;	
	}
	
	if(trackid_track.size() > 1 || trackid_track.size() ==0)return;
	else{	
	   sim::Track the_track = std::get<1>(trackid_track[0]); 
	   momenta.push_back(the_track.GetPx());  
	   momenta.push_back(the_track.GetPy());  
	   momenta.push_back(the_track.GetPz());
	
	   TVector3 p(the_track.GetPx(), the_track.GetPy(), the_track.GetPz());
	   	 
	   pos.push_back(the_track.GetX());
	   pos.push_back(the_track.GetY());
	   pos.push_back(the_track.GetZ());

	   TVector3 pp(the_track.GetX()/10, the_track.GetY()/10,0); 
 	   LL = ArichUtils->identifyParticle(&histo, p.Mag()/1e3, pp, p.Unit());

	   pdg = the_track.GetPId();	
  	  	  
 	   fARICHTree->Fill();
       }
	momenta.clear();
	pos.clear();
        LL.clear();
 
 } // end analyze

}
DEFINE_ART_MODULE(emph::GetDataML)
