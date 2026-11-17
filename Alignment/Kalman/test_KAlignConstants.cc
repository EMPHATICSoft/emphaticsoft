#define BOOST_TEST_MODULE test_KAlignConstants
#include <boost/test/unit_test.hpp>

#include <cmath>

#include "Alignment/Kalman/KAlignConstants.h"

using namespace emph::alignment;

namespace {

  SensorFrame MakeFrame(int plane, double angle)
  {
    SensorFrame f;
    f.key = {1, plane, 0};
    f.u = std::cos(angle);
    f.v = std::sin(angle);
    f.centerX = 3.0 + plane;
    f.centerY = -5.0;
    f.t0 = {{0.4, -0.7, 0.2}};
    return f;
  }

}

BOOST_AUTO_TEST_CASE(composition_matches_direct_transform)
{
  const SensorFrame f = MakeFrame(0, 0.9);
  const double phi0 = 0.013; // existing rotation about z
  const SensorCorrection a{{0.05, -0.02, 0.3, 0.004}};
  const SensorConstants out = ComposeSensor(f, a);

  const double tx = a[0] * f.u - a[1] * f.v;
  const double ty = a[0] * f.v + a[1] * f.u;
  const double c = std::cos(a[3]), s = std::sin(a[3]);
  const double c0 = std::cos(phi0), s0 = std::sin(phi0);

  for (double x : {-50., 0., 12.}) {
    for (double y : {-30., 4., 25.}) {
      // current alignment: x' = R0 x + t0
      const double xa = c0 * x - s0 * y + f.t0[0];
      const double ya = s0 * x + c0 * y + f.t0[1];
      // correction: rotate about the center, then translate
      const double xd = c * (xa - f.centerX) - s * (ya - f.centerY) + f.centerX + tx;
      const double yd = s * (xa - f.centerX) + c * (ya - f.centerY) + f.centerY + ty;
      // new constants: Rz(rz) * R0 * x + t
      const double cn = std::cos(phi0 + out.rz), sn = std::sin(phi0 + out.rz);
      const double xn = cn * x - sn * y + out.t[0];
      const double yn = sn * x + cn * y + out.t[1];
      BOOST_CHECK_SMALL(xd - xn, 1e-12);
      BOOST_CHECK_SMALL(yd - yn, 1e-12);
    }
  }
  BOOST_CHECK_CLOSE(out.t[2], f.t0[2] + a[2], 1e-12);
}

BOOST_AUTO_TEST_CASE(jacobian_matches_finite_difference)
{
  const SensorFrame f = MakeFrame(2, -1.3);
  const SensorCorrection a{{0.05, -0.02, 0.3, 0.004}};
  const auto j = ComposeJacobian(f, a);
  const double h = 1e-6;
  for (int d = 0; d < kNSensorDOF; ++d) {
    SensorCorrection up = a, down = a;
    up[d] += h;
    down[d] -= h;
    const auto cu = ComposeSensor(f, up);
    const auto cd = ComposeSensor(f, down);
    const double numeric[4] = {(cu.t[0] - cd.t[0]) / (2 * h),
                               (cu.t[1] - cd.t[1]) / (2 * h),
                               (cu.t[2] - cd.t[2]) / (2 * h),
                               (cu.rz - cd.rz) / (2 * h)};
    for (int k = 0; k < 4; ++k) BOOST_CHECK_SMALL(j[k][d] - numeric[k], 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(covariance_propagation)
{
  KAlignParameters p;
  p.AddSensor({1, 0, 0});
  p.AddSensor({1, 1, 0});
  p.Fix("1:0:0:Tz");
  p.Fix("1:1:0:*");
  p.Finalize();
  // free: sensor 0 Tn, Tpar, Rz
  BOOST_REQUIRE_EQUAL(p.NFree(), 3);

  const std::vector<SensorFrame> frames = {MakeFrame(0, 0.4), MakeFrame(1, 1.1)};
  const std::vector<double> alpha = {0.05, -0.02, 0.004};
  const std::vector<std::vector<double>> cov = {
    {0.04, 0.01, 0.0002}, {0.01, 0.09, -0.0001}, {0.0002, -0.0001, 1e-4}};

  const auto out = PropagateConstantsCovariance(p, frames, alpha, cov);
  BOOST_REQUIRE_EQUAL(out.size(), 8u);

  // Sensor 1 is entirely fixed, so it has no variance or covariance.
  for (size_t i = 4; i < 8; ++i)
    for (size_t k = 0; k < 8; ++k) {
      BOOST_CHECK_EQUAL(out[i][k], 0.0);
      BOOST_CHECK_EQUAL(out[k][i], 0.0);
    }
  // Tz is fixed on sensor 0.
  BOOST_CHECK_EQUAL(out[2][2], 0.0);

  // rz variance passes straight through; cov(rz, Tn) too, via t_x/t_y rows.
  BOOST_CHECK_CLOSE(out[3][3], 1e-4, 1e-9);
  const auto j = ComposeJacobian(frames[0], {{alpha[0], alpha[1], 0., alpha[2]}});
  const double expected_xx =
    j[0][0] * j[0][0] * 0.04 + j[0][1] * j[0][1] * 0.09 +
    j[0][3] * j[0][3] * 1e-4 + 2 * j[0][0] * j[0][1] * 0.01 +
    2 * j[0][0] * j[0][3] * 0.0002 + 2 * j[0][1] * j[0][3] * -0.0001;
  BOOST_CHECK_CLOSE(out[0][0], expected_xx, 1e-9);
  // symmetric
  for (size_t i = 0; i < 8; ++i)
    for (size_t k = 0; k < 8; ++k) BOOST_CHECK_SMALL(out[i][k] - out[k][i], 1e-15);
}
