////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create BACkovHit objects from raw data and 
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
#include "RecoBase/BACkovHit.h"
#include "RecoBase/ADC.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class BACkovHitReco : public art::EDProducer {
  public:
    explicit BACkovHitReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~BACkovHitReco();
    
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
    void GetBACkovHit(art::Handle< std::vector<emph::rawdata::WaveForm> > &,std::unique_ptr<std::vector<rb::BACkovHit>> & BACkovHits);
    
    emph::cmap::ChannelMap* fChannelMap;
	 runhist::RunHistory* fRunHistory;
    int mom;
    std::vector<std::vector<int>> BACkov_signal;
    std::vector<std::vector<int>> PID_table; //in the form {e,mu,pi,k,p} w/ {1,1,0,0,0} being e/mu are possible particles
    int pid_num;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    int event;
    TH1F*       fBACkovChargeHist[6];

  };

  //.......................................................................
  
  BACkovHitReco::BACkovHitReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

    this->produces< std::vector<rb::BACkovHit>>();

    this->reconfigure(pset);

  }

  //......................................................................
  
  BACkovHitReco::~BACkovHitReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void BACkovHitReco::reconfigure(const fhicl::ParameterSet& pset)
  {
    mom = pset.get<int>("momentum",0);
  }

  //......................................................................
  //
  void BACkovHitReco::beginRun(art::Run& run)
  {
    // initialize channel map
    fChannelMap = new emph::cmap::ChannelMap();
	 fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
  }

  //......................................................................

  void BACkovHitReco::beginJob()
  {
    fNEvents=0;

    art::ServiceHandle<art::TFileService> tfs;
    char hname[64];
    for (int i=0; i<=5; ++i) {
      sprintf(hname,"BACkovQ_%d",i);
      if(i!=5) fBACkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge BACkov Channel %i",i),260,-1,25);
      else fBACkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge BACkov Channel %i",i),410,-2,80);
      fBACkovChargeHist[i]->GetXaxis()->SetTitle("Charge (pC)");
      fBACkovChargeHist[i]->GetYaxis()->SetTitle("Number of Events"); 
    }

    std::cout<<"**************************************************"<<std::endl;
    std::cout<< "Beam Configuration: "<<mom<<" GeV/c"<<std::endl;
    std::cout<<"**************************************************"<<std::endl;

    //Initialize truth table for checking BACkov signals
    
    if(mom==4){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1},{1,0,0},{0,0,0}}); // {e/mu/pi,k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,0,0},{0,0,1,0,0},{0,0,0,0,1}}); // {e/mu/pi,k,p}
      pid_num=3;
    }
    else if(mom==8){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1},{1,0,0}}); // {e/mu/pi/k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,1,0},{0,0,0,0,1}}); // {e/mu/pi/k,p}
      pid_num=2;
    }
    else if(mom==12){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1},{1,1,0}}); // {e/mu/pi/k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,1,0},{0,0,0,0,1}}); // {e/mu/pi/k,p}
      pid_num=2;
    }
    else if(mom==120){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1}}); // {e/mu/pi/k/p}
      PID_table.insert(PID_table.end(),{{1,1,1,1,1}}); // {e/mu/pi/k/p}
      pid_num=1;
    }
    else {std::cout<<"Error: Invalid Beam Momentum for BACkov signals"<<std::endl; exit(0);}
  }

  //......................................................................
  
  void BACkovHitReco::endJob()
  {
  }
  
    //......................................................................
  
  void BACkovHitReco::GetBACkovHit(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH, std::unique_ptr<std::vector<rb::BACkovHit>> & BACkovHits)
  {
    //Create empty vectors to hold charge values
    float Qvec[6];

    //int BACnchan = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
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
            float Q = wvr.BACkovCharge(wvfm_ptr);
            fBACkovChargeHist[detchan]->Fill(Q);
            Qvec[detchan]=Q;
	  }  
    }
    float low_q = Qvec[0]+Qvec[1]+Qvec[2];
    float mid_q = Qvec[3]+Qvec[4];
    float high_q = Qvec[5];
   
    //Checking BACkov Hits
    
    std::vector<bool> BACkov_Result;
    bool PID_prob[5]={0,0,0,0,0};


    if (high_q>1) BACkov_Result.push_back(1);
    else BACkov_Result.push_back(0);
    if (mid_q>1) BACkov_Result.push_back(1);
    else BACkov_Result.push_back(0);
    if (low_q>1) BACkov_Result.push_back(1);
    else BACkov_Result.push_back(0);

    for (int k=0; k<pid_num; ++k){
      int result_flag=0;
      for (int l=0; l<3; ++l){
        if (BACkov_Result[l]==BACkov_signal[k][l]) result_flag+=1;
      }
      if(result_flag==3){
	for(int j=0; j<5; ++j) PID_prob[j]=PID_table[k][j];
      }
    }
    
    //Create object and store BACkov Charge and PID results
    rb::BACkovHit BACkovHit;
    BACkovHit.SetCharge(Qvec);
    BACkovHit.SetPID(PID_prob);
    BACkovHits->push_back(BACkovHit);
  }
 
  //......................................................................
  void BACkovHitReco::produce(art::Event& evt)
  { 
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();

    std::string labelStr = "raw:BACkov";
    art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;

    std::unique_ptr<std::vector<rb::BACkovHit> > BACkovHitv(new std::vector<rb::BACkovHit>);

    try {
	evt.getByLabel(labelStr, wfHandle);

	if (!wfHandle->empty()) {
	  GetBACkovHit(wfHandle,  BACkovHitv);
	}
      }
      catch(...) {

      }
      evt.put(std::move(BACkovHitv));
  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::BACkovHitReco)