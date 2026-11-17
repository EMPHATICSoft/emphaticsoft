#include "Alignment/Kalman/KAlignConstants.h"

#include <cmath>

namespace emph {
namespace alignment {

  SensorConstants ComposeSensor(const SensorFrame& frame,
                                const SensorCorrection& alpha)
  {
    const double tn = alpha[static_cast<int>(DOF::Tn)];
    const double tpar = alpha[static_cast<int>(DOF::Tpar)];
    const double tz = alpha[static_cast<int>(DOF::Tz)];
    const double rz = alpha[static_cast<int>(DOF::Rz)];
    const double c = std::cos(rz), s = std::sin(rz);

    // Translation in the xy plane: along the strip normal and the strip.
    const double Tx = tn * frame.u - tpar * frame.v;
    const double Ty = tn * frame.v + tpar * frame.u;

    const double rx = frame.t0[0] - frame.centerX;
    const double ry = frame.t0[1] - frame.centerY;

    SensorConstants out;
    out.t[0] = c * rx - s * ry + frame.centerX + Tx;
    out.t[1] = s * rx + c * ry + frame.centerY + Ty;
    out.t[2] = frame.t0[2] + tz;
    out.rz = rz;
    return out;
  }

  std::array<std::array<double, kNSensorDOF>, kNSensorDOF>
  ComposeJacobian(const SensorFrame& frame, const SensorCorrection& alpha)
  {
    const double rz = alpha[static_cast<int>(DOF::Rz)];
    const double c = std::cos(rz), s = std::sin(rz);
    const double rx = frame.t0[0] - frame.centerX;
    const double ry = frame.t0[1] - frame.centerY;

    std::array<std::array<double, kNSensorDOF>, kNSensorDOF> j{};
    // x: Tn, Tpar, Tz, Rz
    j[0][0] = frame.u;
    j[0][1] = -frame.v;
    j[0][3] = -s * rx - c * ry;
    // y
    j[1][0] = frame.v;
    j[1][1] = frame.u;
    j[1][3] = c * rx - s * ry;
    // z
    j[2][2] = 1.0;
    // rotation angle
    j[3][3] = 1.0;
    return j;
  }

  SensorCorrection CorrectionOf(const KAlignParameters& parameters,
                                const SensorKey& key,
                                const std::vector<double>& alpha)
  {
    SensorCorrection a{};
    for (int d = 0; d < kNSensorDOF; ++d) {
      const int index = parameters.Index(key, static_cast<DOF>(d));
      if (index >= 0) a[d] = alpha.at(index);
    }
    return a;
  }

  std::vector<std::vector<double>> PropagateConstantsCovariance(
    const KAlignParameters& parameters,
    const std::vector<SensorFrame>& frames,
    const std::vector<double>& alpha,
    const std::vector<std::vector<double>>& covariance)
  {
    const size_t nOut = kNSensorDOF * frames.size();
    const size_t nFree = static_cast<size_t>(parameters.NFree());

    // Linear map M (nOut x nFree).
    std::vector<std::vector<double>> M(nOut, std::vector<double>(nFree, 0.0));
    for (size_t is = 0; is < frames.size(); ++is) {
      const auto a = CorrectionOf(parameters, frames[is].key, alpha);
      const auto j = ComposeJacobian(frames[is], a);
      for (int d = 0; d < kNSensorDOF; ++d) {
        const int index = parameters.Index(frames[is].key, static_cast<DOF>(d));
        if (index < 0) continue;
        for (int k = 0; k < kNSensorDOF; ++k)
          M[kNSensorDOF * is + k][index] = j[k][d];
      }
    }

    // MC = M * C
    std::vector<std::vector<double>> MC(nOut, std::vector<double>(nFree, 0.0));
    for (size_t i = 0; i < nOut; ++i)
      for (size_t k = 0; k < nFree; ++k) {
        if (M[i][k] == 0.0) continue;
        for (size_t j = 0; j < nFree; ++j) MC[i][j] += M[i][k] * covariance[k][j];
      }

    // (M C) M^T
    std::vector<std::vector<double>> out(nOut, std::vector<double>(nOut, 0.0));
    for (size_t i = 0; i < nOut; ++i)
      for (size_t l = 0; l < nOut; ++l) {
        double sum = 0.0;
        for (size_t j = 0; j < nFree; ++j) sum += MC[i][j] * M[l][j];
        out[i][l] = sum;
      }
    return out;
  }

} // namespace alignment
} // namespace emph
