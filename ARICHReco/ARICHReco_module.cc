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
    fTrackLabel	= std::string(pset.get<std::string>("LabelTracks"));
    fModelPath = std::string(pset.get<std::string>("ModelPath"));

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
    if(fFillTree) {
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
    }

    ArichUtils = new arichreco::ARICH_UTILS();
    std::string source_path = getenv("CETPKG_SOURCE");
    TString PDfile_path = source_path + PDfile;
    ArichUtils->SetUpDet(PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile_path);
    ArichUtils->SetUpArich(up_n,down_n,up_pos,up_thick,down_pos,down_thick);
    
    Model = new tml::NeuralNet();
    Model->loadModel(source_path.append(fModelPath));
//    mf::LogError("ARICH NN path") <<"model path " << source_path.append(fModelPath);    

 /*  fARICHTree->Branch("BINS_PDF_pion", &bins_pdf_pion);
    fARICHTree->Branch("VALS_PDF_pion", &vals_pdf_pion);
    fARICHTree->Branch("BINS_PDF_kaon", &bins_pdf_kaon);
    fARICHTree->Branch("VALS_PDF_kaon", &vals_pdf_kaon);
    fARICHTree->Branch("BINS_PDF_prot", &bins_pdf_prot);
    fARICHTree->Branch("VALS_PDF_prot", &vals_pdf_prot);
*/
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
    std::unique_ptr<std::vector<rb::ArichID>> ARICH_LL(new std::vector<rb::ArichID>);

    art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;	
    art::Handle<std::vector<rb::Track>> TracksH;

    evt.getByLabel(fARICHLabel,arich_clusters);

    evt.getByLabel(fTrackLabel,TracksH);  

    if( (int)arich_clusters->size() != 0 && (int)TracksH->size() !=0){

      for(int i = 0; i < (int)TracksH->size(); i++){
        rb::Track track = TracksH->at(i);

        double posx = track.Vtx()[0];
        double posy = track.Vtx()[1];
        double posz = track.Vtx()[2];

        double px = track.P()[0];
        double py = track.P()[1];
        double pz = track.P()[2];

        float mom = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));
        if (mom == 0) {
          mf::LogWarning("ARICHReco") << "Track " << i << " has zero momentum. Skipping.";
          continue;
        }

        float finalx = posx + (192.0 - posz) * track.P()[0]/mom;
        float finaly = posy + (192.0 - posz) * track.P()[1]/mom;

        TVector3 dir_(px/mom,py/mom,pz/mom);
        TVector3 pos_(finalx/10,finaly/10,0.);  //in cm
        for(int k = 0; k < (int)arich_clusters->size(); k++)
        {
          if(arich_clusters->at(k).NDigits() < 3) continue;
          std::vector<std::pair<int,int>> digs = arich_clusters->at(k).Digits();
          TH2D* event_hist = ArichUtils->DigsToHist(digs);
          std::vector<double> LL = ArichUtils->identifyParticle(event_hist, mom, pos_, dir_);
          delete event_hist;

          rb::ArichID arich_id;
          arich_id.scores = LL;
          arich_id.trackID = i;
          arich_id.nhit = digs.size();

          ARICH_LL->push_back(arich_id);
        }
      } //end track loop
    } // end if clusters     	 

	else if( (int)arich_clusters->size() != 0 ) {
	  for(int k = 0; k < (int)arich_clusters->size(); k++) {
      if(arich_clusters->at(k).NDigits() < 3) continue;

      std::vector<std::pair<int,int>> digs = arich_clusters->at(k).Digits();
      TH2D* event_hist = ArichUtils->DigsToHist(digs);

      at::Tensor tensor_event = TH2DToTensor(event_hist);
		  at::Tensor tensor_mom = at::full({1,1}, mom, at::kFloat);
 	
		  std::vector<at::Tensor> inputs = {tensor_event, tensor_mom};

		  at::Tensor pred = Model->predict(inputs); 

      mf::LogError("tensor pred") <<"pred " <<  pred << std::endl;
      delete event_hist;
		}
	}

	momenta.clear();
	dir.clear();
	pos.clear();	
	evt.put(std::move(ARICH_LL));	   
    
  } // end produce 
} // namespace emph

DEFINE_ART_MODULE(emph::ARICHReco)
