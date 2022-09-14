////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create GasCkovHit objects from raw data and 
///          store them in the art output file
/// \author  $Author: jmirabit $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/WaveForm.h"
#include "RecoBase/GasCkovHit.h"
#include "RecoBase/ADC.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class GasCkovHitReco : public art::EDProducer {
  public:
    explicit GasCkovHitReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~GasCkovHitReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginRun(art::Run& run);
    //void endRun(art::Run const&);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    void GetGasCkovHit(art::Handle< std::vector<rawdata::WaveForm> > &,std::unique_ptr<std::vector<rb::GasCkovHit>> & GasCkovHits);
    
    emph::cmap::ChannelMap* fChannelMap;
	 runhist::RunHistory* fRunHistory;
    int mom;
    std::vector<std::vector<int>> GasCkov_signal;
    std::vector<std::vector<int>> PID_table; //in the form {e,mu,pi,k,p} w/ {1,1,0,0,0} being e/mu are possible particles
    int pid_num;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    int event;
    TH1F*       fGasCkovChargeHist[3];

  };

  //.......................................................................
  
  GasCkovHitReco::GasCkovHitReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

    this->produces< std::vector<rb::GasCkovHit>>();

    this->reconfigure(pset);

  }

  //......................................................................
  
  GasCkovHitReco::~GasCkovHitReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void GasCkovHitReco::reconfigure(const fhicl::ParameterSet& pset)
  {
    
    mom = pset.get<int>("momentum",0);
    
  }

  //......................................................................
  //
  void GasCkovHitReco::beginRun(art::Run& run)
  {
    // initialize channel map 
    fChannelMap = new emph::cmap::ChannelMap();
	 fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
  }

  //......................................................................
  
  void GasCkovHitReco::beginJob()
  {
    fNEvents=0;

    art::ServiceHandle<art::TFileService> tfs;
    char hname[64];
    for (int i=0; i<=2; ++i) {
      sprintf(hname,"GasCkovQ_%d",i);
      if(i==1) fGasCkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge GasCkov Channel %i",i),250,-10,50);
      else fGasCkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge GasCkov Channel %i",i),250,-10,100);
      fGasCkovChargeHist[i]->GetXaxis()->SetTitle("Charge (pC)");
      fGasCkovChargeHist[i]->GetYaxis()->SetTitle("Number of Events"); 
    }

    std::cout<<"**************************************************"<<std::endl;
    std::cout<< "Beam Configuration: "<<mom<<" GeV/c"<<std::endl;
    std::cout<<"**************************************************"<<std::endl;

    //Initialize truth table for checking GasCkov signals
    
    if(mom==2 || mom==4){
      GasCkov_signal.insert(GasCkov_signal.end(),{{1,0,1},{0,0,0}}); // {e,mu/pi/k/p}
      PID_table.insert(PID_table.end(),{{1,0,0,0,0},{0,1,1,1,1}}); // {e,mu/pi/k/p}
      pid_num=2;
    }
    else if(mom==8){
      GasCkov_signal.insert(GasCkov_signal.end(),{{1,0,1},{1,1,0},{0,0,0}}); // {e/mu,pi,k/p}
      PID_table.insert(PID_table.end(),{{1,1,0,0,0},{0,0,1,0,0},{0,0,0,1,1}}); // {e/mu,pi,k/p}
      pid_num=3;
    }
    else if(mom==12){
      GasCkov_signal.insert(GasCkov_signal.end(),{{1,0,1},{0,1,0},{0,0,0}}); // {e/mu,pi,k/p}
      PID_table.insert(PID_table.end(),{{1,1,0,0,0},{0,0,1,0,0},{0,0,0,1,1}}); // {e/mu,pi,k/p}
      pid_num=3;
    }
    else if(mom==20){
      GasCkov_signal.insert(GasCkov_signal.end(),{{1,0,1},{0,0,1},{0,1,0},{0,0,0}}); // {e/mu,pi,k,p}
      PID_table.insert(PID_table.end(),{{1,1,0,0,0},{0,0,1,0,0},{0,0,0,1,0},{0,0,0,0,1}}); // {e/mu,pi,k,p}
      pid_num=4;
    }
    else if(mom==31){
      GasCkov_signal.insert(GasCkov_signal.end(),{{1,0,1},{0,0,1},{0,0,0}}); // {e/mu,pi/k,p}
      PID_table.insert(PID_table.end(),{{1,1,0,0,0},{0,0,1,1,0},{0,0,0,0,1}}); // {e/mu,pi/k,p}
      pid_num=4;
    }
    else if(mom==60){
      GasCkov_signal.insert(GasCkov_signal.end(),{{1,0,1},{1,1,0},{0,0,0}}); // {e/mu/pi,k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,0,0},{0,0,0,1,0},{0,0,0,0,1}}); // {e/mu/pi,k,p}
      pid_num=4;
    }
    //Abitrary assignment for 120 GeV which should all be protons
    else if(mom==120){
      GasCkov_signal.insert(GasCkov_signal.end(),{{0,0,0}}); // {e/mu/pi/k/p}
      PID_table.insert(PID_table.end(),{{0,0,0,0,1}}); // {e/mu/pi/k/p}
      pid_num=1;
    }
    else {std::cout<<"Error: Invalid Beam Momentum for GasCkov signals"<<std::endl; exit(0);}
  }

  //......................................................................
  
  void GasCkovHitReco::endJob()
  {
  }
  
    //......................................................................
  
  void GasCkovHitReco::GetGasCkovHit(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH, std::unique_ptr<std::vector<rb::GasCkovHit>> & GasCkovHits)
  {
    //Create empty vectors to hold charge values
    float Qvec[3];

    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);
    event = fNEvents;
    if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm wvfm = (*wvfmH)[idx];
	    const rawdata::WaveForm* wvfm_ptr = &wvfm; 
	    const rb::ADC wvr;
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
            echan.SetBoard(board);
            echan.SetChannel(chan);
            emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
            float Q = wvr.Charge(wvfm_ptr);
            fGasCkovChargeHist[detchan]->Fill(Q);
            Qvec[detchan]=Q;
	  }  
    }
   
    //Checking GasCkov Hits
    
    std::vector<bool> GasCkov_Result;
    bool PID_prob[5]={0,0,0,0,0};


    if (Qvec[0]>10) GasCkov_Result.push_back(1);
    else GasCkov_Result.push_back(0);
    if (Qvec[1]>5) GasCkov_Result.push_back(1);
    else GasCkov_Result.push_back(0);
    if (Qvec[2]>40) GasCkov_Result.push_back(1);
    else GasCkov_Result.push_back(0);

    for (int k=0; k<pid_num; ++k){
      int result_flag=0;
      for (int l=0; l<3; ++l){
        if (GasCkov_Result[l]==GasCkov_signal[k][l]) result_flag+=1;
      }
      if(result_flag==3){
	for(int j=0; j<5; ++j) PID_prob[j]=PID_table[k][j];
      }
    }
    
    //Create object and store GasCkov Charge and PID results
    rb::GasCkovHit GasCkovHit;
    GasCkovHit.SetCharge(Qvec);
    GasCkovHit.SetPID(PID_prob);
    GasCkovHits->push_back(GasCkovHit);
  }
 
  //......................................................................
  void GasCkovHitReco::produce(art::Event& evt)
  { 
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();

    std::string labelStr = "raw:GasCkov";
    art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;

    std::unique_ptr<std::vector<rb::GasCkovHit> > GasCkovHitv(new std::vector<rb::GasCkovHit>);

    try {
	evt.getByLabel(labelStr, wfHandle);

	if (!wfHandle->empty()) {
	  GetGasCkovHit(wfHandle,  GasCkovHitv);
	}
      }
      catch(...) {

      }
      evt.put(std::move(GasCkovHitv));
  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::GasCkovHitReco)
