////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create ARing objects from raw data and 
///          store them in the art output file
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
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/ARing.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class ARICHReco : public art::EDProducer {
  public:
    explicit ARICHReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~ARICHReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    //void beginRun(art::Run const&);
    //void endRun(art::Run const&);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    void GetARings(art::Handle< std::vector<rawdata::TRB3RawDigit> > &, std::unique_ptr<std::vector<rb::ARing>> &);
    
    emph::cmap::ChannelMap* fChannelMap;
    std::string fChanMapFileName;    
    TH2F*       fARICH2DHist;

  };

  //.......................................................................
  
  ARICHReco::ARICHReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

    this->produces< std::vector<rb::ARing>>();

    this->reconfigure(pset);

  }

  //......................................................................
  
  ARICHReco::~ARICHReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void ARICHReco::reconfigure(const fhicl::ParameterSet& pset)
  {
    
    fChanMapFileName = pset.get<std::string>("channelMapFileName","");
    
  }

  //......................................................................
  
  void ARICHReco::beginJob()
  {
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

    art::ServiceHandle<art::TFileService> tfs;
    
    fARICH2DHist = tfs->make<TH2F>("ARICH2DHist","",
				   24,0,24,24,0,24);
  }
  
  //......................................................................
  
  void ARICHReco::endJob()
  {
  }
  
    //......................................................................
  
  void ARICHReco::GetARings(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H, std::unique_ptr<std::vector<rb::ARing>> & rings )
  {
    fARICH2DHist->Reset();

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
	
	// make hit with a leading time
	// and at least a trailing time found
	if (trail_found.size()>0) {
	  
	  emph::cmap::EChannel echan = lCh->first;
	  emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	  if (dchan.DetId()!=emph::geo::ARICH) {
	    std::cout << echan;
	    std::cout << " doesn't belong to the ARICH" << std::endl;
	    continue;
	  }
	  int pmt = dchan.HiLo();
	  int dch = dchan.Channel();
	  
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
	  int pxlx = fARICH2DHist->GetXaxis()->GetBinCenter(pxlxbin+1);
	  int pxly = fARICH2DHist->GetYaxis()->GetBinCenter(pxlybin+1);
	  fARICH2DHist->Fill(pxlx,pxly);
	  
	}//if trailing time found
	
      }//leading time loop
      
    }//leading time channel map loop
    
    rb::ARing ring;
    ring.SetNHits(fARICH2DHist->GetEntries());
    
    rings->push_back(ring);
    
  }
  
  //......................................................................
  void ARICHReco::produce(art::Event& evt)
  { 
    std::string labelstr = "raw:ARICH";
    // get arich trb3digits
    std::unique_ptr<std::vector<rb::ARing> > aringv(new std::vector<rb::ARing>);
    
    art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbhandle;
    try {
      evt.getByLabel(labelstr, trbhandle);
      if (!trbhandle->empty()) {
	GetARings(trbhandle,aringv);
      }
    }
    catch(...) {
      
    }
    evt.put(std::move(aringv));

  }

} // end namespace demo

DEFINE_ART_MODULE(emph::ARICHReco)
