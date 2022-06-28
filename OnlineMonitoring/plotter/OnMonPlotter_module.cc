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
#include "ChannelMap/ChannelMap.h"
#include "Geometry/DetectorDefs.h"
#include "OnlineMonitoring/plotter/HistoSet.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/Settings.h"
#include "OnlineMonitoring/util/Ticker.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"


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

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const&);
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
      void   FillToFPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &,
			  art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
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

      OnMonProdIPC* fIPC;         ///< Communicates with viewer
      std::string   fSHMname;     ///< Shared memory for communication
      bool          fuseSHM;      ///< Use SHM to communicate with a viewer?
      std::atomic<bool> fSHMThreadRunning;
      std::unique_ptr<std::thread> fSHMThreadPtr;
      bool          fTickerOn;    ///< Turned on in the control room

      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
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
      
      // define histograms
      TH2F*  fNRawObjectsHisto;  
      TH1F*  fNTriggerVsDet;
      TH2F*  fTriggerVsSubrun;
      TH1F*  fAllToFHisto;
      
      TH2F* fT0TDCChanVsADCChan;

      TH1F* fT0ADCDist[nChanT0];
      TH1F* fT0NTDC[nChanT0];
      TH2F* fT0TDCVsADC[nChanT0];
      TH1F* fT0TDC[nChanT0];
      TH1F* fRPCTDC[nChanRPC];
      TH1F* fRPCTOT[nChanRPC];
      TH1F* fRPCNTDC[nChanRPC];
      TH1F* fToFHisto[nChanT0];
      TH1F* fLGCaloADCDist[nChanCal];
      TH1F* fBACkovADCDist[nChanBACkov];
      std::vector<TH1F*> fBACkovWaveForm;
      std::vector<unsigned int> fNEventsBACkov;
      TH1F* fGasCkovADCDist[nChanGasCkov];
      TH1F* fTriggerADCDist[nChanTrig];
      std::vector<TH1F*> fSSDProf;
      std::vector<TH1F*> fSSDNHit;

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
      : EDAnalyzer(pset)
    {

      this->reconfigure(pset);
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
    void OnMonPlotter::reconfigure(const fhicl::ParameterSet& pset)
    {
      fSHMname = pset.get<std::string>("SHMHandle");
      fuseSHM = pset.get<bool>("useSHM");
      fTickerOn = pset.get<bool>("TickerOn");

      //if (fIPC) delete fIPC;
      if (fuseSHM) fIPC = new OnMonProdIPC(kIPC_SERVER, fSHMname.c_str());

      fChanMapFileName = pset.get<std::string>("channelMapFileName","");
      fMakeWaveFormPlots = pset.get<bool>("makeWaveFormPlots",true);
      fMakeTRB3Plots = pset.get<bool>("makeTRB3Plots",true);
      fMakeSSDPlots = pset.get<bool>("makeSSDPlots",false);
      
      // try to find the correct path to the .csv file.
      std::string filename = pset.get< std::string > ("CSVFile");
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
         csvpath += filename;
         if (access(csvpath.c_str(), F_OK)!=-1) {
          Settings::Instance().fCSVFile = csvpath;
          break;
         }
      } // loop on directory attempts

      Settings::Instance().fDet = kEMPH;
    }

    //......................................................................

    void OnMonPlotter::beginRun(art::Run const&) {
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
      // initialize channel map
      fChannelMap = 0;
      if (!fChanMapFileName.empty()) {
	fChannelMap = new emph::cmap::ChannelMap();
	if (!fChannelMap->LoadMap(fChanMapFileName)) {
	  std::cerr << "Failed to load channel map from file " << fChanMapFileName << std::endl;
	  delete fChannelMap;
	  fChannelMap = 0;
	}
	std::cout << "Loaded channel map from file " << fChanMapFileName << std::endl;
      }
    
      //
      // Make all-detector plots
      //
      HistoSet& h = HistoSet::Instance();
      fNRawObjectsHisto = h.GetTH2F("NRawObjectsHisto");
      fNTriggerVsDet    = h.GetTH1F("NTriggerVsDet");
      fTriggerVsSubrun  = h.GetTH2F("TriggerVsSubrun");
      fAllToFHisto      = h.GetTH1F("AllToFHisto");
      
      // label x-axis
      std::string labelStr;
      int i=0;
      for (; i<emph::geo::NDetectors; ++i) {
	labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	if (i == emph::geo::T0) labelStr += "ADC";
	fNTriggerVsDet->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
	fNRawObjectsHisto->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
      }
      // now add T0TDC
      labelStr = emph::geo::DetInfo::Name(emph::geo::T0) + "TDC";
      fNTriggerVsDet->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
      fNRawObjectsHisto->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
    
      MakeGasCkovPlots();
      MakeBACkovPlots();
      MakeT0Plots();
      MakeSSDPlots();
      MakeARICHPlots();
      MakeLGCaloPlots();
      MakeRPCPlots();
      MakeToFPlots();
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
      if (fNEvents > 0) {
	float scale;
	for (size_t i=0; i<fBACkovWaveForm.size(); ++i) {
	  scale = 1./float(fNEventsBACkov[i]);
	  fBACkovWaveForm[i]->Scale(scale);
	}
      }

      //if(fIPC)    { delete fIPC; fIPC = 0; }

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
        std::cout << "Making BACkov ADC OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"BACkovADC_%d",i);
          fBACkovADCDist[i] = h.GetTH1F(hname);
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
          sprintf(hname,"T0ADC_%d",i);
          fT0ADCDist[i] = h.GetTH1F(hname);
	}
	std::cout << "Making T0TDC OnMon plots (new)" << std::endl;
	for (int i=0; i<nchannel; ++i) {
	  sprintf(hname,"T0TDC_%d",i);
	  fT0TDC[i] = h.GetTH1F(hname);
        }
      }
      if (fMakeTRB3Plots) {
        std::cout << "Making T0TDC OnMon plots" << std::endl;
	fT0TDCChanVsADCChan = h.GetTH2F("T0TDCChanVsADCChan");
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"T0NTDC_%d",i);
          fT0NTDC[i] = h.GetTH1F(hname);
	  //fT0TDCVsADC[i] = h.GetTH2F(hname);
        }
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
      if (fMakeTRB3Plots)
	std::cout << "Making ARICH OnMon plots" << std::endl;
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
	  sprintf(hname,"RPCNTDC_%d",i);
          fRPCNTDC[i] = h.GetTH1F(hname);
	  sprintf(hname,"RPCTDC_%d",i);
	  fRPCTDC[i] = h.GetTH1F(hname);
	  sprintf(hname,"RPCTOT_%d",i);
	  fRPCTOT[i] = h.GetTH1F(hname);
	}
      }
    }
    //......................................................................
    void  OnMonPlotter::MakeToFPlots()
    {
      HistoSet& h = HistoSet::Instance();

      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::T0);
      char hname[256];
      if (fMakeTRB3Plots) {
	std::cout << "Making ToF OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"ToFHisto_%d",i);
	  fToFHisto[i] = h.GetTH1F(hname);
          //fT0TDCVsADC[i] = h.GetTH2F(hname);                                                                                                
        }
      }
    }
    //......................................................................
    
    void  OnMonPlotter::MakeTrigPlots()
    {
      HistoSet& h = HistoSet::Instance();
      
      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      char hname[256];
      if (fMakeWaveFormPlots) {
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
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw)
		fGasCkovADCDist[detchan]->Fill(adc);
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
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      // now fill ADC dist plot
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fBACkovADCDist[detchan]->Fill(adc);
		// now fill waveform plot
		auto adcvals = wvfm.AllADC();
		fNEventsBACkov[detchan]++;
		for (size_t i=0; i<adcvals.size(); ++i) {
		  fBACkovWaveForm[detchan]->Fill(i+1,adcvals[i]);
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
      double trb3LinearLowEnd = 15.0;
      double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0
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
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fT0ADCDist[detchan]->Fill(adc);
		vT0ADChits[detchan]=1; 
	      }
	    }
	  }
	}
      }

      if (fMakeTRB3Plots) {
	if (! trb3H->empty()) {
	  //std::cout<<"New Event!"<<std::endl;
	  int i = 0;
	  i++;
	  std::vector<int> hitCount;
	  hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::T0));
	  boardType = emph::cmap::TRB3;
	  echan.SetBoardType(boardType);
	  const rawdata::TRB3RawDigit& trb3Trigger = (*trb3H)[0];
	  long double triggerTime = trb3Trigger.GetEpochCounter()*10240026.0 + trb3Trigger.GetCoarseTime() * 5000.0 - ((trb3Trigger.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	  for (size_t idx=0; idx < trb3H->size(); ++idx) {
	    const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];	  
	    int chan = trb3.GetChannel() + 65*(trb3.fpga_header_word-1280);
	    int board = 100;
	    echan.SetBoard(board);	
	    echan.SetChannel(chan);
	    emph::cmap::DChannel  dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    long double time_T0 = trb3.GetEpochCounter()*10240026.0 + trb3.GetCoarseTime() * 5000.0 - ((trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	    //std::cout<<"detchan value: "<<detchan<<std::endl;
	    //std::cout<<"T0 Time after trigger: "<<(time_T0-triggerTime)<<std::endl;
	    if (detchan < nchan) { // watch out for channel 500!
	      hitCount[detchan] += 1;
	      fT0TDC[detchan]->Fill((time_T0-triggerTime)/1000);
	    }
	  }
	  //std::cout<<"\n"<<std::endl;
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
	    int sensor=-1;
	    if (station == 0)
	      sensor = module;
	    else {
	      if (station == 1) 
		sensor = module+4;
	      else {
		if (station == 2)
		  sensor = module+10;
		else 
		  sensor = module+16;
	      }
	    }

	    if (station >= 0) {
	      fSSDProf[sensor]->Fill(row);
	      nhits[sensor]++;
	    }
	  }
	  for (int i=0; i<nchannel; ++i)
	    fSSDNHit[i]->Fill(nhits[i]);
	}
      }
    }
    
    //......................................................................

    void OnMonPlotter::FillARICHPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & )
    {
    }

    //......................................................................

    void    OnMonPlotter::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
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
	      if (adc > 5*blw)
		fLGCaloADCDist[detchan]->Fill(adc);
	    }
	  }
	}
      }
      
    }

    //......................................................................
    void    OnMonPlotter::FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::RPC);
      emph::cmap::EChannel echan;
      emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
      double trb3LinearLowEnd = 15.0;
      //double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0
      double trb3LinearHighEnd = 476.0; // For FPGA3? -- RPC
      if (fMakeTRB3Plots) {
        if (! trb3H->empty()) {
	  //std::cout<<"New Event!"<<std::endl;
	  std::vector<int> hitCount;
          hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::RPC));
          echan.SetBoardType(boardType);
	  //The First hit for every event was in channel 500 (trigger)
	  const rawdata::TRB3RawDigit& trb3Trigger = (*trb3H)[0];
	  long double triggerTime = trb3Trigger.GetEpochCounter()*10240026.0 + trb3Trigger.GetCoarseTime() * 5000.0 - ((trb3Trigger.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	  ///////NOTE: Only looking at the first event that stores rising & falling edges////////
          for (size_t idx=0; idx < trb3H->size(); ++idx) {
            const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];
            int chan = trb3.GetChannel() + 65*(trb3.fpga_header_word-1280);
	    int board = 100;
            echan.SetBoard(board);
            echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
	    //std::cout<<"Found TRB3 hit: IsLeading: "<<trb3.IsLeading()<<"; IsTrailing: "<<trb3.IsTrailing()<<";
	    long double rising_time_RPC = trb3.GetEpochCounter()*10240026.0 + trb3.GetCoarseTime() * 5000.0 - ((trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	    //long double falling_time_RPC = trb3_falling.GetEpochCounter()*10240026.0 + trb3_falling.GetCoarseTime() * 5000.0 - ((trb3_falling.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	    if ((detchan < nchan)) { // watch out for channel 500!                                                                  
              hitCount[detchan] += 1;
	      fRPCTDC[detchan]->Fill((rising_time_RPC - triggerTime)/1000);
	      //fRPCTOT[detchan]->Fill((falling_time_RPC - rising_time_RPC)/1000);
	    }
          }
	  //std::cout<<"\n"<<std::endl;
          for (size_t i=0; i<hitCount.size(); ++i){
            fRPCNTDC[i]->Fill(hitCount[i]);	
	  }
	}
      }
    }
    //.....................................}.................................
    void OnMonPlotter::FillToFPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & T0trb3H, art::Handle< std::vector<rawdata::TRB3RawDigit> > & RPCtrb3H)
    {
      int T0nchan = emph::geo::DetInfo::NChannel(emph::geo::T0);
      int RPCnchan = emph::geo::DetInfo::NChannel(emph::geo::RPC);
      emph::cmap::EChannel T0echan;
      emph::cmap::EChannel RPCechan;
      emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
      double trb3LinearLowEnd = 15.0;
      double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0 
      double trb3LinearHighEnd_RPC = 476.0; // For FPGA3? -- RPC 
      if (fMakeTRB3Plots) {
	if (! T0trb3H->empty() && ! RPCtrb3H->empty()) {
	  T0echan.SetBoardType(boardType);
	  RPCechan.SetBoardType(boardType);
	  for (size_t idx=0; idx < T0trb3H->size(); ++idx) {
	    const rawdata::TRB3RawDigit& T0trb3 = (*T0trb3H)[idx];
	    const rawdata::TRB3RawDigit& RPCtrb3 = (*RPCtrb3H)[idx];
	    int T0chan = T0trb3.GetChannel() + 65*(T0trb3.fpga_header_word-1280);
            int RPCchan = RPCtrb3.GetChannel() + 65*(RPCtrb3.fpga_header_word-1280);
	    int board = 100;
            T0echan.SetBoard(board);
	    RPCechan.SetBoard(board);
            T0echan.SetChannel(T0chan);
	    RPCechan.SetChannel(RPCchan);
	    emph::cmap::DChannel T0dchan = fChannelMap->DetChan(T0echan);
	    emph::cmap::DChannel RPCdchan = fChannelMap->DetChan(RPCechan);
            int T0detchan = T0dchan.Channel();
	    int RPCdetchan = RPCdchan.Channel();
	    long double time_T0 = T0trb3.GetEpochCounter()*10240026.0 + T0trb3.GetCoarseTime() * 5000.0 - ((T0trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	    long double time_RPC = RPCtrb3.GetEpochCounter()*10240026.0 + RPCtrb3.GetCoarseTime() * 5000.0 - ((RPCtrb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd_RPC-trb3LinearLowEnd))*5000.0;
	    if ((T0detchan < T0nchan) && (RPCdetchan < RPCnchan)) {
	      fToFHisto[T0detchan]->Fill((time_RPC - time_T0)/1000);
	      fAllToFHisto->Fill((time_RPC-time_T0)/1000);
	    }
	  }
	}
      }
    }
    //.......................................................................
    void   OnMonPlotter::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
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
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw)
		fTriggerADCDist[detchan]->Fill(adc);
	    }
	  }
	}
      }
    }

    //......................................................................
    void OnMonPlotter::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      fRun = evt.run();
      fSubrun = evt.subRun();     
      std::string labelStr;
      std::string labelStr2;

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
	    fNRawObjectsHisto->Fill(i,wfHandle->size());
	    fNTriggerVsDet->Fill(i);
	    fTriggerVsSubrun->Fill(fSubrun,i);
	    if (i == emph::geo::Trigger) FillTrigPlots(wfHandle);
	    if (i == emph::geo::GasCkov) FillGasCkovPlots(wfHandle);
	    if (i == emph::geo::BACkov)  FillBACkovPlots(wfHandle);
	    if (i == emph::geo::LGCalo)  FillLGCaloPlots(wfHandle);
	    if (i == emph::geo::T0) {
	      int j = emph::geo::NDetectors;
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
	  fNRawObjectsHisto->Fill(i,trbHandle->size());
	  fNTriggerVsDet->Fill(i);
	  fTriggerVsSubrun->Fill(fSubrun,i);
	  FillRPCPlots(trbHandle);
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
	  fNRawObjectsHisto->Fill(i,ssdHandle->size());
	  fNTriggerVsDet->Fill(i);
	  fTriggerVsSubrun->Fill(fSubrun,i);
	  FillSSDPlots(ssdHandle);
	}
      }
      catch(...) {

      }
      //Time of Flight (T0+RPC TRB3Digits)
      i = emph::geo::T0;
      int j = emph::geo::RPC;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      labelStr2 = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(j));
      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle1;
      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle2;
      try {
	evt.getByLabel(labelStr, trbHandle1);
	evt.getByLabel(labelStr2, trbHandle2);
	if (!trbHandle1->empty() && !trbHandle2->empty()) {
	  FillToFPlots(trbHandle1, trbHandle2);
	}
      }
      catch(...) {
	
      }	 

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::OnMonPlotter)
