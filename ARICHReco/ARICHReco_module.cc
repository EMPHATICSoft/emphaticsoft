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
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"
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
    struct Config {
      fhicl::Atom<std::string> LabelHits{fhicl::Name("LabelHits")};
      fhicl::Atom<bool> FillTree{fhicl::Name("FillTree")};
      fhicl::Atom<std::string> LabelTracks{fhicl::Name("LabelTracks")};
      fhicl::Atom<std::string> ModelPath{fhicl::Name("ModelPath")};
      fhicl::Atom<std::string> PD_file{fhicl::Name("PD_file")};
      fhicl::Atom<double> RefractiveIndex_UpstreamAerogel{fhicl::Name("RefractiveIndex_UpstreamAerogel")};
      fhicl::Atom<double> Position_UpstreamAerogel{fhicl::Name("Position_UpstreamAerogel")};
      fhicl::Atom<double> Thinkness_UpstreamAerogel{fhicl::Name("Thinkness_UpstreamAerogel")};
      fhicl::Atom<double> RefractiveIndex_DownstreamAerogel{fhicl::Name("RefractiveIndex_DownstreamAerogel")};
      fhicl::Atom<double> Position_DownstreamAerogel{fhicl::Name("Position_DownstreamAerogel")};
      fhicl::Atom<double> Thickness_DownstreamAerogel{fhicl::Name("Thickness_DownstreamAerogel")};
      fhicl::Atom<double> PD_Darkrate{fhicl::Name("PD_Darkrate")};
      fhicl::Atom<double> Trigger_window{fhicl::Name("Trigger_window")};
      fhicl::Atom<double> PD_FillFactor{fhicl::Name("PD_FillFactor")};
      fhicl::Atom<double> PD_Position{fhicl::Name("PD_Position")};
    };
    using Parameters = art::EDProducer::Table<Config>;

    explicit ARICHReco(Parameters const& pset);
    // Required! explicit tag tells the compiler this is not a copy constructor
    ~ARICHReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    at::Tensor TH2DToTensor(TH2D* hist);

  private:

    arichreco::ARICH_UTILS* ArichUtils;
    TTree* 	fARICHTree;    
 
    int         fEvtNum;
    std::string fARICHLabel;  
    std::string fTrackLabel;  //for now using sim::Tracks
 
    std::string  fModelPath;

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
   
    TRandom3* rand_gen;
 
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    

    std::vector<double> momenta;
    std::vector<TVector3> dir;
    std::vector<TVector3> pos;
    std::vector<double> LLs;

    TH2D event_hist, pdf_pion, pdf_kaon, pdf_prot;
    float mom;   
 
	
  };

  //.......................................................................
  emph::ARICHReco::ARICHReco(Parameters const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::ArichID>>();
    fARICHLabel = pset().LabelHits();
    fFillTree   = pset().FillTree();
    fTrackLabel	= pset().LabelTracks();
    fModelPath = pset().ModelPath();

      //ARICH RECO UTILS STUFF
      PDfile  =  pset().PD_file();
      up_n = pset().RefractiveIndex_UpstreamAerogel();
      up_pos = pset().Position_UpstreamAerogel();
      up_thick = pset().Thinkness_UpstreamAerogel();
      down_n = pset().RefractiveIndex_DownstreamAerogel();
      down_pos = pset().Position_DownstreamAerogel();
      down_thick = pset().Thickness_DownstreamAerogel();
      PDdarkrate = pset().PD_Darkrate();
      PDwin = pset().Trigger_window();
      PDfillfactor = pset().PD_FillFactor();
      PDzpos = pset().PD_Position();
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
    if(fFillTree) {
     art::ServiceHandle<art::TFileService const> tfs;
     fARICHTree = tfs->make<TTree>("ARICHRECO","event");
     fARICHTree->Branch("event_hist", &event_hist);
     fARICHTree->Branch("pdf_pion", &pdf_pion);
     fARICHTree->Branch("pdf_kaon", &pdf_kaon);
     fARICHTree->Branch("pdf_prot", &pdf_prot);
     fARICHTree->Branch("momenta", &mom);
     fARICHTree->Branch("event_id", &fEvtNum);
     }

    ArichUtils = new arichreco::ARICH_UTILS();
    rand_gen = new TRandom3(0);
    std::string source_path = getenv("CETPKG_SOURCE");
    TString PDfile_path = source_path + PDfile;
    ArichUtils->SetUpDet(PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile_path);
    ArichUtils->SetUpArich(up_n,down_n,up_pos,up_thick,down_pos,down_thick);
     
    Model = new tml::NeuralNet();
    Model->loadModel(source_path.append(fModelPath));
//    mf::LogError("ARICH NN path") <<"model path " << source_path.append(fModelPath);    

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

//	std::cout << "Momenta " << mom << std::endl;
	if (mom == 0) {
          mf::LogWarning("ARICHReco") << "Track 1 has zero momentum. Skipping.";
        }

        float finalx = posx + (1920 - posz) * px/pz;
        float finaly = posy + (1920 - posz) * py/pz;
	
	//std::cout << "vertex (" << posx << ", " << posy << ", " << posz << ") final pos ( "<< finalx << ", " << finaly << ")"<< std::endl;        	

	TVector3 dir_(px/mom,py/mom,pz/mom);
        TVector3 pos_(finalx/10.,finaly/10.,0.);  //in cm
	 
	int max_cluster=-1;
        int max_size = 0;


       for(int u = 0; u < (int)arich_clusters->size(); u++){
          int size = arich_clusters->at(u).NDigits();
          if(size > max_size){
             max_size = size;
             max_cluster = u;
           }
        }

	std::vector<std::pair<int,int>> digs = arich_clusters->at(max_cluster).Digits(); //cluster where the physics is 
	event_hist = *ArichUtils->DigsToHist(digs);
	std::vector<double> LL = ArichUtils->identifyParticle(&event_hist, mom, pos_, dir_);

	if(fFillTree){
	  std::vector<TH2D> pdfs = ArichUtils->GetPDF(mom, pos_, dir_);
	  pdf_pion = pdfs[0];
	  pdf_kaon = pdfs[1];
	  pdf_prot = pdfs[2];
	}

          rb::ArichID arich_id;
          arich_id.scoresLL = LL;
          arich_id.trackID = 1;
          arich_id.nhit = digs.size();

 	  at::Tensor tensor_event = TH2DToTensor(&event_hist);
 	  at::Tensor tensor_mom = at::full({1,1}, mom, at::kFloat);
 	  std::vector<at::Tensor> inputs = {tensor_event, tensor_mom};
	  at::Tensor pred = Model->predict(inputs); 

	  std::vector<double> temp;
	  auto accessor = pred.accessor<float, 2>();
	  for (int i = 0; i < accessor.size(0); ++i) {
            for (int j = 0; j < accessor.size(1); ++j) {
            	temp.push_back((double)accessor[i][j]);
	     }
	  }

          arich_id.scoresML = temp;
	
	ARICH->push_back(arich_id);	  
	 
	if(fFillTree)fARICHTree->Fill();
	    
	} // end if clusters     	 

	evt.put(std::move(ARICH));	   
  } // end produce 

} // namespace emph

DEFINE_ART_MODULE(emph::ARICHReco)
