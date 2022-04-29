////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create online monitoring plots
/// \author  $Author: jpaley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
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
#include "OnlineMonitoring/util/Settings.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace onmon {
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
      void   FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);

      void   MakeGasCkovPlots();
      void   MakeBACkovPlots();
      void   MakeT0Plots();
      void   MakeSSDPlots();
      void   MakeARICHPlots();
      void   MakeLGCaloPlots();
      void   MakeRPCPlots();
      void   MakeTrigPlots();

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
      
      TH2F* fT0TDCChanVsADCChan;

      TH1F* fT0ADCDist[nChanT0];
      TH1F* fT0NTDC[nChanT0];
      TH2F* fT0TDCVsADC[nChanT0];
      TH1F* fT0TDC[nChanT0];
      TH1F* fRPCTDC[nChanRPC];
      TH1F* fRPCNTDC[nChanRPC];
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
      MakeTrigPlots();

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

      char filename[32];
      sprintf(filename,"onmon_r%d_s%d.root", fRun, fSubrun);
      TFile* f = new TFile(filename,"RECREATE");
      HistoSet::Instance().WriteToRootFile(f);
      f->Close();
      delete f; f=0;
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
	  std::vector<int> hitCount;
	  hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::T0));
	  boardType = emph::cmap::TRB3;
	  echan.SetBoardType(boardType);	
	  for (size_t idx=0; idx < trb3H->size(); ++idx) {
	    const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];	  
	    int chan = trb3.GetChannel() + 65*(trb3.fpga_header_word-1280);
	    int board = 100;
	    echan.SetBoard(board);	
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan < nchan) { // watch out for channel 500!
	      hitCount[detchan] += 1;
	      fT0TDC[detchan]->Fill(trb3.GetCoarseTime());
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
      double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0
      //double trb3LinearHighEnd_RPC = 476.0; // For FPGA3? -- RPC

      if (fMakeTRB3Plots) {
        if (! trb3H->empty()) {
	  std::vector<int> hitCount;
          hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::RPC));
          echan.SetBoardType(boardType);
          for (size_t idx=0; idx < trb3H->size(); ++idx) {
            const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];
            int chan = trb3.GetChannel() + 64*(trb3.fpga_header_word-1280);
            int board = 100;
            echan.SetBoard(board);
            echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
	    //std::cout<<"Found TRB3 hit: IsLeading: "<<trb3.IsLeading()<<"; IsTrailing: "<<trb3.IsTrailing()<<"; Fine Time: " <<trb3.GetFineTime()<<"; Course Time: "<<trb3.GetCoarseTime()<<"; Epoch Counter: "<<trb3.GetEpochCounter()<<std::endl;
            if (detchan < nchan) { // watch out for channel 500!                                                                  
              hitCount[detchan] += 1;
	      double time_RPC = trb3.GetEpochCounter()*10240026.0 + trb3.GetCoarseTime() * 5000.0 - ((trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0; 
	      //std::cout << "RPC calculated time is" << time_RPC << std::endl;
	      fRPCTDC[detchan]->Fill(time_RPC);
            }
          }
          for (size_t i=0; i<hitCount.size(); ++i){
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

      for (int i=0; i<emph::geo::NDetectors; ++i) {

	labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	try {
	  evt.getByLabel(labelStr, wfHandle);

	  if (!wfHandle->empty()) {
	    fNRawObjectsHisto->Fill(i,wfHandle->size());
	    fNTriggerVsDet->Fill(i);
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
	  FillSSDPlots(ssdHandle);
	}
      }
      catch(...) {

      }

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::OnMonPlotter)
