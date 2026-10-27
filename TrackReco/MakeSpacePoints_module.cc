////////////////////////////////////////////////////////////////////////
/// \brief Construct SSD line segments and 3D space points.
////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "DetGeoMap/service/DetGeoMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"
#include "TrackReco/SingleTrackAlgo.h"

namespace emph {

  class MakeSpacePoints : public art::EDProducer {
  public:
    explicit MakeSpacePoints(fhicl::ParameterSet const& pset);
    void beginRun(art::Run& run) override;
    void produce(art::Event& evt) override;

  private:
    std::string fClusterLabel;
    size_t fMaxClust;
    size_t nPlanes = 0;
    size_t nStations = 0;
  };

  MakeSpacePoints::MakeSpacePoints(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
      fClusterLabel(pset.get<std::string>("ClusterLabel")),
      fMaxClust(pset.get<size_t>("MaxClust"))
  {
    produces<std::vector<rb::LineSegment>>();
    produces<std::vector<rb::SpacePoint>>();
  }

  void MakeSpacePoints::beginRun(art::Run&)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    nPlanes = geo->Geo()->NSSDPlanes();
    nStations = geo->Geo()->NSSDStations();
  }

  void MakeSpacePoints::produce(art::Event& evt)
  {
    auto lineSegments = std::make_unique<std::vector<rb::LineSegment>>();
    auto spacePoints = std::make_unique<std::vector<rb::SpacePoint>>();
    art::Handle<std::vector<rb::SSDCluster>> clusters;
    evt.getByLabel(fClusterLabel, clusters);

    if (!clusters || clusters->empty() || clusters->size() >= fMaxClust) {
      evt.put(std::move(lineSegments));
      evt.put(std::move(spacePoints));
      return;
    }

    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
    std::vector<const rb::SSDCluster*> clusterPointers;
    std::vector<rb::LineSegment> allLineSegments;
    std::vector<std::vector<std::vector<const rb::SSDCluster*>>> clusterGroups(nStations);
    std::vector<std::vector<std::vector<const rb::LineSegment*>>> lineGroups(nStations);
    for (size_t station = 0; station < nStations; ++station) {
      clusterGroups[station].resize(nPlanes);
      lineGroups[station].resize(nPlanes);
    }

    for (const auto& cluster : *clusters) {
      if (cluster.AvgStrip() > 640) continue;
      rb::LineSegment line;
      line.SetSSDInfo(cluster.Station(), cluster.Plane(), cluster.Sensor(), cluster.MaxStrip());
      if (!dgm->Map()->SSDClusterToLineSegment(cluster, line)) continue;
      allLineSegments.push_back(line);
      lineSegments->push_back(line);
      clusterPointers.push_back(&cluster);
    }

    for (size_t index = 0; index < clusterPointers.size(); ++index) {
      int station = clusterPointers[index]->Station();
      int plane = clusterPointers[index]->Plane();
      if (station < 0 || static_cast<size_t>(station) >= nStations ||
          plane < 0 || static_cast<size_t>(plane) >= nPlanes) {
        mf::LogWarning("MakeSpacePoints") << "Skipping out-of-range SSD cluster station="
                                          << station << " plane=" << plane;
        continue;
      }
      clusterGroups[station][plane].push_back(clusterPointers[index]);
      lineGroups[station][plane].push_back(&allLineSegments[index]);
    }

    SingleTrackAlgo algo(-1, nStations, nPlanes);
    auto reconstructed = algo.MakeHits(lineGroups, clusterGroups);
    for (auto& spacePoint : reconstructed) {
      if (spacePoint.NLineSegments() == 3) {
        for (size_t line = 0; line < spacePoint.NLineSegments(); ++line) {
          const auto* segment = spacePoint.GetLineSegment(line);
          spacePoint.AddLineSegmentDistance(std::abs(segment->DistanceToPoint(
            spacePoint.Pos()[0], spacePoint.Pos()[1])));
        }
      }
      spacePoints->push_back(spacePoint);
    }

    evt.put(std::move(lineSegments));
    evt.put(std::move(spacePoints));
  }
}

DEFINE_ART_MODULE(emph::MakeSpacePoints)
