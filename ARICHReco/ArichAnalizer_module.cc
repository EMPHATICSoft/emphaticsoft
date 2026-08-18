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
#include "TRandom3.h"

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

#include"TH1D.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RawData/TRB3RawDigit.h"
#include "Simulation/Track.h"
#include "Simulation/ARICHHit.h"
#include "RecoBase/ARICHCluster.h"
// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"


namespace emph {  

  class ArichAnalizer: public art::EDAnalyzer {
  public:
    explicit ArichAnalizer(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~ArichAnalizer();
    
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    void CheckDigitization(std::vector<sim::ARICHHit> sim_hits_vector, std::vector<emph::rawdata::TRB3RawDigit> digitized_hits_vector);
   // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void endJob();
	

  private:

    arichreco::ARICH_UTILS* ArichUtils;
    TTree* 	fARICHTree;    
 
    int         fEvtNum;
    std::string fARICHLabel;
    std::string fRawARICHLabel; 
    std::string fSimLabel;    

    bool fFillTree;
    
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;    
    art::ServiceHandle<emph::geo::GeometryService> geom;
    TRandom3* rand_gen;


    std::vector<double> h_wl;
    std::vector<double> h_wl_survived;
    int nhits_dig; 
    int nhits_sim;
    TH2D* bin_display_npmt;
    TH2D* bin_display_npin;

};

  //.......................................................................
  
  emph::ArichAnalizer::ArichAnalizer(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
 { 

    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fRawARICHLabel = std::string(pset.get<std::string >("LabelRawHits"));
    fSimLabel = std::string(pset.get<std::string >("SimLabel"));
    fFillTree   = bool(pset.get<bool>("FillTree"));
    //ARICH RECO UTILS STUFF
    fEvtNum = 0;
    bin_display_npmt = new TH2D("bin_display_npmt","bin_display_npmt", 24,-12,12,24,-12,-12);    
    bin_display_npin = new TH2D("bin_display_npin","bin_display_npin", 24,-12,12,24,-12,-12);
  }	
  //......................................................................
 
  emph::ArichAnalizer::~ArichAnalizer()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void emph::ArichAnalizer::beginJob()
  {    
    if (fFillTree){
    art::ServiceHandle<art::TFileService const> tfs;
    fARICHTree = tfs->make<TTree>("ARICHRECO","event");
    fARICHTree->Branch("h_wl", &h_wl);
    fARICHTree->Branch("h_wl_survived", &h_wl_survived);
    fARICHTree->Branch("nhits_sim", &nhits_sim);
    fARICHTree->Branch("nhits_dig", &nhits_dig);

  }
    ArichUtils = new arichreco::ARICH_UTILS();

    rand_gen =  new TRandom3(0);
}
    
//......................................................................
void emph::ArichAnalizer::endJob()
{
}

void emph::ArichAnalizer::CheckDigitization(std::vector<sim::ARICHHit> sim_hits_vector, std::vector<emph::rawdata::TRB3RawDigit> digitized_hits_vector){

  for(size_t i = 0; i < sim_hits_vector.size(); i++){

         sim::ARICHHit arichhit = sim_hits_vector[i];
         int blockID = arichhit.GetBlockNumber();
         const arich_util::PMT& mpmt= geom->Geo()->FindPMTByBlockNumber(blockID);
         double wavelength =  arichhit.GetWavelength(); //nm
	 h_wl.push_back(wavelength);

	 if(wavelength != 0.){ //wl == 0 is the central blimp 
           if(!mpmt.ifDet(wavelength/1e6))continue; //check if PMT can detected that photon                   
	   else if( mpmt.ifDet(wavelength/1e6) && rand_gen->Uniform() < 0.05)continue;
         }

	 h_wl_survived.push_back(wavelength);
    }
	nhits_sim = h_wl_survived.size();    
	nhits_dig = digitized_hits_vector.size();

   for(size_t j = 0; j < digitized_hits_vector.size(); j++){

	const emph::rawdata::TRB3RawDigit& trb3 = digitized_hits_vector[j];
        if (trb3.GetChannel()==0) continue;
          //
        int fpga = trb3.GetBoardId();
        int ech = trb3.GetChannel();
        emph::cmap::EChannel echan(emph::cmap::TRB3,fpga,ech);	
	emph::cmap::DChannel dchan = cmap->DetChan(echan);
        int pmt = dchan.HiLo();
        int dch = dchan.Channel(); 
	
   }

}
//......................................................................
void emph::ArichAnalizer::analyze(const art::Event& evt)
 { 

       auto arich_dig = evt.getHandle<std::vector<emph::rawdata::TRB3RawDigit> >(fRawARICHLabel);
       std::vector<emph::rawdata::TRB3RawDigit> ArichDigs(*arich_dig);
       auto arich_sim = evt.getHandle<std::vector<sim::ARICHHit>>(fSimLabel);
       std::vector<sim::ARICHHit> arichHits(*arich_sim);
       
       CheckDigitization(arichHits,ArichDigs);

        art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;	
 
        evt.getByLabel(fARICHLabel,arich_clusters);

	art::Handle<std::vector<sim::Track>> sim_tracks;
	evt.getByLabel(fSimLabel,sim_tracks);

	if(fFillTree){
	  fARICHTree->Fill();
	  h_wl.clear();
	  h_wl_survived.clear();
	}
	//std::cout << "Target USZ "<< geom->Geo()->TargetUSZPos() << std::endl;
	//std::cout << "Target DSZ "<< geom->Geo()->TargetDSZPos() << std::endl;

	//from phace1c.gdml, aerogels are between 1915 - 1955 mm 

        //std::cout << "FOUND " << (int)arich_clusters->size() << " clusters" << std::endl;
	//std::cout << "FOUND " << (int)sim_tracks->size() << " sim tracks" << std::endl;

 } // end analyze

}
DEFINE_ART_MODULE(emph::ArichAnalizer)
