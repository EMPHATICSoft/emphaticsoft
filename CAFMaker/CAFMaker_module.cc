////////////////////////////////////////////////////////////////////////
/// \brief   This module creates Common Analysis Files.
////////////////////////////////////////////////////////////////////////
#include "CAFMaker/CAFMakerParams.h"

// C/C++ includes
#include <algorithm>
#include <cmath>
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

#include <IFDH_service.h>

// emphaticsoft includes

// StandardRecord
#include "StandardRecord/SRSpillTruthBranch.h"
#include "StandardRecord/StandardRecord.h"

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

  }; // end class

  CAFMaker::CAFMaker(const Parameters& params)
    : EDProducer(params), fParams(params()), fIsRealData(false), fFile(0) {
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
      char temp[fb.fileName().size() + 1];
      std::strcpy(temp, fb.fileName().c_str());
      fCafFilename = basename(temp);
      const size_t dotpos = fCafFilename.find('.');
      assert(dotpos != std::string::npos);  // Must have a dot, surely?
      fCafFilename.resize(dotpos);
      fCafFilename += fParams.FileExtension();

      InitializeOutfile();
    }
  }

  //......................................................................
  void CAFMaker::beginJob() {
    if (!fCafFilename.empty()) InitializeOutfile();
  }

  //......................................................................
  void CAFMaker::InitializeOutfile() {
    assert(!fFile);
    assert(!fCafFilename.empty());

    hEvents = new TH1D("TotalEvents", "TotalEvents;; Events", 1, 0, 1);

    fRecTree = new TTree("recTree", "records");

    // Tell the tree it's expecting StandardRecord objects
    StandardRecord* rec = 0;
    fRecTree->Branch("rec", "caf::StandardRecord", &rec);

    fTotalEvents = 0;

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

    // Get metadata information for header
    unsigned int run = evt.run();
    unsigned int subrun = evt.subRun();
    unsigned int spillNum = evt.id().event();

    rec.hdr = SRHeader();

    rec.hdr.run    = run;
    rec.hdr.subrun = subrun;
    rec.hdr.evt    = spillNum;

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
    bool EmptyFile = false;
    if (fTotalEvents == 0) {
      mf::LogWarning("CAFMaker") << "Making an empty CAF file." << std::endl;
      EmptyFile = true;
    }

    fFile->Write();

    fFile->cd();
    hEvents->Fill(.5, fTotalEvents);

    hEvents->Write();
    fFile->Write();

  } // end endJob

  DEFINE_ART_MODULE(CAFMaker)

}  // end namespace caf