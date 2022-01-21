//-------------------------------------------------
//---------------------------------------

////////////////////////////////////////////////////////////////////////
// Class:       TRB3Dump
// Module Type: analyzer
// File:        TRB3Dump_module.cc
// Description: Prints out information about each event.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "canvas/Utilities/Exception.h"

#include "emphatic-artdaq/Overlays/TRB3Fragment.hh"
#include "artdaq-core/Data/Fragment.hh"

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
#include <bitset>

namespace emphaticdaq {
  class TRB3Dump;
}

/**************************************************************************************************/

class emphaticdaq::TRB3Dump : public art::EDAnalyzer {

public:
  struct Config {
    //--one atom for each parameter
    fhicl::Atom<art::InputTag> DataLabel {
      fhicl::Name("data_label"), // "daq::TRB3"
      fhicl::Comment("Tag for the input data product")
    };
  }; //--configuration
  using Parameters = art::EDAnalyzer::Table<Config>;

  explicit TRB3Dump(Parameters const & pset);
  virtual ~TRB3Dump();

  void analyze(const art::Event& evt) override;
  void beginJob() override;
  void endJob() override;

private:

  //--default values
  uint32_t nChannels;//    = 8;
  uint32_t Ttt_DownSamp;// =  4;
 /* the trigger time resolution is 16ns when waveforms are sampled at
                               * 500MHz sampling. The trigger timestamp is thus
                               * sampled 4 times slower than input channels*/

  TH1F*    hEventCounter;

  TTree* fEventTree;
  int fRun;
  int fSubrun;
  art::EventNumber_t fEvent;

  bool firstEvt = true;
  art::InputTag fDataLabel;

}; //--class TRB3Dump


emphaticdaq::TRB3Dump::TRB3Dump(TRB3Dump::Parameters const& pset): art::EDAnalyzer(pset)
{
  fDataLabel = pset().DataLabel();
}

void emphaticdaq::TRB3Dump::beginJob()
{
  art::ServiceHandle<art::TFileService> tfs;
  /************************************************************************************************/
  hEventCounter   = tfs->make<TH1F>("hEventCounter","Event Counter Histogram",10000,0,10000);
  /************************************************************************************************/
  //--make tree to store the channel waveform info:
  fEventTree = tfs->make<TTree>("events","trb3 tree");
  fEventTree->Branch("fRun",&fRun,"fRun/I");
  fEventTree->Branch("fSubrun",&fSubrun,"fSubrun/I");
  fEventTree->Branch("fEvent",&fEvent,"fEvent/I");
}

void emphaticdaq::TRB3Dump::endJob()
{
  std::cout << "Ending TRB3Dump...\n";
}


emphaticdaq::TRB3Dump::~TRB3Dump()
{
}


void emphaticdaq::TRB3Dump::analyze(const art::Event& evt)
{

  fRun = evt.run();
  fSubrun = evt.subRun();
  fEvent = evt.event();

  /************************************************************************************************/
  art::Handle< std::vector<artdaq::Fragment> > trb3_handle;
  if ( !evt.getByLabel(fDataLabel, trb3_handle) ) {
//    art::fill_ptr_vector(Frags,trb3_handle);
//  }
//  else {
    std::cout << "Requested fragments with label : " << fDataLabel << "but none exist\n";
    return;
  }

  if (trb3_handle.isValid()) {

    std::cout << "######################################################################\n";
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << fEvent << " has " << trb3_handle->size()
              << " fragment(s).\n";

    if (trb3_handle->size() != 1) {
      std::cout << "Error! " << evt.run() << ", subrun " << evt.subRun()
                << ", event " << fEvent << " has " << trb3_handle->size()
                << " fragments. We only expect 1.\n";
      return;
    }

    const auto& frag((*trb3_handle)[0]);
    TRB3Fragment trbFrag(frag);
    TRB3Fragment::TRB3EventHeader const* header = trbFrag.dataBegin();
    std::cout << "word 1: " << std::hex << header->unknown_word_1 << std::endl;
    std::cout << "word 2: " << header->unknown_word_2 << std::endl;
    std::cout << "event_size: " << header->event_size << std::endl;
    std::cout << "endian: " << header->endian_marker_1 << " trigger type: " << header->trigger_type << " reserved: " << header->unknown_word_3 << " word_size: " << header->word_size << " 0x0: " << header->unknown_word_3b << std::endl;
    std::cout << "sub_event_id: " << header->sub_event_id << " rest of word 5: " << header->unknown_word_4 << std::endl;
    std::cout << "trigger code: " << header->trigger_code << " trigger number: " << header->trigger_number << std::endl;

    std::cout << "Size of header: " << std::dec << sizeof(TRB3Fragment::TRB3EventHeader)/sizeof(uint32_t) << std::endl;
    std::cout << "Words in sub event: " << header->event_size/4 <<std::endl;

    const uint32_t* data_word = reinterpret_cast<uint32_t const*>(frag.dataBeginBytes() + sizeof(TRB3Fragment::TRB3EventHeader));

    // loop over sub sub events (0500, 0501, 0502, 0503)
    for (unsigned int isse=0; isse<4; isse++) {
      TRB3Fragment::TRB3SubEventHeader const* sseheader = reinterpret_cast<TRB3Fragment::TRB3SubEventHeader const*>(data_word);
      std::cout << "sse_id: " << std::hex << sseheader->subevent_id << " sse length: " << sseheader->subevent_size << std::endl;
      data_word++;
      // loop over words in sub sub event
      for (unsigned int iword=0; iword<sseheader->subevent_size; iword++){
        std::cout << "word: " << std::hex << *(data_word+iword) << std::endl;
      }
      data_word+=sseheader->subevent_size;
    }

    // this ends the tdc data - do we want to check anything with the trailer?

    // Just for testing timestamp stuff now
    // data_word = reinterpret_cast<uint32_t const*>(frag.dataBeginBytes() + sizeof(TRB3Fragment::TRB3EventHeader));
    // // we're now at the start of the first sub sub event. We want stuff from 502, so skip past the first 2 sses.
    // for (unsigned int isse=0; isse<2; isse++){
    //   TRB3Fragment::TRB3SubEventHeader const* sseheader = reinterpret_cast<TRB3Fragment::TRB3SubEventHeader const*>(data_word);
    //   data_word+=sseheader->subevent_size+1;
    // }
    // // should now be at start of third sub sub event: 0502
    // TRB3Fragment::TRB3SubEventHeader const* sseheader = reinterpret_cast<TRB3Fragment::TRB3SubEventHeader const*>(data_word);
    // std::cout << "sse_id: " << std::hex << sseheader->subevent_id << " sse length: " << sseheader->subevent_size << std::endl;
    // data_word++;
    // uint32_t epoch_word = 0;
    // uint32_t tdc_word = 0;
    // for (unsigned int iword=0; iword<sseheader->subevent_size; iword++){
    //   if ( (*(data_word+iword) & 0xe0000000) ==  0x60000000)
    //     epoch_word = *(data_word+iword);
    //   if ( (*(data_word+iword) & 0xe0000000) ==  0x80000000)
    //     tdc_word = *(data_word+iword);
    //   if (epoch_word && tdc_word)
    //     break;
    // }
    // if ( !(epoch_word && tdc_word) ) {
    //   std::cout << "Did not find an epoch and/or tdc word for 0502! This shouldn't happen. Can't assign a timestamp." <<std::endl;
    //   std::cout << "epoch word: " << epoch_word << " tdc word: " << tdc_word << " Aborting!"<<std::endl;
    //   abort();
    // }
    //
    // std::cout << "epoch word: " << epoch_word << " tdc word: " << tdc_word << std::endl;
    // std::cout << "epoch: " << std::dec << ( epoch_word & 0xfffffff) << std::endl;
    // std::cout << "channel: " << ( (tdc_word & 0xfc00000) >> 22 ) << std::endl;
    // std::cout << "coarse time: " << ( tdc_word & 0x7ff ) << std::endl;
    //
    // auto epoch_time  = (epoch_word & 0xfffffff)*10240.026; // epoch tick is 10240.026 ns
    // auto coarse_time = (tdc_word & 0x7ff)*5; // coarse tick is 5 ns
    // std::cout << "Timestamp: " << epoch_time+coarse_time << " ns" << std::endl;
    // std::cout << "Timestamp: " << (epoch_time + coarse_time)/1e9 << " s" << std::endl;

    fEventTree->Fill();

  } //--valid fragments
}

DEFINE_ART_MODULE(emphaticdaq::TRB3Dump)
