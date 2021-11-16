////////////////////////////////////////////////////////////////////////
// Class:       CAENV1730WaveformAna
// Module Type: analyzer
// File:        CAENV1730WaveformAna_module.cc
// Description: Makes a tree with waveform information.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "canvas/Utilities/Exception.h"

#include "sbndaq-artdaq-core/Overlays/Common/CAENV1730Fragment.hh"
#include "artdaq-core/Data/Fragment.hh"

//#include "art/Framework/Services/Optional/TFileService.h" //before art_root_io transition
#include "art_root_io/TFileService.h"
#include "TH1F.h"
#include "TNtuple.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <numeric>

namespace sbndaq {
  class CAENV1730WaveformAna;
}

/*****/

class sbndaq::CAENV1730WaveformAna : public art::EDAnalyzer {

public:
  explicit CAENV1730WaveformAna(fhicl::ParameterSet const & pset); // explicit doesn't allow for copy initialization
  virtual ~CAENV1730WaveformAna();
  
  virtual void analyze(art::Event const & evt);
  
private:
  
  TNtuple* nt_header; //Ntuple header
  TNtuple* nt_wvfm;
  std::vector< std::vector<uint16_t> >  fWvfmsVec;
  
};

//Define the constructor
sbndaq::CAENV1730WaveformAna::CAENV1730WaveformAna(fhicl::ParameterSet const & pset)
  : EDAnalyzer(pset)
{
  art::ServiceHandle<art::TFileService> tfs; //pointer to a file named tfs
  nt_header = tfs->make<TNtuple>("nt_header","CAENV1730 Header Ntuple","art_ev:caen_ev:caen_ev_tts");
  nt_wvfm = tfs->make<TNtuple>("nt_wvfm","Waveform information Ntuple","art_ev:caen_ev:caen_ev_tts:ch:ped:rms:temp");
}

sbndaq::CAENV1730WaveformAna::~CAENV1730WaveformAna()
{
}

void sbndaq::CAENV1730WaveformAna::analyze(art::Event const & evt)
{
  
  art::EventNumber_t eventNumber = evt.event();
  
  art::Handle< std::vector<artdaq::Fragment> > rawFragHandle; // it is a pointer to a vector of art fragments
  evt.getByLabel("daq","CAENV1730", rawFragHandle); // it says how many fragments are in an event
  
  if (!rawFragHandle.isValid()) return;
  
  std::cout << "######################################################################" << std::endl;
  std::cout << std::endl;
  
  std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
	    << ", event " << eventNumber << " has " << rawFragHandle->size()
	    << " fragment(s)." << std::endl;
  
  for (size_t idx = 0; idx < rawFragHandle->size(); ++idx) { // loop over the fragments of an event
    
    const auto& frag((*rawFragHandle)[idx]); // use this fragment as a refernce to the same data
    CAENV1730Fragment bb(frag);
    
    auto const* md = bb.Metadata();

    CAENV1730Event const* event_ptr = bb.Event();
    CAENV1730EventHeader header = event_ptr->Header;
    
    std::cout << "\tFrom header, event counter is " << header.eventCounter << std::endl;
    std::cout << "\tFrom header, triggerTimeTag is " << header.triggerTimeTag << std::endl;
    
    nt_header->Fill(eventNumber,header.eventCounter,header.triggerTimeTag);
    
    
    //get the number of 32-bit words from the header
    size_t const& ev_size(header.eventSize);
    
    size_t nChannels = md->nChannels; //fixme
    fWvfmsVec.resize(nChannels);
    
    //use that to get the number of 16-bit words for each channel
    size_t n_samples = (ev_size - sizeof(CAENV1730EventHeader)/sizeof(uint32_t))*2/nChannels;
    const uint16_t* data = reinterpret_cast<const uint16_t*>(frag.dataBeginBytes() + sizeof(CAENV1730EventHeader));
    
    for(size_t i_ch=0; i_ch<nChannels; ++i_ch){
      fWvfmsVec[i_ch].resize(n_samples);
      
      //fill...
      for (size_t i_t=0; i_t<n_samples; ++i_t){
	if(i_t%2==0) fWvfmsVec[i_ch][i_t] = *(data+n_samples+i_t+1);
	else if(i_t%2==1) fWvfmsVec[i_ch][i_t] = *(data+n_samples+i_t-1);
      }

      //by here you have a vector<uint16_t> that is the waveform, in fWvfmsVec[i_ch]

      
      //get mean
      float wvfm_mean = std::accumulate(fWvfmsVec[i_ch].begin(),fWvfmsVec[i_ch].end(),0.0) / fWvfmsVec[i_ch].size();
      
      //get rms
      float wvfm_rms=0.0;
      for(auto const& val : fWvfmsVec[i_ch])
	wvfm_rms += (val-wvfm_mean)*(val-wvfm_mean);
      wvfm_rms = std::sqrt(wvfm_rms/fWvfmsVec[i_ch].size());
      
      nt_wvfm->Fill(eventNumber,header.eventCounter,header.triggerTimeTag,
		    i_ch,wvfm_mean,wvfm_rms,md->chTemps[i_ch]);
    }
  }
  
}

DEFINE_ART_MODULE(sbndaq::CAENV1730WaveformAna)
//this is where the name is specified
