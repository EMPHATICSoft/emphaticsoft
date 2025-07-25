////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create online monitoring plots
/// \author  $Author: jpaley $
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
#include "art/Framework/Core/EDAnalyzer.h"
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
#include "OnlineMonitoring/plotter/HistoSet.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/Settings.h"
#include "OnlineMonitoring/util/Ticker.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "RecoBase/ADC.h"
#include "SignalTime/SignalTime.h"
#include "ADCUtils/ADCUtils.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace onmon {

    ///
    /// A class for communication with the viewer via shared memory segment
    ///
    class OnMonProdIPC : public onmon::IPC
    {
    public:
      OnMonProdIPC(int m, const char* hdl);
    private:
      TH1F* FindTH1F(const char* nm);
      TH2F* FindTH2F(const char* nm);
      void  HistoList(std::list<std::string>& hlist);
    };

    class OnMonPlotter : public art::EDAnalyzer {
    public:
      explicit OnMonPlotter(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~OnMonPlotter();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const& /*run*/);
      void endRun(art::Run const&);
      void endSubRun(art::SubRun const&);
      void endJob();

    private:
      void   FillGasCkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > &,
			 art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillSSDPlots(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > &);
      void   FillARICHPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      //void   FillToFPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &,
      //art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);

      void   MakeGasCkovPlots();
      void   MakeBACkovPlots();
      void   MakeT0Plots();
      void   MakeSSDPlots();
      void   MakeARICHPlots();
      void   MakeLGCaloPlots();
      void   MakeRPCPlots();
      void   MakeToFPlots();
      void   MakeTrigPlots();

      void HandleRequestsThread();

      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
      emph::st::SignalTime stmap;

      OnMonProdIPC* fIPC;         ///< Communicates with viewer
      std::string   fCSVFile;     ///< Name of csv file that defines histograms to make/view
      std::string   fSHMname;     ///< Shared memory for communication
      bool          fuseSHM;      ///< Use SHM to communicate with a viewer?
      std::atomic<bool> fSHMThreadRunning;
      std::unique_ptr<std::thread> fSHMThreadPtr;
      bool          fTickerOn;    ///< Turned on in the control room
      art::Timestamp fFirstEventTime;
      art::Timestamp fLastEventTime;

      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;
      
      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanT0  = 20;
      static const unsigned int nChanRPC = 16;
      static const unsigned int nChanCal = 9;
      static const unsigned int nChanBACkov = 6;
      static const unsigned int nChanGasCkov = 3;
      static const unsigned int nChanTrig = 4;

      static const unsigned int kTrigOffset = 0;
      static const unsigned int kGasCkovOffset = 0;
      static const unsigned int kBACkovOffset = 0;
      static const unsigned int kT0Offset = 0;
      static const unsigned int kRPCOffset = 1;
      static const unsigned int kSSDOffset = 1;
      static const unsigned int kARICHOffset = kSSDOffset+27;
      static const unsigned int kLGCaloOffset = kARICHOffset+8;
      // define histograms

      TH2F* fNRawObjectsHisto;  
      TH1F* fNTriggerVsDet;
      TH2F* fTriggerVsSubrun;
      TH2F* fHitEffPerChannel;
      TH2F* fNTriggerLGArray;      
      TH2F* fT0TDCChanVsADCChan;
      TH1F* fT0RisingTimeSum;
      TH1F* fT0FallingTimeSum;
      TH1F* fRPCRisingTimeSum;
      TH1F* fRPCFallingTimeSum;
      TH1F* fT0ADCDist[nChanT0];
      TH1F* fT0NTDC[nChanT0];
      TH2F* fT0TDCVsADC[nChanT0];
      TH1F* fT0RisingTime[nChanT0];
      TH1F* fT0FallingTime[nChanT0];
      TH1F* fRPCNTDC[nChanRPC];
      TH1F* fRPCRisingTime[nChanRPC];
      TH1F* fRPCFallingTime[nChanRPC];
      TH1F* fRPCTOT[nChanRPC];
      TH1F* fLGCaloADCDist[nChanCal];
      TH1F* fBACkovQDist[nChanBACkov];
      std::vector<TH1F*> fBACkovWaveForm;
      std::vector<unsigned int> fNEventsBACkov;
      std::vector<unsigned int> fNEventsLGCalo;
      std::vector<TH1F*> fLGCaloWaveForm;
      std::vector<TH1F*> fLGCaloIntCharge;
      TH1F* fLGCaloTotalCharge;
      TH1F* fLGCaloCenterRatio;
      TH1F* fLGCaloEdgeRatio;
      TH2F*  fLGCaloIntChgVsRatio;
      TH1F* fGasCkovADCDist[nChanGasCkov];
      TH1F* fTriggerADCDist[nChanTrig];
      TH1F* fTriggerEff;
      TH1F* fTriggerDeltaT;
      TH1F* fTriggerTime;
      std::vector<TH1F*> fSSDProf;
      std::vector<TH1F*> fSSDNHit;

      TH1F*  fARICHNHits;
      TH2F*  fTRB3NHitsPerChannel;
      TH2F*  fARICHNHitsPxl;
      TH1F*  fARICHHitTimes;
      TH1F*  fARICHHitToT;
      TH1F*  fARICHNTrails;

      bool fMakeWaveFormPlots;
      bool fMakeTRB3Plots;
      bool fMakeSSDPlots;

    };

    OnMonProdIPC::OnMonProdIPC(int m, const char* hdl) : onmon::IPC(m, hdl) { }
    TH1F* OnMonProdIPC::FindTH1F(const char* nm) {
      return HistoSet::Instance().FindTH1F(nm);
    }
    TH2F* OnMonProdIPC::FindTH2F(const char* nm) {
      return HistoSet::Instance().FindTH2F(nm);
    }
    void OnMonProdIPC::HistoList(std::list<std::string>& hlist) {
      HistoSet::Instance().GetNames(hlist);
    }

    //.......................................................................
    OnMonPlotter::OnMonPlotter(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
	fCSVFile           (pset.get<std::string>("CSVFile")),
	fSHMname           (pset.get<std::string>("SHMHandle")),
	fuseSHM            (pset.get<bool>("useSHM")),
	fTickerOn          (pset.get<bool>("TickerOn")),
	fMakeWaveFormPlots (pset.get<bool>("makeWaveFormPlots",true)),
	fMakeTRB3Plots     (pset.get<bool>("makeTRB3Plots",true)),
	fMakeSSDPlots      (pset.get<bool>("makeSSDPlots",false))
    {

      //if (fIPC) delete fIPC;
      if (fuseSHM) fIPC = new OnMonProdIPC(kIPC_SERVER, fSHMname.c_str());
      else fIPC = nullptr;
      
      // try to find the correct path to the .csv file.
      std::string csvpath;
      for (int itry=0; itry<3; ++itry) {
         switch (itry) {
         case 0: csvpath = "./";      break;
         case 1: csvpath = "./util/"; break;
         case 2:
          csvpath = getenv("CETPKG_SOURCE");
          csvpath += "/OnlineMonitoring/util/";
          break;
         }
         csvpath += fCSVFile;
         if (access(csvpath.c_str(), F_OK)!=-1) {
          Settings::Instance().fCSVFile = csvpath;
          break;
         }
      } // loop on directory attempts

      Settings::Instance().fDet = kEMPH;

      HistoTable::Instance(Settings::Instance().fCSVFile.c_str(),
		       Settings::Instance().fDet);

    }

    //......................................................................
    OnMonPlotter::~OnMonPlotter()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................

    void OnMonPlotter::beginRun(art::Run const& /*run*/) {

      if(fSHMThreadPtr && fSHMThreadPtr->joinable()) {
	fSHMThreadRunning = false;
	fSHMThreadPtr->join();}
      fSHMThreadRunning = true;
      fSHMThreadPtr.reset(new std::thread(&emph::onmon::OnMonPlotter::HandleRequestsThread, this)); 
    }
    
    //......................................................................
    
    void OnMonPlotter::endRun(art::Run const&) {     
      if(fSHMThreadPtr && fSHMThreadPtr->joinable()) {
	fSHMThreadRunning = false;
	fSHMThreadPtr->join();
      } 
    }

    //......................................................................

    void OnMonPlotter::HandleRequestsThread() {
      if(!fuseSHM) return;
      while(fSHMThreadRunning) {
	fIPC->HandleRequests();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    }
    
    //......................................................................

    void OnMonPlotter::beginJob()
    {
      fNEvents=0;
    
      //
      // Make all-detector plots
      //
      HistoSet& h = HistoSet::Instance();
      fNRawObjectsHisto = h.GetTH2F("NRawObjectsHisto");
      fNTriggerVsDet    = h.GetTH1F("NTriggerVsDet");
      fLGCaloIntChgVsRatio = h.GetTH2F("LGCaloIntChgVsRatio");
      fNTriggerLGArray     = h.GetTH2F("NTriggerLGArray"); //
      fTriggerVsSubrun  = h.GetTH2F("TriggerVsSubrun");
      fHitEffPerChannel = h.GetTH2F("HitEffPerChannel");
      fTRB3NHitsPerChannel = h.GetTH2F("TRB3NHitsPerChannel");

      // label x-axis
      std::string labelStr;
      int i=0;
      for (; i<emph::geo::NDetectors; ++i) {
        int j=i;
        if (i > emph::geo::T0)
          j=i+1;
	labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	if (i == emph::geo::T0) labelStr += "ADC";
	fNTriggerVsDet->GetXaxis()->SetBinLabel(j+1,labelStr.c_str());
	fNRawObjectsHisto->GetXaxis()->SetBinLabel(j+1,labelStr.c_str());
        fNTriggerVsDet->GetXaxis()->LabelsOption("h");
        fNRawObjectsHisto->GetXaxis()->LabelsOption("h");
        if (i == emph::geo::USLAPPD || i == emph::geo::DSLAPPD) {
          fNTriggerVsDet->GetXaxis()->ChangeLabel(j+1,350,-1,0,-1,-1,labelStr.c_str());
          fNRawObjectsHisto->GetXaxis()->ChangeLabel(j+1,350,-1,0,-1,-1,labelStr.c_str());
        }
      }
      // now add T0TDC
      labelStr = emph::geo::DetInfo::Name(emph::geo::T0) + "TDC";
      fNTriggerVsDet->GetXaxis()->SetBinLabel(emph::geo::T0+2,labelStr.c_str());
      fNRawObjectsHisto->GetXaxis()->SetBinLabel(emph::geo::T0+2,labelStr.c_str());
      
      //label x-axis for HitEff plot.
      i=0;
      int j=1;
      for (; i<emph::geo::NDetectors; ++i) {
	labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	if (i <= emph::geo::BACkov) {
	  labelStr += " ADC";
	  fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,labelStr.c_str());
	}
	else if (i == emph::geo::T0) {
	  labelStr += " ADC";
	  fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,labelStr.c_str());
	  labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	  labelStr += " TDC";
	  fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,labelStr.c_str());
	}
	else if (i == emph::geo::RPC) {
	  labelStr += " TDC";
	  fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,labelStr.c_str());
	}
	else if (i == emph::geo::SSD) {
	  int nchannel = emph::geo::DetInfo::NChannel(emph::geo::SSD);
	  char label[64];
	  for (int ic=0; ic<nchannel; ++ic) {
	    labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	    sprintf(label,"%s %02d",labelStr.c_str(),ic+1);
	    fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,label);	    
	  }
	}
	else if (i == emph::geo::ARICH) {
	  char label[64];
	  for (int ic=0; ic<9; ++ic) {
	    labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	    sprintf(label,"%s %d",labelStr.c_str(),ic+1);
	    fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,label);
	  }
	}
	else if (i == emph::geo::LGCalo) {
	  labelStr += " ADC";
	  fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,labelStr.c_str());
	}
	else if (i == emph::geo::USLAPPD || i == emph::geo::DSLAPPD) {
	  fHitEffPerChannel->GetXaxis()->SetBinLabel(j++,labelStr.c_str());
        } 
      }
      for (int bin=1; bin < fHitEffPerChannel->GetNbinsX()+1; bin++) {
        fHitEffPerChannel->GetXaxis()
          ->ChangeLabel(bin,270,-1,2,-1,-1,
                        fHitEffPerChannel->GetXaxis()->GetBinLabel(bin));
      }
    
      MakeGasCkovPlots();
      MakeBACkovPlots();
      MakeT0Plots();
      MakeSSDPlots();
      MakeARICHPlots();
      MakeLGCaloPlots();
      MakeRPCPlots();
      MakeTrigPlots();

      // T0nTDC
      if (fMakeTRB3Plots) {
      	int nchannel = emph::geo::DetInfo::NChannel(emph::geo::RPC);
      	for (int i=0; i<nchannel; ++i) {
      	  labelStr = "RPC Channel "+std::to_string(i)+" Number of TDCs";
      	  fRPCNTDC[i]->GetXaxis()->SetTitle(labelStr.c_str());
      	}
      }
    
    }
    
    //......................................................................

    void OnMonPlotter::endSubRun(const art::SubRun&)
    {
      if (fNEvents > 0) {
	float scale = 1./float(fNEvents);
	fTriggerEff->Scale(scale);
	for (size_t i=0; i<fBACkovWaveForm.size(); ++i) {
	  scale = 1./float(fNEventsBACkov[i]);
	  fBACkovWaveForm[i]->Scale(scale);
	  fNEventsBACkov[i]=0;
	}

	fNTriggerLGArray->SetBinContent(1.5,1.5,fNEventsLGCalo[6]);
	fNTriggerLGArray->SetBinContent(2.5,2.5,fNEventsLGCalo[4]);
	fNTriggerLGArray->SetBinContent(3.5,3.5,fNEventsLGCalo[2]);
	fNTriggerLGArray->SetBinContent(1.5,3.5,fNEventsLGCalo[0]);
	fNTriggerLGArray->SetBinContent(2.5,3.5,fNEventsLGCalo[1]);
	fNTriggerLGArray->SetBinContent(1.5,2.5,fNEventsLGCalo[3]);
	fNTriggerLGArray->SetBinContent(3.5,2.5,fNEventsLGCalo[5]);
	fNTriggerLGArray->SetBinContent(2.5,1.5,fNEventsLGCalo[7]);
	fNTriggerLGArray->SetBinContent(3.5,1.5,fNEventsLGCalo[8]);
        for (size_t i=0; i<fLGCaloWaveForm.size(); ++i) {
          scale = 1./float(fNEventsLGCalo[i]);
          fLGCaloWaveForm[i]->Scale(scale);
	  fNEventsLGCalo[i]=0;
        }
	fNEvents=0;
      }

      std::cout<<"Writing file for run/subrun: " << fRun << "/" << fSubrun << std::endl;
      char filename[32];
      sprintf(filename,"onmon_r%d_s%d.root", fRun, fSubrun);
      TFile* f = new TFile(filename,"RECREATE");
      HistoSet::Instance().WriteToRootFile(f);
      f->Close();
      delete f; f=0;

    }
      

    //......................................................................

    void OnMonPlotter::endJob()
    {
      char filename[32];
      sprintf(filename,"onmon_r%d_s%d.root", fRun, fSubrun);
      TFile* f = new TFile(filename,"RECREATE");
      HistoSet::Instance().WriteToRootFile(f);
      f->Close();
      delete f; f=0;
      if(fIPC)    { delete fIPC; fIPC = 0; }

    }
    
    //......................................................................

    void  OnMonPlotter::MakeGasCkovPlots()
    {
      HistoSet& h = HistoSet::Instance();
      
      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::GasCkov);
      char hname[256];
      if (fMakeWaveFormPlots) {
        std::cout << "Making Gas Ckov ADC OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"GasCkovADC_%d",i);
          fGasCkovADCDist[i] = h.GetTH1F(hname);
	       }
      }
    }

    //......................................................................

    void  OnMonPlotter::MakeBACkovPlots()
    {
      HistoSet& h = HistoSet::Instance();
      
      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
      char hname[256];
      if (fMakeWaveFormPlots) {
        std::cout << "Making BACkov Charge OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"BACkovQ_%d",i);
          fBACkovQDist[i] = h.GetTH1F(hname);
        }
        std::cout << "Making BACkov WaveForm OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"BACkovWaveForm_%d",i);
          fBACkovWaveForm.push_back(h.GetTH1F(hname));
	  fBACkovWaveForm[i]->SetBit(TH1::kIsAverage);
	  fNEventsBACkov.push_back(0);
        }
      }
    }

    //......................................................................

    void  OnMonPlotter::MakeT0Plots()
    {
      HistoSet& h = HistoSet::Instance();

      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::T0);
      char hname[256];
      if (fMakeWaveFormPlots) {
        std::cout << "Making T0ADC OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"T0ADC_%d",i+1);
          fT0ADCDist[i] = h.GetTH1F(hname);
	}
      }
      if (fMakeTRB3Plots) {
        std::cout << "Making T0TDC OnMon plots" << std::endl;
	fT0TDCChanVsADCChan = h.GetTH2F("T0TDCChanVsADCChan");
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"T0NTDC_%d",i+1);
          fT0NTDC[i] = h.GetTH1F(hname);
	  //fT0TDCVsADC[i] = h.GetTH2F(hname);
        }
        for (int i=0; i < nchannel; ++i) {
          sprintf(hname, "T0RisingTime_%d",i+1);
          fT0RisingTime[i] = h.GetTH1F(hname);
        }
	for (int i=0; i < nchannel; ++i) {
          sprintf(hname, "T0FallingTime_%d",i+1);
          fT0FallingTime[i] = h.GetTH1F(hname);
	}
        fT0RisingTimeSum = h.GetTH1F("T0RisingTimeSum");
	fT0FallingTimeSum = h.GetTH1F("T0FallingTimeSum");
      }
    }
    
    //......................................................................

    void  OnMonPlotter::MakeSSDPlots()
    {
      if (fMakeSSDPlots) {
	HistoSet& h = HistoSet::Instance();
	
	int nchannel = emph::geo::DetInfo::NChannel(emph::geo::SSD);
	char hname[256];
	
	std::cout << "Making SSD OnMon plots" << std::endl;
	for (int i=0; i<nchannel; ++i) {
	  sprintf(hname,"SSDProfile_%d",i);
	  fSSDProf.push_back(h.GetTH1F(hname));
	  sprintf(hname,"SSDNHits_%d",i);
	  fSSDNHit.push_back(h.GetTH1F(hname));
	}
      }

    }

    //......................................................................

    void  OnMonPlotter::MakeARICHPlots()
    {
      HistoSet& h = HistoSet::Instance();
      if (fMakeTRB3Plots) {
	std::cout << "Making ARICH OnMon plots" << std::endl;
        fARICHNHits = h.GetTH1F("ARICHNHits");
        fARICHNHitsPxl = h.GetTH2F("ARICHNHitsPxl");
        fARICHHitTimes = h.GetTH1F("ARICHHitTimes");
        fARICHHitToT = h.GetTH1F("ARICHHitToT");
        fARICHNTrails = h.GetTH1F("ARICHNTrails");
        // hardcoded ARICH display bins including gaps between PMTs
        std::vector<double> ARICHBins = {-78.95, -72.7, -66.7, -60.7, -54.7, -48.7, -42.7, -36.7, -30.45, -24.25, -18, -12, -6, 0, 6, 12, 18, 24.25, 30.45, 36.7, 42.7, 48.7, 54.7, 60.7, 66.7, 72.7, 78.95};
        fARICHNHitsPxl->GetXaxis()->SetTitle("X (mm)");
        fARICHNHitsPxl->GetYaxis()->SetTitle("Y (mm)");
        fARICHNHitsPxl->SetBins(ARICHBins.size()-1,ARICHBins.data(),ARICHBins.size()-1,ARICHBins.data());
      }
    }

    //......................................................................

    void  OnMonPlotter::MakeLGCaloPlots()
    {
      HistoSet& h = HistoSet::Instance();
      
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
      }
    }

    //......................................................................
    
    void  OnMonPlotter::MakeRPCPlots()
    {
      HistoSet& h = HistoSet::Instance();

      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::RPC);
      char hname[256];
      if (fMakeTRB3Plots) {
	std::cout << "Making RPC OnMon plots" << std::endl;
	for (int i=0; i<nchannel; ++i) {
	  sprintf(hname,"RPCNTDC_%d",i+1);
          fRPCNTDC[i] = h.GetTH1F(hname);
	  sprintf(hname,"RPCRisingTime_%d",i+1);
	  fRPCRisingTime[i] = h.GetTH1F(hname);
	  sprintf(hname,"RPCFallingTime_%d",i+1);
          fRPCFallingTime[i] = h.GetTH1F(hname);
	  sprintf(hname,"RPCTOT_%d",i+1);
	  fRPCTOT[i] = h.GetTH1F(hname);
	}
	fRPCRisingTimeSum = h.GetTH1F("RPCRisingTimeSum");
        fRPCFallingTimeSum = h.GetTH1F("RPCFallingTimeSum");
      }
    }
    //......................................................................
    
    void  OnMonPlotter::MakeTrigPlots()
    {
      HistoSet& h = HistoSet::Instance();

      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      char hname[256];
      if (fMakeWaveFormPlots) {
	sprintf(hname,"TriggerEff");
	fTriggerEff = h.GetTH1F(hname);
	sprintf(hname,"TriggerTime");
	fTriggerTime = h.GetTH1F(hname);
	sprintf(hname,"TriggerDeltaT");
	fTriggerDeltaT = h.GetTH1F(hname);
        std::cout << "Making Trigger ADC OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {	  
          sprintf(hname,"TriggerADC_%d",i);
          fTriggerADCDist[i] = h.GetTH1F(hname);
        }
      }
    }

    //......................................................................

    void OnMonPlotter::FillGasCkovPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::GasCkov);
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
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fGasCkovADCDist[detchan]->Fill(adc);
		fHitEffPerChannel->Fill((int)dchan.DetId()+kGasCkovOffset,
					detchan);
	      }
	    }
	  }
	}
      }      
    }

    //......................................................................

    void OnMonPlotter::FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      if (fMakeWaveFormPlots) {
	if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm wvfm = (*wvfmH)[idx];
        const emph::adcu::ADCUtils ADCUtil(wvfm,stmap,10,15);
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      // now fill ADC dist plot
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      float q = ADCUtil.Charge();
	      fBACkovQDist[detchan]->Fill(q);
	      if (adc > 5*blw) {
		// now fill waveform plot
		auto adcvals = wvfm.AllADC();
		fNEventsBACkov[detchan]++;
		for (size_t i=0; i<adcvals.size(); ++i) {
		  fBACkovWaveForm[detchan]->Fill(i+1,adcvals[i]);
		  fHitEffPerChannel->Fill((int)dchan.DetId()+kBACkovOffset,
					  detchan);
		}
	      }
	    }
	  }
	}
      }
    }

    //......................................................................

    void OnMonPlotter::FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH, art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::T0);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      std::vector<int> vT0ADChits(nchan,0);	    
      std::vector<int> vT0TDChits(nchan,0);	  
      if (fMakeWaveFormPlots) {
	if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan != 500) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fT0ADCDist[detchan-1]->Fill(adc);
		vT0ADChits[detchan-1]=1; 
		fHitEffPerChannel->Fill((int)dchan.DetId()+kT0Offset,
					detchan);
	      }
	    }
	  }
	}
      }

      if (fMakeTRB3Plots) {
	if (! trb3H->empty()) {
	  std::vector<int> hitCount;
	  hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::T0));
	  boardType = emph::cmap::TRB3;
	  echan.SetBoardType(boardType);
          double startTime = (*trb3H)[0].GetFinalTime();
	  
          bool risingChannelFilled[nChanT0] {false};
	  bool fallingChannelFilled[nChanT0] {false};
	  for (size_t idx=0; idx < trb3H->size(); ++idx) {
	    const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];	  
	    int board = trb3.GetBoardId();
	    int chan = trb3.GetChannel();
            double time = trb3.GetFinalTime();
            echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel  dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    fHitEffPerChannel->Fill((int)dchan.DetId()+kT0Offset+1,
				    detchan);
	    
	    if (detchan != 500 
		&& dchan.HiLo() == 0) {
	      ///Adding a count per hit for each channel (only for rising edges)
	      hitCount[detchan-1] += 1;
	      ///Filling plot depicting hits per fpga channel on TRB3
	      fTRB3NHitsPerChannel->Fill(chan,board);
	    }

	    //// The Following Checks if the hit is rising (dchan.HiLo == 0) or falling ( == 1), makes sure the detector is not trigger ( detchan != 500), and that only 1 hit per trigger is filling the histograms.////
	    if (dchan.HiLo() == 0
		&& detchan != 500
		&& !risingChannelFilled[detchan-1]) { // watch out for channel 500!
	      fT0RisingTime[detchan-1]->Fill(time - startTime);
	      fT0RisingTimeSum->Fill(time - startTime);
	      risingChannelFilled[detchan-1] = true;
	    }
	    if (dchan.HiLo() == 1
		&& detchan != 500
		&& !fallingChannelFilled[detchan-1]) { // watch out for channel 500!                                                                                 
              fT0FallingTime[detchan-1]->Fill(time - startTime);
              fT0FallingTimeSum->Fill(time - startTime);
              fallingChannelFilled[detchan-1] = true;
	    }
	  }
	  for (size_t i=0; i<hitCount.size(); ++i) {
      	    fT0NTDC[i]->Fill(hitCount[i]);
      	    vT0TDChits[i] = hitCount[i];	  
      	  }
	}
      }
      for(int i=0; i<(int)vT0ADChits.size(); i++) {
      	if(vT0ADChits[i]==1)
      	  for(int j = 0; j<(int)vT0TDChits.size(); j++) {
      	    fT0TDCChanVsADCChan->Fill(i,j,vT0TDChits[j]);
      	  }
      }
    }
    //......................................................................

    void OnMonPlotter::FillSSDPlots(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > & ssdH)
    {
      if (fMakeSSDPlots) {
	if (!ssdH->empty()) {
	  std::vector<int> nhits;
	  int nchannel = emph::geo::DetInfo::NChannel(emph::geo::SSD);
	  for (int i=0; i<nchannel; ++i)
	    nhits.push_back(0);
	  
	  for (size_t idx=0; idx < ssdH->size(); ++idx) {
	    const rawdata::SSDRawDigit& ssd = (*ssdH)[idx];
	    int station = ssd.FER();
	    int module = ssd.Module();
	    int row = ssd.getSensorRow(ssd.Chip(), ssd.Set(), ssd.Strip());

	    emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,station,module);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
            int sensor = dchan.Channel();

	    if (station >= 0) {
	      fSSDProf[sensor]->Fill(row);
	      fHitEffPerChannel->Fill(emph::geo::SSD+kSSDOffset+sensor,
				      row%64);
	      nhits[sensor]++;
	    }
	  }
	  for (int i=0; i<nchannel; ++i)
	    fSSDNHit[i]->Fill(nhits[i]);
	}
      }
    }
    
    //......................................................................

    void OnMonPlotter::FillARICHPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
      if (fMakeTRB3Plots) {

        // find reference time for each fpga
        std::map<int,double> refTime;
        for (size_t idx=0; idx < trb3H->size(); ++idx) {

          const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];

          if (trb3.GetChannel()==0) {
            int fpga = trb3.GetBoardId();
            if (refTime.find(fpga)==refTime.end()) {
              refTime[fpga] = trb3.GetFinalTime();
            }
            else {
              std::cout << "Reference time for fpga " << fpga
                        << " already exists."
                        << " Time difference "
                        << (trb3.GetFinalTime()-refTime[fpga])/1e3 << " (ns)" << std::endl;
            }
          }
        }

        // separate leading and trailing times per channel
        std::map<emph::cmap::EChannel,std::vector<double>> leadTimesCh;
        std::map<emph::cmap::EChannel,std::vector<double>> trailTimesCh;

	for (size_t idx=0; idx < trb3H->size(); ++idx) {

          const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];

          // skip timing channel
          if (trb3.GetChannel()==0) continue;

          int fpga = trb3.GetBoardId();
          int ech = trb3.GetChannel();
          emph::cmap::EChannel echan(emph::cmap::TRB3,fpga,ech);

          double time = (trb3.GetFinalTime()-refTime[fpga])/1e3;//ns

          if (trb3.IsLeading())  leadTimesCh[echan].push_back(time);
          if (trb3.IsTrailing()) trailTimesCh[echan].push_back(time);

        }

        // loop over channel with leading times
        int nhits = 0;
        for (auto lCh=leadTimesCh.begin();lCh!=leadTimesCh.end();lCh++) {

          // check if channel has trailing times
          // and skip channel if not
          auto tCh = trailTimesCh.find(lCh->first);
          if (tCh==trailTimesCh.end()) continue;

          // sort leading and trailing times in ascendent order
          std::vector<double> leadTimes  = lCh->second;;
          std::vector<double> trailTimes = tCh->second;

          std::sort(leadTimes.begin(),leadTimes.end());
          std::sort(trailTimes.begin(),trailTimes.end());

          // get all trailing times between 2 consecutive leading times
          for (unsigned int l=0;l<leadTimes.size();l++) {

            double lead  = leadTimes[l];
            double lead_next  = l<leadTimes.size()-1 ? leadTimes[l+1] : 1000;
            std::vector<double> trail_found;
            for (unsigned int t=0;t<trailTimes.size();t++) {
              if (trailTimes[t]>lead && trailTimes[t]<lead_next ) {
                trail_found.push_back(trailTimes[t]);
              }
            }

            fARICHNTrails->Fill(trail_found.size());

            // make hit with a leading time
            // and at least a trailing time found
            if (trail_found.size()>0) {

              emph::cmap::EChannel echan = lCh->first;
              emph::cmap::DChannel dchan = cmap->DetChan(echan);
              if (dchan.DetId()!=emph::geo::ARICH) {
                std::cout << echan;
                std::cout << " doesn't belong to the ARICH" << std::endl;
                continue;
              }
              int pmt = dchan.HiLo();
              int dch = dchan.Channel();
              fHitEffPerChannel->Fill((int)dchan.DetId()+kARICHOffset+pmt,dch);

              nhits++;

              // fill time histograms
              fARICHHitTimes->Fill(lead);
              fARICHHitTimes->Fill(trail_found[0]);
              fARICHHitToT->Fill(trail_found[0]-lead);

              // fill electronic channel plot
              fTRB3NHitsPerChannel->Fill(echan.Channel(),echan.Board());

              // fill pixel position plot
              // the arich consist of 3x3 pmts
              // and there are 8x8 pixels in each pmt
              // pmt 0 and pixel 0  is on the bottom right
              // pmt 8 and pixel 63 is on the top left
              // there is a gap of 1 bin size between pmts
              int pxlxbin0 = 25-pmt*9+(pmt/3)*27;
              int pxlybin0 = (pmt/3)*9;
              int pmtrow = dch/8;
              int pmtcol = dch-pmtrow*8;
              int pxlxbin = pxlxbin0-pmtcol;
              int pxlybin = pxlybin0+pmtrow;
              int pxlx = fARICHNHitsPxl->GetXaxis()->GetBinCenter(pxlxbin+1);
              int pxly = fARICHNHitsPxl->GetYaxis()->GetBinCenter(pxlybin+1);
              fARICHNHitsPxl->Fill(pxlx,pxly);

            }//if trailing time found

          }//leading time loop

        }//leading time channel map loop

        fARICHNHits->Fill(nhits);

      }
    }

    //......................................................................

    void    OnMonPlotter::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      float c1=0; float e1=0; float t1=0;
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
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fHitEffPerChannel->Fill((int)dchan.DetId()+kLGCaloOffset,
					detchan);
		fLGCaloADCDist[detchan]->Fill(adc);
		//now fill waveform plot
	        auto adcvals = wvfm.AllADC();
                fNEventsLGCalo[detchan]++;
                for (size_t i=0; i<adcvals.size(); ++i) {
                  fLGCaloWaveForm[detchan]->Fill(i+1,adcvals[i]);
		}
		// now fill integrated charge plot
		float x1=20; float nsamp=25; //range where the signal is
		float avg = 0; int ic = 0;
		float sum=0;
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
		if (detchan==4) c1 += sum;
                if (detchan!=4) e1 += sum;
                t1 += sum;

		if (e1!=0 && c1!=0){
 	          fLGCaloEdgeRatio->Fill(e1/t1);
                  fLGCaloCenterRatio->Fill(c1/t1);

                  fLGCaloIntChgVsRatio->Fill(c1/t1,sum);
		}
	      }
            }
	  }
	}
      } 
    }

    //......................................................................
    void    OnMonPlotter::FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
      //int nchan = emph::geo::DetInfo::NChannel(emph::geo::RPC);
      emph::cmap::EChannel echan;
      emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
       if (fMakeTRB3Plots) {
        if (! trb3H->empty()) {
	  std::vector<int> hitCount;
          hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::RPC));
          echan.SetBoardType(boardType);
	  //The First hit for every event was in channel 500 (trigger)
	  double startTime = (*trb3H)[0].GetFinalTime();
          double prevTime = 0;
          int prevChan = 0;
          bool risingChannelFilled[nChanRPC] {false};
	  bool fallingChannelFilled[nChanRPC] {false};
          for (size_t idx=0; idx < trb3H->size(); ++idx) {
            const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];
            double time = trb3.GetFinalTime();
	    int chan = trb3.GetChannel();
            int board = trb3.GetBoardId();
            echan.SetBoard(board);
            echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
            int detchan = dchan.Channel();
	    if (detchan != 500 
		&& dchan.HiLo() == 0) {
	      //adding a count for each hit in a detector channel
	      hitCount[detchan-1] += 1;
	      //Filling plot which shows number of hits per FPGA channel
	      fTRB3NHitsPerChannel->Fill(chan,board);
	    }
            if (detchan != 500
                && chan % 2 == 0
                && chan == prevChan + 1) {
              if (prevTime == 0) {
                std::cout << "WARNING: prevTime should never be 0" << std::endl;
              } else if (prevChan == 0) {
                std::cout << "WARNING: prevChan should never be 0" << std::endl;
              } else {
                // find the time over threshold for channels 1-32.
                // Grabs the first time from each channel pair
                fRPCTOT[detchan-1]->Fill(time - prevTime);
              }
            }
	    //// The Following Checks if the hit is rising (dchan.HiLo == 0) or falling ( == 1), makes sure the detector is not trigger ( detchan != 500), and that only 1 hit per trigger is filling the histograms (by setting the risingChannelFilled boolean to true after filling within an event.////             
	    if (dchan.HiLo() == 0
                && detchan != 500
		&& !risingChannelFilled[detchan-1]) { // watch out for channel 500!
	      fRPCRisingTime[detchan-1]->Fill(time - startTime);
	      fRPCRisingTimeSum->Fill(time - startTime);
	      risingChannelFilled[detchan-1] = true;
	    }
	    if (dchan.HiLo() == 1
                && detchan != 500
		&& !fallingChannelFilled[detchan-1]) { // watch out for channel 500!
	      fRPCFallingTime[detchan-1]->Fill(time - startTime);
	      fRPCFallingTimeSum->Fill(time - startTime);
	      fallingChannelFilled[detchan-1] = true;
	    }
	    prevChan = chan;
	    prevTime = time;
	  }
	  for (size_t i=0; i<hitCount.size(); ++i) {
	    fRPCNTDC[i]->Fill(hitCount[i]);
	  }

	}
       }
    }
	//.....................................}.................................
    
    void   OnMonPlotter::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      if (fMakeWaveFormPlots) {
	bool eff[4] = {false,false,false,false};
	if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fTriggerADCDist[detchan]->Fill(adc);
		eff[detchan] = true;
		fHitEffPerChannel->Fill((int)dchan.DetId(),detchan);
	      }
	    }
	  }
	}
	if (!eff[0]&&eff[1]&&eff[2]&&eff[3])
	  fTriggerEff->Fill(0);
	if (eff[0]&&!eff[1]&&eff[2]&&eff[3])
	  fTriggerEff->Fill(1);
	if (eff[0]&&eff[1]&&!eff[2]&&eff[3])
	  fTriggerEff->Fill(2);
	if (eff[0]&&eff[1]&&eff[2]&&!eff[3])
	  fTriggerEff->Fill(3);
      }
    }

    //......................................................................
    void OnMonPlotter::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      fRun = evt.run();
      fSubrun = evt.subRun();     
      if(!stmap.IsTimeMapLoaded()) stmap.LoadMap(fRun);
      std::string labelStr;
      std::string labelStr2;

      if (fNEvents == 1) {
	fFirstEventTime = evt.time();
	fLastEventTime = evt.time();
      }

      fTriggerTime->Fill((evt.time().timeHigh()-fFirstEventTime.timeHigh()) + (evt.time().timeLow() - fFirstEventTime.timeLow())*1.e-9);
      if (fNEvents > 1) {
	fTriggerDeltaT->Fill((evt.time().timeHigh()-fFirstEventTime.timeHigh())*1.e-3 + (evt.time().timeLow() - fLastEventTime.timeLow())*1.e-6);
	fLastEventTime = evt.time();
      }
      
      if (fuseSHM) fIPC->HandleRequests();

      static unsigned int count = 0;
      if (++count%10==0) {
	if (fuseSHM) fIPC->PostResources(fRun, fSubrun, evt.event());
	// std::cout << "onmon_plot: run/sub/evt="
	// 	  << fRun << "/" << fSubrun << "/" << evt.event()
	// 	  << std::endl;
      }

      //
      // Update the ticker so it can notify its subscribers.
      // Do this BEFORE unpacking so that there are no overlaps in plots
      // reset every 24 hours.
      //
      if (fTickerOn) Ticker::Instance().Update(fRun, fSubrun);

      for (int i=0; i<emph::geo::NDetectors; ++i) {

	labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	try {
	  evt.getByLabel(labelStr, wfHandle);
	  if (!wfHandle->empty()) {
            int j=i;
            if (i > emph::geo::T0)
              j=i+1;
	    fNRawObjectsHisto->Fill(j,wfHandle->size());
	    fNTriggerVsDet->Fill(j);
	    fTriggerVsSubrun->Fill(fSubrun,j);
	    if (i == emph::geo::Trigger) FillTrigPlots(wfHandle);
	    if (i == emph::geo::GasCkov) FillGasCkovPlots(wfHandle);
	    if (i == emph::geo::BACkov)  FillBACkovPlots(wfHandle);
	    if (i == emph::geo::LGCalo)  FillLGCaloPlots(wfHandle);
	    if (i == emph::geo::T0) {
	      j = emph::geo::T0+1;
	      labelStr = "raw:T0";
	      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle;
	      try {
		evt.getByLabel(labelStr, trbHandle);
		if (!trbHandle->empty()) {
		  fNRawObjectsHisto->Fill(j,trbHandle->size());
		  fNTriggerVsDet->Fill(j);
		  fTriggerVsSubrun->Fill(fSubrun,j);
		  FillT0Plots(wfHandle, trbHandle);
		}
		else
		  std::cout << "**No TRB3 digits found for the T0!" << std::endl;
		
	      }
	      catch(...) {
		std::cout << "No TRB3 digits found for the T0!" << std::endl;
	      }
	    }
	    
	  }
	}
	catch(...) {
	  //	  std::cout << "Nothing found in " << labelStr << std::endl;
	}
      }
      // get RPC TRB3digits
      int i = emph::geo::RPC;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle;
      try {
	evt.getByLabel(labelStr, trbHandle);
	if (!trbHandle->empty()) {
          int j=1;
          if (i > emph::geo::T0)
            j=i+1;
	  fNRawObjectsHisto->Fill(j,trbHandle->size());
	  fNTriggerVsDet->Fill(j);
	  fTriggerVsSubrun->Fill(fSubrun,j);
	  FillRPCPlots(trbHandle);
	}
      }
      catch(...) {

      }
      // get ARICH TRB3digits
      i = emph::geo::ARICH;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      try {
	evt.getByLabel(labelStr, trbHandle);
	if (!trbHandle->empty()) {
          int j=1;
          if (i > emph::geo::T0)
            j=i+1;
	  fNRawObjectsHisto->Fill(j,trbHandle->size());
	  fNTriggerVsDet->Fill(j);
	  fTriggerVsSubrun->Fill(fSubrun,j);
	  FillARICHPlots(trbHandle);
	}
      }
      catch(...) {

      }
      // get SSDdigits
      i = emph::geo::SSD;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
      try {
	evt.getByLabel(labelStr, ssdHandle);

	if (!ssdHandle->empty()) {
          int j=1;
          if (i > emph::geo::T0)
            j=i+1;
	  fNRawObjectsHisto->Fill(j,ssdHandle->size());
	  fNTriggerVsDet->Fill(j);
	  fTriggerVsSubrun->Fill(fSubrun,j);
	  FillSSDPlots(ssdHandle);
	}
      }
      catch(...) {

      }
      //Time of Flight (T0+RPC TRB3Digits)
      //i = emph::geo::T0;
      //int j = emph::geo::RPC;
      //labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      //labelStr2 = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(j));
      //art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle1;
      //art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle2;
      //try {
      //evt.getByLabel(labelStr, trbHandle1);
      //evt.getByLabel(labelStr2, trbHandle2);
      //if (!trbHandle1->empty() && !trbHandle2->empty()) {
      //FillToFPlots(trbHandle1, trbHandle2);
      //}
      //}
      //catch(...) {
	
      //}	 

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::OnMonPlotter)
