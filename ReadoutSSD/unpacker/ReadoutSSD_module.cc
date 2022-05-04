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
#include "TTree.h"

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
#include "RawData/SSDRawDigit.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace onmon {
    class ReadoutSSD : public art::EDAnalyzer {
    public:
      explicit ReadoutSSD(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~ReadoutSSD();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
      void   FillSSDPlots(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > &);

      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;

      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanCal = 9;
      
      // define histograms
      
      bool fMakeSSDPlots;
		TFile* f; 
		TTree *tree;
		int fer, module, row;
    };

    //.......................................................................
    ReadoutSSD::ReadoutSSD(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset)
    {

      this->reconfigure(pset);

    }

    //......................................................................
    ReadoutSSD::~ReadoutSSD()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void ReadoutSSD::reconfigure(const fhicl::ParameterSet& pset)
    {
      fChanMapFileName = pset.get<std::string>("channelMapFileName","");
      fMakeSSDPlots = pset.get<bool>("makeSSDPlots",false);
      
    }

    //......................................................................
    void ReadoutSSD::beginJob()
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
   
		char filename[32];
      sprintf(filename,"SSD.root");
      f = new TFile(filename,"RECREATE");
   
		tree=new TTree("SSDtree","");
		tree->Branch("fer",&fer);
		tree->Branch("module",&module);
		tree->Branch("row",&row);

    }
    
    //......................................................................

    void ReadoutSSD::endJob()
    {

		tree->Write();
      f->Close();
      delete f; f=0;
    }
    
    //......................................................................

    void ReadoutSSD::FillSSDPlots(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > & ssdH)
    {
      if (fMakeSSDPlots) {
	if (!ssdH->empty()) {
	  std::vector<int> nhits;
	  int nchannel = emph::geo::DetInfo::NChannel(emph::geo::SSD);
	  for (int i=0; i<nchannel; ++i)
	    nhits.push_back(0);
	  
	  for (size_t idx=0; idx < ssdH->size(); ++idx) {
	    const rawdata::SSDRawDigit& ssd = (*ssdH)[idx];
		 std::cout<<ssd<<std::endl;
	    //fer = ssd.Station();
	    fer = ssd.FER();
	    module = ssd.Module();
	    row = ssd.getSensorRow(ssd.Chip(), ssd.Set(), ssd.Strip());

		 tree->Fill();

	  }
      }
    }
	 }
    
    //......................................................................

    void ReadoutSSD::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      fRun = evt.run();
      fSubrun = evt.subRun();     
      std::string labelStr;

      // get SSDdigits
      int i = emph::geo::SSD;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
      try {
	evt.getByLabel(labelStr, ssdHandle);

	if (!ssdHandle->empty()) {
	  FillSSDPlots(ssdHandle);
	}
      }
      catch(...) {

      }

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::ReadoutSSD)
