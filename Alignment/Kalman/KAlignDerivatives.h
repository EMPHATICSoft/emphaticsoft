#ifndef ALIGNMENT_KALIGNDERIVATIVES_H
#define ALIGNMENT_KALIGNDERIVATIVES_H

#include <array>
#include <vector>

#include "Alignment/Kalman/KAlignParameters.h"

namespace emph {
namespace alignment {

  // Strip orientation and sensor center for one hit. (u, v) is the unit
  // normal of the strip line in the xy plane, as used by KLSMeasurement.
  // The strip direction is t = (-v, u).
  struct HitGeometry {
    double u = 1.0;
    double v = 0.0;
    double centerX = 0.0; // sensor center in the global frame (mm)
    double centerY = 0.0;
  };

  // Track state at the hit plane: position and slopes (dx/dz, dy/dz).
  struct TrackAtPlane {
    double x = 0.0;
    double y = 0.0;
    double tx = 0.0;
    double ty = 0.0;
  };

  // Derivatives of the measurement prediction m = u*x + v*y - d with respect
  // to the corrections of the sensor DOFs, in DOF order (Tn, Tpar, Tz, Rz;
  // the FieldX and FieldY entries are zero because the prediction depends on
  // the field translation only through the track state). The sensor is moved
  // by Tn along the normal, Tpar along the strip and Tz along the beam, and
  // rotated by Rz (counter-clockwise, radians) about its center. Tpar has no first-order effect on a single strip measurement
  // and is constrained by its prior only.
  std::array<double, kNDOF> PredictionDerivatives(const HitGeometry& hit,
                                                  const TrackAtPlane& track);

  // The row of derivatives restricted to the free parameters of `parameters`
  // (length parameters.NFree(); zero for parameters of other sensors).
  std::vector<double> FreeParameterRow(const KAlignParameters& parameters,
                                       const SensorKey& sensor,
                                       const HitGeometry& hit,
                                       const TrackAtPlane& track);

} // namespace alignment
} // namespace emph

#endif
