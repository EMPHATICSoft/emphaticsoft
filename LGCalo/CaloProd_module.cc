////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to make calorimeter data products
///       
/// \author  $Author: robert chirco $
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
#include "TH3F.h"
#include "TTree.h"
#include "TSpectrum.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
//#include "canvas/Persistency/Common/Ptr.h"
//#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// EMPHATICSoft includes
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "RecoBase/ADC.h"
#include "RecoBase/CaloHit.h"

using namespace emph;

// This pair is used to store the X and Y
// coordinates of a point respectively
#define pdd std::pair<double, double>

///package to illustrate how to write modules
namespace emph {
  ///
  class CaloProd : public art::EDProducer {
  public:
    explicit CaloProd(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    //~CaloProd();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //void endSubRun(art::SubRun const&);
    void beginJob();
    void endSubRun(art::SubRun& subrun);
    void endJob();
    
  private:

    void    MakeLGCaloPlots();
    void    fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > &);
    void    FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
    void    getBaseline(const std::vector<uint16_t> &wfm, double signif, size_t startBin, bool calo);
    int     findPeaks(const std::vector<uint16_t> &wfm); //, double sigma, double threshold=0.05);
    float    getIntChargePlots(const std::vector<uint16_t> &adcvals, int detchan);

    emph::cmap::ChannelMap* fChannelMap;
    runhist::RunHistory* fRunHistory;
    emph::geo::Geometry *emgeo;
    TTree*       RunInfo;
    //unsigned int fRun;
    //unsigned int fSubRun;
    int fRun, fSubRun;

    unsigned int fEvtNum;
    unsigned int fNEvents;
 
    static const unsigned int nChanCal = 9;
    int nchancal = 9;
    //TH1F* fNext;
    TH1F* fSignal;
    TH1F* fLGCaloADCDist[nChanCal];
    std::vector<unsigned int> fNEventsLGCalo;
    TH1F* fLGCaloWaveForm[nChanCal];
    TH1F* fLGCaloIntCharge[nChanCal];
    std::vector< std::vector<double> > intchg;

    static const unsigned int nChanTrig = 4;
    double fBaseline = 0;
    double fBaselineWidth = 0;
    size_t peakStartBin = 0;
    float c1=0; float e1=0; float t1=0;

    bool fMakeWaveFormPlots;
    bool fWant;

    std::vector<rb::CaloHit> calohits;

  };

  //.......................................................................
  
  emph::CaloProd::CaloProd(fhicl::ParameterSet const& pset)
    : EDProducer{pset}
  {
    //fMakeWaveFormPlots = pset.get<bool>("makeWaveFormPlots",true);
    this->reconfigure(pset);
    this->produces< std::vector<rb::CaloHit> >();
    //fEvtNum = 0;
    //fCheckClusters     (pset.get< bool >("CheckClusters"));
  }

  //......................................................................
  
//  CaloProd::~CaloProd()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  void emph::CaloProd::reconfigure(const fhicl::ParameterSet& pset){
        fMakeWaveFormPlots = pset.get<bool>("makeWaveFormPlots",true);
  }

  //......................................................................
  
  void CaloProd::beginRun(art::Run& run)
  {
    fChannelMap = new emph::cmap::ChannelMap();
    fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
  }
 
  void emph::CaloProd::beginJob()
  {
    std::cout<<"Begin Job"<<std::endl;
    fNEvents= 0;
    std::string labelStr;

    art::ServiceHandle<art::TFileService> tfs;

    //fNext = tfs->make<TH1F>("Next","Placeholder Trigger Waveform",108,0.,108.); 
    fSignal = tfs->make<TH1F>("Signal","Signal",108,0.,108.);

    for (int i=0; i<nchancal; ++i){
          fLGCaloADCDist[i] = tfs->make<TH1F>(Form("LGCaloADCDist_%d",i),Form("LG Calo ADC Distribution %d",i),512,0.,4095.);
    }
    for (int i=0; i<nchancal; ++i){
          fLGCaloWaveForm[i] = tfs->make<TH1F>(Form("LGCaloWaveForm_%d",i),Form("LG Calo Waveform %d",i),108,0.,108.);
          fNEventsLGCalo.push_back(0);
    }
    for (int i=0; i<nchancal; ++i){
          fLGCaloIntCharge[i] = tfs->make<TH1F>(Form("LGCaloIntCharge_%d",i),Form("LG Calo Integrated Charge %d",i),200,0.,26000.);
    }

    RunInfo = tfs->make<TTree>("RunInfo","");
    RunInfo->Branch("fRun",&fRun,"fRun/I");
    RunInfo->Branch("fSubRun",&fSubRun,"fSubRun/I");
    //RunInfo->Fill();
  }

  //......................................................................

  void emph::CaloProd::endSubRun(art::SubRun& subrun)
  {
  /*  std::cout<<"Writing file for run/subrun: " << fRun << "/" << fSubrun << std::endl;
  char filename[32];
  sprintf(filename,"lgcaloana_r%d_s%d.root", fRun, fSubrun);
  TFile* f = new TFile(filename,"RECREATE");
  HistoSet::Instance().WriteToRootFile(f);
  f->Close();
  delete f; f=0;
  */
  }
 
  //......................................................................
  
  void emph::CaloProd::endJob()
  {
    if (fNEvents > 0) {
       float scale = 1./float(fNEvents);
       for (int i=0; i<nchancal; ++i) { //fLGCaloWaveForm.size(); ++i) {   
           scale = 1./float(fNEventsLGCalo[i]);
           fLGCaloWaveForm[i]->Scale(scale);
           fLGCaloWaveForm[i]->SetOption("hist");
       }
    }
    fSignal->SetTitle("I'm filler! Ignore me");
    //delete fSignal;
    //fSignal->SetDirectory(nullptr);
    //caloprod_r%r_s%s.root
    RunInfo->Fill();
  }

  //......................................................................

  void emph::CaloProd::MakeLGCaloPlots()
  {
      if (fMakeWaveFormPlots) {
        for (int i=0; i<nchancal; ++i) {
          fLGCaloWaveForm[i]->SetBit(TH1::kIsAverage);
        }
      }
  }

  //......................................................................

  void emph::CaloProd::fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
     if (fRun <2000){
       int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
       emph::cmap::FEBoardType boardType = emph::cmap::V1720;
       emph::cmap::EChannel echan;
       echan.SetBoardType(boardType);

       if (fMakeWaveFormPlots) {
         if (!wvfmH->empty()) {
            for (size_t idx=0; idx < wvfmH->size(); ++idx) {
              const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
              int chan = wvfm.Channel();
              int board = wvfm.Board();
              echan.SetBoard(board);
              echan.SetChannel(chan);
              emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
              int detchan = dchan.Channel();
              if (detchan >= 0 && detchan < nchan) {
                std::vector<uint16_t> adcvals = wvfm.AllADC();
                getBaseline(adcvals,8.0,10,false);
                //cut on trigger waveform
                float adc = wvfm.Baseline()-wvfm.PeakADC();
                float blw = wvfm.BLWidth();
                if (adc > 5*blw && findPeaks(adcvals) == 1){
                  if (adc > 500) fWant=true; //return true;
                }
              }
            }
          }
        }
      }
      if (fRun >= 2140 && fRun <= 2411){
	 std::cout<<"This run has bad calorimeter waveforms :/"<<std::endl;
         fWant = false;
      }
      else fWant = true;
      //if (fEvtNum != 12) fMakeWaveFormPlots = false; //pulse on first block
      //if (fEvtNum != 6088) fMakeWaveFormPlots = false; //negative int charge on block 5
      //if (fEvtNum != 16797) fMakeWaveFormPlots = false;
      //else fMakeWaveFormPlots = true;
  }

  //......................................................................
 
  void emph::CaloProd::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
  {
      t1=0; c1=0; e1=0;
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::LGCalo);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      if (fMakeWaveFormPlots) {
//std::cout<<"hey"<<std::endl;
        if (!wvfmH->empty()) {
          for (size_t idx=0; idx < wvfmH->size(); ++idx) {
            const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
            int chan = wvfm.Channel();
            int board = wvfm.Board();
            echan.SetBoard(board);
            echan.SetChannel(chan);
            emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
            if (detchan >= 0 && detchan < nchan) {
              float adc = wvfm.Baseline()-wvfm.PeakADC();
              float blw = wvfm.BLWidth();
              if (adc > 5*blw) {
                fLGCaloADCDist[detchan]->Fill(adc);
                //now fill waveform plot
                auto adcvals = wvfm.AllADC();
                fNEventsLGCalo[detchan]++;
                for (size_t i=0; i<adcvals.size(); ++i) {
                  fLGCaloWaveForm[detchan]->Fill(i,adcvals[i]); //i+1,adcvals[i]);
                }
                getBaseline(adcvals,8.0,10,true);
		   
                if (findPeaks(adcvals)==1){
	           float intchg = getIntChargePlots(adcvals,detchan);
		   rb::CaloHit onehit;
		   onehit.SetChannel(detchan);
		   onehit.SetTime(adcvals);
		   onehit.SetIntCharge(intchg);
                   //rb::CaloHit onehit(adcvals,detchan);
		   calohits.push_back(onehit);
	           //std::cout<<"I'm here: "<<fEvtNum<<std::endl; //could be up to 9 times 
		}
              }
            }
          }
        }
      }
  }

  //......................................................................

  void emph::CaloProd::getBaseline(const std::vector<uint16_t> &wfm, double signif, size_t startBin, bool calo){
       //check startBin
       if (startBin < 3) {
           std::cerr << "That's the wrong startBin = " << startBin << " no room to get the baseline " << std::endl;
       }
       //find baseline
       double aa=0.; double aa2=0.;
       size_t stopBin = startBin-1;
       for (size_t k=0; k != stopBin; k++) { aa += wfm[k]; aa2 += wfm[k]*wfm[k]; }
       fBaseline = aa/stopBin;
       fBaselineWidth = std::max(0.5, ( std::sqrt((aa2 - stopBin*fBaseline*fBaseline)/(stopBin-1))));  // we do not want zero width..

       //find peak start if calo set to true
       if (calo == true){
       peakStartBin = startBin;
          while (peakStartBin < wfm.size()) {
             const double val =  fBaseline - static_cast<double>(wfm[peakStartBin]);
             if (val/fBaselineWidth > signif) {
                size_t nextBin = peakStartBin + 1;
                if (nextBin < wfm.size()) {
                   const double valNext =  fBaseline - static_cast<double>(wfm[nextBin]);
                   if (valNext/fBaselineWidth > signif) {
                   break;
                   }
                }
              }
              peakStartBin++;
          }
       }

  }

  //......................................................................

  

  //......................................................................

  int emph::CaloProd::findPeaks(const std::vector<uint16_t> &wfm){ //, double sigma, double threshold){
      Int_t nMaxPeak =3;
      TSpectrum* s = new TSpectrum(nMaxPeak);
      TH1F adcs("adcs","adcs",wfm.size(),0.0,wfm.size());

      for (size_t i=0; i<wfm.size(); ++i) {
          adcs.Fill(i,fBaseline-wfm[i]);
      }
      adcs.SetOption("hist");
      Int_t nFound=0;
      fSignal = (TH1F*) adcs.Clone();
      try{
          nFound = s->Search(fSignal,2,"goff",0.1);
      } catch (...){}
      return nFound;
  }

  //......................................................................

  float emph::CaloProd::getIntChargePlots(const std::vector<uint16_t> &adcvals, int detchan){
    float x1=peakStartBin;
    float nsamp=35; //25; //range where the signal is
    float avg = 0; int ic = 0;
    float sum=0;

    for (size_t i=0; i<adcvals.size(); ++i){
             if (i<size_t(x1) || i>size_t(x1+nsamp)) avg += float(adcvals[i]), ++ic;
    }

    avg /= float(ic); //baseline for each signal

    for (size_t i=x1; i<size_t(x1+nsamp) && i<adcvals.size(); ++i){
        sum += (avg-adcvals[i]); //total (baseline-subtracted) integrated charge over the range
    }
    //similar funtions in RawData/Waveform.h but not fully sufficient 

    fLGCaloIntCharge[detchan]->Fill(sum);

    //std::cout<<"Event Number: "<<fEvtNum<<std::endl;
    if (sum < 0){
       std::cout<<"Negative sum found: "<<fEvtNum<<std::endl;
       //std::cout<<"Peak starts at: "<<peakStartBin<<std::endl;
       //std::cout<<"Baseline is: "<<avg<<std::endl;
       //std::cout<<"Integrated charge is: "<<sum<<std::endl;
    }

    return sum;
  }

  //......................................................................

  void emph::CaloProd::produce(art::Event& evt)
  {
    std::unique_ptr< std::vector<rb::CaloHit> > calohitv(new std::vector<rb::CaloHit>);

    //std::cout<<"new event eaaow"<<std::endl;

    ++fNEvents;
    fRun = evt.run();
    fSubRun = evt.subRun();
    fEvtNum = evt.id().event();
    std::string labelStr;

    //std::vector<rb::CaloHit> calohits;

    for (int i=0; i<emph::geo::NDetectors; ++i) {
        calohits.clear();

        labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
        art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
        try {
          evt.getByLabel(labelStr, wfHandle);

          if (!wfHandle->empty()) {
            if (i == emph::geo::Trigger){fWantLGCalo(wfHandle);}
            if (i == emph::geo::LGCalo){ //  FillLGCaloPlots(wfHandle);
               if (fWant){
		  FillLGCaloPlots(wfHandle);
		  for (size_t i=0; i<calohits.size(); i++){
		      calohitv->push_back(calohits[i]);
		  }
	       } 
            }
          }
      
        }
        catch(...) {
        }
    }
    evt.put(std::move(calohitv));
  }  
} // end namespace emph

DEFINE_ART_MODULE(emph::CaloProd) //_Robert)
