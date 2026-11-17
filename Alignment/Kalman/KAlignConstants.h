#ifndef ALIGNMENT_KALIGNCONSTANTS_H
#define ALIGNMENT_KALIGNCONSTANTS_H

#include <array>
#include <vector>

#include "Alignment/Kalman/KAlignParameters.h"

namespace emph {
namespace alignment {

  // Geometry of one sensor as it was when the fitted corrections were
  // determined (i.e. including the constants already applied by the
  // AlignService). All positions are in the global frame, in mm.
  struct SensorFrame {
    SensorKey key;
    double u = 1.0;        // strip normal (unit vector in xy)
    double v = 0.0;
    double centerX = 0.0;  // sensor center in the aligned frame
    double centerY = 0.0;
    std::array<double, 3> t0{{0., 0., 0.}}; // current translation constant
  };

  // New constants of one sensor. The corrected transform is
  //   x -> Rz(rz) * (R0 * x + t0 - c) + c + T  =  Rz(rz) * R0 * x + t,
  // with R0 and t0 the current constants, c the sensor center, and T the
  // fitted translation. So the new rotation is Rz(rz) * R0.
  struct SensorConstants {
    std::array<double, 3> t{{0., 0., 0.}};
    double rz = 0.0; // additional rotation about z (rad), left-multiplied
  };

  // Fitted corrections of one sensor in DOF order (Tn, Tpar, Tz, Rz).
  using SensorCorrection = std::array<double, kNSensorDOF>;

  SensorConstants ComposeSensor(const SensorFrame& frame,
                                const SensorCorrection& alpha);

  // d(t_x, t_y, t_z, rz) / d(Tn, Tpar, Tz, Rz); row = output, column = input.
  std::array<std::array<double, kNSensorDOF>, kNSensorDOF>
  ComposeJacobian(const SensorFrame& frame, const SensorCorrection& alpha);

  // Covariance of the new constants of all sensors, four entries per sensor
  // (t_x, t_y, t_z, rz) in the order of `frames`, from the covariance of the
  // free parameters (linear propagation). Fixed parameters contribute no
  // variance. The result is square with 4 * frames.size() rows.
  std::vector<std::vector<double>> PropagateConstantsCovariance(
    const KAlignParameters& parameters,
    const std::vector<SensorFrame>& frames,
    const std::vector<double>& alpha,
    const std::vector<std::vector<double>>& covariance);

  // The correction of one sensor from the free-parameter vector (zero for
  // fixed parameters).
  SensorCorrection CorrectionOf(const KAlignParameters& parameters,
                                const SensorKey& key,
                                const std::vector<double>& alpha);

} // namespace alignment
} // namespace emph

#endif
