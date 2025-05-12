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

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>

#include <bits/stdc++.h>
// std::minmax_element of a vector
// - NTK

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
    fNFER = ps.get<int>("NFER",0);
    fBCOx = ps.get<double>("BCOx",151.1515152);
    fFirstSubRunHasExtraTrigger = ps.get<bool>("firstSubRunHasExtraTrigger",false);
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

  void Unpacker::calcTimeWalkCorr()
  {
	  return; // tmp to avoid -NTK (remove later)<@@>
    std::cout << "Entering \"calcTimeWalkCorr\"" << std::endl;

    if (! fdTvsT.empty()) return; // note, this should _never_ happen...

    art::ServiceHandle<art::TFileService> tfs;
    art::TFileDirectory tdir3 = tfs->mkdir("TimeWalk","");
    char hname[256];
    char htitle[256];
    // create histograms for CAEN and TRB3 boards
    for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
      auto fragId = fFragId[ifrag];
      sprintf(hname,"tBoard_%d",fragId);
      sprintf(htitle,";Board %d Timestamp;Num. Fragments",fragId);
      fTHist[fragId] = tdir3.make<TH1D>(hname,htitle,500,0.,5.);
      sprintf(hname,"tw_%d",fragId);
      sprintf(htitle,"Event Time Walk, Board %d; Board 0 Timestamp (s); Board %d-Board 0 (ns)",
	      fragId, fragId);
      fdTvsT[fragId] = tdir3.make<TH2D>(hname,htitle,250,0.,5.,2000,-10000.,10000.);
      sprintf(hname,"tVsFrag_%d",fragId);
      sprintf(htitle,"; Board %d Frag. Num.; Board %d Timestamp",
	      fragId, fragId);
      fTvsFrag[fragId] = tdir3.make<TH2D>(hname,htitle,500,0.,float(fFragTimestamps[fragId].size()), 500,0.,5.);
    }
    // now create histograms for SSDs
    sprintf(hname,"tSSD");
    sprintf(htitle,";SSD Timestamp;Num. Fragments");
    fSSDTHist = tdir3.make<TH1D>(hname,htitle,500,0.,5.);
    sprintf(hname,"twSSD");
    sprintf(htitle,"Event Time Walk SSD; Board 0 Timestamp (s); SSD (ns)");
    fSSDdTvsT = tdir3.make<TH2D>(hname,htitle,250,0.,5.,2000,-10000.,10000.);
    sprintf(hname,"tVsSSDFrag");
    sprintf(htitle,"; SSD Frag. Num.; SSD Timestamp");
    fSSDTvsFrag = tdir3.make<TH2D>(hname,htitle,500,0.,float(fSSDRawDigits.size()), 500,0.,5.);
    sprintf(hname,"tSSDdT");
    sprintf(htitle,"; t (Fragment); t (SSD)");
    fSSDdTHist = tdir3.make<TH2D>(hname,htitle,1000,0.,5.,1000,0.,5.);

    double tB;

    for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
      auto fragId = fFragId[ifrag];
      for (size_t i=fFragCounter[fragId]; i<fFragTimestamps[fragId].size(); ++i) {
	tB = double(fFragTimestamps[fragId][i] - fT0[fragId]);
	fTHist[fragId]->Fill(tB*1.e-9);
	fTvsFrag[fragId]->Fill(float(i),fFragTimestamps[fragId][i]*1.e-9);
	/*
	  if ((i-fFragCounter[fragId])<20) {
	  std::cout << "Board " << (int)fragId << ", frag " << i << ", t = "
	  << tB << std::endl;
	  }
	*/
      }
    }

    int nBadFER=0;
    for (size_t issdEvt=1; issdEvt<fSSDRawDigits.size(); ++issdEvt) {
      auto & digvec = fSSDRawDigits[issdEvt].second;
      tB = double(fSSDRawDigits[issdEvt].first)-fSSDT0;
      //      std::cout << "tB(ssd) = " << tB*150 << std::endl;
      fSSDTHist->Fill(tB*fBCOx*1.e-9);
      fSSDTvsFrag->Fill(float(issdEvt),tB*fBCOx*1.e-9);
    }

    // fill time walk histos.
    // first, loop over fragment times of Board 0
    double tB0;
    double dtB;
    size_t nneigh = 500;

    double tssd;
    double tfrag;
    auto fragId = fFragId[0];
    std::cout << "Using Fragment " << fragId << std::endl;

    for (size_t i=fFragCounter[fragId]; i<fFragTimestamps[fragId].size(); ++i) {
      tfrag = (fFragTimestamps[fragId][i] - fT0[fragId])*1.e-9;
      for (size_t j=0; j<fSSDRawDigits.size(); ++j) {
	tssd = (fSSDRawDigits[j].first - fSSDT0)*fBCOx*1.e-9;
	//	std::cout << i << "," << j << ":" << tfrag << ", " << tssd << std::endl;
	fSSDdTHist->Fill(tfrag,tssd,1./(1+fabs(tfrag-tssd)));
      }
    }

    //    for (size_t i=fFragCounter[0]; i<fFragTimestamps[0].size(); ++i) {
    for (size_t i=nneigh; i<fFragTimestamps[fragId].size(); ++i) {
      tB0 = double(fFragTimestamps[fragId][i] - fT0[fragId]);
      //      std::cout << "tB0 = " << tB0 << std::endl;

      // now loop over Boards
      for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
	auto fragId = fFragId[ifrag];
	if (fragId == 0) continue;

	// find fragment times that are within 10 us
	//	double dtmin=1.e9;
	if (i<fFragTimestamps[fragId].size()) {
	  tB = fFragTimestamps[fragId][i] - fT0[fragId];
	  dtB = tB - tB0;
	  if (abs(dtB) < 10000)
	    fdTvsT[fragId]->Fill(tB0*1.e-9,dtB);

	  for (size_t j=1; j<nneigh; ++j) {
	    if ((i+j) == fFragTimestamps[fragId].size()) break;
	    tB = fFragTimestamps[fragId][i+j] - fT0[fragId];
	    dtB = tB - tB0;
	    if (abs(dtB) < 10000)
	      fdTvsT[fragId]->Fill(tB0*1.e-9,dtB);

	    tB = fFragTimestamps[fragId][i-j] - fT0[fragId];
	    dtB = tB - tB0;
	    if (abs(dtB) < 10000)
	      fdTvsT[fragId]->Fill(tB0*1.e-9,dtB);
	  }
	}
      }

      // do the same for the SSDs
      if (i<fSSDRawDigits.size()) {
	tB = double(fSSDRawDigits[i].first - fSSDT0);
	tB *= fBCOx;
	dtB = tB - tB0;
	double mindtB = abs(dtB);
	//	std::cout << "tB0 = " << tB0 << ", tB = " << tB << ", dtB = " << dtB << std::endl;
	if (abs(dtB) < 10000)
	  fSSDdTvsT->Fill(tB0*1.e-9,dtB);

	for (size_t j=1; j<nneigh; ++j) {
	  if ((i+j) == fSSDRawDigits.size()) break;
	  tB = double(fSSDRawDigits[i+j].first - fSSDT0);
	  tB *= fBCOx;
	  dtB = tB - tB0;
	  if (abs(dtB) < mindtB) mindtB = abs(dtB);
	  //	  std::cout << "tB0 = " << tB0 << ", tB = " << tB << ", dtB = " << dtB << std::endl;
	  if (abs(dtB) < 10000)
	    fSSDdTvsT->Fill(tB0*1.e-9,dtB);

	  tB = double(fSSDRawDigits[i-j].first - fSSDT0);
	  tB *= fBCOx;
	  dtB = tB - tB0;
	  if (abs(dtB) < mindtB) mindtB = abs(dtB);
	  //	  std::cout << "tB0 = " << tB0 << ", tB = " << tB << ", dtB = " << dtB << std::endl;
	  if (abs(dtB) < 10000)
	    fSSDdTvsT->Fill(tB0*1.e-9,dtB);
	}

	//	std::cout << "tB0 = " << tB0 << ", min. dtB = " << mindtB << std::endl;
      }

	//	std::cout << "Frag. " << fragId << " nearest ts is " << nndist << " fragments away, dt = " << dtmin << std::endl;
	/*
	if (!nfill) {
	  std::cout << "No events found nearby!" << std::endl;
	  for (size_t j=i-nneigh; j<i+nneigh; ++j) {
	  tB = double(fFragTimestamps[fragId][j] - fT0[fragId]);
	  dtB = tB - tB0;
	  if (abs(dtB) < 50000) {
	    //	    tratio = tB/tB0;
	    fdTvsT[fragId]->Fill(tB*1.e-9,tB0*1.e-9);
	    ++nfill;
	    //	    std::cout << "Frag " << fragId << " dt: " << dtB << std::endl;
	  }
	}
	//	  std::cout << "tB = " << double(fFragTimestamps[fragId][i] - fT0[fragId]) << std::endl;
      }
	*/

    }

    for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
      auto fragId = fFragId[ifrag];
      if (fragId < 100) { // CAEN boards.  We assume these are all in sync.
	fTWCorr0[fragId] = 0.;
	fTWCorr1[fragId] = 1.;
      }
      else {
	double ymean = fdTvsT[fragId]->GetMean(2);
	double yrms = fdTvsT[fragId]->GetRMS(2);
	//	double xmean = twHist[i]->GetMean(1);
	double xrms = fdTvsT[fragId]->GetRMS(1);
	fdTvsT[fragId]->GetYaxis()->SetRangeUser(ymean-2*yrms,ymean+2*yrms);
	fdTvsT[fragId]->Fit("pol1","","",0.,3.5*xrms);
	TF1* f1 = (TF1*)fdTvsT[fragId]->GetFunction("pol1");
	std::cout << "Board " << fragId << " twcorr slope = " << f1->GetParameter(1) << std::endl;
	fTWCorr0[fragId] = f1->GetParameter(0);
	fTWCorr1[fragId] = f1->GetParameter(1);
      }
    }
  }

  /***************************************************************************/

#define binFactor 10

	template <typename T, typename S> // this type will likely be 'double'
	std::vector<T> calcDifferences(S timestampA, S timestampB, size_t N_compare, std::vector<size_t> skip, T scale) {
		// timestamps from A and B, comparing N_compare stamps and skipping sample indices of skip[index]

		std::vector<T> timeDiffs; timeDiffs.reserve(N_compare*N_compare);

		// sort by descending order so we can sequentially pop off skipped sample's indices
		sort(skip.begin(), skip.end(), std::greater<>());

		size_t _N_compare = N_compare; // local copy of _N_compare to adjust with a given N_skipped samples

		size_t skipMe = N_compare + 1;
		if(!skip.empty()) {
			skipMe = skip.back();
			skip.pop_back();
		}

		for (size_t i=0; i<N_compare; ++i) {
			for (size_t j=0; j<_N_compare; ++j) {
				if(j == skipMe) {
					++_N_compare; // examine more entries
					if(!skip.empty()) {
						skipMe = skip.back();
						skip.pop_back();
					}
					continue;
				}
				T dt = timestampA[i] - timestampB[j];
				if(scale != 1)
					dt = timestampA[i] - timestampB[j]*scale;
				timeDiffs.push_back(dt);
			}
		}
		return timeDiffs;
	}

	template <typename T>
	std::tuple<int,int,double> findOffset(std::vector<T> dt) {
		// returns <offsetBin, N_occurrences, standardDeviation>
		// - NTK
		char hname[256];
		char htitle[256];
		// nbins is rounded
		art::ServiceHandle<art::TFileService> tfs; // for drawing the histograms to file <@@>
		art::TFileDirectory tdir2 = tfs->mkdir("TimeOffsetHistograms",""); // for drawing the histograms to file <@@>

		static int histIndex = 0;
      sprintf(hname,"Time_Offset_%d",histIndex);
      sprintf(htitle,"Fragment Time Differences for pair #%d",histIndex);
		histIndex++;
		const auto [min, max] = std::minmax_element(dt.begin(), dt.end());
		TH1I dtHist(hname, htitle, abs(*max - *min + 1)/binFactor, *min-0.5, *max+0.5);

		for(auto x : dt)
			dtHist.Fill(x);

		tdir2.make<TH1I>(dtHist); // this draws the histogram <@@>

		return { dtHist.GetMaximumBin(), dtHist.GetMaximum(), dtHist.GetStdDev() };
	}

	template <typename T>
	int binToTime(std::vector<T> dt, int bin) {
		const auto [min, max] = std::minmax_element(dt.begin(), dt.end());
		int nbins = abs(*max - *min + 1) / binFactor;
		return  1.0 *  bin / (nbins-1) * (*max - *min) + 1.0 * *min / (nbins-1) * nbins - 1.0 * *max/(nbins-1);
	}

	bool Unpacker::findT0s()	{

		std::unordered_map <
			artdaq::Fragment::fragment_id_t,
			std::vector<int>
		> dtVec;
		std::vector<long unsigned int> ssdTimeStamps; ssdTimeStamps.reserve(fSSDRawDigits.size());
		for(size_t i = 0; i < fSSDRawDigits.size(); ++i) {
			ssdTimeStamps.push_back(fSSDRawDigits[i].first);
		}

		std::cout << "In findT0s()" << std::endl;

      std::array<artdaq::Fragment::fragment_id_t,3> fragIdGrandfather = {fFragId[0]};

		std::cout << "First SSD timestamp: " << ssdTimeStamps[0] << std::endl;
		std::cout << "Second SSD timestamp: " << ssdTimeStamps[1] << std::endl;
		for(int i = 0; i < 2; ++i) { // iterate over sensor types
			size_t samplesGrandfather = 0;
			for (size_t ifrag=0; ifrag < fFragId.size(); ++ifrag) {
				auto fragId = fFragId[ifrag];
				if(fragId >= 10*i && fragId <= 10*(i+1)) continue;
				std::cout << "First timestamp: " << *fFragTimestamps[fragId].begin() << std::endl;
				// remove first event from each sensor <@@>
				fFragTimestamps[fragId].erase(fFragTimestamps[fragId].begin());
				std::cout << "First timestamp after removal: " << *fFragTimestamps[fragId].begin() << std::endl;
				// Zero clocks to first event
				//for(auto& time : fFragTimestamps[fragId]) {
				//	time += -*fFragTimestamps[fragId].begin();
				//}
				// determine grandfather clock
				if(fFragTimestamps[fragId].size() > samplesGrandfather) {
					fragIdGrandfather[i] = fragId;
					samplesGrandfather = fFragTimestamps[fragId].size();
				}
			}
		}
		return false;

		/*******************************************************************************************
		// Synthetic dataset
		dtVec[1].push_back(-100);
		dtVec[1].push_back(100);
		for(int i = 0; i < 50; ++i){
			//dtVec[0].push_back(i);
			//dtVec[2].push_back(i);
			dtVec[1].push_back(0);
		}
		std::vector<size_t> skip; // not skipping anything yet add this criterion later? <@@>
		//dtVec[1] = calcDifferences<double>(dtVec[0], dtVec[2], 50, skip);

		auto minmax = normalizeData(dtVec[1]);
		const auto [bin, N_occur, stdDev] = findOffset(dtVec[1]);
		std::cout << "\nOffset (bin): " << bin
			<< "\nOffset (time): " << binToTime(bin, minmax)
			<< "\nOccurences: " << N_occur
			<< "\nStandard Deviation: " << stdDev << std::endl;
		*******************************************************************************************/

		 // ifrag == 0 is our reference
		 // - NTK
		size_t N_compare=200; // Number of events to compare
		double scale = 1;
		for(int i = 0; i < 2; ++i) { // iterate over sensor types
			auto grandfather =fragIdGrandfather[i];
			for (size_t ifrag=0; ifrag < fFragId.size()+1; ++ifrag) {
				auto fragId = fFragId[ifrag];
				if(fragId == grandfather) continue; // skip if comparing to grandfather clock
				if(grandfather/10 != fragId/10) continue;

				std::vector<size_t> skip; // not skipping anything yet add this criterion later? <@@>
				if(ifrag > fFragId.size()) {
					dtVec[fragIdGrandfather[0]] = calcDifferences<int>(fFragTimestamps[grandfather], ssdTimeStamps, N_compare, skip, scale);
				} else {
					dtVec[fragId] = calcDifferences<int>(fFragTimestamps[grandfather], fFragTimestamps[fragId], N_compare, skip, scale);
				}

				// Find offsetBin
				// { offset, N_occurrences, standardDeviation }
				auto [indexBin, N_occur, stdDev]  = findOffset(dtVec[fragId]);

				auto timeOffset = binToTime(dtVec[fragId],indexBin);
				std::cout << "\nPair (fragId A, fragId B): ("
					<< grandfather << ", " << fragId << ")"
					<< "\nScale : " << scale
					<< "\nOffset (bin): " << indexBin
					<< "\nOffset (time): " << timeOffset
					<< "\nOccurrences: " << N_occur
					<< "\nStandard Deviation: " << stdDev << std::endl;
				// Scale back set of events by the calculated offset
					for(auto& time : fFragTimestamps[fragId])
						time += timeOffset;
			 }
		}

		std::cout << "comparing the grandfathers" << std::endl;;
		std::vector<size_t> skip; // not skipping anything yet add this criterion later? <@@>
		dtVec[fragIdGrandfather[1]] = calcDifferences<int>(fFragTimestamps[fragIdGrandfather[0]], fFragTimestamps[fragIdGrandfather[1]], N_compare, skip, scale);

		// Find offsetBin
		// { offset, N_occurrences, standardDeviation }
		auto [indexBin, N_occur, stdDev]  = findOffset(dtVec[fragIdGrandfather[1]]);

		std::cout << "\nPair (fragId A, fragId B): ("
			<< fragIdGrandfather[0] << ", " << fragIdGrandfather[1] << ")"
			<< "\nScale : " << scale
			<< "\nOffset (bin): " << indexBin
			<< "\nOffset (time): " << binToTime(dtVec[fragIdGrandfather[1]],indexBin)
			<< "\nOccurrences: " << N_occur
			<< "\nStandard Deviation: " << stdDev << std::endl;

	 /* Old stuff (dt method)
	// Do the same for the SSDs
	std::vector<double> dtVec_SSD;
	for (size_t i=0; i<100; ++i) {
		double dt = fSSDRawDigits[i+1].first-fSSDRawDigits[i].first;
		dtVec_SSD.push_back(dt);
	}

    // low find offset indices for each board relative to board 0 so that dt's line up across all CAEN boards.
	bool linedUp = true;
	double dt;
	for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
		auto fragId = fFragId[ifrag];
		iboff[fragId] = 20;
		if (fragId == 0) continue;
		//if (fragId > 10) continue;
		size_t ioff = 0;
		std::cout << "Testing fragID" << fragId << std::endl;
		for ( ; ioff<2*iboff[0]; ++ioff) {
			//      int64_t dtsum=0;
			linedUp = true;
			for (size_t j=0; j<4; ++j) {
				dt = dtVec[fragId][ioff+j] - dtVec[0][iboff[0]+j];
				if (abs(dt) > 100.) {
					linedUp = false;
					break;
				}
				if (linedUp) {
				  std::cout << "Fragment " << fragId << " lines up at offset "
						 << ioff << "(dt = " << dt << ")" << std::endl;
				  iboff[fragId] = ioff;
				  break;
				}
			}
		}
	}

	// Do the same for the SSDs
	size_t iboff_SSD = 20;
	for (size_t ioff=0; ioff<2*iboff[0]; ++ioff) {
		//      int64_t dtsum=0;
		linedUp = true;
		for (size_t j=0; j<4; ++j) {
			dt = dtVec_SSD[ioff+j] - dtVec[0][iboff[0]+j];
			if (abs(dt) > 100.) {
				linedUp = false;
				break;
			}
		}
		if (linedUp) {
		  std::cout << "SSD " << " lines up at offset "
				 << ioff << "(dt = " << dt << ")" << std::endl;
		  iboff_SSD = ioff;
		  break;
		}
	}

	 if (!linedUp) {
		std::cout << "Unable to find where SSD board lines up..." << std::endl;
		return false;
	 }

	 //    std::cout << "here" << std::endl;
	 // we should now try to find a smaller "offset" than 20, but that can wait
	 // for when I have more time, for now we eat the loss of 20ish events
	 for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
		auto fragId = fFragId[ifrag];
		fT0[fragId] = fFragTimestamps[fragId][iboff[fragId]];
		fFragCounter[fragId] = iboff[fragId];
	 }
	 */

	 // this is what we used to do:
	 /*
	 for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
		auto fragId = fFragId[ifrag];
		fT0[fragId] = fFragTimestamps[fragId][0];

		if (fVerbosity)
	std::cout << "T0[" << fragId << "] = " << fT0[fragId] << std::endl;

		if (fFirstSubRunHasExtraTrigger) {
	if (fSubrun == 1) { // skip these extra fragments
	  if ((fragId <= 5) || fragId == 104) {
		 fT0[fragId] = fFragTimestamps[fragId][1];
		 fFragCounter[fragId] += 1;
	  }
	}
		}

		}
	 */
	 std::cout << "Done finding T0s" << std::endl;
	 return true;
  }

  /***************************************************************************/

  bool Unpacker::readNext(art::RunPrincipal* const& ,//inR,
			  art::SubRunPrincipal* const& ,//inSR,
			  art::RunPrincipal* & outR,
			  art::SubRunPrincipal* & outSR,
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
	if (! createSSDDigits()) return false;

      // determine t0s for each board
      if (!findT0s())
	abort();

      if (fMakeTDiffHistos)
	makeTDiffHistos();

      if (fMakeTimeWalkHistos)
	calcTimeWalkCorr();

      fPrevTS = 0;

      fIsFirst = false;
    }

    return true;
    //    art::ServiceHandle<emph::cmap::ChannelMapService> fChannelMap;

    auto tB0ns = fFragTimestamps[0][fFragCounter[0]] - fT0[0];

    if (fCreateArtEvents) {
      std::vector<std::unique_ptr<std::vector<emph::rawdata::WaveForm> > > evtWaveForms;
      for (int idet=0; idet<emph::geo::NDetectors; ++idet)
	evtWaveForms.push_back(std::make_unique<std::vector<emph::rawdata::WaveForm>  >());

      std::vector<std::unique_ptr<std::vector<emph::rawdata::TRB3RawDigit> > > evtTRB3Digits;
      for (int idet=0; idet<emph::geo::NDetectors; ++idet)
	evtTRB3Digits.push_back(std::make_unique<std::vector<emph::rawdata::TRB3RawDigit>  >());

      std::vector<emph::rawdata::SSDRawDigit> evtSSDVec;
      auto evtSSDRawDigits = std::make_unique<std::vector<emph::rawdata::SSDRawDigit> >();

      // find digits for the next event by finding nearest data w.r.t. Board 0 timestamp

      //      uint64_t earliestTimestamp = 0;
      int nObjects = 0;

      artdaq::Fragment::fragment_id_t thisFragId;
      //      uint64_t thisFragTimestamp;
      //      bool isFirstFrag = true;

      //      auto tB0ns = fFragTimestamps[0][fFragCounter[0]] - fT0[0];
      auto tB0d = tB0ns*1.e-9;

      for (size_t ifrag=0; ifrag<fFragId.size(); ++ifrag) {
	auto fragId = fFragId[ifrag];

	auto fragCnt = fFragCounter[fragId];
	// bounds check:
	if (fragCnt == fFragTimestamps[fragId].size()) continue;

	std::cout << "fragCnt B" << fragId << " = " << fragCnt << std::endl;

	double dt = 0.;
	int icoff = 0;
	double mindt = 1.e20;
	if (fragId != 0) { // skip this step for Board 0 data.

	  auto dtCorr = fTWCorr0[fragId] + fTWCorr1[fragId]*tB0d;
	  std::cout << "dtCorr = " << dtCorr << std::endl;
	  // find nearest fragment within +-200 fragments
	  mindt = abs(fFragTimestamps[fragId][fragCnt] - fT0[fragId] - tB0ns - dtCorr);
	  std::cout << "mindt = " << mindt << std::endl;
	  for (size_t ioff=1; ioff<200; ++ioff) {
	    if (fragCnt+ioff < fFragTimestamps[fragId].size()) {
	      dt = fFragTimestamps[fragId][fragCnt+ioff] - fT0[fragId] - tB0ns - dtCorr;
	      std::cout << "dt = " << dt << ", mindt = " << mindt << std::endl;
	      if (abs(dt) < mindt) {
		mindt = abs(dt);
		icoff = ioff;
	      }
	    }
	    if (fragCnt >= ioff) {
	      dt = fFragTimestamps[fragId][fragCnt-ioff] - fT0[fragId] - tB0ns - dtCorr;
	      std::cout << "dt = " << dt << ", mindt = " << mindt << std::endl;
	      if (abs(dt) < mindt) {
		mindt = abs(dt);
		icoff = -ioff;
	      }
	    }
	  }
	}
	std::cout << "Board " << fragId << " min dt = " << mindt
		  << ", icoff = " << icoff << std::endl;
	if (abs(mindt) < 100.) { // associate these data with the same event
	  fFragCounter[fragId] += icoff;
	  auto thisFragCount = fFragCounter[fragId];
	  std::cout << "thisFragCount=" << thisFragCount << std::endl;
	  if (fWaveForms.count(fragId)) {
	    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
	    int boardNum = fragId;
	    emph::cmap::EChannel echan;
	    echan.SetBoardType(boardType);
	    echan.SetBoard(boardNum);
	    for (size_t jfrag=0;
		 jfrag<fWaveForms[fragId][thisFragCount].size(); ++jfrag) {
	      auto & tdig = fWaveForms[fragId][thisFragCount][jfrag];
	      echan.SetChannel(tdig.Channel());
	      if (! fChannelMap->IsValidEChan(echan)) continue;
	      emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	      tdig.SetDetChannel(dchan.Channel());
	      //	      std::cout << echan << " maps to " << dchan << std::endl;
	      evtWaveForms[dchan.DetId()]->push_back(tdig);
	      ++nObjects;
	    }
	  }
	  else if (fReadTRB3Data) {
	    if (fTRB3RawDigits.count(fragId)) {
	      emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
	      emph::cmap::EChannel echan;
	      echan.SetBoardType(boardType);
	      for (size_t jfrag=0; jfrag<fTRB3RawDigits[thisFragId][thisFragCount].size(); ++jfrag) {
		auto & tdig = fTRB3RawDigits[thisFragId][thisFragCount][jfrag];
		int channel = tdig.GetChannel();
		int boardNum = tdig.GetBoardId();
		echan.SetChannel(channel);
		echan.SetBoard(boardNum);
		emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
		if (dchan.DetId() != emph::geo::NDetectors){
		  tdig.SetDetChannel(dchan.Channel());
		  evtTRB3Digits[dchan.DetId()]->push_back(tdig);
		}
		++nObjects;
	      }

	    }
	  }
	  fFragCounter[fragId] += 1;
	}
      } // end loop over artdaq fragments

      // now deal with SSDs
      if ((fEvtCount > 0) && (fSSDEvtIdx > 0)  &&
	  (fSSDEvtIdx < fSSDRawDigits.size()-1) && fReadSSDData) {

	/*
	  int64_t ssdArtDt = earliestTimestamp - fPrevTS - (fSSDRawDigits[fSSDEvtIdx].first - fSSDRawDigits[fSSDEvtIdx-1].first)*150;

	  //	std::cout << "dT (artdaq-ssdots) = " << ssdArtDt << std::endl;

	  if (abs(ssdArtDt) < (int64_t)fTimeWindow) {
	*/
	auto & ssdDigs = fSSDRawDigits[fSSDEvtIdx].second;
	if (!ssdDigs.empty()) {
	  //	    std::cout << "writing out " << ssdDigs.size() << " SSD digits"
	  //		      << std::endl;
	  for (auto ssdDig : ssdDigs) {
	    auto tssdDig(ssdDig);
	    //	      std::cout << tssdDig << std::endl;
	    evtSSDRawDigits->push_back(tssdDig);
	  }
	}
	++fSSDEvtIdx;
	//	evtSSDRawDigits.reset(evtSSDVec);
      }

      // check that we're at the end, if we found nothing else to write out
      if (nObjects == 0) return false;

      // write out waveforms and TDCs to appropriate folders

      art::Timestamp evtTime(fSpillTime.value() + tB0ns);
      outE = fSourceHelper.makeEventPrincipal(fRun, fSubrun, fEvtCount++,
					      evtTime);

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
