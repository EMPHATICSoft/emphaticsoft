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
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraph.h"
#include "TVector.h"

#include <iostream>
#include <fstream>

#include <cmath>
#include <iterator>
#include <algorithm>
#include <limits>

#include "TimeSync.h"

namespace {
  // Read product for given type T
  template <typename T>
  T const* readProduct(TTree& events, std::string const& branchName)
  {
    art::EDProduct* p = nullptr;
    TBranch* branch = events.GetBranch(branchName.c_str());
    if (!branch) return nullptr;
    events.SetBranchAddress(branchName.c_str(), &p);
    // Read only this branch: TTree::GetEntry would also re-read the branches of earlier calls,
    // whose addresses point at those calls' (gone) local pointers
    branch->GetEntry(0);

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
    // One art file (one spill) per job: the time sync, the event stack and previousFile/nextFile
    // belong to that spill. A second file used to be skipped without a word.
    auto fileNames = ps.get<std::vector<std::string> >("fileNames", {});
    if (fileNames.size() > 1)
      throw art::Exception(art::errors::Configuration)
	<< "RawDataUnpacker: one input file per job (got " << fileNames.size() << "); run one job per subrun file\n";

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
    fSeedTimeSync = ps.get<bool>("seedTimeSync",true); // seed TWCorr0/1 from inter-trigger gaps (see seedSync in TimeSync.h)
    fSeedMinMatchFraction = ps.get<double>("seedMinMatchFraction",0.5); // a seed counts only if this fraction of the child's events match
    fMinMatchFraction = ps.get<double>("minMatchFraction",0.2); // a board (or the SSDs) with fewer matched events after calibration is left out of the events
    fSSDSynced = true;
    // neighbouring art files (same run, subrun -1 and +1) for misfiled TRB3 data; empty = not used
    for (int fileOffset : {-1, +1}) {
      std::string label = fileOffset < 0 ? "previous" : "next";
      auto fileName = ps.get<std::string>(label + "File", "");
      if (fReadTRB3Data && !fileName.empty())
	fTRB3Neighbours.push_back({label, fileOffset, fileName});
    }
    help.reconstitutes<emph::dq::SyncQuality, art::InSubRun>("raw","TimeSync");
    help.reconstitutes<emph::dq::EventSync, art::InEvent>("raw","TimeSync");
    fLostDetectors = 0;
    fPartialDetectors = 0;
    fMakeTDiffHistos = ps.get<bool>("makeTDiffHistos",false);
    fMakeTimeWalkHistos = ps.get<bool>("makeTimeWalkHistos",false);

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
    while (!ssdFile.eof()) {
      ssdDigs = SSDUnpack::readSSDHitsFromFileStream(ssdFile,false);
      fSSDRawDigits.push_back(ssdDigs);
    }
    std::cout <<  "Found " << fSSDRawDigits.size() << " SSD events"
	      << std::endl;

    ssdFile.close();
    return true;
  }

  /***************************************************************************/

  void Unpacker::readTRB3Containers(TTree& events,
				    std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::TRB3RawDigit> > >& digits,
				    std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> >& timestamps,
				    std::vector<artdaq::Fragment::fragment_id_t>& boards)
  {
    // TRB3 digits and timestamps per board from one art file's Events tree; boards in container order
    std::unordered_map<artdaq::Fragment::fragment_id_t,artdaq::FragmentPtrs> TRB3ContainerFragments;
    auto TRB3CFrag = readProduct<std::vector<artdaq::Fragment> >(events, "artdaq::Fragments_daq_ContainerTRB3_DAQEventBuilder.");
    if (!TRB3CFrag) return;
    for (const auto& cont : *TRB3CFrag) {
      artdaq::ContainerFragment contf(cont);
      if (contf.fragment_type() != ots::detail::FragmentType::TRB3) {
	std::cout << "oh oh" << std::endl;
	break;
      }
      for (size_t ifrag=0; ifrag < contf.block_count(); ++ifrag) {
	TRB3ContainerFragments[cont.fragmentID()].emplace_back(std::move(contf[ifrag]));
	if (std::find(boards.begin(),boards.end(),cont.fragmentID()) == boards.end())
	  boards.push_back(cont.fragmentID());
      }
    }
    std::cout << "TRB3 block count: " << TRB3CFrag->size()
	      << std::endl;

    for (auto& [cfragId, cfrags] : TRB3ContainerFragments) {
      while (!cfrags.empty()) {
	auto& cfrag = *(cfrags.front());
	emphaticdaq::TRB3Fragment trb3frag(cfrag);
	digits[cfragId].push_back(Unpack::GetTRB3RawDigitsFromFragment(trb3frag));
	timestamps[cfragId].push_back(cfrag.timestamp());
	cfrags.pop_front();
      }
      std::cout << "Made " << digits[cfragId].size()
		<< " vectors of TRB3 digits for Frag Id "
		<< cfragId << std::endl;
    }
  }

  /***************************************************************************/

  bool Unpacker::readNeighbourTRB3(size_t ineighbour)
  {
    auto& neighbour = fTRB3Neighbours[ineighbour];
    if (neighbour.isRead) return !neighbour.boards.empty();
    neighbour.isRead = true;
    std::unique_ptr<TFile> input_file{TFile::Open(neighbour.fileName.c_str())};
    if (!input_file || input_file->IsZombie()) {
      std::cerr << "TRB3: cannot open " << neighbour.label << " file " << neighbour.fileName << std::endl;
      return false;
    }
    std::unique_ptr<TTree> events{input_file->Get<TTree>("Events")};
    if (!events) {
      std::cerr << "TRB3: no Events tree in " << neighbour.label << " file " << neighbour.fileName << std::endl;
      return false;
    }
    std::cout << "TRB3: reading the " << neighbour.label << " file " << neighbour.fileName << std::endl;
    readTRB3Containers(*events, neighbour.digits, neighbour.timestamps, neighbour.boards);
    return !neighbour.boards.empty();
  }

  /***************************************************************************/

  bool Unpacker::createDigitsFromArtdaqEvent()
  {
    std::unordered_map<artdaq::Fragment::fragment_id_t,artdaq::FragmentPtrs> C1720ContainerFragments;

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
      std::vector<artdaq::Fragment::fragment_id_t> trb3Boards;
      readTRB3Containers(*events, fTRB3RawDigits, fFragTimestamps, trb3Boards);
      for (auto fragId : trb3Boards) {
	fFragCounter[fragId] = 0;
	if (std::find(fFragId.begin(),fFragId.end(),fragId) == fFragId.end())
	  fFragId.push_back(fragId);
	if (std::find(fTRB3FragIds.begin(),fTRB3FragIds.end(),fragId) == fTRB3FragIds.end())
	  fTRB3FragIds.push_back(fragId);
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
      if (std::find(fCAENFragIds.begin(),fCAENFragIds.end(),(*cfragIter).first) == fCAENFragIds.end())
	fCAENFragIds.push_back((*cfragIter).first);

      // now make digits
      while (! ((*cfragIter).second.empty())) {
	auto& cfrag = *((*cfragIter).second.front());
	auto cfragId = (*cfragIter).first;
	emphaticdaq::CAENV1720Fragment caenfrag(cfrag);
	fWaveForms[cfragId].push_back(Unpack::GetWaveFormsFrom1720Fragment(caenfrag,cfragId));
	fFragTimestamps[cfragId].push_back(cfrag.timestamp());
	fCAENEventCounter[cfragId].push_back(caenfrag.Event()->Header.eventCounter);
	fCAENHostTime[cfragId].push_back(caenfrag.Metadata() ? uint64_t(caenfrag.Metadata()->timeStampSec) * 1000000000ULL + caenfrag.Metadata()->timeStampNSec : 0);
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
      sprintf(htitle,"Fragment Time Differences, Board %d;Time differences (ns);# of Occurrences",fragId);
      tdiffHist.push_back(tdir2.make<TH1I>(hname,htitle,250,0,500000));
      for (size_t i=1; i<fFragTimestamps[fragId].size(); ++i) {
	uint64_t tdiff = fFragTimestamps[fragId][i]-fFragTimestamps[fragId][i-1];
	tdiffHist[ifrag]->Fill(tdiff);
      }
    }
  }

  /***************************************************************************/

	void Unpacker::makeDiffDiffHistos(artdaq::Fragment::fragment_id_t idA, artdaq::Fragment::fragment_id_t idB) {
		const auto ssdId = 773; // ssd fragId equivalent
		art::ServiceHandle<art::TFileService> tfs;
		art::TFileDirectory tdir2 = tfs->mkdir("DiffOfDiff","");
		//std::vector<TH1I*> DiffOfDiff;
		char hname[256];
		char htitle[256];
		sprintf(hname,"diff_%d__wrt_%d", idA, idB);
		sprintf(htitle,"Fragment Time Differences of Differences, Boards (%d,%d);Difference of Time Differences (ns);# of Occurrences", idA, idB);
		TH1I* diffDiff = tdir2.make<TH1I>(hname,htitle,200,-200,200);

		int64_t timeBuffer[2] = {0,0};
		bool isFirst = true;
		for(auto event : eventStack) {
			int gotBoth = 0;
			// Check if next event has both
			for(auto attendee : event.second) {
				auto fragId = attendee.first;
				if(fragId == idA || fragId == idB) {
					if(gotBoth > 0) { // break on second time through this check
						gotBoth++;
						break;
					} else // first time through this check
						gotBoth++;
				}
			}

			if(gotBoth != 2) continue; // check next event

			int64_t difference = timeBuffer[0] - timeBuffer[1];
			for(auto attendee : event.second) {
				auto fragId = attendee.first;
				auto index = attendee.second;

				int64_t time = 0;
				if(fragId == ssdId) // SSD
					time = std::round(fTWCorr0[fragIdGrandfather] + fTWCorr1[fragIdGrandfather] * fBCOx * fSSDRawDigits[index].first);
				else if(fragId != fragIdGrandfather) // CAEN and TRB3 children
					time = std::round(fTWCorr0[fragId] + fTWCorr1[fragId]*fFragTimestamps[fragId][index]);
				else // grandfather
					time = fFragTimestamps[fragIdGrandfather][index];
				if(fragId == idA) {
					difference -= time;
					timeBuffer[0] = time;
				}
				if(fragId == idB) {
					difference += time;
					timeBuffer[1] = time;
				}
			}

			if(!isFirst) {
				diffDiff->Fill(difference);
			} else {
				isFirst = false;
			}
		}
	}

  /***************************************************************************/
	void Unpacker::calcTimeWalkCorr(artdaq::Fragment::fragment_id_t idChild) {
		if(idChild == fragIdGrandfather) {
			std::cout << "Calculating linear fit for (" << fragIdGrandfather <<", SSDs)"<< std::endl;
		} else {
			std::cout << "Calculating linear fit for (" << fragIdGrandfather << ", " <<idChild << ")" << std::endl;
		}

		// Set up names
		art::ServiceHandle<art::TFileService> tfs;
		art::TFileDirectory tdir2 = tfs->mkdir("TimeWalk","");
		char hname[256];
		char htitle[256];
		if(idChild == fragIdGrandfather) {
			// store SSDs graph in the grandfather part of the array
			sprintf(hname,"twSSD");
			sprintf(htitle,"Event Time Walk SSD; Board %d Timestamp (ns); SSD (ns)", fragIdGrandfather);
		} else {
			sprintf(hname,"tw_%d",idChild);
			sprintf(htitle,"Event Time Walk, Board %d; Board %d Timestamp (ns); Board %d (ns)",
				idChild, fragIdGrandfather, idChild);
		}
		if(fMakeTimeWalkHistos)
			fTvsT[idChild] = tdir2.make<TGraph>();
		else
			fTvsT[idChild] = new TGraph();
		fTvsT[idChild]->SetTitle(htitle);

		if(masks[idChild].empty()) return;

		// iG = index of grandfather
		auto counter = 0;
		for(size_t iG = 0; iG < fFragTimestamps[fragIdGrandfather].size(); ++iG) {
			// iC = index of child
			auto iC = masks[idChild][iG];
			if(iC != -1) {
				auto timeStamp = fFragTimestamps[idChild][iC];
				if(idChild == fragIdGrandfather)
					timeStamp = fBCOx*fSSDRawDigits[iC].first;
				// Make grandfather Y-Axis to make conversion easier
				fTvsT[idChild]->SetPoint(counter++, timeStamp, fFragTimestamps[fragIdGrandfather][iG]);
			}
		}

		char graph[256];

		if(counter < 3) { // no line through fewer points; dropUnsyncedBoards() leaves such a board out
			std::cout << "Too few matches (" << counter << ") for a linear fit, correction unchanged" << std::endl;
			return;
		}
		TFitResultPtr fitResult = fTvsT[idChild]->Fit("pol1", "S");

		// save fits
		TF1* f1 = fTvsT[idChild]->GetFunction("pol1");
		if(!f1) return;
		if(idChild == fragIdGrandfather) sprintf(graph,"tw_SSD_Fit");
		else sprintf(graph,"tw_%d_Fit", idChild);

		if(fMakeTimeWalkHistos) {
			fTvsT[idChild]->Write(hname);
			//f1->Write(graph);
		}
#ifdef VERBOSE
		if(idChild == idChild) {
			printf("Board SSD: twcorr intercept = %16.16f\n", f1->GetParameter(0));
			printf("Board SSD: twcorr slope = %16.16f\n", f1->GetParameter(1));
		} else {
			printf("Board %d: twcorr intercept = %16.16f\n", idChild, f1->GetParameter(0));
			printf("Board %d: twcorr slope = %16.16f\n", idChild, f1->GetParameter(1));
		}
#endif

		if(fTWErr0[idChild] < 1.1*f1->GetParError(0) || fTWErr1[idChild] < 1.1*f1->GetParError(1)) {
			// if fit is worse, don't update parameters
			return;
		}

		fTWCorr0[idChild] = f1->GetParameter(0);
		fTWCorr1[idChild] = f1->GetParameter(1);

		fTWErr0[idChild] = f1->GetParError(0);
		fTWErr1[idChild] = f1->GetParError(1);

		// error of the corrected time at the first and last matched event (largest there):
		// var(t) = var(p0) + 2 t cov(p0,p1) + t^2 var(p1)
		fTWEdgeErr[idChild] = INT_MAX;
		if(fitResult.Get() && fitResult->IsValid()) {
			const double* x = fTvsT[idChild]->GetX();
			const double edges[2] = {*std::min_element(x, x + fTvsT[idChild]->GetN()), *std::max_element(x, x + fTvsT[idChild]->GetN())};
			double edgeErr = 0;
			for(double time : edges)
				edgeErr = std::max(edgeErr, std::sqrt(std::max(0., fitResult->CovMatrix(0,0) + 2*time*fitResult->CovMatrix(0,1) + time*time*fitResult->CovMatrix(1,1))));
			fTWEdgeErr[idChild] = edgeErr;
		}
	}

	void Unpacker::calcTimeWalkCorr() {
		for (auto fragId : fFragId) {
			calcTimeWalkCorr(fragId);
		}
	}

	bool Unpacker::fixSSDTimestamps() {
		// The first SSD event is usually the trigger ~1.4 s before the beam, the same trigger as the CAEN
		// boards' first event (timestamp 0); like theirs, it is kept and skipped in the matching (index 0).
		// Some SSD files start with SSD-only triggers from between the spills (no hits; e.g. 2426 s75: 27
		// over the 41 s before the beam); they match nothing.
		bool isJittery = false;
		fSSDMasked.assign(fSSDRawDigits.size(), false);
		if(fSSDRawDigits.empty()) return false;
		// Zero to first event
		const uint64_t firstTime = fSSDRawDigits[0].first;
		fSSDRawDigits[0].first = 0;
		// Check that clock doesn't have a weird jump
		for(size_t index = 1; index < fSSDRawDigits.size(); ++index) {
			fSSDRawDigits[index].first = fSSDRawDigits[index].first - firstTime;
			if(fSSDRawDigits[index].first - fSSDRawDigits[index-1].first >= 0xFFFFFFFF) {
				fSSDRawDigits[index].first = fSSDRawDigits[index].first & 0xFFFFFFFF;
				fSSDMasked[index] = true;
				isJittery = true;
			}
		}
		return isJittery;
	}
	bool Unpacker::fixCAENTimestamps() {
		// The V1720 timestamps come from a 31-bit counter with 8 ns ticks, so they wrap every
		// 2^31 * 8 ns = 17.18 s, which can fall inside a spill (e.g. run 2126 subrun 50: every
		// board steps back by 17.147 s at the same trigger). Unwrap each board in file order:
		// a wrap shows up as a step back of 17.18 s minus the gap to the next trigger (gaps are
		// at most ~5.3 s, after the spill), so a step back of more than 10 s is a wrap.
		//
		// Board resets: in some spills (e.g. 2128 s33, 2118 s46, 2126 s10) the boards are reset
		// mid-spill: the header event counter starts again at 1 and the trigger time tag restarts
		// from 0, so every event after a reset is labelled "spill start + time since the reset"
		// (blocks of events seconds off, overlapping each other). Each block after a reset is put
		// back in two steps:
		//  - coarse: the readout PC's clock (fragment metadata) runs through the reset; the block's
		//    median (host time - timestamp) against the clean block's gives the shift to ~1 ms;
		//  - fine: against the SSDs (they never reset): line SSD = p0 + p1 * CAEN from the clean
		//    block, then the densest 2 us window of (SSD - predicted) within +-3 ms per block.
		// Blocks that don't anchor to the SSDs keep the coarse shift. Spills without resets are
		// unchanged. The first timestamp of a spill is always 0 (a marker, skipped in the matching).
		const uint64_t wrap = (uint64_t(1) << 31) * 8;
		const uint64_t maxBackStep = 10000000000ULL; // 10 s in ns
		bool wrapped = false;

		std::vector<int64_t> ssdTimes;
		if(fReadSSDData)
			for(size_t index = 1; index < fSSDRawDigits.size(); ++index)
				ssdTimes.push_back(std::round(fBCOx * fSSDRawDigits[index].first));

		for(auto fragId : fCAENFragIds) {
			auto& ts = fFragTimestamps[fragId];
			const auto& counter = fCAENEventCounter[fragId];
			const auto& host = fCAENHostTime[fragId];

			// blocks: a new block starts where the event counter goes back (board reset); the 24-bit
			// counter's own wrap (0xFFFFFF -> 0) is not a reset
			std::vector<size_t> blockStart = {1};
			if(counter.size() == ts.size())
				for(size_t index = 2; index < ts.size(); ++index) {
					bool counterWrap = counter[index-1] > 0xFFFF00 && counter[index] < 0x100;
					if(counter[index] < counter[index-1] && !counterWrap) blockStart.push_back(index);
				}
			blockStart.push_back(ts.size());
			const size_t N_blocks = blockStart.size() - 1;

			// unwrap each block on its own
			for(size_t iblock = 0; iblock < N_blocks; ++iblock) {
				uint64_t add = 0;
				for(size_t index = blockStart[iblock] + 1; index < blockStart[iblock+1]; ++index) {
					if(ts[index] + add + maxBackStep < ts[index-1]) {
						add += wrap;
						wrapped = true;
					}
					ts[index] += add;
				}
			}
			if(N_blocks < 2) continue;

			if(host.size() != ts.size() || blockStart[1] - blockStart[0] < 3) {
				std::cout << "CAEN board " << fragId << ": " << N_blocks - 1 << " board resets, not repaired (no host times or no clean block)" << std::endl;
				fSyncQuality.board(fragId).nResets = N_blocks - 1;
				fSyncQuality.board(fragId).nUnpinned = N_blocks - 1;
				fSyncQuality.flags |= emph::dq::SyncQuality::kCAENReset | emph::dq::SyncQuality::kCAENResetUnpinned;
				fSuspectSpans[fragId].push_back({std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()}); // the whole spill
				continue;
			}

			// coarse: readout PC clock
			auto medianHostOffset = [&](size_t first, size_t last) {
				std::vector<int64_t> dt;
				for(size_t index = first; index < last; ++index)
					dt.push_back(int64_t(host[index]) - int64_t(ts[index]));
				std::nth_element(dt.begin(), dt.begin() + dt.size()/2, dt.end());
				return dt[dt.size()/2];
			};
			const int64_t baseline = medianHostOffset(blockStart[0], blockStart[1]);
			for(size_t iblock = 1; iblock < N_blocks; ++iblock) {
				int64_t shift = medianHostOffset(blockStart[iblock], blockStart[iblock+1]) - baseline;
				for(size_t index = blockStart[iblock]; index < blockStart[iblock+1]; ++index)
					ts[index] = uint64_t(int64_t(ts[index]) + shift);
			}

			// fine: SSDs, line SSD = p0 + p1 * CAEN from the clean block
			size_t N_fine = 0, N_matched = 0;
			std::vector<bool> isPinned(N_blocks, false); // block placed by the SSDs (the clean block counts as placed)
			isPinned[0] = true;
			if(ssdTimes.size() >= 20) {
				std::vector<int64_t> clean(ts.begin() + blockStart[0], ts.begin() + blockStart[1]);
				std::vector<int64_t> cleanCalibrate(clean.begin(), clean.begin() + std::min<size_t>(clean.size(), 400));
				std::vector<int64_t> dt = calcDifferences<int64_t>(ssdTimes, cleanCalibrate);
				auto [N_occur, timeOffset] = findOffset(dt, 2500);
				double p0 = timeOffset, p1 = 1;
				for(double matchWindow : {5e3, 1e3, 300.0}) {
					std::vector<double> fitCAEN, fitSSD;
					for(auto time : clean) {
						double predicted = p0 + p1 * time;
						size_t above = std::lower_bound(ssdTimes.begin(), ssdTimes.end(), predicted) - ssdTimes.begin();
						above = std::min(std::max(above, size_t(1)), ssdTimes.size() - 1);
						size_t nearest = (std::abs(ssdTimes[above-1] - predicted) < std::abs(ssdTimes[above] - predicted)) ? above - 1 : above;
						if(std::abs(ssdTimes[nearest] - predicted) < matchWindow) {
							fitCAEN.push_back(time);
							fitSSD.push_back(ssdTimes[nearest]);
						}
					}
					if(fitCAEN.size() > 2) fitLine(fitCAEN, fitSSD, p0, p1);
				}

				for(size_t iblock = 1; iblock < N_blocks; ++iblock) {
					// SSD - predicted within +-3 ms, densest 2 us window
					std::vector<double> diffs;
					for(size_t index = blockStart[iblock]; index < blockStart[iblock+1]; ++index) {
						double predicted = p0 + p1 * ts[index];
						auto first = std::lower_bound(ssdTimes.begin(), ssdTimes.end(), predicted - 3e6);
						auto last = std::upper_bound(ssdTimes.begin(), ssdTimes.end(), predicted + 3e6);
						for(auto it = first; it != last; ++it) diffs.push_back(*it - predicted);
					}
					if(diffs.empty()) continue;
					std::sort(diffs.begin(), diffs.end());
					size_t bestCount = 0, bestStart = 0;
					for(size_t k = 0, j = 0; k < diffs.size(); ++k) {
						if(j < k) j = k;
						while(j < diffs.size() && diffs[j] - diffs[k] <= 2000) ++j;
						if(j - k > bestCount) { bestCount = j - k; bestStart = k; }
					}
					size_t N_block = blockStart[iblock+1] - blockStart[iblock];
					if(bestCount < std::min<size_t>(3, N_block)) continue;
					double fine = 0;
					for(size_t k = bestStart; k < bestStart + bestCount; ++k) fine += diffs[k];
					fine /= bestCount;
					for(size_t index = blockStart[iblock]; index < blockStart[iblock+1]; ++index)
						ts[index] = uint64_t(int64_t(ts[index]) + std::llround(fine / p1));
					++N_fine;
					N_matched += bestCount;
					isPinned[iblock] = true;
				}
			}

			// suspect spans for the events' sync record: the gap at each reset (the board lost triggers
			// there), and every block placed by the readout PC clock only (+-1 ms)
			const int64_t coarseUncertainty = 1000000; // 1 ms in ns
			auto blockRange = [&](size_t iblock) {
				auto first = ts.begin() + blockStart[iblock], last = ts.begin() + blockStart[iblock+1];
				return std::make_pair(int64_t(*std::min_element(first, last)), int64_t(*std::max_element(first, last)));
			};
			for(size_t iblock = 1; iblock < N_blocks; ++iblock) {
				auto [previousMin, previousMax] = blockRange(iblock - 1);
				auto [blockMin, blockMax] = blockRange(iblock);
				fSuspectSpans[fragId].push_back({std::min(previousMax, blockMin), std::max(previousMax, blockMin)});
				if(!isPinned[iblock])
					fSuspectSpans[fragId].push_back({blockMin - coarseUncertainty, blockMax + coarseUncertainty});
			}
			std::cout << "CAEN board " << fragId << ": " << N_blocks - 1 << " board resets repaired (host clock), "
			          << N_fine << " blocks fixed to the SSDs (" << N_matched << " events)" << std::endl;
			fSyncQuality.board(fragId).nResets = N_blocks - 1;
			fSyncQuality.board(fragId).nUnpinned = N_blocks - 1 - N_fine;
			fSyncQuality.flags |= emph::dq::SyncQuality::kCAENReset;
			if(N_fine < N_blocks - 1) fSyncQuality.flags |= emph::dq::SyncQuality::kCAENResetUnpinned;
		}
		return wrapped;
	}
	bool Unpacker::determineGrandfather() {
		// determine grandfather clock
		fragIdGrandfather = fFragId[0];
		auto samplesGrandfather = fFragTimestamps[fragIdGrandfather].size();
		for(auto fragId : fFragId) {
			if(fFragTimestamps[fragId].size() > samplesGrandfather) {
				fragIdGrandfather = fragId;
				samplesGrandfather = fFragTimestamps[fragId].size();
			}
		}
// No need to check SSDs (usually less than the others)
//		if(fSSDRawDigits.size() > samplesGrandfather) {
//			fragIdGrandfather = 773;
//			samplesGrandfather = fSSDRawDigits.size();
//		}
		return true;
	}

	bool Unpacker::seedTimeSync() {
		const uint32_t ssdId = 773; // SSDs' entry in the sync status
		// Initial TWCorr0/TWCorr1 for every clock from the gaps between consecutive triggers
		// (seedSync in TimeSync.h), so the first matching pass starts with the right offset and drift.
		// The grandfather from determineGrandfather() is kept if the SSDs seed against it; if they
		// don't (e.g. its data belong to another spill, or are scrambled), the next board by number
		// of events is tried. Without SSD data a board must seed at least half of the others.
		// Boards that don't seed keep the identity start (TWCorr0 = 0, TWCorr1 = 1).
		auto boardTimes = [&](artdaq::Fragment::fragment_id_t fragId) {
			// first timestamp of a spill is a marker
			return std::vector<uint64_t>(fFragTimestamps[fragId].begin() + std::min<size_t>(1, fFragTimestamps[fragId].size()), fFragTimestamps[fragId].end());
		};
		std::vector<double> ssdTimes;
		if(fReadSSDData)
			for(size_t index = 1; index < fSSDRawDigits.size(); ++index)
				ssdTimes.push_back(fBCOx * fSSDRawDigits[index].first);

		auto isGood = [&](uint64_t N_matched, size_t N_child) {
			return N_matched > 0 && N_matched >= fSeedMinMatchFraction * N_child;
		};
		auto report = [&](const std::string& childName, artdaq::Fragment::fragment_id_t grandId, uint64_t N_matched, double timeOffset, double scale, size_t N_child) {
			if(N_matched == 0)
				printf("Seed %s vs board %d: failed\n", childName.c_str(), grandId);
			else
				printf("Seed %s vs board %d: drift %.3f ppm, offset %.0f ns, matched %lu of %zu%s\n", childName.c_str(), grandId,
				       (scale - 1) * 1e6, timeOffset, (unsigned long)N_matched, N_child, isGood(N_matched, N_child) ? "" : ": too few matched, not used");
		};

		// candidates: most events first; ties keep fFragId order, so the first is determineGrandfather's choice
		std::vector<artdaq::Fragment::fragment_id_t> candidates(fFragId);
		std::stable_sort(candidates.begin(), candidates.end(), [&](auto a, auto b) { return fFragTimestamps[a].size() > fFragTimestamps[b].size(); });

		bool found = false;
		double ssdOffset = 0, ssdScale = 1;
		for(auto candidate : candidates) {
			auto grandTimes = boardTimes(candidate);
			if(!ssdTimes.empty()) {
				auto [N_matched, timeOffset, scale] = seedSync(std::vector<double>(grandTimes.begin(), grandTimes.end()), ssdTimes);
				report("SSD", candidate, N_matched, timeOffset, scale, ssdTimes.size());
				if(!isGood(N_matched, ssdTimes.size())) continue;
				ssdOffset = timeOffset;
				ssdScale = scale;
			} else {
				size_t N_good = 0, N_other = 0;
				for(auto fragId : fFragId) {
					if(fragId == candidate) continue;
					++N_other;
					auto childTimes = boardTimes(fragId);
					auto [N_matched, timeOffset, scale] = seedSync(grandTimes, childTimes);
					if(isGood(N_matched, childTimes.size())) ++N_good;
				}
				if(2 * N_good < N_other) {
					printf("Seed: board %d seeds only %zu of %zu boards\n", candidate, N_good, N_other);
					continue;
				}
			}
			if(candidate != fragIdGrandfather) {
				std::cout << "Seed: grandfather board " << fragIdGrandfather << " replaced by board " << candidate << std::endl;
				fSyncQuality.flags |= emph::dq::SyncQuality::kGrandfatherReplaced;
			}
			fragIdGrandfather = candidate;
			found = true;
			break;
		}
		if(!found) {
			std::cout << "Seed: no board works as grandfather, keeping board " << fragIdGrandfather << " unseeded" << std::endl;
			fSyncQuality.flags |= emph::dq::SyncQuality::kSeedFailed;
			return false;
		}

		fSyncQuality.board(fragIdGrandfather).isSeeded = true;
		if(!ssdTimes.empty()) { // the SSD correction is stored under the grandfather's id
			fTWCorr0[fragIdGrandfather] = ssdOffset;
			fTWCorr1[fragIdGrandfather] = ssdScale;
			fSyncQuality.board(ssdId).isSeeded = true;
		}
		auto grandTimes = boardTimes(fragIdGrandfather);
		std::vector<artdaq::Fragment::fragment_id_t> unseededTRB3;
		for(auto fragId : fFragId) {
			if(fragId == fragIdGrandfather) continue;
			auto childTimes = boardTimes(fragId);
			auto [N_matched, timeOffset, scale] = seedSync(grandTimes, childTimes);
			report("board " + std::to_string(fragId), fragIdGrandfather, N_matched, timeOffset, scale, childTimes.size());
			if(!isGood(N_matched, childTimes.size())) {
				if(std::find(fTRB3FragIds.begin(), fTRB3FragIds.end(), fragId) != fTRB3FragIds.end())
					unseededTRB3.push_back(fragId);
				continue;
			}
			fTWCorr0[fragId] = timeOffset;
			fTWCorr1[fragId] = scale;
			fSyncQuality.board(fragId).isSeeded = true;
		}

		// Misfiled TRB3 data (DAQ): after beamless spills a subrun's file can hold the next spill's TRB3
		// data, so this spill's are in the previous file (e.g. 2118 s32-s56), or none at all. A TRB3 board
		// that doesn't seed here, or is missing here, takes the data of the first neighbouring file
		// (previous, then next) that seeds against the grandfather.
		bool needNeighbours = !unseededTRB3.empty() || fTRB3FragIds.empty();
		for(size_t ineighbour = 0; needNeighbours && ineighbour < fTRB3Neighbours.size(); ++ineighbour) {
			if(!readNeighbourTRB3(ineighbour)) continue;
			auto& neighbour = fTRB3Neighbours[ineighbour];
			for(auto fragId : neighbour.boards) {
				bool isUnseeded = std::find(unseededTRB3.begin(), unseededTRB3.end(), fragId) != unseededTRB3.end();
				bool isMissing = std::find(fFragId.begin(), fFragId.end(), fragId) == fFragId.end();
				if(!isUnseeded && !isMissing) continue;
				auto& neighbourTimes = neighbour.timestamps[fragId];
				std::vector<uint64_t> childTimes(neighbourTimes.begin() + std::min<size_t>(1, neighbourTimes.size()), neighbourTimes.end());
				auto [N_matched, timeOffset, scale] = seedSync(grandTimes, childTimes);
				report("board " + std::to_string(fragId) + " (" + neighbour.label + " file)", fragIdGrandfather, N_matched, timeOffset, scale, childTimes.size());
				if(!isGood(N_matched, childTimes.size())) continue;

				std::cout << "TRB3 board " << fragId << ": this spill's data taken from the " << neighbour.label << " file" << std::endl;
				fTRB3RawDigits[fragId] = std::move(neighbour.digits[fragId]);
				fFragTimestamps[fragId] = std::move(neighbourTimes);
				if(isMissing) {
					fFragId.push_back(fragId);
					fTRB3FragIds.push_back(fragId);
					fFragCounter[fragId] = 0;
					fTWErr0[fragId] = INT_MAX;
					fTWErr1[fragId] = INT_MAX;
					fTWEdgeErr[fragId] = INT_MAX;
				}
				fTWCorr0[fragId] = timeOffset;
				fTWCorr1[fragId] = scale;
				fSyncQuality.board(fragId).isSeeded = true;
				fSyncQuality.board(fragId).fileOffset = neighbour.fileOffset;
				fSyncQuality.flags |= emph::dq::SyncQuality::kTRB3FromNeighbour;
				if(isUnseeded)
					unseededTRB3.erase(std::find(unseededTRB3.begin(), unseededTRB3.end(), fragId));
			}
		}

		for(auto fragId : fFragId)
			if(!fSyncQuality.board(fragId).isSeeded)
				fSyncQuality.flags |= emph::dq::SyncQuality::kSeedFailed;
		return true;
	}

	void Unpacker::setBoardDetectors() {
		// Which detectors each board reads out (channel map), so that the data quality can say per
		// event which detectors a dropped or degraded board takes with it. The first events are enough.
		const size_t N_look = 50;
		emph::cmap::EChannel echan;
		for(auto fragId : fCAENFragIds) {
			auto& status = fSyncQuality.board(fragId);
			echan.SetBoardType(emph::cmap::V1720);
			echan.SetBoard(fragId);
			const auto& events = fWaveForms[fragId];
			for(size_t index = 0; index < std::min(N_look, events.size()); ++index)
				for(const auto& waveForm : events[index]) {
					echan.SetChannel(waveForm.Channel());
					if(!fChannelMap->IsValidEChan(echan)) continue;
					auto detId = fChannelMap->DetChan(echan).DetId();
					if(detId != emph::geo::NDetectors) status.detectors |= 1u << detId;
				}
		}
		for(auto fragId : fTRB3FragIds) {
			auto& status = fSyncQuality.board(fragId);
			echan.SetBoardType(emph::cmap::TRB3);
			const auto& events = fTRB3RawDigits[fragId];
			for(size_t index = 0; index < std::min(N_look, events.size()); ++index)
				for(const auto& digit : events[index]) {
					echan.SetBoard(digit.GetBoardId());
					echan.SetChannel(digit.GetChannel());
					auto detId = fChannelMap->DetChan(echan).DetId();
					if(detId != emph::geo::NDetectors) status.detectors |= 1u << detId;
				}
		}
		if(fReadSSDData)
			fSyncQuality.board(773).detectors = 1u << emph::geo::SSD;
		// no TRB3 data at all: its detectors from the channel map
		if(fSyncQuality.hasFlag(emph::dq::SyncQuality::kTRB3Missing))
			for(const auto& entry : fChannelMap->EChanMap()) {
				emph::cmap::EChannel echannel = entry.first; // copies: EChannel/DChannel getters aren't const
				emph::cmap::DChannel dchannel = entry.second;
				if(echannel.BoardType() == emph::cmap::TRB3 && dchannel.DetId() != emph::geo::NDetectors)
					fSyncQuality.missingDetectors |= 1u << dchannel.DetId();
			}
	}

	void Unpacker::prepareEventSync() {
		// Spill-wide masks, and the suspect spans of each used board in grandfather time (the events' time)
		fLostDetectors = fSyncQuality.lostDetectors();
		fPartialDetectors = fSyncQuality.partialDetectors();
		fSuspectWindows.clear();
		for(auto& [fragId, spans] : fSuspectSpans) {
			const auto& status = fSyncQuality.board(fragId);
			if(!status.isUsed) continue; // lost anyway
			for(auto [start, end] : spans) {
				if(start == std::numeric_limits<int64_t>::min()) { // the whole spill
					fSuspectWindows.emplace_back(start, end, status.detectors);
					continue;
				}
				auto toGrandfather = [&](int64_t time) {
					return fragId == fragIdGrandfather ? time : int64_t(std::llround(fTWCorr0[fragId] + fTWCorr1[fragId] * time));
				};
				fSuspectWindows.emplace_back(toGrandfather(start), toGrandfather(end), status.detectors);
			}
		}
		// summary per board: number of windows and the time they cover (ms, grandfather time)
		std::map<uint32_t, std::pair<size_t, double> > summary;
		for(const auto& [start, end, detectors] : fSuspectWindows) {
			auto& [N_windows, covered] = summary[detectors];
			++N_windows;
			covered += start == std::numeric_limits<int64_t>::min() ? -1 : (end - start) * 1e-6;
		}
		for(const auto& [detectors, entry] : summary)
			std::cout << "Sync windows for detectors 0x" << std::hex << detectors << std::dec << ": " << entry.first
			          << " windows, " << entry.second << " ms" << std::endl;
		if(fVerbosity > 0)
			for(const auto& [start, end, detectors] : fSuspectWindows)
				std::cout << "  window 0x" << std::hex << detectors << std::dec << ": " << start * 1e-6 << " - " << end * 1e-6 << " ms" << std::endl;
	}

	void Unpacker::dropUnsyncedBoards() {
		const uint32_t ssdId = 773; // SSDs' entry in the sync status
		// After calibration, a board whose events mostly don't match the grandfather (data of another
		// spill, broken timestamps) would only put wrong hits into the events: it is left out of the
		// event building, with a message. The same for the SSDs (matched under the grandfather's id).
		// Without this the fit of such a board has no points ("Fit data is empty") and the job crashes.
		auto countMatched = [&](artdaq::Fragment::fragment_id_t idChild) {
			return size_t(std::count_if(masks[idChild].begin(), masks[idChild].end(), [](int index) { return index != -1; }));
		};
		auto isSynced = [&](size_t N_matched, size_t N_child) {
			return N_matched >= 3 && N_matched >= fMinMatchFraction * N_child;
		};
		std::vector<artdaq::Fragment::fragment_id_t> keptFragIds;
		for(auto fragId : fFragId) {
			size_t N_child = fFragTimestamps[fragId].size() > 1 ? fFragTimestamps[fragId].size() - 1 : 0; // first is a marker
			size_t N_matched = fragId == fragIdGrandfather ? N_child : countMatched(fragId);
			auto& status = fSyncQuality.board(fragId);
			status.nEvents = N_child;
			status.nMatched = N_matched;
			if(fragId != fragIdGrandfather && !isSynced(N_matched, N_child)) {
				std::cout << "Board " << fragId << " not synced (" << N_matched << " of " << N_child
				          << " events matched to board " << fragIdGrandfather << "): left out of the events" << std::endl;
				status.isUsed = false;
				fSyncQuality.flags |= emph::dq::SyncQuality::kBoardDropped;
				continue;
			}
			keptFragIds.push_back(fragId);
		}
		fFragId = keptFragIds;

		if(fReadSSDData) {
			size_t N_child = fSSDRawDigits.size() > 1 ? fSSDRawDigits.size() - 1 : 0;
			size_t N_matched = countMatched(fragIdGrandfather);
			auto& status = fSyncQuality.board(ssdId);
			status.nEvents = N_child;
			status.nMatched = N_matched;
			if(!isSynced(N_matched, N_child)) {
				fSSDSynced = false;
				status.isUsed = false;
				fSyncQuality.flags |= emph::dq::SyncQuality::kSSDNotSynced;
				std::cout << "SSDs not synced (" << N_matched << " of " << N_child
				          << " events matched to board " << fragIdGrandfather << "): left out of the events" << std::endl;
			}
		}
	}

  /***************************************************************************/
	// individual routine
	bool Unpacker::findMatches(artdaq::Fragment::fragment_id_t idChild, int timeUncertainty) {
		if(idChild == fragIdGrandfather) {
			std::cout << "Finding matches for (" << fragIdGrandfather <<", SSDs)"<< std::endl;
		} else {
			std::cout << "Finding matches for (" << fragIdGrandfather << ", " << idChild << ")" << std::endl;
		}
		// Prepare local copy of timestamps
		std::vector<uint64_t> tsGrand = fFragTimestamps[fragIdGrandfather];
		std::vector<uint64_t> tsChild; tsChild.reserve(fFragTimestamps[idChild].size());
		// Configure local copy of child
		if(idChild == fragIdGrandfather) { // SSDs
			// Scale clock with linear fit parameters and BCOx
			for(auto time : fSSDRawDigits)
				tsChild.push_back(std::round(fTWCorr0[idChild] + fTWCorr1[idChild] * fBCOx * time.first));
		} else { // Not SSDs
			tsChild = fFragTimestamps[idChild];
			// Scale clock with linear fit parameters
			for(auto& time : tsChild)
				time = std::round(fTWCorr0[idChild] + fTWCorr1[idChild] * time);
		}

		// Too few events to match (e.g. a beamless spill, a few triggers): compareGrandfather works on
		// windows of 30 events and needs more on each side. No matches; dropUnsyncedBoards() leaves the
		// board (or the SSDs) out
		const size_t N_minEvents = 31 + 1; // + the spill-start marker
		if(tsGrand.size() < N_minEvents || tsChild.size() < N_minEvents) {
			std::cout << "Too few events to match (" << tsGrand.size() << " and " << tsChild.size() << " with the marker)" << std::endl;
			masks[idChild].assign(fFragTimestamps[fragIdGrandfather].size(), -1);
			return false;
		}

		// Ignore first event
		tsGrand.erase(tsGrand.begin());
		tsChild.erase(tsChild.begin());

		masks[idChild].clear();
		masks[idChild].push_back(-1);
		// Find matches
		auto remainder = compareGrandfather(tsGrand, tsChild, timeUncertainty);
		masks[idChild].insert(masks[idChild].end(), remainder.begin(), remainder.end());

		// because we ignored the first event, we need to +1 each matched index
		for(auto &index : masks[idChild]) {
			if(index != -1)
				index+=1;
		}
		return !masks[idChild].empty();
	}

	bool Unpacker::findMatches(int timeUncertainty)	{
		for(auto fragId : fFragId) {
			findMatches(fragId, timeUncertainty);
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

			if (fMakeTDiffHistos)
				makeTDiffHistos();

			// determine t0s for each board
			for (auto fragId : fFragId) {
				fTWCorr0[fragId] = 0;
				fTWCorr1[fragId] = 1;
				fTWErr0[fragId] = INT_MAX;
				fTWErr1[fragId] = INT_MAX;
				fTWEdgeErr[fragId] = INT_MAX;
			}

			if(fixSSDTimestamps()) {
				std::cout << "SSDs are jittery: " << std::count(fSSDMasked.begin(), fSSDMasked.end(), true) << " timestamps kept only their lower 32 bits" << std::endl;
				fSyncQuality.flags |= emph::dq::SyncQuality::kSSDTimestampMasked;
			}

			if(fixCAENTimestamps()) {
				std::cout << "CAEN timestamps wrapped (31 bit x 8 ns), unwrapped" << std::endl;
				fSyncQuality.flags |= emph::dq::SyncQuality::kCAENWrapped;
			}

			if(!determineGrandfather())
				std::cout << "Unable to determine grandfather clock" << std::endl;

			if(fSeedTimeSync)
				seedTimeSync();
			if(fReadTRB3Data && fTRB3FragIds.empty()) {
				std::cout << "TRB3: no data for this spill" << (fTRB3Neighbours.empty() ? "" : " (also not in the neighbouring files)") << std::endl;
				fSyncQuality.flags |= emph::dq::SyncQuality::kTRB3Missing;
			}

			if (!findMatches(100)) abort();
			calcTimeWalkCorr();

			// do the SSDs twice
			findMatches(fragIdGrandfather, 100);
			calcTimeWalkCorr(fragIdGrandfather);

			// Noah's metric (checking precision of linear fit params): retry until the corrected time is
			// known to within maxEdgeError at both ends of the spill (a tenth of the 100 ns match window).
			// It used to ask for intercept error < 10 ns and slope error < 1e-9, which the SSDs never reach
			// (43 ns jitter: slope error ~3e-9 at ~300 events; intercept taken at t = 0, far outside the
			// spill when the SSD clock starts at a stray trigger) although they are known to < 7 ns.
			const double maxEdgeError = 10; // ns
			size_t maxTries = 10;
			for(auto fragId : fFragId) {
				size_t tries = 0;
				while(fTWEdgeErr[fragId] > maxEdgeError) {
					auto reportFailed = [&](const char* reason) {
						if(fTWErr0[fragId] >= INT_MAX) { // never fitted (e.g. beamless spill, too few matched events): no errors to show
							std::cout << "Close Enough Metric: " << (fragId == fragIdGrandfather ? std::string("SSDs") : "board " + std::to_string(fragId))
							          << (fragId == fragIdGrandfather ? " have" : " has") << " no time-walk fit (too few matched events)" << std::endl;
							return;
						}
						std::cout << "Close Enough Metric: failed for " << (fragId == fragIdGrandfather ? std::string("SSDs") : "board " + std::to_string(fragId))
						          << " (" << reason << "): time error at the spill edges " << fTWEdgeErr[fragId] << " ns (limit " << maxEdgeError
						          << "), intercept error " << fTWErr0[fragId] << " ns, slope error " << fTWErr1[fragId] << ", after " << tries << " passes" << std::endl;
					};
					if(tries >= maxTries){ reportFailed("too many passes"); break; }
					++tries;
					const double previousCorr0 = fTWCorr0[fragId], previousCorr1 = fTWCorr1[fragId];
					findMatches(fragId, 100);
					calcTimeWalkCorr(fragId);
					// a pass that leaves the correction unchanged (no better fit) would only be repeated
					// with identical inputs and results: stop (e.g. a board of another spill: 10 identical passes)
					if(fTWCorr0[fragId] == previousCorr0 && fTWCorr1[fragId] == previousCorr1
					   && fTWEdgeErr[fragId] > maxEdgeError) {
						reportFailed("fit no longer improves");
						break;
					}
				}
			}

			dropUnsyncedBoards();
			setBoardDetectors();
			fSyncQuality.grandfather = fragIdGrandfather;
			prepareEventSync();
			std::cout << fSyncQuality << std::endl;
			art::put_product_in_principal(std::make_unique<emph::dq::SyncQuality>(fSyncQuality), *outSR, "raw", "TimeSync");

			{// limit scope of punch card
				std::cout << "Preparing punch card" << std::endl;
				std::vector<std::list<size_t>> punchCards;
				// Set up punch card for each CAEN, TRB3, and SSD
				for (size_t ifrag=0; ifrag <= fFragId.size(); ++ifrag) {
					auto fragId = ssdId;
					auto size = fSSDSynced ? fSSDRawDigits.size() : 0;
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
							tsA = std::round(fTWCorr0[fragIdGrandfather] + fTWCorr1[fragIdGrandfather]*fBCOx*fSSDRawDigits[iA].first);
						else if(fragA != fragIdGrandfather) // CAEN and TRB3 children
							tsA = std::round(fTWCorr0[fragA] + fTWCorr1[fragA]*fFragTimestamps[fragA][iA]);
						else // grandfather
							tsA = fFragTimestamps[fragA][iA];
						event.push_back(std::make_pair(fragA, iA));

						for (size_t jfrag=ifrag + 1; jfrag <= fFragId.size(); ++jfrag) {
							auto fragB = ssdId;
							if(jfrag < fFragId.size())
								fragB = fFragId[jfrag];

							uint64_t best = 100;
							for(auto jt = punchCards[jfrag].begin(); jt != punchCards[jfrag].end(); ++jt) {
								auto iB = *jt; // index of time stamp to be checked
								int64_t tsB;
								// Project timestamp to grandfather
								if(fragB == ssdId) // SSD
									tsB = std::round(fTWCorr0[fragIdGrandfather] + fTWCorr1[fragIdGrandfather]*fBCOx*fSSDRawDigits[iB].first);
								else if(fragB != fragIdGrandfather)
									tsB = std::round(fTWCorr0[fragB] + fTWCorr1[fragB]*fFragTimestamps[fragB][iB]);
								else
									tsB = fFragTimestamps[fragB][iB];

								// Adjust this resolution as needed
								if(std::llabs(tsA - tsB) <= best) { // FOUND YOU
									best = std::llabs(tsA - tsB);
								} else if(best < 100) {
									event.push_back(std::make_pair(fragB, iB-1));
									// Remove element from punch card to indicate this event has been handled
									punchCards[jfrag].erase(--jt);
									break;
								}
							}
						}
						// After checking all other fragID's, push this event on the stack
						eventStack.push_back(std::make_pair(tsA, event));
					}
				}
			}
			// Difference of Differences Metric
			for(auto fragId: fFragId) {
				if(fragId == fragIdGrandfather) fragId = ssdId;
				makeDiffDiffHistos(fragIdGrandfather, fragId);
			}
			// Now we have our event stack for packing events (hard part is over?)
			{// Getting some stats for events
				size_t gotSSD = 0;
				size_t gotSSDHits = 0; // SSD readout with at least one hit (many triggers have an empty one)
				size_t gotALL = 0;
				size_t eventCount = 0;
				for(auto event : eventStack) {
					//std::cout << "Event:\t" << event.first << "\twith these attendees:\t";
					eventCount++;
					bool triple[3] = {false,false,false};
					for(const auto& attendee : event.second) {
						//std::cout << attendee.first << "\t";
						if(attendee.first == ssdId) {
							triple[0] = true;
							if(!fSSDRawDigits[attendee.second].second.empty()) gotSSDHits++;
						}
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
				std::cout << "Percentage events with SSD: " << 1.0*gotSSD/eventCount << std::endl;
				std::cout << "Percentage events with SSD hits: " << 1.0*gotSSDHits/eventCount << std::endl;
				std::cout << "Percentage events with All: " << 1.0*gotALL/eventCount << std::endl;

				// Independent check of the sync: the V1720 header event counter numbers the triggers on every
				// board (constant offset between boards), so the CAEN boards in one event must carry the same
				// trigger once their offsets are taken out. (Boards reset mid-spill restart their counter.)
				if(fCAENFragIds.size() > 1) {
					auto counterDiff = [](uint64_t a, uint64_t b) { return int32_t(uint32_t((a - b) & 0xFFFFFF) << 8) >> 8; }; // 24-bit counter
					artdaq::Fragment::fragment_id_t refBoard = fCAENFragIds[0];
					for(auto fragId : fCAENFragIds)
						if(fCAENEventCounter[fragId].size() > fCAENEventCounter[refBoard].size()) refBoard = fragId;
					// each board's counter offset to the reference board, from the events with both
					std::unordered_map<artdaq::Fragment::fragment_id_t, std::vector<int32_t> > counterDiffs;
					for(const auto& event : eventStack) {
						int64_t refIndex = -1;
						for(const auto& attendee : event.second)
							if(attendee.first == refBoard) refIndex = attendee.second;
						if(refIndex < 0) continue;
						for(const auto& attendee : event.second)
							if(attendee.first != refBoard && fCAENEventCounter.count(attendee.first))
								counterDiffs[attendee.first].push_back(counterDiff(fCAENEventCounter[attendee.first][attendee.second], fCAENEventCounter[refBoard][refIndex]));
					}
					std::unordered_map<artdaq::Fragment::fragment_id_t, int32_t> counterOffset = {{refBoard, 0}};
					for(auto& [fragId, diffs] : counterDiffs) counterOffset[fragId] = std::lround(findMedian(diffs));
					size_t N_checked = 0, N_agree = 0;
					for(const auto& event : eventStack) {
						std::vector<std::pair<uint64_t, int32_t> > caen; // counter, offset
						for(const auto& attendee : event.second)
							if(counterOffset.count(attendee.first))
								caen.emplace_back(fCAENEventCounter[attendee.first][attendee.second], counterOffset[attendee.first]);
						if(caen.size() < 2) continue;
						++N_checked;
						bool agree = true;
						for(size_t k = 1; k < caen.size(); ++k)
							if(counterDiff(caen[k].first, caen[0].first) != caen[k].second - caen[0].second) agree = false;
						if(agree) ++N_agree;
					}
					std::cout << "CAEN trigger-counter check: " << N_agree << " of " << N_checked
					          << " events with two or more CAEN boards carry the same trigger" << std::endl;
				}
			}

			std::cout << "Sorting event stack" << std::endl;
			// Sorting in reverse so that we can pop events off the end
			std::sort(eventStack.begin(), eventStack.end(), std::greater<>());

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

			const int64_t eventTime = event.first;
			uint32_t degradedHere = 0; // detectors degraded in this event only
			fPrevTS = event.first; // set this to guarantee at least one run through the loop
			while(fPrevTS - event.first < 100) {
				eventStack.pop_back(); // remove event from stack
				fPrevTS = event.first;
				for(auto attendee : event.second) {
					auto fragId = attendee.first;
					auto index = attendee.second;
					if(fReadSSDData && fragId == ssdId) { // SSD
						if(index < fSSDMasked.size() && fSSDMasked[index]) degradedHere |= 1u << emph::geo::SSD;
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

			// time-sync record of this event
			auto eventSync = std::make_unique<emph::dq::EventSync>();
			eventSync->notSyncedDetectors = fLostDetectors;
			eventSync->degradedDetectors = fPartialDetectors | degradedHere;
			for(const auto& [start, end, detectors] : fSuspectWindows)
				if(eventTime >= start && eventTime <= end) eventSync->degradedDetectors |= detectors;
			put_product_in_principal(std::move(eventSync), *outE, "raw", "TimeSync");

			// Write out event data
			if (!evtSSDRawDigits->empty())
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
