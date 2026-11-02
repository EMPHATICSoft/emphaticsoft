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

    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);

  private:

    //fcl parameters
    std::string fTrkSegLabel;
    double      fTrgtZ;
  };

  //.......................................................................
  
  emph::BeamTrackReco::BeamTrackReco(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fTrkSegLabel       (pset.get< std::string >("TrkSegLabel"))
    {
      fTrgtZ = -99999.;
      this->produces< std::vector<rb::Track> >();
      
    }
  
  //......................................................................
  
  void BeamTrackReco::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    fTrgtZ = (emgeo->TargetDSZPos()+emgeo->TargetUSZPos())/2.;

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
              // add linesegments from the spacepoints to the track
              int nSP = ts.NSpacePoints();
              for (int isp=0; isp<nSP; ++isp) { // loop over spacepoints of the track segment
                auto sp = ts.GetSpacePoint(isp);
                int nLS = sp->NLineSegments();
                for (int ils=0; ils<nLS; ++ils) { // loop over line segments that form the spacepoint
                  auto ls = sp->GetLineSegment(ils);
                  beamtrk.Add(rb::LineSegment(*ls));                  
                } // loop over line segments
              } // loop over spacepoints
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
            } // if region 1
          } // loop over track segments
        } // try
        catch (...)
        {
        }
    evt.put(std::move(trackv));
  } // end produce
} // end namespace emph

DEFINE_ART_MODULE(emph::BeamTrackReco)
