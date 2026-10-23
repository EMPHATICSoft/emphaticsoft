////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct beam tracks
/// \author  $Author: jpaley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// EMPHATICSoft includes
#include "Align/service/AlignService.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RecoBase/SSDCluster.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RecoBase/LineSegment.h"
#include "StandardRecord/SRBaseDefs.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/Track.h"
#include "RecoUtils/RecoUtils.h"
#include "StandardRecord/SRTrackSegment.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class BeamTrackReco : public art::EDProducer {
  public:
    explicit BeamTrackReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~BeamTrackReco() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    void AddSSDLineSegmentsToTrack(rb::Track& trk);

    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

  private:
  
    TTree*      spacepoint;
    int         run,subrun,event;
    int         fEvtNum;

    std::vector<const rb::TrackSegment*> trksegs;
    std::vector<const rb::TrackSegment*> trksegs1;
    std::vector<const rb::TrackSegment*> trksegs2;
    std::vector<const rb::TrackSegment*> trksegs3;
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::TrackSegment> tsv;
    std::vector<const rb::TrackSegment*> tsvcut;

    std::map<std::pair<int, int>, int> clustMap;

    bool        fMakePlots;
    int 	goodclust = 0;
    int         badclust = 0; 
    size_t      nPlanes;
    size_t      nStations;

    //fcl parameters
    bool        fCheckClusters;     //Check clusters for event 
    bool        fCheckTrackSeg;
    std::string fClusterLabel;
    std::string fTrkSegLabel;
    bool        fShortOn;
    int         fPBeamTmp;
    bool        fLessStrict;
    double      fTrgtZ;
  };

  //.......................................................................
  
  emph::BeamTrackReco::BeamTrackReco(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")), 
    fCheckTrackSeg     (pset.get< bool >("CheckTrackSeg")),
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fTrkSegLabel       (pset.get< std::string >("TrkSegLabel")),
    fShortOn           (pset.get< bool >("ShortOn")),
    fPBeamTmp          (pset.get< int >("PBeamTmp")),
    fLessStrict        (pset.get< bool >("LessStrict"))
    {
      fTrgtZ = -99999.;
      this->produces< std::vector<rb::Track> >();
      
    }
  
  //......................................................................
  
//  BeamTrackReco::~BeamTrackReco()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................

  // void BeamTrackReco::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }

  //......................................................................
  
  void BeamTrackReco::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();
    fTrgtZ = (emgeo->TargetDSZPos()+emgeo->TargetUSZPos())/2.;

  }

  //......................................................................
   
  void emph::BeamTrackReco::beginJob()
  {
    std::cerr<<"Starting BeamTrackReco"<<std::endl;

    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");  
  }
 
  //......................................................................

  void emph::BeamTrackReco::AddSSDLineSegmentsToTrack(rb::Track& trk)
  {
    int nTrackSegs = trk.NTrackSegments();
    for (int its=0; its<nTrackSegs; ++its) { // loop over track segments      
      auto ts = trk.GetTrackSegment(its);
      int nSP = ts->NSpacePoints();
      for (int isp=0; isp<nSP; ++isp) { // loop over spacepoints of the track segment
        auto sp = ts->GetSpacePoint(isp);
        int nLS = sp->NLineSegments();
        for (int ils=0; ils<nLS; ++ils) { // loop over line segments that form the spacepoint
          auto ls = sp->GetLineSegment(ils);
          trk.Add(rb::LineSegment(*ls));
        }
      }
    }

  }
  //......................................................................

  void emph::BeamTrackReco::produce(art::Event &evt)
  {

    std::unique_ptr<std::vector<rb::Track>> trackv(new std::vector<rb::Track>);

    auto hastrackseg = evt.getHandle<std::vector<rb::TrackSegment>>(fTrkSegLabel);
    if (!hastrackseg)
    {
      mf::LogError("BeamTrackReco") << "No track segments found in event but CheckTrackSeg set to true!";
      abort();
    }

    art::Handle<std::vector<rb::TrackSegment>> trksegH;

    try
      {
        evt.getByLabel(fTrkSegLabel, trksegH);
        for (size_t idx = 0; idx < trksegH->size(); ++idx)
          {
            auto ts = (*trksegH)[idx];
            if (ts.region == caf::kRegion1) {

              rb::Track beamtrk;
              beamtrk.Add(ts);
              beamtrk.mom = ts.mom;   // SetP(t1.P());
              beamtrk.vtx = ts.vtx;   // SetVtx(t1.Vtx());
            // fill position and momentum projected to the center of the target
              beamtrk.momTrgt = ts.mom;
              double posAtTrgt[3];
              // dz should be positive since we are projecting forward to the target
              double dz = fTrgtZ - ts.pointB.Z();
              posAtTrgt[2] = fTrgtZ;
              posAtTrgt[0] = (ts.mom.X() / ts.mom.Z()) * dz + ts.pointB.X();
              posAtTrgt[1] = (ts.mom.Y() / ts.mom.Z()) * dz + ts.pointB.Y();

              beamtrk.posTrgt.SetCoordinates(posAtTrgt);
              trackv->push_back(beamtrk);

            }
          }
        }
            sectrk.posTrgt.SetCoordinates(posAtTrgt);
            sectrk.chi2 = t2.chi2 + t3.chi2;
            AddSSDLineSegmentsToTrack(sectrk);
            trackv->push_back(sectrk);
          }
        } // clust not empty
      } // try
      catch (...)
      {
      }

    } // want plots
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeSingleTracks)
