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
#include "TTimeStamp.h"

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
#include "RecoBase/GasCkovHit.h"
#include "RecoBase/ADC.h"
#include "RecoBase/Spill.h"

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
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginSubRun(art::SubRun &sr);
    void endSubRun(art::SubRun &sr);
    //void endRun(art::Run const&);
    void endJob();
    
  private:
    void GetGasCkovHit(art::Handle< std::vector<rb::ADC> > &,std::unique_ptr<std::vector<rb::GasCkovHit>> & GasCkovHits);

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    
    int mom; // This should really be pulled from SpillInfo instead of set in the fcl.
    TTimeStamp spilltime;
    float pmtadc1;
    float pmtadc2;
    float pmtadc3;
    float pressure5a;
    float pressure5b;
    float pressure6a;
    float pressure6b;

    std::vector<std::vector<bool>> GasCkov_signal;
    int pid_num;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    int event;
    TH1F*       fGasCkovChargeHist[3];

  };

  //.......................................................................
  
  GasCkovHitReco::GasCkovHitReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset),
      mom (pset.get<int>("momentum",0))
  {

    this->produces< std::vector<rb::GasCkovHit>>();

  }

  //......................................................................
  
  GasCkovHitReco::~GasCkovHitReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
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

  }

  //......................................................................
 
  void GasCkovHitReco::beginSubRun(art::SubRun& sr){
      art::Handle<rb::Spill> spillHandle;

      try {
          sr.getByLabel("spillinfo",spillHandle);

          pressure5a = spillHandle->MT5CPR();
          pressure5b = spillHandle->MT5CP2();
          pressure6a = spillHandle->MT6CPR();
          pressure6b = spillHandle->MT6CP2();
          spilltime = TTimeStamp(spillHandle->Timestamp());

      }
      catch(...) {
          std::cout << "No spill info object found!  Aborting..." << std::endl;
          std::abort();
      }

      std::cout << "Got info for spill at " << spilltime.AsString() << std::endl;
      std::cout<<"**************************************************"<<std::endl;
      std::cout<< "Beam Configuration: "<<mom<<" GeV/c"<<std::endl;
      std::cout << "Gas Cherenkov Upstream Pressure:   " << pressure5a <<" PSIA"<< std::endl;
      std::cout << "Gas Cherenkov Downstream Pressure: " << pressure6a <<" PSIA"<< std::endl;
      std::cout<<"**************************************************"<<std::endl;

    // Calculate cherenkov thresholds, format of arrays are {e,mu,pi,k,p}
    std::array<double,5> particle_m = {0.511, 105.7, 139.57, 493.68, 938.27}; //MeV
    for (int i=0; i<5; ++i) particle_m[i] = particle_m[i]*1e-3; //convert to GeV
    
    // Calculate v using p=gamma*m*v -> solve for v
    std::array<double,5> particle_v ={};
    for (int i=0; i<5; ++i){
        particle_v[i] = 1/(sqrt(1+pow(particle_m[i]/mom,2)));
    }

    // Calculate index of refraction (n) for upstream and downstream gas cherenkov
    // Uses P = P_0 * (n-1)/(n_0-1) with P_0 = 14.6959 PSIA (1 atm) and n_0 = 1.00045 for CO2 -> n = (P/P_0)*(n_0-1) + 1
    double P_0 = 14.6959;
    double n_0 = 1.00045;
    double n_upstream = (pressure5a/P_0)*(n_0-1) + 1;
    double n_downstream = (pressure6a/P_0)*(n_0-1) + 1;

    //Cherenkov condition is v>1/n for upstream GC
    //For downstream if cherenkov angle is >30mrad we expect outer PMT signal else we expect inner PMT signal
    std::array<std::vector<bool>,5> particle_sig;
    for (int i=0; i<5; ++i){
        if (particle_v[i] > (1/n_upstream)) particle_sig[i].push_back(1);
        else (particle_sig[i].push_back(0));

        if (particle_v[i] > (1/n_downstream)){
            //Calculate cherenkov angle for each particle cos(theta) = 1/(v*n)
            double theta  = acos(1/(particle_v[i]*n_downstream)) * 1e3; //cherenkov angle in mrad
            if (theta < 7) {
                particle_sig[i].push_back(1);
                particle_sig[i].push_back(0);
            }
            else{
                particle_sig[i].push_back(0);
                particle_sig[i].push_back(1);
            }
        } 
        else{
            particle_sig[i].push_back(0);
            particle_sig[i].push_back(0);
        }
        GasCkov_signal.push_back(particle_sig[i]);
    }
  }

  //......................................................................
  
  void GasCkovHitReco::endSubRun(art::SubRun& sr){
      GasCkov_signal.clear();
  }

  //......................................................................
  
  void GasCkovHitReco::endJob()
  {
  }
  
    //......................................................................
  
  void GasCkovHitReco::GetGasCkovHit(art::Handle< std::vector<rb::ADC> > & adcH, std::unique_ptr<std::vector<rb::GasCkovHit>> & GasCkovHits)
  {
    //Create empty vectors to hold charge values
    float Qvec[3];
    float Tvec[3];

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
        float q = ADC.Charge();
        float t = ADC.Time();
        fGasCkovChargeHist[detchan]->Fill(q);
        Qvec[detchan]=q;
        Tvec[detchan]=t;
	  }  
    }
   
    //Checking GasCkov Hits
    
    std::vector<bool> GasCkov_Result;
    bool PID_prob[5]={0,0,0,0,0};


    if (Qvec[0]>10) GasCkov_Result.push_back(1);
    else GasCkov_Result.push_back(0);
    if (Qvec[1]>5) GasCkov_Result.push_back(1);
    else GasCkov_Result.push_back(0);
    if (Qvec[2]>10) GasCkov_Result.push_back(1);
    else GasCkov_Result.push_back(0);

    for (int i=0; i<5; ++i){
        int result_flag=0;
        for (int j=0; j<3; ++j){
            if (GasCkov_Result[j] == GasCkov_signal[i][j]) result_flag+=1;
        }
        if (result_flag==3) PID_prob[i]=1;
    }
    
    //Create object and store GasCkov Charge and PID results
    rb::GasCkovHit GasCkovHit;
    GasCkovHit.SetCharge(Qvec);
    GasCkovHit.SetTime(Tvec);
    GasCkovHit.SetPID(PID_prob);
    GasCkovHits->push_back(GasCkovHit);
  }
 
  //......................................................................
  void GasCkovHitReco::produce(art::Event& evt)
  { 
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();

    std::string labelStr = "adcreco:GasCkov";
    art::Handle< std::vector<rb::ADC> > ADCHandle;

    std::unique_ptr<std::vector<rb::GasCkovHit> > GasCkovHitv(new std::vector<rb::GasCkovHit>);

    try {
	evt.getByLabel(labelStr, ADCHandle);

	if (!ADCHandle->empty()) {
        GetGasCkovHit(ADCHandle,  GasCkovHitv);
    }
    }
    catch(...) {

    }
      evt.put(std::move(GasCkovHitv));
  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::GasCkovHitReco)
