////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create BACkovHit objects from raw data and 
///          store them in the art output file
/// \author  $Author: jmirabit $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TProfile.h"

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
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/WaveForm.h"
#include "RecoBase/Spill.h"
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
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginSubRun(art::SubRun &sr);
    void endJob();
    
  private:
    void GetBACkovHit(art::Handle< std::vector<rb::ADC> > &,std::unique_ptr<std::vector<rb::BACkovHit>> & BACkovHits);
    void FillWaveforms(art::Handle< std::vector<emph::rawdata::WaveForm> > &);

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    
    int mom; // This should really be pulled from SpillInfo instead of set in the fcl.
    std::vector<std::vector<int>> BACkov_signal;
    std::vector<std::vector<int>> PID_table; //in the form {e,mu,pi,k,p} w/ {1,1,0,0,0} being e/mu are possible particles
    int pid_num;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    int event;
    TH1F*       fBACkovChargeHist[6];
    TH2F*       fBACkovChargeTimeHist[6];
 
    static constexpr int kNChan   = 6;    ///< BACkov channels
    static constexpr int kMaxSamp = 128;  ///< max V1720 samples we store (readout is 108)

    bool        fDumpWaveforms;  ///< master switch for everything below
    bool        fFillTree;     ///< also fill a per-event tree with all 6 waveforms

    TProfile*   fBACkovAvgWvfm[kNChan];  ///< mean waveform shape per channel
    TTree*      fWvfmTree;
    int         fWRun, fWSubrun, fWEvent, fWNSamp;
    float       fWWvfm[kNChan][kMaxSamp];
    float       fWBaseline[kNChan], fWBLWidth[kNChan];
    int         fWPeakADC[kNChan], fWPeakTDC[kNChan];
    float       fQtree[kNChan], fTtree[kNChan];  ///< the charges/times used for the PID above
  };

  //.......................................................................
  
  BACkovHitReco::BACkovHitReco(fhicl::ParameterSet const& pset)
  : EDProducer(pset),
    fDumpWaveforms (pset.get<bool>("DumpWaveforms",false)),
    fFillTree    (pset.get<bool>("FillTree",false)),
    fWvfmTree(0)
  {

    this->produces< std::vector<rb::BACkovHit>>();

  }

  //......................................................................
  
  BACkovHitReco::~BACkovHitReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }
  //......................................................................
  void BACkovHitReco::beginJob()
  {
    fNEvents=0;

    art::ServiceHandle<art::TFileService> tfs;
    char hname[64];
    for (int i=0; i<=5; ++i) {
        sprintf(hname,"BACkovQ_%d",i);
        if(i!=3 && i!=5) fBACkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge BACkov Channel %i",i),100,-1,10);
        else if (i!=5) fBACkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge BACkov Channel %i",i),150,-1,25);
        else fBACkovChargeHist[i] = tfs->make<TH1F>(hname,Form("Charge BACkov Channel %i",i),400,-2,80);
        fBACkovChargeHist[i]->GetXaxis()->SetTitle("Charge (pC)");
        fBACkovChargeHist[i]->GetYaxis()->SetTitle("Number of Events"); 
    }
    for (int i=0; i<=5; ++i) {
        sprintf(hname,"BACkovQT_%d",i);
        if(i!=3 && i!=5) fBACkovChargeTimeHist[i] = tfs->make<TH2F>(hname,Form("Charge vs. Time BACkov Channel %i",i),108,0,432,100,-1,10);
        else if (i!=5) fBACkovChargeTimeHist[i] = tfs->make<TH2F>(hname,Form("Charge vs. Time BACkov Channel %i",i),108,0,432,150,-1,25);
        else fBACkovChargeTimeHist[i] = tfs->make<TH2F>(hname,Form("Charge vs. Time BACkov Channel %i",i),108,0,432,400,-2,80);
        fBACkovChargeTimeHist[i]->GetXaxis()->SetTitle("Time (ns)");
        fBACkovChargeTimeHist[i]->GetYaxis()->SetTitle("Charge (pC)");
    }

      for (int i=0; i<kNChan; ++i) {
        sprintf(hname,"BACkovAvgWvfm_%d",i);
        fBACkovAvgWvfm[i] = tfs->make<TProfile>(hname,Form("Mean waveform, BACkov channel %i",i),
                                                108,0,108);
        fBACkovAvgWvfm[i]->GetXaxis()->SetTitle("sample (4 ns each)");
        fBACkovAvgWvfm[i]->GetYaxis()->SetTitle("ADC");
      }
	if(fFillTree){		
        fWvfmTree = tfs->make<TTree>("BACkovWvfmTree","BACkov raw waveforms");
        fWvfmTree->Branch("run",     &fWRun,    "run/I");
        fWvfmTree->Branch("subrun",  &fWSubrun, "subrun/I");
        fWvfmTree->Branch("event",   &fWEvent,  "event/I");
        fWvfmTree->Branch("nsamp",   &fWNSamp,  "nsamp/I");
        fWvfmTree->Branch("wvfm",    fWWvfm,    Form("wvfm[%d][%d]/F",kNChan,kMaxSamp));
        fWvfmTree->Branch("baseline",fWBaseline,Form("baseline[%d]/F",kNChan));
        fWvfmTree->Branch("blwidth", fWBLWidth, Form("blwidth[%d]/F",kNChan));
        fWvfmTree->Branch("peakadc", fWPeakADC, Form("peakadc[%d]/I",kNChan));
        fWvfmTree->Branch("peaktdc", fWPeakTDC, Form("peaktdc[%d]/I",kNChan));
        fWvfmTree->Branch("charge",  fQtree,    Form("charge[%d]/F",kNChan));
        fWvfmTree->Branch("time",    fTtree,    Form("time[%d]/F",kNChan));
     }  
}

  //......................................................................

  void BACkovHitReco::beginSubRun(art::SubRun& sr)
  {
      art::Handle<rb::Spill> spillHandle;
      try {
          sr.getByLabel("spillinfo",spillHandle);

          mom = spillHandle->Momentum();
      }
      catch(...) {
          std::cout << "No spill info object found!  Aborting..." << std::endl;
          std::abort();
      }

    std::cout<<"**************************************************"<<std::endl;
    std::cout<< "Beam Configuration: "<<mom<<" GeV/c"<<std::endl;
    std::cout<<"**************************************************"<<std::endl;

    //Initialize truth table for checking BACkov signals
    
    if(abs(mom)==4){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1},{1,0,0},{0,0,0}}); // {e/mu/pi,k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,0,0},{0,0,1,0,0},{0,0,0,0,1}}); // {e/mu/pi,k,p}
      pid_num=3;
    }
    else if(abs(mom)==8){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1},{1,0,0}}); // {e/mu/pi/k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,1,0},{0,0,0,0,1}}); // {e/mu/pi/k,p}
      pid_num=2;
    }
    else if(abs(mom)==12){
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1},{1,1,0}}); // {e/mu/pi/k,p}
      PID_table.insert(PID_table.end(),{{1,1,1,1,0},{0,0,0,0,1}}); // {e/mu/pi/k,p}
      pid_num=2;
    }
    else{
      BACkov_signal.insert(BACkov_signal.end(),{{1,1,1}}); // {e/mu/pi/k/p}
      PID_table.insert(PID_table.end(),{{1,1,1,1,1}}); // {e/mu/pi/k/p}
      pid_num=1;
    }
  }

  //......................................................................
  
  void BACkovHitReco::endJob()
  {
  }
  
    //......................................................................
  
  void BACkovHitReco::GetBACkovHit(art::Handle< std::vector<rb::ADC> > & adcH, std::unique_ptr<std::vector<rb::BACkovHit>> & BACkovHits)
  {
    //Create empty vectors to hold charge and time values
    float Qvec[6];
    float Tvec[6];

    //int BACnchan = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);
    event = fNEvents;
    if (!adcH->empty()) {
	  for (size_t idx=0; idx < adcH->size(); ++idx) {
	    const rb::ADC& ADC = (*adcH)[idx];
	    int chan = ADC.Chan();
        int board = ADC.Board();
        echan.SetBoard(board);
        echan.SetChannel(chan);
        emph::cmap::DChannel dchan = cmap->DetChan(echan);
        int detchan = dchan.Channel();
        //if (detchan!=5) wvr.CalcFitCharge(wvfm);

        float q = ADC.Charge();
        float t = ADC.Time();
        fBACkovChargeHist[detchan]->Fill(q);
        fBACkovChargeTimeHist[detchan]->Fill(t,q);
        Qvec[detchan]=q;
        Tvec[detchan]=t;
	  }  
    }
    float low_q = Qvec[0]+Qvec[1]+Qvec[2];
    float mid_q = Qvec[3]+Qvec[4];
    float high_q = Qvec[5];
   
    //Checking BACkov Hits
    
    std::vector<bool> BACkov_Result;
    bool PID_prob[5]={0,0,0,0,0};


    if (high_q>4) BACkov_Result.push_back(1);
    else BACkov_Result.push_back(0);
    if (mid_q>2) BACkov_Result.push_back(1);
    else BACkov_Result.push_back(0);
    if (low_q>2) BACkov_Result.push_back(1);
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
   
    for (int i=0; i<kNChan; ++i) { fQtree[i] = Qvec[i]; fTtree[i] = Tvec[i]; }
 
    //Create object and store BACkov Charge and PID results
    rb::BACkovHit BACkovHit;
    BACkovHit.SetCharge(Qvec);
    BACkovHit.SetTime(Tvec);
    BACkovHit.SetPID(PID_prob);
    BACkovHits->push_back(BACkovHit);
  }
  
   void BACkovHitReco::FillWaveforms(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH)
  {
    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);

    for (int i=0; i<kNChan; ++i) {
      fWBaseline[i] = -999.; fWBLWidth[i] = -999.;
      fWPeakADC[i]  = -999;  fWPeakTDC[i] = -999;
      for (int j=0; j<kMaxSamp; ++j) fWWvfm[i][j] = 0.;
    }
    fWNSamp = 0;

    art::ServiceHandle<art::TFileService> tfs;
    char hname[128];

    for (size_t idx=0; idx < wvfmH->size(); ++idx) {
      const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
      echan.SetBoard(wvfm.Board());
      echan.SetChannel(wvfm.Channel());
      emph::cmap::DChannel dchan = cmap->DetChan(echan);
      int detchan = dchan.Channel();
      if (detchan < 0 || detchan >= kNChan) continue;

      int nsamp = int(wvfm.NADC());
      if (nsamp > kMaxSamp) nsamp = kMaxSamp;
      if (nsamp > fWNSamp) fWNSamp = nsamp;

      fWBaseline[detchan] = wvfm.Baseline();
      fWBLWidth[detchan]  = wvfm.BLWidth();
      fWPeakADC[detchan]  = wvfm.PeakADC();
      fWPeakTDC[detchan]  = wvfm.PeakTDC();

      for (int i=0; i<nsamp; ++i) {
        float adc = float(wvfm.ADC(i));
        fWWvfm[detchan][i] = adc;
        fBACkovAvgWvfm[detchan]->Fill(i,adc);
      }

  	if(fFillTree && fDumpWaveforms){    
        sprintf(hname,"BACkovWvfm_r%u_s%u_e%d_ch%d",fRun,fSubrun,fWEvent,detchan);
        TH1F* h = tfs->make<TH1F>(hname,
                                  Form("Run %u subrun %u event %d, BACkov channel %d;sample (4 ns each);ADC",
                                       fRun,fSubrun,fWEvent,detchan),
                                  nsamp,0,nsamp);
        for (int i=0; i<nsamp; ++i) h->SetBinContent(i+1,fWWvfm[detchan][i]);
       }
   }
 }
  //......................................................................
  void BACkovHitReco::produce(art::Event& evt)
  { 
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();

    //std::string labelStr = "ADC:BACkov";
    std::string labelStr = "adcreco:BACkov";
    art::Handle< std::vector<rb::ADC> > ADCHandle;

    std::unique_ptr<std::vector<rb::BACkovHit> > BACkovHitv(new std::vector<rb::BACkovHit>);

    for (int i=0; i<kNChan; ++i) { fQtree[i] = -999.; fTtree[i] = -999.; }
    try {
	evt.getByLabel(labelStr, ADCHandle);

	if (!ADCHandle->empty()) {
	  GetBACkovHit(ADCHandle,  BACkovHitv);
	}
    }
    catch(...) {

    }
       std::string wvfmLabel = "raw:" + emph::geo::DetInfo::Name(emph::geo::BACkov);
      art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
      fWRun = fRun; fWSubrun = fSubrun; fWEvent = (int)evt.event();
      try {
	evt.getByLabel(wvfmLabel, wfHandle);
	if (!wfHandle->empty()) {
	  FillWaveforms(wfHandle);
	  if (fWvfmTree) fWvfmTree->Fill();
	}
      }
      catch(...) {
	std::cout << "BACkovHitReco: no " << wvfmLabel << " waveforms in this event"
		  << std::endl;
      }
    evt.put(std::move(BACkovHitv));
  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::BACkovHitReco)
