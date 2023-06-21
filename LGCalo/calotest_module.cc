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
#include "TTree.h"
#include "TGraphErrors.h"

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

      void    FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void    MakeLGCaloPlots();
      void    fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > &);

      void    getBaseline(const std::vector<uint16_t> &wfm, double signif, size_t startBin, bool calo);
      int     findPeaks(const std::vector<uint16_t> &wfm); //, double sigma, double threshold=0.05);
      void    getIntChargePlots(const std::vector<uint16_t> &adcvals, int detchan);
      void    checkWF(const std::vector<uint16_t> &adcvals,int detchan,bool partial);
      bool    partialPeak(const std::vector<uint16_t> &wfm, size_t k);
      void    getBaselineForPartial(const std::vector<uint16_t> &wfm);
      bool    checkPulse(const std::vector<uint16_t> &wfm);
      void    plotForWidth(const std::vector<uint16_t> &wfm);

      static const unsigned int nChanCal = 9;
      int nchancal = 9;

      //int one=0; int many=0;
      //int trycount = 0;
      //int catchcount = 0;

      TTree* goodWFfrac;

      TH1F* fNext;   
      TH1F* fSignal;
      TObjArray fManyPeaks;
      TObjArray fOnePeak;
      TObjArray fChannel0;
      TObjArray fChannel4;
      TObjArray fPartial;
      int peakctr=0;
      int notlost=0;
      int chan0ctr=0;
      int chan4ctr=0;
      int npartial=0;
      TGraphErrors* check;

      TH1F* fLGCaloADCDist[nChanCal];
      std::vector<unsigned int> fNEventsLGCalo;
      TH1F* fLGCaloWaveForm[nChanCal];
      TH1F* fLGCaloIntCharge[nChanCal];
      TH1F* fLGCaloTotalCharge;
      TH1F* fLGCaloCenterRatio;
      TH1F* fLGCaloEdgeRatio;
      TH2F*  fLGCaloIntChgVsRatio;
      TH1F* fPFW;
 
      bool halfpulse = false;
      bool fMakeWaveFormPlots;
      bool want;
      double gwff;
      bool badrun;
      size_t fCheckDeriv = 3;

      //std::vector<double> fTrigADCs; 
 
      emph::cmap::ChannelMap* fChannelMap;
      runhist::RunHistory* fRunHistory;
      
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
  
      static const unsigned int nChanTrig = 4;

      double fBaseline = 0;
      double fBaselineWidth = 0;
      size_t peakStartBin = 0;

      //float t1=0;
      float c1=0; float e1=0; float t1=0;
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
      std::cout<<"Begin Job"<<std::endl;
      fNEvents= 0;
      std::string labelStr;

      MakeLGCaloPlots();
      art::ServiceHandle<art::TFileService> tfs;
      //goodWFfrac = tfs->make<TTree>("goodWFfrac","");
      //goodWFfrac->Branch("gwff",&gwff,"gwff/I");

      fNext = tfs->make<TH1F>("Next","Placeholder Trigger Waveform",108,0.,108.); //600,0.,120.);
      fSignal = tfs->make<TH1F>("Signal","Signal",108,0.,108.);
      //check = tfs->make<TGraphErrors>("check","check",6,vk,vpred,0,vres);

      for (int i=0; i<nchancal; ++i){
	  fLGCaloADCDist[i] = tfs->make<TH1F>(Form("LGCaloADCDist_%d",i),Form("LG Calo ADC Distribution %d",i),512,0.,4095.);	    
      }
	  //fLGCaloADCDist.push_back(fLGCaloADCDist_i);	
      for (int i=0; i<nchancal; ++i){
          fLGCaloWaveForm[i] = tfs->make<TH1F>(Form("LGCaloWaveForm_%d",i),Form("LG Calo Waveform %d",i),108,0.,108.);
          //fLGCaloWaveForm.push_back(fLGCaloWaveForm_i); 
          fNEventsLGCalo.push_back(0);
      }
      for (int i=0; i<nchancal; ++i){
          fLGCaloIntCharge[i] = tfs->make<TH1F>(Form("LGCaloIntCharge_%d",i),Form("LG Calo Integrated Charge %d",i),200,0.,26000.);
	  //fNEventsLGCalo.push_back(0);
      }
      fLGCaloTotalCharge = tfs->make<TH1F>("LGCaloTotalCharge","LG Calo Total Integrated Charge",200,0.,26000.);
      fLGCaloCenterRatio = tfs->make<TH1F>("LGCaloCenterRatio","LG Calo Center Ratio",100,0.,1.);
      fLGCaloEdgeRatio = tfs->make<TH1F>("LGCaloEdgeRatio","LG Calo Edge Ratio",100,0.,1.);
      fLGCaloIntChgVsRatio = tfs->make<TH2F>("LGCaloIntChgVsCenterRatio","Integrated Charge Vs Center Ratio",100,0.,1.,200,0.,26000.);
      fPFW = tfs->make<TH1F>("PFW","Dist for Width",2000,-1000.,1000.);
    }

    //......................................................................

    void calotest::beginRun(art::Run const& run)
    {
     std::cout<<"beginRun"<<std::endl;

      // initialize channel map
      fChannelMap = new emph::cmap::ChannelMap();
      fRunHistory = new runhist::RunHistory(run.run());
      //std::cout<<"Gonna load"<<std::endl;
      fChannelMap->LoadMap(fRunHistory->ChanFile());
      //std::cout<<"Loaded"<<std::endl;
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
     //std::cout<<"One peak: "<<one<<std::endl;
     //std::cout<<"Many peaks: "<<many<<std::endl;
     //std::cout<<"Tried and succeeded: "<<trycount<<std::endl;
     //std::cout<<"Caught...: "<<catchcount<<std::endl;	

     //fManyPeaks.Write();
     fOnePeak.Write();
     fChannel0.Write();
//     fPartial.Write();
     //fChannel4.Write();
     //std::cout<<"chan0ctr: "<<chan0ctr<<std::endl;
     //std::cout<<"chan4ctr: "<<chan4ctr<<std::endl;

     std::cout<<"There are "<<peakctr<<" waveforms."<<std::endl;
     std::cout<<"Not lost: "<<notlost<<std::endl;
     gwff = double(notlost)/double(peakctr);
     std::cout<<gwff*100.<<"% of waveforms are available to use."<<std::endl;


     art::ServiceHandle<art::TFileService> tfs; 
     goodWFfrac = tfs->make<TTree>("goodWFfrac","");
     goodWFfrac->Branch("gwff",&gwff,0,1);
     goodWFfrac->Fill();
//     peakcounts = tfs->make<TTree>("peakcounts","");
//     peakcounts->Branch("manypeaks",&fManyPeaks,32000,0);
//     peakcounts->Branch("onepeak",&fOnePeak,"fOnePeak/I");

     if (fNEvents > 0) {
        float scale = 1./float(fNEvents);
      for (int i=0; i<nchancal; ++i) { //fLGCaloWaveForm.size(); ++i) {
      //for (size_t i=0; i<fLGCaloWaveForm.size(); ++i) {
          scale = 1./float(fNEventsLGCalo[i]);
          fLGCaloWaveForm[i]->Scale(scale);
	  fLGCaloWaveForm[i]->SetOption("hist");
	  //fLGCaloWaveForm[i]->Print("all");
        }
      }
      fSignal->SetTitle("I'm filler! Ignore me");

//     fNTriggerLGArray->SetBinContent(1.5,1.5,fNEventsLGCalo[6]);
//     fNTriggerLGArray->SetBinContent(2.5,2.5,fNEventsLGCalo[4]);
//     fNTriggerLGArray->SetBinContent(3.5,3.5,fNEventsLGCalo[2]);
//     fNTriggerLGArray->SetBinContent(1.5,3.5,fNEventsLGCalo[0]);
//     fNTriggerLGArray->SetBinContent(2.5,3.5,fNEventsLGCalo[1]);
//     fNTriggerLGArray->SetBinContent(1.5,2.5,fNEventsLGCalo[3]);
//     fNTriggerLGArray->SetBinContent(3.5,2.5,fNEventsLGCalo[5]);
//     fNTriggerLGArray->SetBinContent(2.5,1.5,fNEventsLGCalo[7]);
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
     if (fRun <2000){
       int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
       emph::cmap::FEBoardType boardType = emph::cmap::V1720;
       emph::cmap::EChannel echan;
       echan.SetBoardType(boardType);

//      if (fEvtNum != 35) fMakeWaveFormPlots = false; //DEB
//      else fMakeWaveFormPlots = true;
//3545       fMakeWaveFormPlots = true;

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
           
            //if (fRun <2000){ 
	      if (detchan >= 0 && detchan < nchan) {
	        std::vector<uint16_t> adcvals = wvfm.AllADC();
                getBaseline(adcvals,8.0,10,false);
		/*if (findPeaks(adcvals) > 1){
 		   for (size_t i=0; i<adcvals.size(); ++i) {
		      fNext->Fill(i,fBaseline-adcvals[i]);
                   }
		   fSignal->SetTitle(Form("Event: %d",fEvtNum));
		   fManyPeaks.Add(fSignal); 
                   fNext->Reset();
		   //many++;
		}
		if (findPeaks(adcvals) == 1){
		   for (size_t i=0; i<adcvals.size(); ++i) {
                      fNext->Fill(i,fBaseline-adcvals[i]);
                   }
                   fSignal->SetTitle(Form("Event: %d",fEvtNum));
		   peakctr++;
 		   if (peakctr <= 200){
                      fOnePeak.Add(fSignal); 
	           }
                   fNext->Reset();
		   //one++;
		}*/
		
                //cut on trigger waveform
                float adc = wvfm.Baseline()-wvfm.PeakADC();
                float blw = wvfm.BLWidth();
                if (adc > 5*blw && findPeaks(adcvals) == 1){
                 //fTriggerADCDist[detchan]->Fill(adc);
                 //want = true;
                  if (adc > 500) want=true; //return true;
                }
       	      }
            }
          }
        }
      }
      //else want=true;
      else{
	  //fMakeWaveFormPlots = true;
	  badrun = true;
	  //badrun = false;
	  //if (fEvtNum == 19815 || fEvtNum == 3545) fMakeWaveFormPlots = true;
	  //else fMakeWaveFormPlots = false;

	  //if (fEvtNum != 9126) fMakeWaveFormPlots = false; //we don't want, pulse in first 30 or so bins
	  //if (fEvtNum != 19815) fMakeWaveFormPlots = false; //don't want, just a big pulse in the middle of the window
          //if (fEvtNum != 3545) fMakeWaveFormPlots = false; //example of WF we want
          //if (fEvtNum != 11289) fMakeWaveFormPlots = false; //WF we want
          //if (fEvtNum != 11150) fMakeWaveFormPlots = false; //we don't want, WF flat in the beginning
          //if (fEvtNum != 6924) fMakeWaveFormPlots = false; //example of WF we don't want
          //if (fEvtNum != 20168) fMakeWaveFormPlots = false; //saturated double peak
	  //if (fEvtNum !=20195) fMakeWaveFormPlots = false; //WF we want
          //if (fEvtNum !=20117) fMakeWaveFormPlots = false; //WF we want
          if (fEvtNum !=20033) fMakeWaveFormPlots = false; //WF we want
	  //if (fEvtNum !=19838) fMakeWaveFormPlots = false; //WF we want
	  else fMakeWaveFormPlots = true;
          //want = true; 
     }
    }

    //......................................................................
   
     void calotest::plotForWidth(const std::vector<uint16_t> &wfm){
          
         for (size_t k=0; k<wfm.size(); k++){
	     fPFW->Fill(fBaseline - wfm[k]);
	 }

    }     

    //......................................................................
    
    bool calotest::checkPulse(const std::vector<uint16_t> &wfm){

//	int nc = 0;
 
/*	for (size_t k=0; k<30;k++){
	 //stop if there is a negative derivative
         if ((fBaseline - wfm[k]) - (fBaseline - wfm[k+1]) < 0 ){ //fBaseline here unnecessary but fine for now
	    if ((fBaseline - wfm[k+1]) - (fBaseline - wfm[k+2]) < 0 ){
               std::cout<<"Oh it's bad for her."<<std::endl; 
               return false;
 //           break;
            }
	 }
	}
*/
        for (size_t k=0; k<30;k++){
            for (size_t j=0; j<fCheckDeriv; j++){
		if ((fBaseline - wfm[k+j]) - (fBaseline - wfm[k+j+1]) < 0){
		     if (j==fCheckDeriv-1){
			std::cout<<"Oh it's bad for her."<<std::endl;
                        return false;
	             }
                }
		else break;
	    }
	}

	std::cout<<"Love her."<<std::endl;

        //check that there is a pulse and it's not just a flat waveform in these bins
        //so check for positive
	for (size_t k=0; k<5;k++){
            for (size_t j=0; j<fCheckDeriv; j++){
	        if (abs(fBaseline - wfm[k+j])/fBaseline < 0.05){
                     if (j==fCheckDeriv-1){
			std::cout<<"fBaseline: "<<fBaseline<<std::endl;
			std::cout<<"OH ITS BAD FOR HER: "<<k+j<<std::endl;
                        return false;
                     }
                }
                else break;
            }
        }

	std::cout<<"LOVE HER"<<std::endl;
	return true;



/*
	 //greater than 5% difference, there could be a pulse
         if (abs(fBaseline - wfm[k])/fBaseline > 0.05){
           //can vary as much as 30% but within the first 10 bins
	   if (abs(fBaseline - wfm[k])/fBaseline < 0.30 && k<10){
	      nc++;
	   //std::cout<<abs(fBaseline - wfm[k])/fBaseline<<std::endl;
	   }
	 }
	}
        //if it happens at least 5 bins then it's the half-pulse we want
	if (nc > 5){
	  halfpulse=true; 
	  //std::cout<<"pulsie"<<std::endl; 
	  return true;}
	else{
	  //std::cout<<"no pulsie"<<std::endl; 
	  return false;}
*/
    }

    //......................................................................

    void calotest::getBaselineForPartial(const std::vector<uint16_t> &wfm){
        //find most common bin i.e. the mode
	std::map<int, int> counters;
        for(auto i: wfm)
        {
	    if (i!=0) ++counters[i];
        }
        using pair_type = decltype(counters)::value_type;
	auto pr = std::max_element(std::begin(counters), std::end(counters),[] (const pair_type & p1, const pair_type & p2) {return p1.second < p2.second;});
        //std::cout << "A mode of the vector: " << pr->first << '\n';
	//std::cout << "with " << pr->second << " counts" <<std::endl;

        //first get baseline width
        auto maxw = *max_element(std::begin(wfm), std::end(wfm));
        fBaselineWidth = abs(pr->first-maxw);
        //std::cout<<"maxw: "<<maxw<<std::endl;
	//std::cout<<fBaselineWidth<<std::endl;

        //NOT THIS alculate baseline from within 2% difference of the mode
        //calculate baseline to be within the baseline width of the mode (a number of bins)
        //then get the average of all values that pass those criteria
	int nk = 0;
	double ksum = 0;
	for (size_t k=0; k< wfm.size();k++){
            if (abs(wfm[k]-pr->first) < fBaselineWidth){
	    //if (abs(wfm[k]-pr->first)/(double)pr->first < 0.02){
	
		//std::cout<<abs(wfm[k]-pr->first)/pr->first<<std::endl;
		nk++;
		ksum += wfm[k];
	    }            
	}	

        //std::cout << "A 2nd mode of the vector: " << pr->second << '\n';
        //weighted average of most common three values?
        fBaseline = ksum/nk; 

        //std::cout<<"fBaselineWidth: "<<fBaselineWidth<<std::endl; 
        //std::cout<<"fBaseline: "<<fBaseline<<std::endl;
    }
    //......................................................................

    void calotest::getBaseline(const std::vector<uint16_t> &wfm, double signif, size_t startBin, bool calo){
	
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
 
     int calotest::findPeaks(const std::vector<uint16_t> &wfm){ //, double sigma, double threshold){
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
         //    trycount++;
         } catch (...){} //{catchcount++;}//{std::cout<<"Caught!  Ya scoundrel!"<<std::endl;}

	 //for (size_t i=0; i<wfm.size(); ++i) {
         //     if (wfm[i]==0){nFound=0; std::cout<<"ZERO BITCH"<<std::endl; break;}
         //}
	
	 return nFound; 
     }
 
    //......................................................................

    void    calotest::getIntChargePlots(const std::vector<uint16_t> &adcvals, int detchan){
         float x1=0;
	 if (halfpulse==false) x1=peakStartBin; 
	 //else x1=0;
         float nsamp=35; //25; //range where the signal is
         float avg = 0; int ic = 0;
         float sum=0;
         //float c1=0; float e1=0; //float t1=0;
         //t1 = 0;

         for (size_t i=0; i<adcvals.size(); ++i){
             if (i<size_t(x1) || i>size_t(x1+nsamp)) avg += float(adcvals[i]), ++ic;
         }

         avg /= float(ic); //baseline for each signal

         for (size_t i=x1; i<size_t(x1+nsamp) && i<adcvals.size(); ++i){
             sum += (avg-adcvals[i]); //total integrated charge over the range
         }
         fLGCaloIntCharge[detchan]->Fill(sum);
         fLGCaloTotalCharge->Fill(sum);

         //now fill the energy ratio plots
         if (sum!=0){
           t1 += sum;
           if (detchan==4) {
	     c1 += sum;
             fLGCaloCenterRatio->Fill(c1/t1);
           }
           if (detchan!=4) {
	     e1 += sum;
             fLGCaloEdgeRatio->Fill(e1/t1);
           }
	   fLGCaloIntChgVsRatio->Fill(c1/t1,sum);
         }
    }

    //......................................................................

    void    calotest::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      //float c1=0; float e1=0; float t1=0;
      //float s1=0; float b1=0;
      t1=0; c1=0; e1=0;
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
                  fLGCaloWaveForm[detchan]->Fill(i,adcvals[i]); //i+1,adcvals[i]);
                }
		if (detchan == 4){peakctr++;}
		getBaseline(adcvals,8.0,10,true);
/*		TH1F wfcheck("wfcheck","wfmcheck",adcvals.size(),0.0,adcvals.size());
 
                for (size_t i=0; i<adcvals.size(); ++i) {
                    wfcheck.Fill(i+1,adcvals[i]);
                }
	        wfcheck.SetTitle(Form("Event: %d",fEvtNum));
                wfcheck.SetOption("hist");
                peakctr++;
                fNext = (TH1F*) wfcheck.Clone();
                if (peakctr <= 200){
                   fOnePeak.Add(fNext);
                }
*/
	        //if (detchan == 4){
                //   if (adcvals[0]/adcvals[adcvals.size()-1] < 0.95){
		//	chan4ctr++;
		//	checkWF(adcvals,detchan);
                //   }
		//}
/*peaks
	        if (findPeaks(adcvals) == 1){
		   if (peakStartBin < adcvals.size()-25){
	              if (detchan == 4 ){
			 //if (adcvals[0]/adcvals[adcvals.size()-1] < 0.95){ //function called partial peak?
		         notlost++;
		         checkWF(adcvals,detchan,false);}
		      }
		}
peaks*/

		if(badrun){
		std::vector<uint16_t> adcpartial;
                  for (size_t i=0; i<20; ++i) {
                      adcpartial.push_back(adcvals[i]);
                  }
		getBaselineForPartial(adcvals);
		//std::cout<<checkPulse(adcvals)<<std::endl;
	        //partialPeak(adcvals,0);
		//if (partialPeak(adcpartial,0) && detchan==4) std::cout<<fEvtNum<<std::endl;

		if (detchan == 4){
                   if (findPeaks(adcpartial)==0){ //no peak in first 20 bins (arbitrary)
		      plotForWidth(adcvals);	
		      if (checkPulse(adcvals)){ //2nd check for negative derivative
		         checkWF(adcvals,detchan,false);
		         getIntChargePlots(adcvals,detchan);
			 notlost++;
                      }
		   }
		   //if (findPeaks(adcvals)){
		   //   notlost++;
		   //   checkWF(adcvals,detchan,false);
		   //}
		//if (fEvtNum == 19815 && detchan == 4){
		//if (fEvtNum == 3545 && detchan == 4){
		  //std::vector<uint16_t> adcpartial;
		  //for (size_t i=0; i<20; ++i) {
                  //    adcpartial.push_back(adcvals[i]);
                  //}
//3545	        //std::cout<<(findPeaks(adcpartial))<<std::endl;
//3545		  if (partialPeak(adcpartial,0)){
//		partialPeak(adcvals,0);
//5-9-2023		if (partialPeak(adcpartial,0)) checkWF(adcpartial,detchan,false);   
//3545		//if (findPeaks(adcpartial) != 0){
//3545	       	  	npartial++;
//3545			checkWF(adcvals,detchan,true);
//3545		  }
		}
		halfpulse = false;
/*		if (findPeaks(adcvals) == 1){
                   if (peakStartBin < adcvals.size()-25){
	              if (detchan == 0){
                         chan0ctr++;
		         checkWF(adcvals,detchan,false);
		      }
		   }
	        }*/
		//if (adcvals[0]/adcvals[adcvals.size()-1] < 0.95){
		//   if (detchan == 4){
		//      chan4ctr++;
		//      checkWF(adcvals,detchan);
		//   }
		//}
		}				
	
		else getIntChargePlots(adcvals,detchan);
              }
            }
          }
        }
      }
    }
 
    //......................................................................

    bool calotest::partialPeak(const std::vector<uint16_t> &wfm, size_t k){ //double bl, const double max_guess, size_t k){


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
/*
	     std::vector<double> vk; 
             for (int i=0; i<6; i++){ vk.push_back(i);}

	     std::vector<double> vpred; 
             vpred.push_back(pred4); vpred.push_back(pred3); vpred.push_back(pred2);
	     vpred.push_back(pred1); vpred.push_back(pred0); vpred.push_back(pred5);

             std::vector<double> vres;
	     vpred.push_back(res4); vpred.push_back(res3); vpred.push_back(res2);
             vpred.push_back(res1); vpred.push_back(res0); vpred.push_back(res5);

	     TGraphErrors* check = new TGraphErrors(6,vk,vpred,0,vres);
*/
             double res_rms = std::sqrt((res0*res0 + res1*res1 + res2*res2 + res3*res3 + res4*res4)/5.);
             double res_3x = 3*res_rms;


             if (std::abs(res5) < res_3x) {std::cout<<"No pulse"<<std::endl; 
	return false;}
             else {std::cout<<"Pulse found"<<std::endl; 
		return true;}
    }

    //......................................................................

    void    calotest::checkWF(const std::vector<uint16_t> &adcvals,int detchan,bool partial)
    {
            TH1F wfcheck("wfcheck","wfmcheck",adcvals.size(),0.0,adcvals.size());

//            for (size_t i=0; i<30; ++i) {
            for (size_t i=0; i<adcvals.size(); ++i) {
                wfcheck.Fill(i+1,adcvals[i]);
            }
            wfcheck.SetTitle(Form("Event: %d, Detchan: %d",fEvtNum,detchan));
            wfcheck.SetOption("hist");
            //peakctr++;

	    //check->Draw("same");

            fNext = (TH1F*) wfcheck.Clone();

	    if (partial){
//3545	    if (npartial <= 25){
//3545	    	   fPartial.Add(fNext);
//3545	   	}
	    }

	    if (detchan == 4){
               //if (notlost>100 && notlost <= 150){
//det4		  std::cout<<notlost<<std::endl;
                  fOnePeak.Add(fNext);
//               }
	       //if (chan4ctr <= 50){
               //   fChannel4.Add(fNext);
               //}
	    }
	    if (detchan == 0){
	       if (chan0ctr <= 50){
 	          fChannel0.Add(fNext);
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

      for (int i=0; i<emph::geo::NDetectors; ++i) {

        labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
        art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
        try {
          evt.getByLabel(labelStr, wfHandle);

          if (!wfHandle->empty()) {
            if (i == emph::geo::Trigger){fWantLGCalo(wfHandle);}
            if (i == emph::geo::LGCalo){ //  FillLGCaloPlots(wfHandle);
               if (want) FillLGCaloPlots(wfHandle);
            }
          }
        }
        catch(...) {
        }
      }
      //goodWFfrac->Fill();
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::lgcalo::calotest)
