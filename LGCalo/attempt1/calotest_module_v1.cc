////////////////////////////////////////////////////////////////////////
///// \brief   Analyzer module to study LG Calorimeter
///// \author  $Author: robert chirco $
//////////////////////////////////////////////////////////////////////////
//// C/C++ includes
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cfloat>
#include <climits>
#include <thread>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TObjArray.h"
#include "TSpectrum.h"
//#include "TTree.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//#include "art/Framework/Principal/Run.h"
//#include "art/Framework/Principal/SubRun.h"

// EMPHATICSoft includes
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "TOF/PeakInWaveForm.h"
#include "RecoBase/ADC.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace lgcalo {

    class calotest : public art::EDAnalyzer {
    public:
      explicit calotest(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~calotest();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);
      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const& run);
      void endSubRun(art::SubRun const&);
      void endJob();

    private:
      //void  FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);

      //TTree*  peakcounts;

      void    FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void    MakeLGCaloPlots();
      void    fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > &);

      bool  REALfindPeak(const std::vector<uint16_t> &wfm, double signif, size_t startBin);

      size_t  peakStart(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
      void    fillInBaseline(const std::vector<uint16_t> &wfm, size_t startBin);
      void    findLocalMax(const  std::vector<uint16_t> &wfm, double signif, size_t startBin);
      bool  extrapolateForPeak(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
      bool  extrapolatePoints(const std::vector<uint16_t> &wfm, size_t k); //double bl, const double max_guess, size_t k);
      bool  peakFinder(const std::vector<uint16_t> &wfm, size_t k);
      bool  simplePeak(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
      int  wrapROOTpFind(const std::vector<uint16_t> &wfm); //, double sigma, double threshold=0.05);

      static const unsigned int nChanCal = 9;
      int nchancal = 9;

      int counter = 0;
      int one=0; int many=0;
      //int trycount = 0;
      //int catchcount = 0;
      TH1F* fNext;   
      TH1F* fSignal;
      TH1* fBackground;   
      TH1F* h2;
      bool sus;
      TObjArray fSusTrig;
      TObjArray fManyPeaks;
      TObjArray fOnePeak;
	int peakctr=0;
      std::string STATE; 

      TH1F* fLGCaloADCDist[nChanCal];
      std::vector<unsigned int> fNEventsLGCalo;
      TH1F* fLGCaloWaveForm[nChanCal];
//      std::vector<TH1F*> fLGCaloWaveForm;
      bool fMakeWaveFormPlots;
      bool want;

      std::vector<double> fTrigADCs; 
      std::vector<emph::tof::PeakInWaveForm> fPeakTriggers;
 
      emph::cmap::ChannelMap* fChannelMap;
      runhist::RunHistory* fRunHistory;
      
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
  
      static const unsigned int nChanTrig = 4;

      //more functions 
      int fStartBin=1024;
      int fPeakBin=1024;
      int fEndBin=1024;
      double fPeakVal = -1;
      double fPeakVal2 = -1;

      int fNPeaks = 0;

      double fBaseline = 0;
      double fBaselineWidth = 0;
      double fCutIntegration = 0.05;
   };
    //.......................................................................

    calotest::calotest(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset)
      //fLGCaloADCDist_i(nullptr),
      //fLGCaloWaveForm_i(nullptr)
      //      fLGCaloADCDist[nChanCal](nullptr); //need to specify all 9?
      //fNEventsLGCalo(0);
     // fLGCaloWaveForm(nullptr);
    //  want(false);

    {

      this->reconfigure(pset);

    }

    //......................................................................

    calotest::~calotest()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }
    //......................................................................

    void calotest::reconfigure(const fhicl::ParameterSet& pset)
    {
      fMakeWaveFormPlots = pset.get<bool>("makeWaveFormPlots",true);
    }
    //......................................................................
    void calotest::beginJob()
    {
      fNEvents= 0;
      std::string labelStr;

     // HistoSet& h = HistoSet::Instance();
     // fLGCaloIntChgVsCenterRatio = h.GetTH2F("LGCaloIntChgVsCenterRatio");
     // fLGCaloIntChgVsEdgeRatio = h.GetTH2F("LGCaloIntChgVsEdgeRatio");
     // fNTriggerLGArray     = h.GetTH2F("NTriggerLGArray");
      MakeLGCaloPlots();
      art::ServiceHandle<art::TFileService> tfs;

      //std::cout<<"beginJob1"<<std::endl; 

      fNext = tfs->make<TH1F>("SusTrig","Weird Trigger Waveform",600,0.,120.);
      fSignal = tfs->make<TH1F>("Signal","Signal",108,0.,108.);

      fBackground = tfs->make<TH1F>("Background","Background",108,0.,108.);
     //for (int i=0; i<2; ++i){
     //     fSusTrig = tfs->make<TH1F>(Form("SusTrig_%d",i),Form("Weird Trigger Waveform %d",i),512,0.,4095.);
      //}
      //
      //peakcounts = tfs->make<TTree>("peakcounts","");
      //peakcounts->Branch("manypeaks",&fManyPeaks,"fManyPeaks/I");
      //peakcounts->Branch("onepeak",&fOnePeak,"fOnePeak/I");

      for (int i=0; i<nchancal; ++i){
	  fLGCaloADCDist[i] = tfs->make<TH1F>(Form("LGCaloADCDist_%d",i),Form("LG Calo ADC Distribution %d",i),512,0.,4095.);	    
      }
          //std::cout<<"beginJob2"<<std::endl; 
	  //fLGCaloADCDist.push_back(fLGCaloADCDist_i);	
      for (int i=0; i<nchancal; ++i){
          fLGCaloWaveForm[i] = tfs->make<TH1F>(Form("LGCaloWaveForm_%d",i),Form("LG Calo Waveform %d",i),600,0.,200.);
          //std::cout<<"beginJob3"<<std::endl;
          //fLGCaloWaveForm.push_back(fLGCaloWaveForm_i); 
          fNEventsLGCalo.push_back(0);
      }
      //std::cout<<"beginJob4"<<std::endl;
    }

    //......................................................................

    void calotest::beginRun(art::Run const& run)
    {
     //std::cout<<"beginRun"<<std::endl;

      // initialize channel map
      fChannelMap = new emph::cmap::ChannelMap();
      fRunHistory = new runhist::RunHistory(run.run());
      fChannelMap->LoadMap(fRunHistory->ChanFile());
    }

    //......................................................................
    void calotest::endSubRun(const art::SubRun&)
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
    void calotest::endJob()
    {
     //std::cout<<"endJob"<<std::endl;
     //std::cout<<"# of Sus Triggers = "<<counter<<std::endl;

     std::cout<<"One peak: "<<one<<std::endl;
     std::cout<<"Many peaks: "<<many<<std::endl;

     //std::cout<<"Tried and succeeded: "<<trycount<<std::endl;
     //std::cout<<"Caught...: "<<catchcount<<std::endl;	

     //fSusTrig.Write();
     //fManyPeaks.Write();
     fOnePeak.Write();

//     art::ServiceHandle<art::TFileService> tfs; 
//     peakcounts = tfs->make<TTree>("peakcounts","");
//     peakcounts->Branch("manypeaks",&fManyPeaks,32000,0);
//fManyPeaks.Write();
//     peakcounts->Branch("onepeak",&fOnePeak,"fOnePeak/I");

    //fSignal->Write();
     //art::ServiceHandle<art::TFileService> tfs;
      //      fNext = tfs->make<TH1F>("SusTrig","Weird Trigger Waveform",600,0.,200.);
     //for (int i=0; i<static_cast<int>(fSusTrig.size()); i++){
  
        //fSusTrig[i] = tfs->make<TH1F>(Form("SusTrig_%d",i),Form("Weird Trigger Waveform %d",i),600,0.,200.);
     //}
     if (fNEvents > 0) {
        float scale = 1./float(fNEvents);
      for (int i=0; i<nchancal; ++i) { //fLGCaloWaveForm.size(); ++i) {
      //for (size_t i=0; i<fLGCaloWaveForm.size(); ++i) {
          scale = 1./float(fNEventsLGCalo[i]);
          fLGCaloWaveForm[i]->Scale(scale);
        }
      }

//     fNTriggerLGArray->SetBinContent(1.5,1.5,fNEventsLGCalo[6]);
//     fNTriggerLGArray->SetBinContent(2.5,2.5,fNEventsLGCalo[4]);
//     fNTriggerLGArray->SetBinContent(3.5,3.5,fNEventsLGCalo[2]);
//     fNTriggerLGArray->SetBinContent(1.5,3.5,fNEventsLGCalo[0]);
//     fNTriggerLGArray->SetBinContent(2.5,3.5,fNEventsLGCalo[1]);
//     fNTriggerLGArray->SetBinContent(1.5,2.5,fNEventsLGCalo[3]);
//     fNTriggerLGArray->SetBinContent(3.5,2.5,fNEventsLGCalo[5]);
//    fNTriggerLGArray->SetBinContent(2.5,1.5,fNEventsLGCalo[7]);
//     fNTriggerLGArray->SetBinContent(3.5,1.5,fNEventsLGCalo[8]);

      //char filename[32];
      //sprintf(filename,"lgcaloana_r%d_s%d.root", fRun, fSubrun);
      //TFile* f = new TFile(filename,"RECREATE");
      //HistoSet::Instance().WriteToRootFile(f);
      //f->Close();
      //delete f; f=0;
     }

    //......................................................................
    void  calotest::MakeLGCaloPlots()
    {
     /* HistoSet& h = HistoSet::Instance();

      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::LGCalo);
      char hname[256];
      if (fMakeWaveFormPlots) {
        std::cout << "Making LGCalo ADC OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"LGCaloADC_%d",i);
          fLGCaloADCDist[i] = h.GetTH1F(hname);
        }
        std::cout << "Making LGCalo WaveForm OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"LGCaloWaveForm_%d",i);
          fLGCaloWaveForm.push_back(h.GetTH1F(hname));
          fLGCaloWaveForm[i]->SetBit(TH1::kIsAverage);

          sprintf(hname,"LGCaloIntCharge_%d",i);
          fLGCaloIntCharge.push_back(h.GetTH1F(hname));
          fNEventsLGCalo.push_back(0);
        }
        fLGCaloTotalCharge = h.GetTH1F("LGCaloTotalCharge");
        fLGCaloCenterRatio = h.GetTH1F("LGCaloCenterRatio");
        fLGCaloEdgeRatio   = h.GetTH1F("LGCaloEdgeRatio");
        fLGCaloSignalRatio   = h.GetTH1F("LGCaloSignalRatio");
      }*/
    }
    //......................................................................
    void    calotest::fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
     //fNext->Reset();

     int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
     emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);

      std::vector<int> numPeaks(fTrigADCs.size(), 0);

//      if (fEvtNum != 35) fMakeWaveFormPlots = false; //DEB
//      else fMakeWaveFormPlots = true;
      fMakeWaveFormPlots = true;

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
	      //std::vector<uint16_t> tmpwf = wvfm.AllADC();
	      std::vector<uint16_t> adcvals = wvfm.AllADC();
	      //size_t startSearchIndex = 0;
//	      int npeaks = 0;
	      //while (startSearchIndex<adcvals.size()){
//	        size_t startSearch = REALfindPeak(adcvals,8.0,10);
//###SUS###                sus = REALfindPeak(adcvals,8.0,10);
                sus = simplePeak(adcvals,8.0,10); //extrapolateForPeak(adcvals,8.0,10);
		//std::cout<<"Doing wraproot"<<std::endl;
		if (wrapROOTpFind(adcvals) > 1){ //;
 		   for (size_t i=0; i<adcvals.size(); ++i) {
		      fNext->Fill(i,fBaseline-adcvals[i]);
                   }
		   fSignal->SetTitle(Form("Event: %d",fEvtNum));
		   fManyPeaks.Add(fSignal); //fSusTrig.Add(fSignal);
                   fNext->Reset();
		   many++;
		}
		if (wrapROOTpFind(adcvals) == 1){
		   for (size_t i=0; i<adcvals.size(); ++i) {
                      fNext->Fill(i,fBaseline-adcvals[i]);
                   }
                   fSignal->SetTitle(Form("Event: %d",fEvtNum));
		   peakctr++;
 		   if (peakctr <= 200){
                   fOnePeak.Add(fSignal); //fSusTrig.Add(fSignal);
	           }
                   fNext->Reset();
		   one++;
		}
	        //std::cout<<"Extrapolated"<<std::endl;

            //fill suspicious trigger waveforms
            //if (sus == true){
// 3/15/23             counter++;
              //fNext->Reset();
// 3/15/23              for (size_t i=0; i<adcvals.size(); ++i) {
                    //fNext->Fill(i+1,fBaseline-adcvals[i]);
// 3/15/23                   fNext->Fill(i,fBaseline-adcvals[i]);
// 3/15/23             }
              //TH1F *h2 = (TH1F*) fNext->Clone();
	      //fNext->SetTitle(Form("State: %s",STATE.c_str()));
              //fSusTrig.Add(h2);
// 3/15/23	      fSusTrig.Add(fSignal);
// 3/15/23             fNext->Reset();
            //}
            //STATE = "";
            //sus = false;
            //cut on trigger waveform
              float adc = wvfm.Baseline()-wvfm.PeakADC();
              float blw = wvfm.BLWidth();
              if (adc > 5*blw){
                //fTriggerADCDist[detchan]->Fill(adc);
              if (adc > 500) want=true; //return true;
              }
            }
          }
        }
      }
    }

    //......................................................................

    bool calotest::simplePeak(const std::vector<uint16_t> &wfm, double signif, size_t startBin){

	//looks for two successive bins over the signif*rms of the bins before startBin
	
       //check startBin
       if (startBin < 3) {
           std::cerr << "That's the wrong startBin = " << startBin << " no room to get the baseline " << std::endl;
           return false;
       }
       //find baseline
       double aa=0.; double aa2=0.;
       size_t stopBin = startBin-1;
       for (size_t k=0; k != stopBin; k++) { aa += wfm[k]; aa2 += wfm[k]*wfm[k]; }
       fBaseline = aa/stopBin;
       fBaselineWidth = std::max(0.5, ( std::sqrt((aa2 - stopBin*fBaseline*fBaseline)/(stopBin-1))));  // we do not want zero width..

	bool peakfound = false;

       //find peak start
       size_t peakStartBin = startBin;
       while (peakStartBin < wfm.size()) {
          const double val =  fBaseline - static_cast<double>(wfm[peakStartBin]);
          if (val/fBaselineWidth > signif) {
             size_t nextBin = peakStartBin + 1;
             if (nextBin < wfm.size()) {
                const double valNext =  fBaseline - static_cast<double>(wfm[nextBin]);
                if (valNext/fBaselineWidth > signif) {
		  peakfound = true; 
                  break;
                }
             }
           }
           peakStartBin++;
       }
       if (peakStartBin == wfm.size()) { std::cerr << "Couldn't find the start of the peak" << std::endl; }
	return peakfound;
     } 
    
     int calotest::wrapROOTpFind(const std::vector<uint16_t> &wfm){ //, double sigma, double threshold){
	 Int_t nMaxPeak =3;
	 //std::cout<<"fBaseline: "<<fBaseline<<std::endl;
	 TSpectrum* s = new TSpectrum(nMaxPeak);
	 
	 //std::cout<<"sigma: "<<sigma<<"..."<<"threshold: "<<threshold<<std::endl;
	 TH1F adcs("adcs","adcs",wfm.size(),0.0,wfm.size());
         //TH1F* adcs = new TH1F("adcs","adcs",wfm.size(),0.0,wfm.size()); 
	 for (size_t i=0; i<wfm.size(); ++i) {
	      //adcs.Sumw2();
              adcs.Fill(i,fBaseline-wfm[i]);
	//      adcs.SetBinError(i,0);
	      //adcs.SetOption("");
	      //adcs->Fill(i,wfm[i]);
         }
	adcs.SetOption("hist");
	 //std::cout<<"wfm.size(): "<<wfm.size()<<std::endl;
	 //std::cout<<"sigma: "<<sigma<<std::endl;
	 //std::cout<<"threshold: "<<threshold<<std::endl;
	 //std::cout<<"gonna search"<<std::endl;
         Int_t nFound=0;
	 //fSignal->Copy(adcs);
	 fSignal = (TH1F*) adcs.Clone();
         try{
	     nFound = s->Search(fSignal,2,"goff",0.1);
             //nFound = s->Search(&adcs,sigma,"goff",threshold);
         //    trycount++;
         } catch (...){} //{catchcount++;}//{std::cout<<"Caught!  Ya scoundrel!"<<std::endl;}
         //std::cout<<"searched"<<std::endl;
         //TH1* bkg = s->Background(&adcs);
         TH1* bkg = s->Background(fSignal);
	 //fSignal = adcs.Copy();
         //fSignal->Copy(adcs);
	 fBackground = bkg;
	 //std::cout<<"nFound: "<<nFound<<std::endl; 
	
	 return nFound; 
     }
 
//  bool calotest::extrapolateForPeak(const std::vector<uint16_t> &wfm, double signif, size_t startBin){

       //find first peak
/*       fStartBin = peakStartBin;
       fPeakBin = wfm.size();
       fPeakVal = -1.;
       size_t peakEndBin = wfm.size();

       for (size_t k=peakStartBin ; k != peakEndBin; k++) {
           const double val2 = fBaseline - wfm[k];
	   const double val1 = fBaseline - wfm[k-1];
           const double m = (val2-val1); // deltaX = k-(k-1) = 1 since the x distance is just one bin
           //std::cout<<m<<std::endl; 
           //const double pred = m*2;
           if (m > 0 && val2 > fPeakVal) {fPeakVal = val2; fPeakBin = k;}
           else break;
       }
       if (fPeakVal > 0) nPeaks++;

       //least squares polynomial fit (separate function?)
       //need to be at least 6 bin away from the end of the waveform i think
       size_t k  = peakStartBin;
       //const double maxg = -1.;
       std::cout<<"Peak Start Bin: "<<k<<std::endl;
       //while (k <= peakEndBin-6){
       for (size_t k=peakStartBin; k <= peakEndBin-6; k++){
             std::cout<<"Starting in bin: "<<k<<std::endl;
	     if (extrapolatePoints(wfm,k)){
		if (peakFinder(wfm,k)){ //fBaseline,maxg,k);
	     	    fNPeaks++; std::cout<<"Peak Confirmed"<<std::endl;
		    std::cout<<"Calculating new baseline..."<<std::endl;
		    fBaseline = (wfm[fEndBin] + wfm[fEndBin+1] +
                                 wfm[fEndBin+2] + wfm[fEndBin+3] + wfm[fEndBin+4] + wfm[fEndBin+5])/6.;		    	    
		}
             k = fEndBin;
             }
	     std::cout<<"Ending in bin: "<<k<<std::endl;
       }
       
       std::cout<<"NPeaks: "<<fNPeaks<<std::endl;
       if (fNPeaks != 1) {fNPeaks = 0; return true;}
       else return false;
  */          
  //  }
   
    //......................................................................

    bool calotest::extrapolatePoints(const std::vector<uint16_t> &wfm, size_t k){ //double bl, const double max_guess, size_t k){
	     //fBaseline = bl;

             double w0 = fBaseline - wfm[k+4]; //const double k0 = 1.0*(k+4); std::vector<double> v0; v0.push_back(w0); v0.push_back(k0); 
             double w1 = fBaseline - wfm[k+3]; //const double k1 = 1.0*(k+3); std::vector<double> v1; v1.push_back(w1); v1.push_back(k1);
             double w2 = fBaseline - wfm[k+2]; //const double k2 = 1.0*(k+2); std::vector<double> v2; v2.push_back(w2); v2.push_back(k2);
       	     double w3 = fBaseline - wfm[k+1]; //const double k3 = 1.0*(k+1); std::vector<double> v3; v3.push_back(w3); v3.push_back(k3);
             double w4 = fBaseline - wfm[k];   //const double k4 = 1.0*(k+0); std::vector<double> v4; v4.push_back(w4); v4.push_back(k4);

             double y0 = w0 + w1 + w2 + w3 + w4;
             double y1 = (-1.)*w1 + (-2.)*w2 + (-3.)*w3 + (-4.)*w4;
             double y2 = w1 + 4.*w2 + 9.*w3 + 16.*w4;
             double y3 = (-1.)*w1 + (-8.)*w2 + (-27.)*w3 + (-64.)*w4;

             double a0 = (1242.*y0 -5.*(23045.*y1 + 17838.*y2 + 3079.*y3))/22260.;
             double a1 = (750.*y0 - 32785.*y1 - 28050.*y2 - 5099.*y3)/8904.;
             double a2 = (162.*y0 - 2125.*y1 - 2052.*y2 - 395.*y3)/4452.;
             double a3 = (6.*y0 + 43.*y1 + 5.*(6.*y2+y3))/1272.;

             double w5 = fBaseline - wfm[k+5]; //const double k5 = 1.0*(k+5); std::vector<double> v5; v5.push_back(w5); v5.push_back(k5);
             double pred5 = a0 + a1*(1.) + a2*(1.)*(1.) + a3*(1.)*(1.)*(1.);       double res5 = w5 - pred5;

             double pred0 = a0;                                                    double res0 = w0 - pred0;
             double pred1 = a0 + a1*(-1.)+a2*(-1.)*(-1.) + a3*(-1.)*(-1.)*(-1.);   double res1 = w1 - pred1;
             double pred2 = a0 + a1*(-2.)+a2*(-2.)*(-2.) + a3*(-2.)*(-2.)*(-2.);   double res2 = w2 - pred2;
             double pred3 = a0 + a1*(-3.)+a2*(-3.)*(-3.) + a3*(-3.)*(-3.)*(-3.);   double res3 = w3 - pred3;
             double pred4 = a0 + a1*(-4.)+a2*(-4.)*(-4.) + a3*(-4.)*(-4.)*(-4.);   double res4 = w4 - pred4;

             double res_rms = std::sqrt((res0*res0 + res1*res1 + res2*res2 + res3*res3 + res4*res4)/5.);
	     double res_3x = 3*res_rms;

             //double max_val = -1.;
             //size_t max_bin = k;
	     if (std::abs(res5) < res_3x) {std::cout<<"No pulse"<<std::endl; return false;}
//             if (std::abs(res5) < 3*res_rms) {std::cout<<"No pulse"<<std::endl; return false;}
	     else {std::cout<<"Pulse found"<<std::endl; return true;}
   }   
   bool calotest::peakFinder(const std::vector<uint16_t> &wfm, size_t k){
            //Pulse starting...
            double max_val = fBaseline - wfm[k];
	    size_t max_bin = k;
	    int nBinDrops = 2;

	    int nDrops = 0;
	    bool dropping = false;

	    double aSum = 0.;

            for (size_t j=k+1; j<wfm.size(); j++){
 		double reading = fBaseline - wfm[j];
		aSum += reading;
		double readingprev = fBaseline - wfm[j-1];
		std::cout<<"Max bin is: "<<max_bin<<std::endl;
	        if (reading > max_val){
		   max_val = reading;
	           max_bin = j;
		   nDrops = 0;
		   std::cout<<"NEW max bin is: "<<max_bin<<std::endl;
		   if (dropping) {fEndBin = j; std::cout<<"Down back up...Bin is: "<<j<<std::endl; return false;} //down back up--> how to handle this?
		}
		else {
		   nDrops++;
		   dropping = true;
	           std::cout<<"Bin is: "<<j<<"...max bin is: "<<max_bin<<std::endl;
         	   std::cout<<"Pulse dropping..."<<std::endl;
		   if (reading/readingprev >= 0.8) { std::cout<<"This is noise..."<<std::endl;}
		}
		if (nDrops >= nBinDrops) {
	            if (((std::abs(reading)/(std::abs(aSum) + 1.0e-10)) < fCutIntegration) && ((static_cast<int>(j) - static_cast<int>(max_bin)) > 4) && ((static_cast<int>(j) - static_cast<int>(max_bin)) < 8 )) {
                       fEndBin = j;
		       std::cout<<"Integration converged..."<<std::endl;
			std::cout<<"Bin is: "<<j<<"...max bin is: "<<max_bin<<std::endl;		
		       return true; //break; //peak
		    }
		    else { std::cout<<"Dist from max bin: "<<static_cast<int>(j) - static_cast<int>(max_bin)<<std::endl;}
		    if ((j-k) > 15) {
                       fEndBin = j;
			std::cout<<"Bin is: "<<j<<std::endl;
          	       std::cout<<"Start and end bins far apart..."<<std::endl;
                       return true; //break; //peak
                    }
                }             	
	    }
            fEndBin = wfm.size();
	    //std::cout<<"Bin is: "<<wfm.size()<<std::endl
	    std::cout<<"End of function..?"<<std::endl;
	    return false;


//	        max_val = w5;
//		max_bin = k5;
                /*for (size_t i=0; i < val_list.size(); i++){
		    //max_bin = bin_list[i];
                    if (val_list[i] > max_val) {
			max_val = val_list[i];
		        max_bin = bin_list[i];
		        std::cout<<"Finding max"<<std::endl; 
		    }
	        }*/



/*		const double w6 = fBaseline - wfm[max_bin+1];
	        if (w6 > max_val){
		    max_val = w6;
	            max_bin++;
	            //fNPeaks++; fPeakBin = max_bin; fPeakVal = max_val;	    
		    return max_bin; 
		   //std::cout<<"Checking max+1"<<std::endl;
		}
	        else{
		    const double w7 = fBaseline - wfm[max_bin+2];
		    if (w7 < w6){
		       if (w7/max_val <= 0.1){
			  std::cout<<"Peak Found!"<<std::endl;	
			  fNPeaks++; fPeakBin = max_bin; fPeakVal = max_val;
			  return max_bin+2; //fix this part?
			}
		    }  
		}
                //if (max_val == max_guess) {fNPeaks++; return k+5;}
                //else return max_bin;
             }
	     return max_bin; // if w7>w6 i.e. peak not confirmed
     	     //if (max_val == max_guess) return 1; //npeaks++ k+5
	     //else return 2; //k of max val        
//    const double max_val = -1.;
//    if (max_val == max_guess) return k;
//    return 0;
*/
    }

    //......................................................................

    void calotest::findLocalMax(const  std::vector<uint16_t> &wfm, double signif, size_t startBin){

       //check startBin
       if (startBin < 3) {
           std::cerr << "That's the wrong startBin = " << startBin << " no room to get the baseline " << std::endl;
           //return false;

       }
       //find baseline
       double aa=0.; double aa2=0.;
       size_t stopBin = startBin-1;
       for (size_t k=0; k != stopBin; k++) { aa += wfm[k]; aa2 += wfm[k]*wfm[k]; }
       fBaseline = aa/stopBin;
       fBaselineWidth = std::max(0.5, ( std::sqrt((aa2 - stopBin*fBaseline*fBaseline)/(stopBin-1))));  // we do not want zero width..
       //find peak start  
       size_t peakStartBin = startBin;
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
       if (peakStartBin == wfm.size()) { std::cerr << "Couldn't find the start of the peak" << std::endl; }

       fStartBin = peakStartBin;
       fPeakBin = wfm.size();
       fPeakVal = -1.;
       //size_t peakEndBin = ((peakStartBin + 10) >= wfm.size()) ? wfm.size() : (peakStartBin + 10);
       size_t peakEndBin = wfm.size();

       std::vector<double> vecValue;
       std::vector<int> vecIndex;

       for (size_t k=peakStartBin ; k != peakEndBin; k++) {
           const double val = fBaseline - wfm[k];	
           if (val > fPeakVal) { fPeakVal = val; fPeakBin = k;} 
	//   vecValue.push_back(val);
	//   vecIndex.push_back(k);
       }
       vecValue.push_back(fPeakVal);
       vecIndex.push_back(fPeakBin);

       std::cout<<"The maximum of the waveform is "<< fPeakVal << " at " << fPeakBin << std::endl;

       double fSecondMax = -1;
       double fSecondPeakBin = wfm.size();
       for (size_t k=peakStartBin ; k != peakEndBin; k++) {
           const double val = fBaseline - wfm[k];
           if (val == vecValue[0]) continue;
           if (val > fSecondMax && fPeakBin-fSecondPeakBin > 5) { fSecondMax = val; fSecondPeakBin = k;}
       }
       
       double dist = std::abs(fPeakBin-fSecondPeakBin);
       if (dist > 5) {std::cout<<"The distance between local max is " << dist << std::endl;} 

       //take an array of all baseline-subtracted values in the waveform
       //return the maximum value and index
       //remove from the array
       //do it again (multiple times?)
       //see how far the indices are from each other
       //if sufficiently far, multiple peaks found


       //std::max_element(vecValue.begin(),vecValue.end());
       //int relMaxIndex = std::distance(vecValue.begin(),std::max_element(vecValue.begin(),vecValue.end()));
       //int actualMaxIndex = vecIndex[relMaxIndex];

     //  std::cout<<"The maximum of the waveform is "<< std::max_element(vecValue.begin(),vecValue.end()) <<std::endl; //<< " at " << actualMaxIndex <<std::endl;
       

           
    }

    //......................................................................

    bool  calotest::REALfindPeak(const std::vector<uint16_t> &wfm, double signif, size_t startBin){
       //sus = false;
       //check startBin 
       if (startBin < 3) {
           std::cerr << "That's the wrong startBin = " << startBin << " no room to get the baseline " << std::endl;
           return false;
           
        }

       //find baseline
       double aa=0.; double aa2=0.;
       size_t stopBin = startBin-1;
       for (size_t k=0; k != stopBin; k++) { aa += wfm[k]; aa2 += wfm[k]*wfm[k]; }
       fBaseline = aa/stopBin;
       fBaselineWidth = std::max(0.5, ( std::sqrt((aa2 - stopBin*fBaseline*fBaseline)/(stopBin-1))));  // we do not want zero width..

       //find peak start
       //bool negValFound  = false;
       size_t peakStartBin = startBin;
       while (peakStartBin < wfm.size()) {
          const double val =  fBaseline - static_cast<double>(wfm[peakStartBin]);
          if (val/fBaselineWidth > signif) {
             size_t nextBin = peakStartBin + 1;
             if (nextBin < wfm.size()) {
                const double valNext =  fBaseline - static_cast<double>(wfm[nextBin]);
                if (valNext/fBaselineWidth > signif) {
                   //negValFound = true;
                   break;
                }
             }
           }
           peakStartBin++;
       }
       if (peakStartBin == wfm.size()) { std::cerr << "Couldn't find the start of the peak" << std::endl; }

       //define state vector
       std::vector<std::string> fState = {"N"}; 

       //find (first) max amplitude bin 
       fStartBin = peakStartBin;
       fPeakBin = wfm.size();
       fPeakVal = -1.;
       //size_t peakEndBin = ((peakStartBin + 10) >= wfm.size()) ? wfm.size() : (peakStartBin + 10);
       size_t peakEndBin = wfm.size(); 
       size_t stateBinsR = 0; size_t stateBinsF = 0;
      
       for (size_t k=peakStartBin ; k != peakEndBin; k++) {
           const double val = fBaseline - wfm[k];
           if (val > fPeakVal) { fPeakVal = val; fPeakBin = k; stateBinsR++;}
           else {stateBinsF++;}
           if (stateBinsR >= 2) {fState.push_back("R"); stateBinsR=0;}
           if (stateBinsF >= 3 && fState[fState.size()-1] != "F" && std::abs(val/fBaseline) < 0.1) {
		//if (fState[fState.size()-1] == "N" && std::abs(val/fBaseline) < 0.25 ){
                fState.push_back("F"); stateBinsF=0;
           }
       }
       if (fState.size()>4){
	  for (std::string i: fState) {
              STATE += i;
              if (fState[1] == "R" && fState[2] == "R") std::cout<<counter<<std::endl;
          } //; std::cout<<i;}
          //std::cout<<std::endl;   
          return true;
       }
       return false;

       //find (first) max amplitude ADC sum
       double aSum = 0.;
       fEndBin = 1024;
       for (size_t k=peakStartBin ; k != peakEndBin; k++) {
           const double val = fBaseline - wfm[k];
           aSum += val;
           if (((std::abs(val)/(std::abs(aSum) + 1.0e-10)) < fCutIntegration) && ((static_cast<int>(k) - static_cast<int>(fPeakBin)) > 4)) {
              fEndBin = k;
              //fState.push_back["N"]
	      break;
           }
           if ((k-peakStartBin) > 15) {
              std::cerr << " ... Max width reached, assigning End  Bin at   " << k <<  std::endl;
              fEndBin = k;
              break;
            }

       }
       //std::cout<<fEndBin<<std::endl; 
//       return fEndBin;
       //find (first) max amplitude ADC sum
       
       //find other local maxima within peak (if they exist)
       //find end bin(s), check widths
       //repeat for potential second peak
    }

    //......................................................................

    size_t calotest::peakStart(const std::vector<uint16_t> &wfm, double signif, size_t startBin) {
	bool negGlitchFound  = false;
          size_t kNeg = startBin;
          while (kNeg < wfm.size()) {
            const double val =  fBaseline - static_cast<double>(wfm[kNeg]);
            if (val/fBaselineWidth > signif) {
              size_t kNegNext = kNeg + 1;
              if (kNegNext < wfm.size()) {
                const double valNext =  fBaseline - static_cast<double>(wfm[kNegNext]);
                if (valNext/fBaselineWidth > signif) {
                    negGlitchFound = true;
                    break;
                }
              }
            }
            kNeg++;
          }
          if (negGlitchFound) return kNeg;
          return wfm.size();
    }

    //......................................................................
 
    void    calotest::fillInBaseline(const std::vector<uint16_t> &wfm, size_t stopBin){
        double aa=0.; double aa2=0.;
        for (size_t k=0; k != stopBin; k++) { aa += wfm[k]; aa2 += wfm[k]*wfm[k]; }
        fBaseline = aa/stopBin;
        fBaselineWidth = std::max(0.5, ( std::sqrt((aa2 - stopBin*fBaseline*fBaseline)/(stopBin-1))));  // we do not want zero width..
    }

    //......................................................................
    void    calotest::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      //float c1=0; float e1=0; float t1=0;
      //float s1=0; float b1=0;
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::LGCalo);
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
              float adc = wvfm.Baseline()-wvfm.PeakADC();
              float blw = wvfm.BLWidth();
              if (adc > 5*blw) {
                //fHitEffPerChannel->Fill((int)dchan.DetId()+kLGCaloOffset,
                //                        detchan);
                fLGCaloADCDist[detchan]->Fill(adc);
                //now fill waveform plot
                auto adcvals = wvfm.AllADC();
                fNEventsLGCalo[detchan]++;
                for (size_t i=0; i<adcvals.size(); ++i) {
                  fLGCaloWaveForm[detchan]->Fill(i+1,adcvals[i]);
                }
              }
            }
          }
        }
      }
    }

    //......................................................................
    void calotest::analyze(const art::Event& evt)
    {
      ++fNEvents;
      fRun = evt.run();
      fSubRun = evt.subRun();
      fEvtNum = evt.id().event();
      std::string labelStr;

      //std::cout<<"hey"<<std::endl;

      for (int i=0; i<emph::geo::NDetectors; ++i) {

	//std::cout<<"hey"<<std::endl;

        labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
        art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
        try {
          evt.getByLabel(labelStr, wfHandle);

          if (!wfHandle->empty()) {
            //if (i == emph::geo::Trigger){FillTrigPlots(wfHandle); fWantLGCalo(wfHandle);} //FillTrigPlots(wfHandle);
            if (i == emph::geo::Trigger){fWantLGCalo(wfHandle);}
            if (i == emph::geo::LGCalo){ //  FillLGCaloPlots(wfHandle);
            if (want) FillLGCaloPlots(wfHandle);
            }


          }
        }
        catch(...) {
        }
      }
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::lgcalo::calotest)
