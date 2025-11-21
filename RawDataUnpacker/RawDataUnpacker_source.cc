///////////////////////////////////////////////////////////////////////
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
#include "RawDataUnpacker/SSDUnpacker.h"
#include "RawDataUnpacker/RawDataUnpacker_source.h"

//#include "ChannelMap/ChannelMapService.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TString.h"
#include "TF1.h"
#include "TGraph.h"
#include "TVector.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "TimeSync.h"

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
    fVerbosity  = ps.get<int>("verbosity",0);
    fSSDFilePrefix = ps.get<std::string>("SSDFilePrefix",
					 "RawDataSaver0FER1_Run");
    fReadSSDData = ps.get<bool>("readSSDData",false);
    fReadCAENData = ps.get<bool>("readCAENData",false);
    fReadTRB3Data = ps.get<bool>("readTRB3Data",false);
    fNFER = ps.get<int>("NFER",0); // Number of Front End Readouts: used for merging SSD data
    fBCOx = ps.get<double>("BCOx",151.1515152); // Scales SSD timestamps (related to clock freq of SSD)
    fFirstSubRunHasExtraTrigger = ps.get<bool>("firstSubRunHasExtraTrigger",false);
    fMakeTimeWalkHistos = ps.get<bool>("makeTimeWalkHistos",false);
    fMakeBenchmarkPlots = ps.get<bool>("makeBenchmarkPlots",false);

    std::string detStr;
    for (int idet=0; idet<emph::geo::NDetectors; ++idet) {
      if (idet == int(emph::geo::SSD)) continue;
      if (idet == int(emph::geo::RPC)) continue;
      if (idet == int(emph::geo::ARICH)) continue;
      detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(idet));
      help.reconstitutes<std::vector<emph::rawdata::WaveForm>, art::InEvent>("raw",detStr);
    }

    detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::ARICH));
    help.reconstitutes<std::vector<emph::rawdata::TRB3RawDigit>, art::InEvent>("raw",detStr);
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

    fSpillTime = 0;

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
    auto ssdDigs = SSDUnpack::readSSDHitsFromFileStream(ssdFile,true);
    fSSDRawDigits.push_back(ssdDigs);
    fSSDT0 = ssdDigs.first; // get time of first event
    while (!ssdFile.eof()) {
      ssdDigs = SSDUnpack::readSSDHitsFromFileStream(ssdFile,false);
      fSSDRawDigits.push_back(ssdDigs);
    }
    if (fSSDRawDigits.size() > 1) {
      fSSDT0 = fSSDRawDigits[1].first;
      fSSDEvtIdx = 1;
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

    art::ServiceHandle<art::TFileService> tfs;
    art::TFileDirectory tdir2 = tfs->mkdir("TimeDiffs","");
    char hname[256];
    char htitle[256];

    if (fReadCAENData) {
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
    }

    if (fReadTRB3Data) {
      auto TRB3CFrag = readProduct<std::vector<artdaq::Fragment> >(*events, "artdaq::Fragments_daq_ContainerTRB3_DAQEventBuilder.");
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
	    fTRB3_HeaderWord.push_back(dig.GetFPGAHeaderWord());
	    fTRB3_Measurement.push_back(dig.GetMeasurement());
	    fTRB3_Channel.push_back(dig.GetChannel());
	    fTRB3_FineTime.push_back(dig.GetFineTime());
	    fTRB3_EpochTime.push_back(dig.GetEpochCounter());
	    fTRB3_CoarseTime.push_back(dig.GetCoarseTime());
	  }
	  fTRB3Tree->Fill();
	}
      }
    }

    auto cfragIter = C1720ContainerFragments.begin();

    while (cfragIter != C1720ContainerFragments.end()) {
      // initialize counter
      fFragCounter[(*cfragIter).first] = 0;

      // now make digits
      while (! ((*cfragIter).second.empty())) {
	auto& cfrag = *((*cfragIter).second.front());
	auto cfragId = (*cfragIter).first;
	emphaticdaq::CAENV1720Fragment caenfrag(cfrag);
	fWaveForms[cfragId].push_back(Unpack::GetWaveFormsFrom1720Fragment(caenfrag,cfragId));
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

	void Unpacker::calcTimeWalkCorr() {
		std::cout << "Entering \"calcTimeWalkCorr\"" << std::endl;
		if(!fTvsT.empty()) return; // only needs to run once when time-stamping

		// Set up names
		art::ServiceHandle<art::TFileService> tfs;
		art::TFileDirectory tdir3 = tfs->mkdir("TimeWalk","");
		char hname[256];
		char htitle[256];
		for (auto fragId : fFragId) {
			if(fragId == fragIdGrandfather) {
				// store SSDs graph in the grandfather part of the array
				sprintf(hname,"twSSD");
				sprintf(htitle,"Event Time Walk SSD; Board %d Timestamp (ns); SSD (ns)", fragIdGrandfather);
			} else {
				sprintf(hname,"tw_%d",fragId);
				sprintf(htitle,"Event Time Walk, Board %d; Board %d Timestamp (ns); Board %d (ns)",
					fragId, fragIdGrandfather, fragId);
			}
			fTvsT[fragId] = new TGraph();
		}

		if(masks.empty()) return;

		TFile* fout;
		char outName[256]; sprintf(outName, "%d_%d.root", fRun, fSubrun);
		if(fMakeTimeWalkHistos) fout = TFile::Open(outName,"UPDATE");

		for (auto fragId : fFragId) {
			// iG = index of grandfather
			auto counter = 0;
			for(size_t iG = 0; iG < fFragTimestamps[fragIdGrandfather].size(); ++iG) {
				// iC = index of child
				auto iC = masks[fragId][iG];
				if(iC != -1) {
					auto timeStamp = fFragTimestamps[fragId][iC];
					if(fragId == fragIdGrandfather)
						timeStamp = fSSDRawDigits[iC].first;
					// Make grandfather Y-Axis to make conversion easier
					fTvsT[fragId]->SetPoint(counter++, timeStamp, fFragTimestamps[fragIdGrandfather][iG]);
				}
			}

			char graph[256];
			if(fragId == fragIdGrandfather) sprintf(graph,"tw_SSD");
			else sprintf(graph,"tw_%d", fragId);

			std::cout << graph << std::endl;
			if(fMakeTimeWalkHistos)
				fTvsT[fragId]->Write(graph);

			fTvsT[fragId]->Fit("pol1");

			// save fits
			TF1* f1 = fTvsT[fragId]->GetFunction("pol1");
			if(fragId == fragIdGrandfather) sprintf(graph,"tw_SSD_Fit");
			else sprintf(graph,"tw_%d_Fit", fragId);

			if(fMakeTimeWalkHistos)
				f1->Write(graph);

			fTWCorr0[fragId] = f1->GetParameter(0);
			fTWCorr1[fragId] = f1->GetParameter(1);
			printf("Board %d: twcorr intercept = %16.16f\n", fragId, f1->GetParameter(0));
			printf("Board %d: twcorr slope = %16.16f\n", fragId, f1->GetParameter(1));
		}
		if(fMakeTimeWalkHistos && fout->IsOpen()) fout->Close();
	}

  /***************************************************************************/
	// adjusted routine
	bool Unpacker::findMatches()	{
		// Prepare ssd timestamp vector
		std::vector<uint64_t> ssdTimestamps; ssdTimestamps.reserve(fSSDRawDigits.size());
		// Check that clock is monotonic
		{
			bool isMonotonic = true;
			for(size_t index = 0; index < fSSDRawDigits.size() - 1; ++index) {
				if(fSSDRawDigits[index].first < fSSDRawDigits[index+1].first) continue;
				std::cout << "SSD Clock is not monotonic" << std::endl;
				isMonotonic = false;
				break;
			}
			for(auto &eventPair : fSSDRawDigits) {
				// 2^32 correction for integers that overflowed
				if(!isMonotonic) eventPair.first = eventPair.first & 0xFFFFFFFF;
				ssdTimestamps.push_back(fBCOx*eventPair.first);
			}
		}

		// Prepare local copy of fragment timestamps
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > fragTimestamps;
		fragTimestamps = fFragTimestamps;

		// determine grandfather clock
      fragIdGrandfather = fFragId[0];
		auto samplesGrandfather = fragTimestamps[fragIdGrandfather].size();
		for(auto fragId : fFragId) {
			if(fragTimestamps[fragId].size() > samplesGrandfather) {
				fragIdGrandfather = fragId;
				samplesGrandfather = fragTimestamps[fragId].size();
			}
		}

		// remove first event from each sensor
		ssdTimestamps.erase(ssdTimestamps.begin());
		for(auto fragId : fFragId) {
			fragTimestamps[fragId].erase(fragTimestamps[fragId].begin());
		}

		// Zero clocks to first event
		{ // set scope
			auto start = *ssdTimestamps.begin();
			for(auto& time : ssdTimestamps) {
				time += -start;
			}
		}
		for(auto fragId : fFragId) {
			auto start = *fragTimestamps[fragId].begin();
			for(auto& time : fragTimestamps[fragId]) {
				time += -start;
			}
		}

		// Perform comparison to grandfather
		for(auto fragId : fFragId) {
			if(fragId == fragIdGrandfather) continue; // skip grandfather
			std::cout << "(" << fragIdGrandfather << ", " << fragId << ")" << std::endl;
			{
				masks[fragId].push_back(-1); // not comparing first event
				auto remainder = compareGrandfather(fragTimestamps[fragIdGrandfather], fragTimestamps[fragId], 200);
				masks[fragId].insert(masks[fragId].end(), remainder.begin(), remainder.end());
			}
			for( auto index : masks[fragId] ) {
				if(index != -1) {
					fT0[fragId] = masks[fragId][0];
					break;
				}
			}
		}
		std::cout << "(Comparing to SSDs)"<< std::endl;
		// Store SSD in the extra mask slot for fragIdGrandfather (no need to compare Grandfather to itself)
		{
			masks[fragIdGrandfather].push_back(-1); // not comparing first event
			auto remainder = compareGrandfather(fragTimestamps[fragIdGrandfather], ssdTimestamps, 200);
			masks[fragIdGrandfather].insert(masks[fragIdGrandfather].end(), remainder.begin(), remainder.end());
		}
		{
			for(auto index : masks[fragIdGrandfather]) {
				if(index != -1) {
					fSSDT0 = masks[fragIdGrandfather][0];
					break;
				}
			}
		}
		// Since we skipped the first event, we need to add 1 to each of our offsets
		for(auto fragId: fFragId) {
			for(auto &index : masks[fragId]) {
				if(index != -1)
					index+=1;
			}
		}
		return !masks.empty(); // returns true if masks has matches
	}
  /***************************************************************************/

  bool Unpacker::readNext(art::RunPrincipal* const& ,//inR,
				art::SubRunPrincipal* const& ,//inSR,
				art::RunPrincipal* & outR,
				art::SubRunPrincipal* & outSR,
				art::EventPrincipal* &outE)
	{
		//    if (!inR) std::cout << "inR is empty" << std::endl;
		//    if (!inSR) std::cout << "inSR is empty" << std::endl;

		// this id functions as a fragID for SSDs later
		const auto ssdId = 773;

		// Structure:
		//		Overall: < <time, event>, <time, event> , ... >
		//			Event:  < <fragId, index>, <fragId, index>, ... >

		if(fIsFirst) {
			std::unique_ptr<TFile> input_file{TFile::Open(fCurrentFilename.c_str())};
			if (!input_file) {
				std::cerr << "Could not open file.\n" << std::endl;
				return false;
			}
			std::unique_ptr<TTree> runs{input_file->Get<TTree>("Runs")};
			if (!runs) {
				std::cerr << "Could not find Runs tree.\n";
				return false;
			}

			std::unique_ptr<TTree> subruns{input_file->Get<TTree>("SubRuns")};
			if (!subruns) {
				std::cerr << "Could not find SubRuns tree.\n";
				return false;
			}

			art::RunAuxiliary runAux;
			art::RunAuxiliary* runAuxPtr = &runAux;
			art::SubRunAuxiliary subrunAux;
			art::SubRunAuxiliary* subrunAuxPtr = &subrunAux;
			runs->SetBranchAddress("RunAuxiliary",&runAuxPtr);
			subruns->SetBranchAddress("SubRunAuxiliary",&subrunAuxPtr);
			runs->GetEvent(0);
			subruns->GetEvent(0);

			// deal with creating Run and Subrun objects
			fRun = runAux.run();
			fSubrun = subrunAux.subRun();
			outR = fSourceHelper.makeRunPrincipal(fRun,runAux.beginTime());
			outSR = fSourceHelper.makeSubRunPrincipal(fRun, fSubrun,
							subrunAux.beginTime());
			fSpillTime = subrunAux.beginTime();

			// initialize channel map
			fChannelMap = new emph::cmap::ChannelMap();
			fRunHistory = new runhist::RunHistory(fRun);
			fChannelMap->LoadMap(fRunHistory->ChanFile());

			// get all of the digits if this is the first event
			// get all of the fragments out and create waveforms and digits
			if (! createDigitsFromArtdaqEvent()) return false;

			// create all of the SSD digits for this spill
			if (fReadSSDData)
				if (! createSSDDigits())
					return false;

			// determine t0s for each board
			if (!findMatches())
				abort();

			if (fMakeTDiffHistos)
				makeTDiffHistos();

			if (fMakeTimeWalkHistos || fMakeBenchmarkPlots) {
				TFile* fout;
				char outName[256]; sprintf(outName, "%d_%d.root", fRun, fSubrun);
				fout = TFile::Open(outName, "RECREATE");
				fout->Close();
			}
			calcTimeWalkCorr();


			{// limit scope of punch card
				std::cout << "Preparing punch card" << std::endl;
				std::vector<std::list<size_t>> punchCards;
				// Set up punch card for each CAEN, TRB3, and SSD
				for (size_t ifrag=0; ifrag <= fFragId.size(); ++ifrag) {
					auto fragId = ssdId;
					auto size = fSSDRawDigits.size();
					if(ifrag < fFragId.size()) {
						fragId = fFragId[ifrag];
						size = fFragTimestamps[fragId].size();
					}
					std::list<size_t> punchCard;
					for(size_t index = 0; index < size; ++index)
						punchCard.push_back(index);
					punchCards.push_back(punchCard);
				}

				std::cout << "Time to stack the events" << std::endl;
				// Iterate over list instead of all timestamps
				for (size_t ifrag=0; ifrag <= fFragId.size(); ++ifrag) {

					auto fragA = ssdId;
					if(ifrag < fFragId.size())
						fragA = fFragId[ifrag];

					for(auto it = punchCards[ifrag].begin(); it != punchCards[ifrag].end(); ++it) {
						auto iA = *it; // index of time stamp to be checked
						std::vector<std::pair<uint64_t,uint64_t>> event;
						int64_t tsA;
						// Project timestamp to grandfather
						if(fragA == ssdId) // SSD
							tsA = fTWCorr0[fragIdGrandfather] + fTWCorr1[fragIdGrandfather]*fSSDRawDigits[iA].first;
						else if(fragA != fragIdGrandfather) // CAEN and TRB3 children
							tsA = fTWCorr0[fragA] + fTWCorr1[fragA]*fFragTimestamps[fragA][iA];
						else // grandfather
							tsA = fFragTimestamps[fragA][iA];
						event.push_back(std::make_pair(fragA, iA));

						for (size_t jfrag=ifrag + 1; jfrag <= fFragId.size(); ++jfrag) {
							auto fragB = ssdId;
							if(jfrag < fFragId.size())
								fragB = fFragId[jfrag];

							for(auto jt = punchCards[jfrag].begin(); jt != punchCards[jfrag].end(); ++jt) {
								auto iB = *jt; // index of time stamp to be checked
								int64_t tsB;
								// Project timestamp to grandfather
								if(fragB == ssdId) // SSD
									tsB = std::round(fTWCorr0[fragIdGrandfather] + fTWCorr1[fragIdGrandfather]*fSSDRawDigits[iB].first);
								else if(fragB != fragIdGrandfather)
									tsB = fTWCorr0[fragB] + fTWCorr1[fragB]*fFragTimestamps[fragB][iB];
								else
									tsB = fFragTimestamps[fragB][iB];

								// Adjust this resolution as needed
								if(std::llabs(tsA - tsB) <= 200) { // FOUND YOU
									event.push_back(std::make_pair(fragB, iB));
									// Remove element from punch card to indicate this event has been handled
									punchCards[jfrag].erase(jt);
									break;
								}
							}
						}
						// After checking all other fragID's, push this event on the stack
						eventStack.push_back(std::make_pair(tsA, event));
					}
				}
			}
			// Now we have our event stack for packing events (hard part is over?)
			// Plots
			if(fMakeBenchmarkPlots) {
				std::sort(eventStack.begin(), eventStack.end());
				// Average time differences between post-aligned data streams per event
				TFile* fout;
				char outName[256]; sprintf(outName, "%d_%d.root", fRun, fSubrun);
				fout = TFile::Open(outName, "UPDATE");
				char hname[256];
				char htitle[256];
				sprintf(hname,"AvgDifference__SSD_TRB3");
				sprintf(htitle,"Average time differences per event for run %d, subrun %d for SSD and TRB3", fRun, fSubrun);
				TH1D averageTimeDifferencesSSD_TRB3(hname, htitle, 6000, -300, 300);

				sprintf(hname,"AvgDifference__SSD_CAEN");
				sprintf(htitle,"Average time differences per event for run %d, subrun %d for SSD and CAEN", fRun, fSubrun);
				TH1D averageTimeDifferencesSSD_CAEN(hname, htitle, 6000, -300, 300);

				sprintf(hname,"AvgDifference__CAEN_TRB3");
				sprintf(htitle,"Average time differences per event for run %d, subrun %d for CAEN and TRB3", fRun, fSubrun);
				TH1D averageTimeDifferencesCAEN_TRB3(hname, htitle, 6000, -300, 300);

				std::vector<uint64_t> timeSSD;
				std::vector<uint64_t> timeTRB3;
				std::vector<uint64_t> timeCAEN;

				size_t gotSSD = 0;
				size_t gotALL = 0;
				size_t eventCount = 0;

				// stuff for looking at time difference between events
				sprintf(hname,"TimeToNext__SSD");
				sprintf(htitle,"Time between SSD events for run %d, subrun %d", fRun, fSubrun);
				TH1D timeToNextSSD(hname, htitle, 10000000, 0, 1000000);

				sprintf(hname,"TimeToNext_TRB3");
				sprintf(htitle,"Time between TRB3 events for run %d, subrun %d", fRun, fSubrun);
				TH1D timeToNextTRB3(hname, htitle, 10000000, 0, 1000000);

				sprintf(hname,"TimeToNext_CAEN");
				sprintf(htitle,"Time between CAEN events for run %d, subrun %d", fRun, fSubrun);
				TH1D timeToNextCAEN(hname, htitle, 10000000, 0, 1000000);

				// stuff for looking at time difference between events; cross detector
				sprintf(hname,"TimeToNext__SSD_TRB3");
				sprintf(htitle,"Time between SSD events and TRB3 events for run %d, subrun %d", fRun, fSubrun);
				TH1D timeToNextSSD_TRB3(hname, htitle, 40000, -2000, 2000);

				sprintf(hname,"TimeToNext_SSD_CAEN");
				sprintf(htitle,"Time between SSD events and CAEN events for run %d, subrun %d", fRun, fSubrun);
				TH1D timeToNextSSD_CAEN(hname, htitle, 40000, -2000, 2000);

				sprintf(hname,"TimeToNext_CAEN_TRB3");
				sprintf(htitle,"Time between CAEN events and TRB3 events for run %d, subrun %d", fRun, fSubrun);
				TH1D timeToNextCAEN_TRB3(hname, htitle, 40000, -2000, 2000);


				double lastIndividual[3] = {0,0,0}; bool isFirst = true;
				double lastTriple[3] = {0,0,0};

				for(size_t iEvent = 0; iEvent < eventStack.size(); ++iEvent) {
					auto event = eventStack[iEvent];
					for(auto attendee : event.second) {
						auto fragId = attendee.first;
						auto index = attendee.second;

						int64_t time = 0;
						if(fragId == ssdId) // SSD
							time = fTWCorr0[fragIdGrandfather] + fTWCorr1[fragIdGrandfather]*fSSDRawDigits[index].first;
						else if(fragId != fragIdGrandfather) // CAEN and TRB3 children
							time = fTWCorr0[fragId] + fTWCorr1[fragId]*fFragTimestamps[fragId][index];
						else // grandfather
							time = fFragTimestamps[fragIdGrandfather][index];

						if(fragId == ssdId)
							timeSSD.push_back(time);
						else if(fragId > 10)
							timeTRB3.push_back(time);
						else
							timeCAEN.push_back(time);
					}
					// and now we move on to the next event!

					// If next event occurs within a given time, then include data by iterating the loop
					auto nextTime = eventStack[iEvent + 1].first;
					if(std::llabs(nextTime - event.first) < 100) continue;

					double averageSSD = 0;
					double averageTRB3 = 0;
					double averageCAEN = 0;
					for(auto time : timeSSD) averageSSD += (1.0/timeSSD.size())*time;
					for(auto time : timeTRB3) averageTRB3 += (1.0/timeTRB3.size())*time;
					for(auto time : timeCAEN) averageCAEN += (1.0/timeCAEN.size())*time;
					if(!timeSSD.empty() && !timeTRB3.empty())
						averageTimeDifferencesSSD_TRB3.Fill(averageSSD - averageTRB3);
					if(!timeSSD.empty() && !timeCAEN.empty())
						averageTimeDifferencesSSD_CAEN.Fill(averageSSD - averageCAEN);
					if(!timeCAEN.empty() && !timeTRB3.empty())
						averageTimeDifferencesCAEN_TRB3.Fill(averageCAEN - averageTRB3);

					if(!timeSSD.empty()) gotSSD++;
					if(!timeSSD.empty() && !timeTRB3.empty() && !timeCAEN.empty()) gotALL++;
					eventCount++;


					if(!isFirst) {
						if(!timeSSD.empty()) timeToNextSSD.Fill(averageSSD - lastIndividual[0]);
						if(!timeTRB3.empty()) timeToNextTRB3.Fill(averageTRB3 - lastIndividual[1]);
						if(!timeCAEN.empty()) timeToNextCAEN.Fill(averageCAEN - lastIndividual[2]);

						if(!timeSSD.empty() && !timeTRB3.empty() && !timeCAEN.empty()) {
							timeToNextSSD_TRB3.Fill(averageSSD - lastTriple[0] - (averageTRB3 - lastTriple[1]));
							timeToNextSSD_CAEN.Fill(averageSSD - lastTriple[0] - (averageCAEN - lastTriple[2]));
							timeToNextCAEN_TRB3.Fill(averageCAEN - lastTriple[2] -(averageTRB3 - lastTriple[1]));
						}
					} else {
						isFirst=false;
					}
					if(!timeSSD.empty()) lastIndividual[0] = averageSSD;
					if(!timeTRB3.empty()) lastIndividual[1] = averageTRB3;
					if(!timeCAEN.empty()) lastIndividual[2] = averageCAEN;
					if(!timeSSD.empty() && !timeTRB3.empty() && !timeCAEN.empty()) {
						lastTriple[0] = averageSSD;
						lastTriple[1] = averageTRB3;
						lastTriple[2] = averageCAEN;
					}

					timeSSD.clear();
					timeTRB3.clear();
					timeCAEN.clear();
				}
				averageTimeDifferencesSSD_TRB3.Write("avgTD_SSD_TRB3");
				averageTimeDifferencesSSD_CAEN.Write("avgTD_SSD_CAEN");
				averageTimeDifferencesCAEN_TRB3.Write("avgTD_CAEN_TRB3");

				timeToNextSSD.Write("t2N_SSD");
				timeToNextTRB3.Write("t2N_TRB3");
				timeToNextCAEN.Write("t2N_CAEN");

				timeToNextSSD_TRB3.Write("t2N_SSD_TRB3");
				timeToNextSSD_CAEN.Write("t2N_SSD_CAEN");
				timeToNextCAEN_TRB3.Write("t2N_CAEN_TRB3");

				TVectorD evtCounts(3);
				evtCounts[0] = gotSSD;
				evtCounts[1] = gotALL;
				evtCounts[2] = eventCount;
				evtCounts.Write("Event Statistics");

				fout->Close();
				std::cout << gotSSD << " out of " << eventCount << " events with SSD event" << std::endl;
				std::cout << gotALL << " out of " << eventCount << " events with all events" << std::endl;
			}

			std::cout << "Sorting event stack" << std::endl;
			// Sorting in reverse so that we can pop events off the end
			std::sort(eventStack.begin(), eventStack.end(), std::greater<>());
			{
				size_t gotSSD = 0;
				size_t gotALL = 0;
				size_t eventCount = 0;
				for(auto event : eventStack) {
					//std::cout << "Event:\t" << event.first << "\twith these attendees:\t";
					eventCount++;
					bool triple[3] = {false,false,false};
					for(const auto& attendee : event.second) {
						//std::cout << attendee.first << "\t";
						if(attendee.first == ssdId)
							triple[0] = true;
						else if(attendee.first < 10)
							triple[1] = true;
						else
							triple[2] = true;
					}
					if(triple[0] && triple[1] && triple[2])
						gotALL++;
					if(triple[0])
						gotSSD++;

					//std::cout << std::endl;
				}
				TFile* fout;
				char outName[256]; sprintf(outName, "%d_%d.root", fRun, fSubrun);
				fout = TFile::Open(outName, "UPDATE");
				TVectorD evtCounts(3);
				evtCounts[0] = gotSSD;
				evtCounts[1] = gotALL;
				evtCounts[2] = eventCount;
				evtCounts.Write("Event Statistics");
				fout->Close();
				std::cout << "Percentage events with SSD: " << 1.0*gotSSD/eventCount << std::endl;
				std::cout << "Percentage events with All: " << 1.0*gotALL/eventCount << std::endl;
			}

			fIsFirst = false;
		}

		// Now with our event stack, we can pack events
		if (fCreateArtEvents) {
			//std::cout << "Creating art events" << std::endl;
			std::vector<std::unique_ptr<std::vector<emph::rawdata::WaveForm> > > evtWaveForms;
			for (int idet=0; idet<emph::geo::NDetectors; ++idet)
				evtWaveForms.push_back(std::make_unique<std::vector<emph::rawdata::WaveForm>  >());

			std::vector<std::unique_ptr<std::vector<emph::rawdata::TRB3RawDigit> > > evtTRB3Digits;
			for (int idet=0; idet<emph::geo::NDetectors; ++idet)
				evtTRB3Digits.push_back(std::make_unique<std::vector<emph::rawdata::TRB3RawDigit>  >());

			std::vector<emph::rawdata::SSDRawDigit> evtSSDVec;
			auto evtSSDRawDigits = std::make_unique<std::vector<emph::rawdata::SSDRawDigit> >();

			if ((fEvtCount%1000) == 0)
				std::cout << "Event " << fEvtCount << std::endl;

			if(eventStack.empty()) {
				std::cout << "No events?" << std::endl;
				return false;
			}

			auto event = eventStack.back();
			art::Timestamp evtTime(event.first + fSpillTime.value());
			outE = fSourceHelper.makeEventPrincipal(fRun, fSubrun, fEvtCount++, evtTime);

			fPrevTS = event.first; // set this to guarantee at least one run through the loop
			while(fPrevTS - event.first < 100) {
				eventStack.pop_back(); // remove event from stack
				fPrevTS = event.first;
				for(auto attendee : event.second) {
					auto fragId = attendee.first;
					auto index = attendee.second;
					if(fReadSSDData && fragId == ssdId) { // SSD
						auto & ssdDigs = fSSDRawDigits[index].second;
						for (auto ssdDig : ssdDigs) {
							auto tssdDig(ssdDig);
							evtSSDRawDigits->push_back(tssdDig);
						}
					} else if (fReadCAENData && fWaveForms.count(fragId)) {
						emph::cmap::FEBoardType boardType = emph::cmap::V1720;
						int boardNum = fragId;
						emph::cmap::EChannel echan;
						echan.SetBoardType(boardType);
						echan.SetBoard(boardNum);
						// This loop is putting the entire waveform into evtWaveForms
						for(auto &tdig : fWaveForms[fragId][index]) {
							echan.SetChannel(tdig.Channel());
							if (! fChannelMap->IsValidEChan(echan)) continue;
							emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
							tdig.SetDetChannel(dchan.Channel());
							//	      std::cout << echan << " maps to " << dchan << std::endl;
							evtWaveForms[dchan.DetId()]->push_back(tdig);
						}
					} else if (fReadTRB3Data && fTRB3RawDigits.count(fragId)) {
						emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
						emph::cmap::EChannel echan;
						echan.SetBoardType(boardType);
						for(auto &tdig : fTRB3RawDigits[fragId][index]) {
							int channel = tdig.GetChannel();
							int boardNum = tdig.GetBoardId();
							echan.SetChannel(channel);
							echan.SetBoard(boardNum);
							emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
							if (dchan.DetId() != emph::geo::NDetectors){
								tdig.SetDetChannel(dchan.Channel());
								evtTRB3Digits[dchan.DetId()]->push_back(tdig);
							}
						}
					}
				}
				if(eventStack.empty()) break;
				event = eventStack.back(); // look at next event!
			}

			// Write out event data
			put_product_in_principal(std::move(evtSSDRawDigits), *outE,"raw","SSD");
			for (int idet=0; idet < emph::geo::NDetectors; ++idet) {
				std::string detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(idet));
				if (!evtWaveForms[idet]->empty())
					put_product_in_principal(std::move(evtWaveForms[idet]), *outE,"raw",detStr);
				if (!evtTRB3Digits[idet]->empty())
					put_product_in_principal(std::move(evtTRB3Digits[idet]), *outE,"raw",detStr);
			}

			return true;
		}
		return false;
	}

}
}

DEFINE_ART_INPUT_SOURCE(art::Source<emph::rawdata::Unpacker>)
