////////////////////////////////////////////////////////////////////////
// Class:       RawDataUnpacker
// Module Type: input source
// Author:      jpaley@fnal.gov, eflumerf@fnal.gov
// Description: start with an artdaq RawInput source, and create art::Events
//              based on the fragments stored there.
////////////////////////////////////////////////////////////////////////

#include "artdaq/ArtModules/ArtdaqFragmentNamingService.h"
#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"
#include "artdaq-core/Data/ContainerFragment.hh"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "emphatic-artdaq/Overlays/TRB3Fragment.hh"
#include "emphatic-artdaq/Overlays/CAENV1720Fragment.hh"
#include "emphatic-artdaq/Overlays/FragmentType.hh"

#include "RawData/WaveForm.h"
#include "RawData/TRB3RawDigit.h"

#include "RawDataUnpacker/Unpacker.h"
#include "RawDataUnpacker/RawDataUnpacker_source.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"

#include <iostream>
#include <iterator>
#include <algorithm>

namespace {
  // Read product for given type T
  template <typename T>
  T const* readProduct(TTree& events, std::string const& branchName)
  {
    art::EDProduct* p = nullptr;
    events.SetBranchAddress(branchName.c_str(), &p);
    events.GetEntry(0); // Read first entry in tree
    
    auto wrapped_product = dynamic_cast<art::Wrapper<T> const*>(p);
    if (!wrapped_product) {
      return nullptr;
    }
    if (!wrapped_product->isPresent()) {
      return nullptr;
    }
    
    return wrapped_product->product();
  }
}

namespace emph {

namespace rawdata {
    
  /***************************************************************************/

  Unpacker::Unpacker(fhicl::ParameterSet const& ps, art::ProductRegistryHelper& help, art::SourceHelper const& pm) :
    fSourceHelper(pm),
    fDAQDataLabel(ps.get<std::string>("daqLabel","daq"))  
  {
    help.reconstitutes<std::vector<emph::rawdata::WaveForm>, art::InEvent>(fDAQDataLabel,"raw");
    help.reconstitutes<std::vector<emph::rawdata::TRB3RawDigit>, art::InEvent>(fDAQDataLabel,"raw");
    //    help.reconstitutes<std::vector<emph::rawdata::SSDRawDigit>, art::InEvent>("ssddaq","raw"); // note, "ssddaq" is nonsense

    fCurrentFilename = "";

    fIsFirst = true;

    fTimeOffset = 0;

    art::ServiceHandle<art::TFileService> tfs;

    fTRB3Tree = tfs->make<TTree>("TRB3Tree","");
    fTRB3Tree->Branch("headerWord",&fTRB3_HeaderWord);
    fTRB3Tree->Branch("measurement",&fTRB3_Measurement);
    fTRB3Tree->Branch("channel",&fTRB3_Channel);
    fTRB3Tree->Branch("finetime",&fTRB3_FineTime);
    fTRB3Tree->Branch("epochtime",&fTRB3_EpochTime);
    fTRB3Tree->Branch("coarsetime",&fTRB3_CoarseTime);
          
  }
  
  /***************************************************************************/
  
  void Unpacker::closeCurrentFile()
  {

  }

  /***************************************************************************/

  void Unpacker::readFile(std::string const &name,
			 art::FileBlock* &fb)
  {
    fCurrentFilename = name;
    fb = new art::FileBlock(art::FileFormatVersion{1, "RawEvent2022"},
			    fCurrentFilename);
  }
  
  /***************************************************************************/

  bool Unpacker::createDigitsFromArtdaqEvent()
  {
    std::unordered_map<artdaq::Fragment::fragment_id_t,artdaq::FragmentPtrs> C1720ContainerFragments;
    std::unordered_map<artdaq::Fragment::fragment_id_t,artdaq::FragmentPtrs> TRB3ContainerFragments;
    
    std::unique_ptr<TFile> input_file{TFile::Open(fCurrentFilename.c_str())};
    if (!input_file) {
      std::cerr << "Could not open file.\n" << std::endl;
      return false;
    }
    std::cout << "opened file" << std::endl;
    std::unique_ptr<TTree> events{input_file->Get<TTree>("Events")};
    if (!events) {
      std::cerr << "Could not find Events tree.\n";
      return false;
    }
    std::cout << "tree size = " << events->GetEntries() << std::endl;
    
    auto V1720CFrag = readProduct<std::vector<artdaq::Fragment> >(*events, "artdaq::Fragments_daq_ContainerCAENV1720_DAQEventBuilder.");
    auto TRB3CFrag = readProduct<std::vector<artdaq::Fragment> >(*events, "artdaq::Fragments_daq_ContainerTRB3_DAQEventBuilder.");
    
    if (V1720CFrag) {
      for (const auto& cont : *V1720CFrag) {
	artdaq::ContainerFragment contf(cont);
	if (contf.fragment_type() != ots::detail::FragmentType::CAENV1720) {
	  std::cout << "oh oh" << std::endl;
	  break;
	}	  
	for (size_t ifrag=0; ifrag < contf.block_count(); ++ifrag) 
	  C1720ContainerFragments[cont.fragmentID()].emplace_back(std::move(contf[ifrag]));
	
      }
      std::cout << "V1720 block count: " << V1720CFrag->size()
		<< std::endl;
    }
    
    if (TRB3CFrag) {
      for (const auto& cont : *TRB3CFrag) {
	artdaq::ContainerFragment contf(cont);
	if (contf.fragment_type() != ots::detail::FragmentType::TRB3) {
	  std::cout << "oh oh" << std::endl;
	  break;
	}
	
	for (size_t ifrag=0; ifrag < contf.block_count(); ++ifrag) {
	  TRB3ContainerFragments[cont.fragmentID()].emplace_back(std::move(contf[ifrag]));
	}
      }
      std::cout << "TRB3 block count: " << TRB3CFrag->size()
		<< std::endl;
    }
    
    auto cfragIter = TRB3ContainerFragments.begin();
    
    while (cfragIter != TRB3ContainerFragments.end()) {
      fFragCounter[(*cfragIter).first] = 0;

      // now make digits
      while (! ((*cfragIter).second.empty())) {
	emphaticdaq::TRB3Fragment trb3frag(*((*cfragIter).second.front()));
	fTRB3RawDigits[(*cfragIter).first].push_back(Unpack::GetTRB3RawDigitsFromFragment(trb3frag));	  
	(*cfragIter).second.pop_front();
      }

      std::cout << "Made " << fTRB3RawDigits[(*cfragIter).first].size()
		<< " vectors of TRB3 digits" << std::endl;

      ++cfragIter;
    }

    // now fill TRB3 TTree
    for (const auto & trb3digMap : fTRB3RawDigits) { // loop over map
      for (auto & digVec : trb3digMap.second) { // loop over vector of vectors
	fTRB3_HeaderWord.clear();
	fTRB3_Measurement.clear();
	fTRB3_Channel.clear();
	fTRB3_FineTime.clear();
	fTRB3_EpochTime.clear();
	fTRB3_CoarseTime.clear();
	for (auto & dig : digVec) { // loop over vector
	  fTRB3_HeaderWord.push_back(dig.fgpa_header_word);
	  fTRB3_Measurement.push_back(dig.GetMeasurement());
	  fTRB3_Channel.push_back(dig.GetChannel());
	  fTRB3_FineTime.push_back(dig.GetFineTime());
	  fTRB3_EpochTime.push_back(dig.GetEpochCounter());
	  fTRB3_CoarseTime.push_back(dig.GetCoarseTime());
	}
	fTRB3Tree->Fill();
      }
    }
    
    cfragIter = C1720ContainerFragments.begin();
    
    while (cfragIter != C1720ContainerFragments.end()) {
      // initialize counter
      fFragCounter[(*cfragIter).first] = 0;

      // now make digits
      while (! ((*cfragIter).second.empty())) {
	emphaticdaq::CAENV1720Fragment caenfrag(*((*cfragIter).second.front()));
	fWaveForms[(*cfragIter).first].push_back(Unpack::GetWaveFormsFrom1720Fragment(caenfrag));	  
	(*cfragIter).second.pop_front();
      }
      std::cout << "Made " << fWaveForms[(*cfragIter).first].size()
		<< " vectors of WaveForms" << std::endl;
      ++cfragIter;
    }  

    // now fill C1720 TTree
    char hname[256];
    char htitle[256];    
    art::ServiceHandle<art::TFileService> tfs;
    for (const auto & wvfmMap : fWaveForms) { // loop over map
      for (auto & wvfmVec : wvfmMap.second) { // loop over vector of vectors=	
	for (auto & wvfm : wvfmVec) { // loop over vector
	  int ichan = wvfm.Board()*100 + wvfm.Channel();
	  sprintf(hname,"C1720_%d_%d",wvfm.Board(),wvfm.Channel());
	  art::TFileDirectory tdir = tfs->mkdir(hname,"");	  
	  if ( ! fC1720_HistCount.count(ichan))
	    fC1720_HistCount[ichan] = 0;	  
	  int ih = fC1720_HistCount[ichan];
	  if (ih < 100) {
	    sprintf(hname,"C1720_%d_%d_h%03d",wvfm.Board(),wvfm.Channel(),ih);
	    sprintf(htitle,"Integrated Waveforms for CAEN 1720 Board %d, Channel %d, Fragment %d",wvfm.Board(),wvfm.Channel(), ih);
	    int nsamp = 400;
	    TH1I* h1 = tdir.make<TH1I>(hname,htitle,nsamp,0.,float(nsamp));
	    std::vector<uint16_t> adc = wvfm.AllADC();
	    for (size_t i=0; i<adc.size() && int(i)<nsamp; ++i)
	      h1->SetBinContent(i+1,adc[i]);	 
	    fC1720_HistCount[ichan] += 1;
	  }
	}
      }
    }

    return true;
  }
  
  
  /***************************************************************************/

  bool Unpacker::readNext(art::RunPrincipal* const& inR,
			  art::SubRunPrincipal* const& inSR,
			  art::RunPrincipal* &outR,
			  art::SubRunPrincipal* &outSR,
			  art::EventPrincipal* &outE)
  {
    // note, at a minimum we must always return an outE
    outE = nullptr;
    outR = nullptr;
    outSR = nullptr;

    if (fIsFirst) {

      std::unique_ptr<TFile> input_file{TFile::Open(fCurrentFilename.c_str())};
      if (!input_file) {
	std::cerr << "Could not open file.\n" << std::endl;
	return false;
      }
      std::cout << "opened file" << std::endl;
      std::unique_ptr<TTree> runs{input_file->Get<TTree>("Runs")};
      if (!runs) {
	std::cerr << "Could not find Runs tree.\n";
	return false;
      }
      std::cout << "runs tree size = " << runs->GetEntries() << std::endl;

      std::unique_ptr<TTree> subruns{input_file->Get<TTree>("SubRuns")};
      if (!subruns) {
	std::cerr << "Could not find SubRuns tree.\n";
	return false;
      }
      std::cout << "subruns tree size = " << subruns->GetEntries() << std::endl;
      
      if (!inR) std::cout << "inR is empty" << std::endl;
      if (!inSR) std::cout << "inSR is empty" << std::endl;
						    
      // deal with creating Run and Subrun objects
      art::Timestamp currentTime = time(0);
      outR = fSourceHelper.makeRunPrincipal(1,currentTime); // 0 --> Timestamp
      outSR = fSourceHelper.makeSubRunPrincipal(1,0,currentTime);
      
      // now get all of the fragments out and create waveforms and digits
      if (! createDigitsFromArtdaqEvent()) return false;

      fIsFirst = false;
    }

    // find digits for the next event.  First, find earliest hits/wvfms
    /*
    auto cfragIter = C1720ContainerFragments.begin();      
    auto ts1 = (*cfragIter).second.front()->timestamp();
    auto tfragIter = TRB3ContainerFragments.begin();      
    auto ts2 = (*tfragIter).second.front()->timestamp();      
    std::cout << "ts1 = " << ts1 << ", ts2 = " << std::endl;
    fTimeOffset = ts2 - ts1;    
    */
    
    /*
	fTRB3RawDigits[(*cfragIter).first] = 
	  Unpack::GetTRB3RawDigitsFromFragment(trb3frag);
	std::cout << "Made " << fTRB3RawDigits[(*cfragIter).first].size()
		  << " TRB3 digits" << std::endl;
	(*cfragIter).second.pop_front();
	*/
      /*	
	if ((*cfragIter).first == earliestFragId) {
	thisEventFragments.emplace_back(std::move((*cfragIter).second.front()));
	(*cfragIter).second.pop_front();
      }
      if ((*cfragIter).second.empty()) continue;
      
      if ((*cfragIter).second.front()->timestamp() -
	  earliestTimestamp < 20000) {
	thisEventFragments.emplace_back(std::move((*cfragIter).second.front()));
	(*cfragIter).second.pop_front();
      }
      ++cfragIter;
    }
      
      while (cfragIter != C1720ContainerFragments.end()) {
	if ((*cfragIter).second.empty()) continue;
	
	auto ts = (*cfragIter).second.front()->timestamp();
	if (ts < earliestTimestamp) {
	  earliestTimestamp = ts;
	  earliestFragId = (*cfragIter).first;
	}
	++cfragIter;
    }
    

      auto evtFragIter = thisEventFragments.begin();
      while (evtFragIter != thisEventFragments.end()) {
	std::cout << "frag " << (*evtFragIter)->fragmentID() << ", sequence "
		  << (*evtFragIter)->sequenceID() << " of type "
		  << (*evtFragIter)->typeString() << " has size "
		  << (*evtFragIter)->size() << std::endl;	      
	
	std::vector<emph::rawdata::TRB3RawDigit> trb3digits;
	std::vector<emph::rawdata::WaveForm> waveforms;
	
	if ((*evtFragIter)->type() == ots::detail::FragmentType::TRB3) {
	  std::cout << "%%%%%%%%%  HERE  %%%%%%%%%" << std::endl;
	  emphaticdaq::TRB3Fragment trb3frag(*(*evtFragIter));
	  trb3digits = Unpack::GetTRB3RawDigitsFromFragment(trb3frag);
	  std::cout << "Made " << trb3digits.size() << " TRB3 digits" << std::endl;
	}
	if ((*evtFragIter)->type() == ots::detail::FragmentType::CAENV1720) {
	  emphaticdaq::CAENV1720Fragment caenfrag(*(*evtFragIter));
	  waveforms = Unpack::GetWaveFormsFrom1720Fragment(caenfrag);
	  std::cout << "Made " << waveforms.size() << " CAEN1720 waveforms" << std::endl;
	}      
	++evtFragIter;
      }
      */

    // now determine the time offset between the TRB3s and CAENs
    // get the timestamp from the first CAEN fragment;
    /*
    auto cfragIter = C1720ContainerFragments.begin();      
    auto ts1 = (*cfragIter).second.front()->timestamp();
    auto tfragIter = TRB3ContainerFragments.begin();      
    auto ts2 = (*tfragIter).second.front()->timestamp();      
    std::cout << "ts1 = " << ts1 << ", ts2 = " << std::endl;
    fTimeOffset = ts2 - ts1;    
      }
      // now extract next event
    
    // first find earliest next data fragment
    artdaq::Fragment::fragment_id_t earliestFragId = artdaq::Fragment::InvalidFragmentID;
    artdaq::Fragment::timestamp_t earliestTimestamp = artdaq::Fragment::InvalidTimestamp;

    auto cfragIter = C1720ContainerFragments.begin();

    while (cfragIter != C1720ContainerFragments.end()) {
      if ((*cfragIter).second.empty()) continue;
      auto ts = (*cfragIter).second.front()->timestamp();
      if (ts < earliestTimestamp) {
	earliestTimestamp = ts;
	earliestFragId = (*cfragIter).first;
      }
      ++cfragIter;
    }
    
    cfragIter = TRB3ContainerFragments.begin();

    while (cfragIter != TRB3ContainerFragments.end()) {
      if ((*cfragIter).second.empty()) continue;
      auto ts = (*cfragIter).second.front()->timestamp();
      if (ts < earliestTimestamp) {
	earliestTimestamp = ts;
	earliestFragId = (*cfragIter).first;
      }
      ++cfragIter;
    }

    if (earliestTimestamp == artdaq::Fragment::InvalidTimestamp)
      // no more data, at end of the input file
      return false;
    
    // look for time-matched fragments from the others
    artdaq::FragmentPtrs thisEventFragments;

    cfragIter = TRB3ContainerFragments.begin();

    while (cfragIter != TRB3ContainerFragments.end()) {
      if ((*cfragIter).first == earliestFragId) {
	thisEventFragments.emplace_back(std::move((*cfragIter).second.front()));
	(*cfragIter).second.pop_front();
      }
      if ((*cfragIter).second.empty()) continue;
      
      if ((*cfragIter).second.front()->timestamp() -
	  earliestTimestamp < 20000) {
	thisEventFragments.emplace_back(std::move((*cfragIter).second.front()));
	(*cfragIter).second.pop_front();
      }
      ++cfragIter;
    }

    cfragIter = C1720ContainerFragments.begin();

    while (cfragIter != C1720ContainerFragments.end()) {
      if ((*cfragIter).first == earliestFragId) {
	thisEventFragments.emplace_back(std::move((*cfragIter).second.front()));
	(*cfragIter).second.pop_front();
      }
      if ((*cfragIter).second.empty()) continue;
      
      if ((*cfragIter).second.front()->timestamp() -
 	  earliestTimestamp < 20000) {
	thisEventFragments.emplace_back(std::move((*cfragIter).second.front()));
	(*cfragIter).second.pop_front();
      }
      ++cfragIter;
    }    
    
    for (auto & frag : thisEventFragments) {
      // do the digit extraction from the waveforms digitizers and TRB3s
      std::cout << "frag " << frag->fragmentID() << ", sequence " << frag->sequenceID() << " of type " << frag->typeString() << " has size " << frag->size() << std::endl;	
      std::vector<emph::rawdata::TRB3RawDigit> trb3digits;
      std::vector<emph::rawdata::WaveForm> waveforms;
      
      if (frag->type() == ots::detail::FragmentType::TRB3) {
	emphaticdaq::TRB3Fragment trb3frag(*frag);
	trb3digits = Unpack::GetTRB3RawDigitsFromFragment(trb3frag);
	std::cout << "Made " << trb3digits.size() << " TRB3 digits" << std::endl;
      }
      if (frag->type() == ots::detail::FragmentType::CAENV1720) {
	emphaticdaq::CAENV1720Fragment caenfrag(*frag);
	waveforms = Unpack::GetWaveFormsFrom1720Fragment(caenfrag);
	std::cout << "Made " << waveforms.size() << " CAEN1720 waveforms" << std::endl;
      }
    }
      */
    
    return true;
  }
  
}
}

DEFINE_ART_INPUT_SOURCE(art::Source<emph::rawdata::Unpacker>)
