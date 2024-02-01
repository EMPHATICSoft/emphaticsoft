///////////////////////////////////////////////////////////////////////
// Class:       SSDUnpacker
// Module Type: input source
// Author:      jpaley@fnal.gov, eflumerf@fnal.gov
// Description: create art::Events using only SSD data
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Persistency/Provenance/RunAuxiliary.h"
#include "canvas/Persistency/Provenance/SubRunAuxiliary.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RawData/WaveForm.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"

#include "RawDataUnpacker/SSDUnpacker.h"
#include "RawDataUnpacker/SSDUnpacker_source.h"

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

  SSDUnpacker::SSDUnpacker(fhicl::ParameterSet const& ps, art::ProductRegistryHelper& help, art::SourceHelper const& pm) :
    fSourceHelper(pm)
  {
    fNEvents    = ps.get<int64_t>("nEvents",-1);
    fVerbosity  = ps.get<int>("verbosity",0);
    
    std::string detStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::SSD));
    help.reconstitutes<std::vector<emph::rawdata::SSDRawDigit>, art::InEvent>("raw",detStr);

    fCurrentFilename = "";
    fIsFirst = true;    
    fEvtCount = 0;
    fSSDEvtIdx = 0;

    // create histogram to record distribution of number of SSD hits/event
    art::ServiceHandle<art::TFileService> tfs;
    fSSDVecSizeHist = tfs->make<TH1I>("hSSDNHits",";Num. Hits",100,0,100);

  }
  
  /***************************************************************************/
  
  void SSDUnpacker::closeCurrentFile()
  {

  }

  /***************************************************************************/
  
  void SSDUnpacker::readFile(std::string const &name,
			     art::FileBlock* & fb)
  {
    fCurrentFilename = name;
    fb = new art::FileBlock(art::FileFormatVersion{1, "RawEvent2022"},
    			    fCurrentFilename);

    std::string fileName = name;
    if (name.find_last_of('/') != std::string::npos) {
      fileName = name.substr(name.find_last_of('/')+1);
    }

    sscanf(fileName.c_str(),"Run%d_%d.dat",&fRun,&fSubrun);
  }
  
  /***************************************************************************/
  
  bool SSDUnpacker::createSSDDigits()
  {
    std::ifstream ssdFile;
    ssdFile.open(fCurrentFilename.c_str()); //fileName);
    if (!ssdFile.is_open()) {
      std::cerr << "Error: cannot open " << fCurrentFilename << std::endl;
      return false;
    }
    auto ssdDigs = SSDUnpack::readSSDHitsFromFileStream(ssdFile,true);
    fSSDRawDigits.push_back(ssdDigs);
    while (!ssdFile.eof()) {
      ssdDigs = SSDUnpack::readSSDHitsFromFileStream(ssdFile,false);
      fSSDRawDigits.push_back(ssdDigs);
    }
    if (fSSDRawDigits.size() > 0) {
      fSSDEvtIdx = 0;
    }
    
    std::cout <<  "Found " << fSSDRawDigits.size() << " SSD events"
	      << std::endl;
    
    ssdFile.close();
    return true;
  }
  
  /***************************************************************************/
  
  bool SSDUnpacker::readNext(art::RunPrincipal* const& ,//inR,
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
    
    if (fIsFirst) {

      // create all of the SSD digits for this spill
      if (! createSSDDigits()) return false;      

      outR = fSourceHelper.makeRunPrincipal(fRun,0); //runAux.beginTime());
      outSR = fSourceHelper.makeSubRunPrincipal(fRun, fSubrun,
						fSubrun*60); //subrunAux.beginTime());
      // initialize channel map
      fChannelMap = new emph::cmap::ChannelMap();
      fRunHistory = new runhist::RunHistory(fRun);
      fChannelMap->LoadMap(fRunHistory->ChanFile());
      
      fIsFirst = false;
    } // end IsFirst

    // check that we're not at the end
    if (fSSDEvtIdx == fSSDRawDigits.size()) return false;

    std::vector<emph::rawdata::SSDRawDigit> evtSSDVec;
    
    // find digits for the next event. 

    auto & ssdDigs = fSSDRawDigits[fSSDEvtIdx].second;	  
    if (!ssdDigs.empty()) {
      for (auto ssdDig : ssdDigs) {
	auto tssdDig(ssdDig);
	//	      std::cout << tssdDig << std::endl;
	evtSSDVec.push_back(tssdDig);
      }
    }
    fSSDVecSizeHist->Fill((int)evtSSDVec.size());

    ++fSSDEvtIdx;
    auto evtSSDRawDigits = std::make_unique<std::vector<emph::rawdata::SSDRawDigit>  >(evtSSDVec);
        
    // write out waveforms and TDCs to appropriate folders
    
    art::Timestamp evtTime(0); // + earliestTimestamp);

    outE = fSourceHelper.makeEventPrincipal(fRun, fSubrun, fEvtCount++,
					    evtTime);

    if (fVerbosity) {
      std::cout << "Event " << fEvtCount << ": " 
		<< "\t" << evtSSDRawDigits->size() << " SSDRawDigits"
		<< std::endl; 
    }
    if (!evtSSDRawDigits->empty()) {
      put_product_in_principal(std::move(evtSSDRawDigits), *outE,"raw","SSD");	  
    }
    
    return true;
  }
    
}
}

DEFINE_ART_INPUT_SOURCE(art::Source<emph::rawdata::SSDUnpacker>)
