////////////////////////////////////////////////////////////////////////
/// \brief Construct regional track segments from persisted 3D space points.
////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "fhiclcpp/ParameterSet.h"

#include "Geometry/service/GeometryService.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "TrackReco/SingleTrackAlgo.h"

namespace emph {

  class MakeTrackSegments : public art::EDProducer {
  public:
    explicit MakeTrackSegments(fhicl::ParameterSet const& pset);
    void beginRun(art::Run& run) override;
    void produce(art::Event& evt) override;

  private:
    std::string fSpacePointLabel;
    double fProjectionTolerance;
    size_t nStations = 0;
    size_t nPlanes = 0;
  };

  MakeTrackSegments::MakeTrackSegments(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
      fSpacePointLabel(pset.get<std::string>("SpacePointLabel")),
      fProjectionTolerance(pset.get<double>("ProjectionTolerance"))
  {
    produces<std::vector<rb::TrackSegment>>();
  }

  void MakeTrackSegments::beginRun(art::Run&)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    nStations = geo->Geo()->NSSDStations();
    nPlanes = geo->Geo()->NSSDPlanes();
  }

  void MakeTrackSegments::produce(art::Event& evt)
  {
    auto trackSegments = std::make_unique<std::vector<rb::TrackSegment>>();
    auto spacePoints = evt.getValidHandle<std::vector<rb::SpacePoint>>(fSpacePointLabel);

    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto geometry = geo->Geo();
    const double targetZ = geometry->GetTarget()->Pos()(2);
    const double magnetUSZ = geometry->MagnetUSZPos();
    const double magnetDSZ = geometry->MagnetDSZPos();

    std::vector<rb::SpacePoint> regions[3];
    for (const auto& spacePoint : *spacePoints) {
      const double z = spacePoint.Pos()[2];
      if (z < targetZ) regions[0].push_back(spacePoint);
      else if (z < magnetUSZ) regions[1].push_back(spacePoint);
      else if (z > magnetDSZ) regions[2].push_back(spacePoint);
    }

    SingleTrackAlgo algo(-1, nStations, nPlanes);
    auto addSegment = [&trackSegments](const rb::TrackSegment& candidate, rb::Region region) {
      rb::TrackSegment segment = candidate;
      segment.region = region;
      trackSegments->push_back(segment);
    };

    for (const auto& candidate : algo.MakeTrackSeg(regions[0])) {
      addSegment(candidate, rb::Region::kRegion1);
    }

    auto sameSpacePoint = [](const rb::SpacePoint& first, const rb::SpacePoint& second) {
      return first.Station() == second.Station() &&
             first.Pos()[0] == second.Pos()[0] &&
             first.Pos()[1] == second.Pos()[1] &&
             first.Pos()[2] == second.Pos()[2];
    };
    auto pairAlreadyUsed = [&sameSpacePoint, &trackSegments](const rb::SpacePoint& first,
                                                               const rb::SpacePoint& second) {
      for (const auto& segment : *trackSegments) {
        bool hasFirst = false;
        bool hasSecond = false;
        for (size_t index = 0; index < segment.NSpacePoints(); ++index) {
          const auto* point = segment.GetSpacePoint(index);
          hasFirst = hasFirst || sameSpacePoint(*point, first);
          hasSecond = hasSecond || sameSpacePoint(*point, second);
        }
        if (hasFirst && hasSecond) return true;
      }
      return false;
    };

    auto makeThreeStationSegments = [&](const std::vector<rb::SpacePoint>& points,
                                         int firstStation,
                                         rb::Region region) {
      std::vector<const rb::SpacePoint*> first;
      std::vector<const rb::SpacePoint*> second;
      std::vector<const rb::SpacePoint*> third;
      for (const auto& point : points) {
        if (point.Station() == firstStation) first.push_back(&point);
        else if (point.Station() == firstStation + 1) second.push_back(&point);
        else if (point.Station() == firstStation + 2) third.push_back(&point);
      }

      for (const auto* firstPoint : first) {
        for (const auto* secondPoint : second) {
          std::vector<rb::SpacePoint> seed{*firstPoint, *secondPoint};
          for (const auto& candidate : algo.MakeTrackSeg(seed)) {
            addSegment(candidate, region);
          }

          const double dz = secondPoint->Pos()[2] - firstPoint->Pos()[2];
          if (std::abs(dz) < 1.e-9) continue;
          for (const auto* thirdPoint : third) {
            const double fraction = (thirdPoint->Pos()[2] - firstPoint->Pos()[2]) / dz;
            const double projectedX = firstPoint->Pos()[0] + fraction *
                                      (secondPoint->Pos()[0] - firstPoint->Pos()[0]);
            const double projectedY = firstPoint->Pos()[1] + fraction *
                                      (secondPoint->Pos()[1] - firstPoint->Pos()[1]);
            const double dx = thirdPoint->Pos()[0] - projectedX;
            const double dy = thirdPoint->Pos()[1] - projectedY;
            if (std::sqrt(dx * dx + dy * dy) > fProjectionTolerance) continue;

            std::vector<rb::SpacePoint> triplet{*firstPoint, *secondPoint, *thirdPoint};
            for (const auto& candidate : algo.MakeTrackSeg(triplet)) {
              if (candidate.NSpacePoints() == 3) addSegment(candidate, region);
            }
            if (!pairAlreadyUsed(*firstPoint, *thirdPoint)) {
              for (const auto& candidate : algo.MakeTrackSeg(
                     std::vector<rb::SpacePoint>{*firstPoint, *thirdPoint})) {
                addSegment(candidate, region);
              }
            }
            if (!pairAlreadyUsed(*secondPoint, *thirdPoint)) {
              for (const auto& candidate : algo.MakeTrackSeg(
                     std::vector<rb::SpacePoint>{*secondPoint, *thirdPoint})) {
                addSegment(candidate, region);
              }
            }
          }
        }
      }
    };

    constexpr int region2FirstStation = 2;
    constexpr int region3FirstStation = 5;
    makeThreeStationSegments(regions[1], region2FirstStation, rb::Region::kRegion2);
    makeThreeStationSegments(regions[2], region3FirstStation, rb::Region::kRegion3);
    evt.put(std::move(trackSegments));
  }
}

DEFINE_ART_MODULE(emph::MakeTrackSegments)
