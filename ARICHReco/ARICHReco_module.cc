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
//TORCH
#include <ATen/ATen.h>
// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TVector3.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TRandom3.h"
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
#include "RecoBase/ARICHCluster.h"
#include "RecoBase/ArichID.h"
#include "RecoBase/Track.h"
#include "Utilities/PMT.h"
#include "TorchML/NeuralNet.h"
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
    at::Tensor TH2DToTensor(TH2D* hist);

  private:

    arichreco::ARICH_UTILS* ArichUtils;
    TTree* 	fARICHTree;    
 
    int         fEvtNum;
    std::string fARICHLabel;  
    std::string fTrackLabel;  //for now using sim::Tracks
 
    std::string  fModelPath;
    std::string  fSource_path;

    tml::NeuralNet* Model;
    
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
  
    double fdxArich;
    double fdyArich; 
 
    double fAbsZAero1;
    double fAbsZPMTPlane;
 
    bool fUseLL;
    bool fUseML;

    TRandom3* rand_gen;
 
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    
    art::ServiceHandle<emph::geo::GeometryService> geo;

    std::vector<double> momenta;
    std::vector<TVector3> dir;
    std::vector<TVector3> pos;

    std::vector<double> LLscores;
    std::vector<double> MLscores;

    std::vector<double> crossing_track_loc;

    TH2D event_hist, pdf_pion, pdf_kaon, pdf_prot;
    float mom;   
    int nhits;
	
  };

  //.......................................................................
  emph::ARICHReco::ARICHReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::ArichID>>();
    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fFillTree   = bool(pset.get<bool>("FillTree"));
    fTrackLabel	= std::string(pset.get<std::string>("LabelTracks"));
    fModelPath = std::string(pset.get<std::string>("ModelPath"));
    fdxArich = double(pset.get<double>("ArichXshift",0));
    fdyArich = double(pset.get<double>("ArichYshift",0));
    fUseLL =  bool(pset.get<bool>("UseLL"));
    fUseML = bool(pset.get<bool>("UseML"));

      //ARICH RECO UTILS STUFF
      PDfile  =  std::string(pset.get< std::string >("PD_file"));
      PDdarkrate = double(pset.get<double>("PD_Darkrate"));
      PDwin = double(pset.get<double>("Trigger_window"));
      PDfillfactor = double(pset.get<double>("PD_FillFactor"));
      fEvtNum = 0;

     fSource_path = getenv("CETPKG_SOURCE");

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
    if(fFillTree) {
     art::ServiceHandle<art::TFileService const> tfs;
     fARICHTree = tfs->make<TTree>("ARICHRECO","event");
     fARICHTree->Branch("event_hist", &event_hist);
     fARICHTree->Branch("pdf_pion", &pdf_pion);
     fARICHTree->Branch("pdf_kaon", &pdf_kaon);
     fARICHTree->Branch("pdf_prot", &pdf_prot);
     fARICHTree->Branch("momenta", &mom);
     fARICHTree->Branch("event_id", &fEvtNum);
     fARICHTree->Branch("nhits", &nhits);
     fARICHTree->Branch("crossing_track_location",&crossing_track_loc);
     }

    ArichUtils = new arichreco::ARICH_UTILS();
    rand_gen = new TRandom3(0);
     
    Model = new tml::NeuralNet();
    std::string model_path = fSource_path + fModelPath;
    Model->loadModel(model_path);
    mf::LogInfo("ARICH NN path") <<" ARICH model path " << model_path <<  std::endl;

  }

void  emph::ARICHReco::beginRun(art::Run &run){
 
 TString PDfile_path = fSource_path + PDfile;

 up_n = geo->Geo()->GetAerogelUS()->RefractiveIdx();
 down_n = geo->Geo()->GetAerogelDS()->RefractiveIdx();
 up_thick = geo->Geo()->GetAerogelUS()->Thickness()*2/10; //reads half of the gdml value, then cm 
 down_thick = geo->Geo()->GetAerogelDS()->Thickness()*2/10; //reads half of the gdml value, then cm

 down_pos = up_thick; //they are touching
 up_pos = 0; //this is the beginning of the frame

 fAbsZAero1 = geo->Geo()->GetAerogelUS()->Pos()[2];
 fAbsZPMTPlane = geo->Geo()->GetmPMTPlanePos()[2];
 
 PDzpos = (fAbsZPMTPlane - fAbsZAero1)/10.; //in  cm

 ArichUtils->SetUpDet(PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile_path);
 ArichUtils->SetUpArich(up_n,down_n,up_pos,up_thick,down_pos,down_thick);

}

//......................................................................
at::Tensor emph::ARICHReco::TH2DToTensor(TH2D* hist){

    int nx = hist->GetNbinsX();
    int ny = hist->GetNbinsY();
	   
    at::Tensor tensor = at::empty({ny, nx}, at::kFloat);

    for (int iy = 1; iy <= ny; ++iy) {
       for (int ix = 1; ix <= nx; ++ix) {
        double val = hist->GetBinContent(ix, iy);
        tensor.index_put_({iy-1, ix-1}, val);
        }
     }
    tensor = tensor.unsqueeze(0).unsqueeze(0);
    return tensor;
}
//......................................................................
void ARICHReco::produce(art::Event& evt)
{ 
    std::unique_ptr<std::vector<rb::ArichID>> ARICH(new std::vector<rb::ArichID>);

    art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;	
    art::Handle<std::vector<rb::Track>> TracksH;

    evt.getByLabel(fARICHLabel,arich_clusters);

    evt.getByLabel(fTrackLabel,TracksH);  
 
    //push a dummy if no clusters	
    if((int)arich_clusters->size() == 0){
    rb::ArichID arich_id;
    ARICH->push_back(arich_id);  
    }

 
    if( (int)arich_clusters->size() != 0 && (int)TracksH->size() !=0){
    
      fEvtNum = evt.event();

     //for(int i =0; i <(int)arich_clusters->size(); i++)std::cout << "Cluster " << i << " hits " << arich_clusters->at(i).Digits().size() << std::endl;

	//std::cout << "Beam P " << TracksH->at(0).P()[2] <<std::endl;

        rb::Track track = TracksH->at(1); //beam track is [0], only interested in "second" track

//	std::cout << "Track segments found " << track.NTrackSegments() << std::endl;
	rb::TrackSegment last_seg = *track.GetTrackSegment(1); //there are 2 segments	

        double posx = last_seg.vtx.X();
        double posy = last_seg.vtx.Y(); 
        double posz = last_seg.vtx.Z(); 

        double px = last_seg.mom.X();
        double py = last_seg.mom.Y();
        double pz = last_seg.mom.Z();


        mom = sqrt(last_seg.mom.Mag2()); //sqrt(pow(px,2) + pow(py,2) + pow(pz,2)); //* rand_gen->Uniform(1-0.03,1+0.03);

	int max_cluster=-1;
        int max_size = 0;
       for(int u = 0; u < (int)arich_clusters->size(); u++){
          int size = arich_clusters->at(u).NDigits();
          if(size > max_size){
             max_size = size;
             max_cluster = u;
           }
        }

	
       if (mom == 0) {
          mf::LogWarning("ARICHReco") << "Track 1 has zero momentum. Skipping.";
        }

	std::vector<std::pair<int,int>> digs = arich_clusters->at(max_cluster).Digits(); //cluster where the physics is 
        event_hist = *ArichUtils->DigsToHist(digs);

       rb::ArichID arich_id;
       arich_id.trackID = 1;
       arich_id.nhit = digs.size();

      if(fUseLL){	
        float finalx = posx + (fAbsZAero1 - posz) * px/pz + fdxArich;
        float finaly = posy + (fAbsZAero1 - posz) * py/pz + fdyArich;

	float track_crossing_x = posx + (fAbsZPMTPlane - posz) * px/pz + fdxArich;
  	float track_crossing_y = posy + (fAbsZPMTPlane - posz) * py/pz + fdyArich;	
	crossing_track_loc.push_back(track_crossing_x);  crossing_track_loc.push_back(track_crossing_y);

	TVector3 dir_(px/mom,py/mom,pz/mom);
        TVector3 pos_(finalx/10,finaly/10,0.);  //in cm
	 
	std::vector<std::pair<int,int>> digs = arich_clusters->at(max_cluster).Digits(); //cluster where the physics is 
	event_hist = *ArichUtils->DigsToHist(digs);
	LLscores = ArichUtils->identifyParticle(&event_hist, mom, pos_, dir_);

	if(fFillTree){
	  std::vector<TH2D> pdfs = ArichUtils->GetPDF(mom, pos_, dir_);
	  pdf_pion = pdfs[0];
	  pdf_kaon = pdfs[1];
	  pdf_prot = pdfs[2];
	  nhits = arich_clusters->at(max_cluster).NDigits();
	}
          arich_id.scoresLL = LLscores;
	}

	if(fUseML){
 	  at::Tensor tensor_event = TH2DToTensor(&event_hist);
 	  at::Tensor tensor_mom = at::full({1,1}, mom, at::kFloat);
 	  std::vector<at::Tensor> inputs = {tensor_event, tensor_mom};
	  at::Tensor pred = Model->predict(inputs); 

	  auto accessor = pred.accessor<float, 2>();
	  for (int i = 0; i < accessor.size(0); ++i) {
            for (int j = 0; j < accessor.size(1); ++j) {
            	MLscores.push_back((double)accessor[i][j]);
	     }
	  }
          arich_id.scoresML = MLscores;
	}

	ARICH->push_back(arich_id);	  
	 
	if(fFillTree){
	  fARICHTree->Fill();
	  crossing_track_loc.clear();
	}
  	
	} // end if clusters     	 
	MLscores.clear(); LLscores.clear();
	evt.put(std::move(ARICH));	   
  } // end produce 

} // namespace emph

DEFINE_ART_MODULE(emph::ARICHReco)
