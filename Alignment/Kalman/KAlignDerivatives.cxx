#include "Alignment/Kalman/KAlignDerivatives.h"

namespace emph {
namespace alignment {

  std::array<double, kNDOF> PredictionDerivatives(const HitGeometry& hit,
                                                  const TrackAtPlane& track)
  {
    // Strip direction and track position along the strip from the center.
    const double tX = -hit.v;
    const double tY = hit.u;
    const double s =
      (track.x - hit.centerX) * tX + (track.y - hit.centerY) * tY;

    std::array<double, kNDOF> d{};
    d[static_cast<int>(DOF::Tn)] = -1.0;
    d[static_cast<int>(DOF::Tpar)] = 0.0;
    d[static_cast<int>(DOF::Tz)] = hit.u * track.tx + hit.v * track.ty;
    d[static_cast<int>(DOF::Rz)] = s;
    return d;
  }

  std::vector<double> FreeParameterRow(const KAlignParameters& parameters,
                                       const SensorKey& sensor,
                                       const HitGeometry& hit,
                                       const TrackAtPlane& track)
  {
    std::vector<double> row(parameters.NFree(), 0.0);
    const auto d = PredictionDerivatives(hit, track);
    for (int i = 0; i < kNDOF; ++i) {
      const int index = parameters.Index(sensor, static_cast<DOF>(i));
      if (index >= 0) row[index] = d[i];
    }
    return row;
  }

} // namespace alignment
} // namespace emph
