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
#include "canvas/Persistency/Provenance/RunAuxiliary.h"
#include "canvas/Persistency/Provenance/SubRunAuxiliary.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "emphatic-artdaq/Overlays/TRB3Fragment.hh"
#include "emphatic-artdaq/Overlays/CAENV1720Fragment.hh"
#include "emphatic-artdaq/Overlays/FragmentType.hh"

#include "RawData/WaveForm.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"

#include "RawDataUnpacker/Unpacker.h"
#include "RawDataUnpacker/RawDataUnpacker_source.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>

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
    fSourceHelper(pm)
  {
    fDAQDataLabel = ps.get<std::string>("daqLabel","daq");
    fCreateArtEvents = ps.get<bool>("createArtEvents",true);
    fNumWaveFormPlots = ps.get<int>("numWaveFormPlots",100);
    fTimeWindow = ps.get<uint64_t>("timeWindow",20000);
    fNEvents    = ps.get<uint64_t>("nEvents",-1);
    fChanMapFileName = ps.get<std::string>("channelMapFileName","");
    fVerbosity  = ps.get<int>("verbosity",0);
    fSSDFilePrefix = ps.get<std::string>("SSDFilePrefix",
					 "RawDataSaver0FER1_Run");
    fReadSSDData = ps.get<bool>("readSSDData",false);
    
    std::string detStr;
    for (int idet=0; idet<emph::geo::NDetectors; ++idet) {
      if (idet == int(emph::geo::SSD)) continue;
      if (idet == int(emph::geo::RPC)) continue;      
      detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(idet));      
      help.reconstitutes<std::vector<emph::rawdata::WaveForm>, art::InEvent>("raw",detStr);
    }

    detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::T0));
    help.reconstitutes<std::vector<emph::rawdata::TRB3RawDigit>, art::InEvent>("raw",detStr);
    detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::RPC));
    help.reconstitutes<std::vector<emph::rawdata::TRB3RawDigit>, art::InEvent>("raw",detStr);
    detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::SSD));
    help.reconstitutes<std::vector<emph::rawdata::SSDRawDigit>, art::InEvent>("raw",detStr);

    fCurrentFilename = "";

    fIsFirst = true;
    
    fTimeOffset = 0;

    fEvtCount = 0;

    fSSDEvtIdx = 0;
    
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
    
    // create TTree for TRB3RawDigits
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

  bool Unpacker::createSSDDigits()
  {
    // make sure we can open SSD raw data files      
    char fileName[256];
    //    bool fSSDFilesOk = true;
    //    for (int i=0; i<4; ++i) {
    std::ifstream ssdFile;
    sprintf(fileName,"%sRun%d_%d.dat",fSSDFilePrefix.c_str(),fRun,fSubrun);
    ssdFile.open(fileName);
    if (!ssdFile.is_open()) {
      //      fSSDFilesOk = false;
      std::cerr << "Error: cannot open " << fileName << std::endl;
      return false;
    }
    //    std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > ssdDigvec;    
    //      fSSDRawDigits.push_back(ssdDigvec);
    auto ssdDigs = Unpack::readSSDHitsFromFileStream(ssdFile,true);
    fSSDRawDigits.push_back(ssdDigs);
    fSSDT0 = ssdDigs.first; // get time of first event
    while (!ssdFile.eof()) {
      ssdDigs = Unpack::readSSDHitsFromFileStream(ssdFile,false);
      fSSDRawDigits.push_back(ssdDigs);
    }
    if (fSSDRawDigits.size() > 1) {
      fSSDT0 = fSSDRawDigits[0].first; 
      fSSDEvtIdx = 0;
    }
    
    std::cout <<  "Found " << fSSDRawDigits.size() << " SSD events"
	      << std::endl;
    
    ssdFile.close();
    return true;
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

    art::ServiceHandle<art::TFileService> tfs;
    art::TFileDirectory tdir2 = tfs->mkdir("TimeDiffs","");	  
    char hname[256];
    char htitle[256];    

    if (V1720CFrag) {
      for (const auto& cont : *V1720CFrag) {
	artdaq::ContainerFragment contf(cont);
	if (contf.fragment_type() != ots::detail::FragmentType::CAENV1720) {
	  std::cout << "oh oh" << std::endl;
	  break;
	}	  
	for (size_t ifrag=0; ifrag < contf.block_count(); ++ifrag) {
	  C1720ContainerFragments[cont.fragmentID()].emplace_back(std::move(contf[ifrag]));
	  if (std::find(fFragId.begin(),fFragId.end(),cont.fragmentID()) == fFragId.end()) 
	    fFragId.push_back(cont.fragmentID());
	}	
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
	  if (std::find(fFragId.begin(),fFragId.end(),cont.fragmentID()) == fFragId.end()) 
	    fFragId.push_back(cont.fragmentID());	  
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
	auto& cfrag = *((*cfragIter).second.front());
	auto cfragId = (*cfragIter).first;
	emphaticdaq::TRB3Fragment trb3frag(cfrag);
	fTRB3RawDigits[cfragId].push_back(Unpack::GetTRB3RawDigitsFromFragment(trb3frag));
	fFragTimestamps[cfragId].push_back(cfrag.timestamp());	
	(*cfragIter).second.pop_front();
      }

      std::cout << "Made " << fTRB3RawDigits[(*cfragIter).first].size()
		<< " vectors of TRB3 digits for Frag Id "
		<< (*cfragIter).first << std::endl;

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
	auto& cfrag = *((*cfragIter).second.front());
	auto cfragId = (*cfragIter).first;
	emphaticdaq::CAENV1720Fragment caenfrag(cfrag);
	fWaveForms[cfragId].push_back(Unpack::GetWaveFormsFrom1720Fragment(caenfrag));	  
	fFragTimestamps[cfragId].push_back(cfrag.timestamp());
	(*cfragIter).second.pop_front();
      }
      std::cout << "Made " << fWaveForms[(*cfragIter).first].size()
		<< " vectors of WaveForms for Frag Id " << (*cfragIter).first
		<< std::endl;
      ++cfragIter;
   }  

    // now fill C1720 TTree
    for (const auto & wvfmMap : fWaveForms) { // loop over map
      for (auto & wvfmVec : wvfmMap.second) { // loop over vector of vectors=	
	for (auto & wvfm : wvfmVec) { // loop over vector
	  int ichan = wvfm.Board()*100 + wvfm.Channel();
	  sprintf(hname,"C1720_%d_%d",wvfm.Board(),wvfm.Channel());
	  art::TFileDirectory tdir = tfs->mkdir(hname,"");	  
	  if ( ! fC1720_HistCount.count(ichan))
	    fC1720_HistCount[ichan] = 0;	  
	  int ih = fC1720_HistCount[ichan];
	  if (ih < fNumWaveFormPlots) {
	    sprintf(hname,"C1720_%d_%d_h%03d",wvfm.Board(),wvfm.Channel(),ih);
	    sprintf(htitle,"Integrated Waveforms for CAEN 1720 Board %d, Channel %d, Fragment %d, Run %d, Subrun %d",wvfm.Board(),wvfm.Channel(), ih, fRun, fSubrun);
	    std::vector<uint16_t> adc = wvfm.AllADC();
	    int nsamp = adc.size();
	    TH1I* h1 = tdir.make<TH1I>(hname,htitle,nsamp,0.,float(nsamp));
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

  void Unpacker::makeTDiffHistos()
  {
    art::ServiceHandle<art::TFileService> tfs;
    art::TFileDirectory tdir2 = tfs->mkdir("TimeDiffs","");	  
    std::vector<TH1I*> tdiffHist;
    char hname[256];
    char htitle[256];    
    for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
      auto fragId = fFragId[ifrag];
      sprintf(hname,"tDiff_%d",fragId);
      sprintf(htitle,"Fragment Time Differences, Board %d",fragId);
      tdiffHist.push_back(tdir2.make<TH1I>(hname,htitle,250,0,500000));
      for (size_t i=1; i<fFragTimestamps[fragId].size(); ++i) {
	uint64_t tdiff = fFragTimestamps[fragId][i]-fFragTimestamps[fragId][i-1];
	tdiffHist[ifrag]->Fill(tdiff);
      }
    }
  }

  /***************************************************************************/

  bool Unpacker::readNext(art::RunPrincipal* const& ,//inR,
			  art::SubRunPrincipal* const& ,//inSR,
			  art::RunPrincipal* &outR,
			  art::SubRunPrincipal* &outSR,
			  art::EventPrincipal* &outE)
  {
    if (fNEvents > 0)
      if (fEvtCount == fNEvents)
	return false;

    if ((fEvtCount%1000) == 0)
      std::cout << "Event " << fEvtCount << std::endl;
    
    //    if (!inR) std::cout << "inR is empty" << std::endl;
    //    if (!inSR) std::cout << "inSR is empty" << std::endl;
    
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
      
      art::RunAuxiliary* runAux;
      art::SubRunAuxiliary* subrunAux;
      runs->SetBranchAddress("RunAuxiliary",&runAux);
      subruns->SetBranchAddress("SubRunAuxiliary",&subrunAux);
      runs->GetEvent(0);
      subruns->GetEvent(0);
      
      // deal with creating Run and Subrun objects
      fRun = runAux->run();
      fSubrun = subrunAux->subRun();
      outR = fSourceHelper.makeRunPrincipal(fRun,runAux->beginTime());
      outSR = fSourceHelper.makeSubRunPrincipal(fRun, fSubrun,
						subrunAux->beginTime());

      // get all of the digits if this is the first event
      // get all of the fragments out and create waveforms and digits
      if (! createDigitsFromArtdaqEvent()) return false;

      // create all of the SSD digits for this spill
      if (fReadSSDData)
	if (! createSSDDigits()) return false;
      
      if (fMakeTDiffHistos)
	makeTDiffHistos();
      
      // determine t0s for each board
      for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
	auto fragId = fFragId[ifrag];
	fT0[fragId] = fFragTimestamps[fragId][0];
      }
      fPrevTS = 0;
      
      fIsFirst = false;
    }
    

    if (fCreateArtEvents) {
      std::vector<std::unique_ptr<std::vector<emph::rawdata::WaveForm> > > evtWaveForms;
      for (int idet=0; idet<emph::geo::NDetectors; ++idet)
	evtWaveForms.push_back(std::make_unique<std::vector<emph::rawdata::WaveForm>  >());

      std::vector<std::unique_ptr<std::vector<emph::rawdata::TRB3RawDigit> > > evtTRB3Digits;
      for (int idet=0; idet<emph::geo::NDetectors; ++idet)
	evtTRB3Digits.push_back(std::make_unique<std::vector<emph::rawdata::TRB3RawDigit>  >());

      std::vector<emph::rawdata::SSDRawDigit> evtSSDVec;
      
      // find digits for the next event.  First, find earliest hits/wvfms
      uint64_t earliestTimestamp = 0;
      
      artdaq::Fragment::fragment_id_t thisFragId;
      uint64_t thisFragTimestamp;
      size_t thisFragCount;
      bool isFirstFrag = true;
      for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {	
	thisFragId = fFragId[ifrag];
	thisFragCount = fFragCounter[thisFragId];
	// bounds check:
	if (thisFragCount == fFragTimestamps[thisFragId].size()) continue;
	if (isFirstFrag)
	  earliestTimestamp = fFragTimestamps[thisFragId][thisFragCount] -
	  fT0[thisFragId];
	else {
	  thisFragTimestamp = fFragTimestamps[thisFragId][thisFragCount] -
	    fT0[thisFragId];
	  if (thisFragTimestamp < earliestTimestamp) {
	    earliestTimestamp = thisFragTimestamp;
	  }
	}
      }
      
      //      std::cout << fEvtCount << ", " << fSSDEvtIdx << ", " << std::endl;
	
      if ((fEvtCount > 0) && (fSSDEvtIdx > 0)  &&
	  (fSSDEvtIdx < fSSDRawDigits.size()-1) && fReadSSDData) {
	
	int64_t ssdArtDt = earliestTimestamp - fPrevTS - (fSSDRawDigits[fSSDEvtIdx].first - fSSDRawDigits[fSSDEvtIdx-1].first)*150;
	
	//	std::cout << "dT (artdaq-ssdots) = " << ssdArtDt << std::endl;
	if (abs(ssdArtDt) < (int64_t)fTimeWindow) {
	  auto & ssdDigs = fSSDRawDigits[fSSDEvtIdx].second;	  
	  if (!ssdDigs.empty()) {
	    //	    std::cout << "writing out " << ssdDigs.size() << " SSD digits"
	    //		      << std::endl;
	    for (auto ssdDig : ssdDigs) {
	      auto tssdDig(ssdDig);
	      //	      std::cout << tssdDig << std::endl;
	      evtSSDVec.push_back(tssdDig);
	    }
	  }
	}
      }

      fPrevTS = earliestTimestamp;
      if (fReadSSDData)
	++fSSDEvtIdx;
      auto evtSSDRawDigits = std::make_unique<std::vector<emph::rawdata::SSDRawDigit>  >(evtSSDVec);
      
      // now that we've found the earliest hit/wvfm, find all that are within fTimeWindow to associate with this art Event
      int nObjects = 0;
      
      for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
	thisFragId = fFragId[ifrag];
	thisFragCount = fFragCounter[thisFragId];
	// bounds check:
	if (thisFragCount == fFragTimestamps[thisFragId].size()) continue;
	
	if (fWaveForms.count(thisFragId)) {
	  thisFragTimestamp = fWaveForms[thisFragId][thisFragCount][0].FragmentTime() - fT0[thisFragId];
	  
	  if ((thisFragTimestamp - earliestTimestamp) < fTimeWindow) {
	    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
	    int boardNum = thisFragId;
	    emph::cmap::EChannel echan;
	    echan.SetBoardType(boardType);
	    echan.SetBoard(boardNum);
	    for (size_t jfrag=0; jfrag<fWaveForms[thisFragId][thisFragCount].size(); ++jfrag) {	      
	      auto & tdig = fWaveForms[thisFragId][thisFragCount][jfrag];
	      echan.SetChannel(tdig.Channel());
	      emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	      //  std::cout << echan << " maps to " << dchan << std::endl;
	      evtWaveForms[dchan.DetId()]->push_back(tdig);
	      ++nObjects;
	    }
	    fFragCounter[thisFragId] += 1;	  
	  }
	}
	else if (fTRB3RawDigits.count(thisFragId)) {
	  thisFragTimestamp = fTRB3RawDigits[thisFragId][thisFragCount][0].fragmentTimestamp - fT0[thisFragId];
	  if ((thisFragTimestamp - earliestTimestamp) < fTimeWindow) {
	    emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
	    int boardNum = thisFragId;
	    emph::cmap::EChannel echan;
	    echan.SetBoardType(boardType);
	    echan.SetBoard(boardNum);
	    //	    emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
	    for (size_t jfrag=0; jfrag<fTRB3RawDigits[thisFragId][thisFragCount].size(); ++jfrag) {
	      auto & tdig = fTRB3RawDigits[thisFragId][thisFragCount][jfrag];
	      int channel = tdig.GetChannel() + 64*(tdig.fgpa_header_word-1280);
	      echan.SetChannel(channel);
	      emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	      if (dchan.DetId() != emph::geo::NDetectors)
		evtTRB3Digits[dchan.DetId()]->push_back(tdig);
	      ++nObjects;
	    }
	    fFragCounter[thisFragId] += 1;
	  }
	}
      }
      
      // check that we're at the end, if we found nothing else to write out
      if (nObjects == 0) return false;
      
      // write out waveforms and TDCs to appropriate folders

      outE = fSourceHelper.makeEventPrincipal(fRun, fSubrun, fEvtCount++,
						  earliestTimestamp);
      
      if (fVerbosity) std::cout << "Event " << fEvtCount << ": " << std::endl;
      
      if (!evtSSDRawDigits->empty()) {
	if (fVerbosity)
	  std::cout << "\t" << evtSSDRawDigits->size() << " SSDRawDigits"
		    << std::endl; 
	put_product_in_principal(std::move(evtSSDRawDigits), *outE,"raw","SSD");	  
      }
      
      for (int idet=0; idet<emph::geo::NDetectors; ++idet) {
	std::string detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(idet));
      	if (!evtWaveForms[idet]->empty()) {
	  if (fVerbosity)
	    std::cout << "\t" << "Det " << detStr << " "
		      << evtWaveForms[idet]->size() << " waveforms"
		      << std::endl; 
	  put_product_in_principal(std::move(evtWaveForms[idet]), *outE,"raw",detStr);
	}
	if (!evtTRB3Digits[idet]->empty()) {
	  if (fVerbosity)
	    std::cout << "\t" << "Det " << detStr << " "
		      << evtTRB3Digits[idet]->size() << " TRB3RawDigits"
		      << std::endl; 
	  put_product_in_principal(std::move(evtTRB3Digits[idet]), *outE,"raw",detStr);
	}
      }
      
      return true;
    }

    return false;
  }
  
}
}

DEFINE_ART_INPUT_SOURCE(art::Source<emph::rawdata::Unpacker>)
