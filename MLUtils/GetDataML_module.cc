////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module that dumps ARICH event displays, momenta and
///          pdg codes into a torch tensor file that python can read back
///          directly with torch.load()
/// \author  $Author: mdallolio $
///
/// Output (one pickled dict of tensors, N = number of saved events):
///
///   images  [N, H, W]  event display, bin content = intensity
///   momenta [N, 3]     px, py, pz of the sim track crossing the aerogels
///   pos     [N, 3]     x, y, z  of the same track
///   pdg     [N]        pdg code of the same track           (int64)
///   evtid   [N, 3]     run, subrun, event                   (int64)
///
/// python:
///   d  = torch.load("arich_ml.pt")        # weights_only=False on torch>=2.6
///   ds = torch.utils.data.TensorDataset(d["images"].unsqueeze(1).float(),
///                                       d["momenta"], d["pdg"])
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
#include "cetlib_except/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "Geometry/service/GeometryService.h"
#include "DetGeoMap/service/DetGeoMapService.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RawData/TRB3RawDigit.h"
#include "Simulation/Track.h"
#include "RecoBase/Track.h"
#include "RecoBase/ARICHCluster.h"

// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"

//TORCH
#include <torch/torch.h>

const float mproton = 0.93827;
const float mpion = 0.13957;
const float mkaon = 0.49368;

namespace torch {
  namespace jit {
    TORCH_API std::vector<char> pickle_save(const c10::IValue& ivalue);    }
}


namespace emph {  

  class GetDataML : public art::EDAnalyzer {
  public:
    explicit GetDataML(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~GetDataML();

    void beginRun(art::Run const& run); 
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void endJob();

  private:

    // fixed binning -> every event display is the same [H,W], so the whole
    // sample stacks into one [N,H,W] tensor
    static torch::Tensor HistToTensor(const TH2D& h);
    std::string ChunkName() const;
    void        WriteChunk();

    arichreco::ARICH_UTILS* ArichUtils;
    TTree* 	fARICHTree;    
 
    int         fEvtNum;
    std::string fARICHLabel;  
    std::string fSimTrackLabel;    
     std::string fRecoTrackLabel;


    bool fFillTree;
    
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    
    art::ServiceHandle<emph::geo::GeometryService> geom;

    std::vector<double> momenta;
    std::vector<double> pos;
    int pdg;

     double fdxArich;
    double fdyArich;
    double fAbsZAero1;
    double fAbsZPMTPlane;
    double fNeff;
    double fLeff;

    TH2D histo; 
    bool fHistInit;   // histo has been assigned at least once

    // --- torch dataset ---
    std::string  fTorchFile;
    torch::Dtype fImgDtype;
    unsigned int fEventsPerFile;   // 0 = everything in a single file

    std::vector<torch::Tensor> fImages;
    std::vector<torch::Tensor> fMomenta;
    std::vector<torch::Tensor> fPos;
    std::vector<torch::Tensor> fEvtIds;
    std::vector<int64_t>       fPdgs;
    std::vector<float>         fRadii;	

    unsigned int fChunk;
    unsigned int fNSaved;
    unsigned int fNClipped;

	
};

  //.......................................................................
  
  emph::GetDataML::GetDataML(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
 { 

    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fSimTrackLabel = std::string(pset.get<std::string >("SimTrackLabel"));
    fRecoTrackLabel = std::string(pset.get<std::string >("RecoTrackLabel"));
    fFillTree   = bool(pset.get<bool>("FillTree"));

    fTorchFile     = pset.get<std::string >("TorchFile", "arich_training.pt");
    fEventsPerFile = pset.get<unsigned int>("EventsPerFile", 0);

    // float32 keeps the bin content as is; uint8/int16 divide the memory
    // (and the file) by 4/2 when the intensities are small integer counts
    const std::string dtype = pset.get<std::string>("ImageDtype", "float32");
    if      (dtype == "float32") fImgDtype = torch::kFloat32;
    else if (dtype == "uint8")   fImgDtype = torch::kUInt8;
    else if (dtype == "int16")   fImgDtype = torch::kInt16;
    else throw cet::exception("GetDataML") << "unknown ImageDtype '" << dtype
					   << "', use float32, uint8 or int16\n";

    //ARICH RECO UTILS STUFF
    fEvtNum = 0;
    fHistInit = false;
    fChunk = 0;
    fNSaved = 0;
    fNClipped = 0;
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
   }
    ArichUtils = new arichreco::ARICH_UTILS();
}
//......................................................................
void emph::GetDataML::beginRun(art::Run const& run)
{

  fdxArich      = geom->Geo()->GetAerogelUS()->Pos()[0];
  fdyArich      = geom->Geo()->GetAerogelUS()->Pos()[1];
  fAbsZAero1    = geom->Geo()->GetAerogelUS()->Pos()[2];
  fAbsZPMTPlane = geom->Geo()->GetmPMTPlanePos()[2];

  double fMPMTz = geom->Geo()->GetmPMTPlanePos()[2];

  double fdz = (fMPMTz - fAbsZAero1);

  double n1 = geom->Geo()->GetAerogelUS()->RefractiveIdx();
  double n2 = geom->Geo()->GetAerogelDS()->RefractiveIdx();
  double t1 = geom->Geo()->GetAerogelUS()->Thickness() * 2.; // mm
  double t2 = geom->Geo()->GetAerogelDS()->Thickness() * 2.; // mm
  double zmid1 = 0.5 * t1;
  double zmid2 = t1 + 0.5 * t2;
  double w1 = t1 * (1. - 1. / (n1 * n1));
  double w2 = t2 * (1. - 1. / (n2 * n2));
  fNeff = (w1 * n1 + w2 * n2) / (w1 + w2);
  double meanEmissionDepth = (w1 * zmid1 + w2 * zmid2) / (w1 + w2);

  double mPMTHalfThick = geom->Geo()->GetmPMTPlaneThickness();
  double fDAir = fdz - 0.5 * t1 - t2 - mPMTHalfThick;

  fLeff = (t1 + t2 - meanEmissionDepth) + fNeff * fDAir;

  std::cout <<"GET DATA " << fNeff << " " << fLeff << std::endl;
}   

 
//......................................................................
// x -> columns, y -> rows, bin content -> intensity, overflows dropped
torch::Tensor emph::GetDataML::HistToTensor(const TH2D& h)
{
  const int nx = h.GetNbinsX();
  const int ny = h.GetNbinsY();

  auto t = torch::zeros({ny, nx}, torch::kFloat32);
  auto a = t.accessor<float,2>();
  for (int iy = 1; iy <= ny; ++iy)
    for (int ix = 1; ix <= nx; ++ix)
      a[iy-1][ix-1] = static_cast<float>(h.GetBinContent(ix, iy));

  return t;
}

//......................................................................
std::string emph::GetDataML::ChunkName() const
{
  if (fEventsPerFile == 0) return fTorchFile;

  std::string base = fTorchFile;
  const size_t dot = base.rfind(".pt");
  if (dot != std::string::npos && dot == base.size() - 3) base = base.substr(0, dot);

  return base + "_" + std::to_string(fChunk) + ".pt";
}

//......................................................................
void emph::GetDataML::WriteChunk()
{
  if (fImages.empty()) return;

  c10::Dict<std::string, torch::Tensor> data;
  data.insert("images",  torch::stack(fImages));                    // [N,H,W]
  data.insert("momenta", torch::stack(fMomenta));                   // [N,3]
  data.insert("pos",     torch::stack(fPos));                       // [N,3]
  data.insert("evtid",   torch::stack(fEvtIds));                    // [N,3]
  data.insert("pdg",     torch::tensor(fPdgs, torch::kInt64));      // [N]
  data.insert("radii",     torch::tensor(fRadii));      // [N]
  // jit::pickle_save (not torch::save) is what python's torch.load() understands
  const std::vector<char> bytes = torch::jit::pickle_save(c10::IValue(data));

  const std::string fname = ChunkName();
  std::ofstream fout(fname, std::ios::out | std::ios::binary);
  if (!fout) throw cet::exception("GetDataML") << "cannot open " << fname << " for writing\n";
  fout.write(bytes.data(), bytes.size());
  fout.close();

  std::cout << "GetDataML: wrote " << fImages.size() << " events of "
	    << fImages.front().size(0) << "x" << fImages.front().size(1)
	    << " to " << fname << std::endl;
  mf::LogInfo("GetDataML") << "wrote " << fImages.size() << " events to " << fname;

  ++fChunk;
  fImages.clear();
  fMomenta.clear();
  fPos.clear();
  fEvtIds.clear();
  fPdgs.clear();
  fRadii.clear();
}

//......................................................................
void emph::GetDataML::endJob()
{
  WriteChunk();

  std::cout << "GetDataML: " << fNSaved << " events in the torch dataset" << std::endl;
  if (fNClipped > 0)
    mf::LogWarning("GetDataML") << fNClipped << " event displays were clipped by the "
				<< "chosen ImageDtype, use float32 or int16 instead";
}

//......................................................................
void emph::GetDataML::analyze(const art::Event& evt)
 { 

      art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;	

      evt.getByLabel(fARICHLabel,arich_clusters);


      art::Handle<std::vector<sim::Track>> sim_tracks;
      evt.getByLabel(fSimTrackLabel,sim_tracks);
	
      art::Handle<std::vector<rb::Track>> TracksH;
      evt.getByLabel(fRecoTrackLabel,TracksH);
	
	++fEvtNum;

	// do not carry the previous event display over to this event
	if (fHistInit) histo.Reset();

	bool have_hist = false;

	for(int u = 0; u < (int)arich_clusters->size(); u++){
           	
	 if(arich_clusters->at(u).NDigits() < 4)continue;            
	
	      std::vector<std::pair<int,int>> digs = arich_clusters->at(u).Digits();  	

	      TH2D* event_hist = ArichUtils->DigsToHist(digs);	

	//if(trackid_pdg_mom.size() == 0) {     std::cout << "Zero sim track crossing aerogels but " << event_hist->Integral() << " hits" << std::endl;
	  //     for(auto track: *sim_tracks)std::cout << "track id " << track.GetTrackID() << " son of " << track.GetParentTrackID() << " pdg " <<  track.GetPId() << " mom " << track.GetPz() << " pos_Z " << track.GetZ() << std::endl;
	    // }  
	     histo = *event_hist;
	     fHistInit = true;
	     have_hist = true;
	     delete event_hist;	
	}

	std::vector<std::tuple<int,sim::Track>> trackid_track;

	for(int h=0; h < (int)sim_tracks->size(); h++){

	sim::Track track = sim_tracks->at(h);

	if(track.GetZ() < 1915  || track.GetZ() > 1955)continue;
	  auto it = std::find_if(trackid_track.begin(), trackid_track.end(), [track](const auto element){ 
		return std::get<0>(element) == track.GetTrackID();
		});
	if(it == trackid_track.end())trackid_track.push_back(std::make_tuple(track.GetTrackID(), track));
	else continue;	
	}

	if(trackid_track.size() > 1 || trackid_track.size() ==0 || (int)TracksH->size() ==0)return;
	else{	

	   sim::Track the_track = std::get<1>(trackid_track[0]); 
	   momenta.push_back(the_track.GetPx()/1e3);  
	   momenta.push_back(the_track.GetPy()/1e3);  
	   momenta.push_back(the_track.GetPz()/1e3);

	   pos.push_back(the_track.GetX());
	   pos.push_back(the_track.GetY());
	   pos.push_back(the_track.GetZ());
	   pdg = the_track.GetPId();	

        const rb::Track& track = TracksH->at(1);
        double posx = track.posARICH.X();
        double posy = track.posARICH.Y();
        double posz = track.posARICH.Z();

        double px = track.momARICH.X();
        double py = track.momARICH.Y();
        double pz = track.momARICH.Z();

        std::cout << Form("pdg %d, pos(%f,%f,%f) mom (%f, %f ,%f)", pdg, posx, posy,posz, px, py, pz) << std::endl;
	std::cout << Form("PZ res: %f, Z res:  %f", (momenta[2]-pz)/momenta[2],  (pos[2]-posz)/pos[2]) << std::endl;

	float track_crossing_x = posx + (fAbsZPMTPlane - posz) * px/pz - fdxArich;
        float track_crossing_y = posy + (fAbsZPMTPlane - posz) * py/pz - fdyArich;

	   if (have_hist) {

	     torch::Tensor img = HistToTensor(histo);

	     if (fImgDtype != torch::kFloat32) {
	       const double maxval = (fImgDtype == torch::kUInt8) ? 255. : 32767.;
	       if (img.max().item<float>() > maxval) {
		 ++fNClipped;
		 img = img.clamp(0., maxval);
	       }
	       img = img.to(fImgDtype);
	     }

	    double trkMom = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
	    double mass =0;
	    if(abs(pdg) == 211)mass= mpion;
	    else if(abs(pdg) == 321)mass= mkaon;
	    else if(abs(pdg) == 2212)mass= mproton;
	    else return;	

	    double beta = trkMom / std::sqrt(trkMom * trkMom + mass * mass);
    	    double sin2theta_air = fNeff * fNeff - 1. / (beta * beta);
    	    double theta_air = std::asin(std::sqrt(sin2theta_air));
    	    double Rout = fLeff * std::tan(theta_air);

	    std::cout << Form("N %f L %f", fNeff, fLeff) << std::endl;
	    std::cout << Form("beta %f radii %f", beta, Rout )<< std::endl;

	     fImages.push_back(img);

	     fMomenta.push_back(torch::tensor(std::vector<float>{(float)the_track.GetPx(),
								(float)the_track.GetPy(),
								(float)the_track.GetPz()},
					      torch::kFloat32));
	     
	     fPos.push_back(torch::tensor(std::vector<float>{(float)track_crossing_x,
							     (float)track_crossing_y,
							     (float)fAbsZPMTPlane},
					  torch::kFloat32));

	     fEvtIds.push_back(torch::tensor(std::vector<int64_t>{(int64_t)evt.run(),
								 (int64_t)evt.subRun(),
								 (int64_t)evt.event()},
					     torch::kInt64));

	     fPdgs.push_back((int64_t)pdg);
	     fRadii.push_back((float)Rout);
	     ++fNSaved;

	     if (fEventsPerFile > 0 && fImages.size() >= fEventsPerFile) WriteChunk();
	   }
  	  	  
 	   if (fFillTree) fARICHTree->Fill();
       }
	momenta.clear();
	pos.clear(); 
 } // end analyze

}
DEFINE_ART_MODULE(emph::GetDataML)
