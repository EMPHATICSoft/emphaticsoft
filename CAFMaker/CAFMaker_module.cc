////////////////////////////////////////////////////////////////////////
/// \brief   This module creates Common Analysis Files.
////////////////////////////////////////////////////////////////////////
#include "CAFMaker/CAFMakerParams.h"

// C/C++ includes
#include <algorithm>
#include <cmath>
#include <cxxabi.h>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileDirectory.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "ifdh_art/IFDHService/IFDH_service.h"

// emphaticsoft includes
#include "RawData/SSDRawDigit.h"
#include "RecoBase/ARing.h"
#include "RecoBase/SSDCluster.h"

// StandardRecord
#include "StandardRecord/StandardRecord.h"

// CAF filler includes
#include "CAFMaker/HeaderFiller.h"
#include "CAFMaker/ARICHFiller.h"
#include "CAFMaker/SRTruthFiller.h"
#include "CAFMaker/SSDHitsFiller.h"
#include "CAFMaker/ClusterFiller.h"

namespace caf {
  /// Module to create Common Analysis Files from ART files
  class CAFMaker : public art::EDProducer {
  public:
    // Allows 'nova --print-description' to work
    using Parameters = art::EDProducer::Table<CAFMakerParams>;

    explicit CAFMaker(const Parameters& params);
    virtual ~CAFMaker();

    void produce(art::Event& evt) noexcept;

    void respondToOpenInputFile(const art::FileBlock& fb);

    void beginJob();
    void endJob();

    virtual void endSubRun(art::SubRun& sr);

  protected:
    CAFMakerParams fParams;

    std::string fCAFFilename;

    double      fTotalEvents;

    TFile*      fFile;
    TTree*      fRecTree;
    TH1D*       hEvents;

    void InitializeOutfile();

    /// Equivalent of evt.getByLabel(label, handle) except failedToGet
    /// prints a message and aborts if StrictMode is true.
    template <class T>
    void GetByLabelStrict(const art::Event& evt, const std::string& label,
			  art::Handle<T>& handle) const;
    
  }; // end class

  CAFMaker::CAFMaker(const Parameters& params)
    : EDProducer(params), fParams(params()), fFile(0) {
    // We update this, so have to pull it out of the config
    fCAFFilename = fParams.CAFFilename();

    // Normally CAFMaker is run without an output ART stream, so these go
    // nowhere, but can be occasionally useful for filtering as part of an
    // ART job.
    produces< std::vector< caf::StandardRecord > >();
  }

  //......................................................................
  CAFMaker::~CAFMaker() {}

  //......................................................................
  void CAFMaker::respondToOpenInputFile(const art::FileBlock& fb) {
    if (!fFile) {
      // Filename wasn't set in the FCL, and this is the
      // first file we've seen
      char *temp = new char[fb.fileName().size() + 1];
      std::strcpy(temp, fb.fileName().c_str());
      fCAFFilename = basename(temp);
      const size_t dotpos = fCAFFilename.find('.');
      assert(dotpos != std::string::npos);  // Must have a dot, surely?
      fCAFFilename.resize(dotpos);
      fCAFFilename += fParams.FileExtension();

      InitializeOutfile();
    }
  }

  //......................................................................
  void CAFMaker::beginJob() {
    if (!fCAFFilename.empty()) InitializeOutfile();
  }

  //......................................................................
  void CAFMaker::InitializeOutfile() {
    assert(!fFile);
    assert(!fCAFFilename.empty());

    mf::LogInfo("CAFMaker") << "Output filename is " << fCAFFilename;

    fFile = new TFile(fCAFFilename.c_str(), "RECREATE");
    
    hEvents = new TH1D("TotalEvents", "TotalEvents;; Events", 1, 0, 1);

    fRecTree = new TTree("recTree", "records");

    // Tell the tree it's expecting StandardRecord objects
    StandardRecord* rec = 0;
    fRecTree->Branch("rec", "caf::StandardRecord", &rec);

    fTotalEvents = 0;

  }

  //......................................................................
  template <class T>
  void CAFMaker::GetByLabelStrict(const art::Event& evt, const std::string& label,
				  art::Handle<T>& handle) const {
    evt.getByLabel(label, handle);
    if(!label.empty() && handle.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*handle).name(), 0, 0, 0)
		<< "' found under label '" << label << "'. " << std::endl;
      abort();
    }
  }
  
  //......................................................................
  void CAFMaker::produce(art::Event& evt) noexcept {
    // Normally CAFMaker is run without an output ART stream, so these go
    // nowhere, but can be occasionally useful for filtering as part of
    // an ART job.
    std::unique_ptr< std::vector< caf::StandardRecord > > srcol (
        new std::vector< caf::StandardRecord >);

    fTotalEvents += 1;

    StandardRecord rec;
    StandardRecord* prec = &rec;  // TTree wants a pointer-to-pointer
    fRecTree->SetBranchAddress("rec", &prec);

    // get header info first
    HeaderFiller hf;
    hf.Fill(evt, rec);

    mf::LogInfo("CAFMaker") << "Run #: " << rec.hdr.run;

    // Get ARing info from ARichReco
	
/*
    ARICHFiller arichf;
    arichf.fLabel = fParams.ARingLabel();
    arichf.Fill(evt,rec);

*/   // Get SRTruth  
/*
   if (fParams.GetMCTruth()) {	// check for the GetMCTruth configuration parameter,
				// set to "true" if needed
  	SRTruthFiller srtruthf;
  	srtruthf.Fill(evt,rec);
    } // end if statement
*/
    // Get SSDClust info from SSDReco
    ClusterFiller clustf; ///arich -> cluster
    clustf.fLabel = fParams.SSDClustLabel();
    clustf.Fill(evt,rec);
    
    // Get SSDHits from RawDigits
    SSDHitsFiller ssdhitsf;
    ssdhitsf.fLabel = fParams.SSDRawLabel();
    ssdhitsf.Fill(evt,rec);

    fRecTree->Fill();
    srcol->push_back(rec);

    evt.put(std::move(srcol));

  } // end produce

  //......................................................................
  void CAFMaker::endSubRun(art::SubRun& sr) {
    mf::LogInfo("CAFMaker")
      << "End of Subrun " << sr.subRun();
  }

  //......................................................................
  void CAFMaker::endJob() {
    
    if (fTotalEvents == 0) {
      mf::LogWarning("CAFMaker") << "Making an empty CAF file." << std::endl;
    }

    fFile->Write();

    fFile->cd();
    hEvents->Fill(.5, fTotalEvents);

    hEvents->Write();

    fFile->Write();

  } // end endJob

  DEFINE_ART_MODULE(CAFMaker)

}  // end namespace caf
