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
#include "ChannelMap/service/ChannelMapService.h"
#include "DataQuality/EventQuality.h"
#include "DataQuality/SpillQuality.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "RawData/SSDRawDigit.h"
#include "RecoBase/ARing.h"
#include "RecoBase/BACkovHit.h"
#include "RecoBase/GasCkovHit.h"
#include "RecoBase/SSDCluster.h"
#include "Simulation/SSDHit.h"

// StandardRecord
#include "StandardRecord/StandardRecord.h"

// CAF filler includes
#include "CAFMaker/HeaderFiller.h"
#include "CAFMaker/ARICHFiller.h"
#include "CAFMaker/BACkovFiller.h"
#include "CAFMaker/EventQualFiller.h"
#include "CAFMaker/GasCkovFiller.h"
#include "CAFMaker/LineSegmentFiller.h"
#include "CAFMaker/SpacePointFiller.h"
#include "CAFMaker/ClusterFiller.h"
#include "CAFMaker/SSDHitsFiller.h"
#include "CAFMaker/TrackFiller.h"
#include "CAFMaker/TrackSegmentFiller.h"
#include "CAFMaker/SRTruthFiller.h"

namespace caf {
  /// Module to create Common Analysis Files from ART files
  class CAFMaker : public art::EDProducer {
  public:
    // Allows 'nova --print-description' to work
    using Parameters = art::EDProducer::Table<CAFMakerParams>;

    explicit CAFMaker(const Parameters& params);
    virtual ~CAFMaker();

    void produce(art::Event& evt) noexcept;
    void beginSubRun(art::SubRun &sr) noexcept;

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

    caf::SRHeader fHeader;

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
      // find last . in filename, drop everything after it and append .caf.root
      const size_t dotpos = fCAFFilename.find_last_of('.');
      assert(dotpos != std::string::npos);  // Must have a dot, surely?
      fCAFFilename.resize(dotpos);
      fCAFFilename += fParams.FileExtension();

      std::cout << fCAFFilename << std::endl;
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

  void CAFMaker::beginSubRun(art::SubRun& sr) noexcept {
    HeaderFiller hf;
    hf.fDQLabel = fParams.DataQualLabel();
    hf.Fill(sr, fHeader);
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

    // set event-level header info first
    rec.hdr = fHeader;
    rec.hdr.evt = evt.id().event();

    // TML: Why are we printing this out for every single event?
    //mf::LogInfo("CAFMaker") << "Run #: " << rec.hdr.run;

    if (!fParams.SSDOnly()) {
      // Get ARing info from ARichReco
      ARICHFiller arichf;
      arichf.fLabel = fParams.ARingLabel();
      arichf.Fill(evt,rec);
    
      // Get BACkov info from BACovHitReco
      BACkovFiller backovf; 
      backovf.fLabel = fParams.BACkovHitLabel();
      backovf.Fill(evt,rec);

      // Get GasCkov info from GasCovHitReco
      GasCkovFiller gasckovf; 
      gasckovf.fLabel = fParams.GasCkovHitLabel();
      gasckovf.Fill(evt,rec);

      // Get LineSegments
      LineSegmentFiller linesegf;
      linesegf.fLabel = fParams.LineSegLabel();
      linesegf.Fill(evt,rec);

      // Get SpacePoints
      SpacePointFiller spcptf;
      spcptf.fLabel = fParams.SpacePointLabel();
      spcptf.Fill(evt,rec);

      // Get TrackSegments
      TrackSegmentFiller trksegf;
      trksegf.fLabel = fParams.TrackSegmentLabel();
      trksegf.Fill(evt,rec);


      // Get Tracks
      TrackFiller trkf;
      trkf.fLabelTracks = fParams.TrackLabel();
      trkf.fLabelArichID = fParams.ArichIDLabel();
      trkf.Fill(evt,rec);
    
    }

    // Get SRTruth  
    if (fParams.GetMCTruth()) {	// check for the GetMCTruth configuration parameter,
				// set to "true" if needed

      rec.hdr.ismc = true;

      SRTruthFiller srtruthf;
      srtruthf.GetG4Hits = fParams.GetMCHits();
      srtruthf.fLabel = fParams.SSDHitLabel();
      srtruthf.Fill(evt,rec);
    } // end if statement

 
    // Get EventQuality info from DataQual
    EventQualFiller evtqualf; 
    evtqualf.fLabel = fParams.DataQualLabel();
    evtqualf.Fill(evt,rec);
    
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
