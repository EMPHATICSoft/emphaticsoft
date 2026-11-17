#define BOOST_TEST_MODULE test_KAlignDerivatives
#include <boost/test/unit_test.hpp>

#include <cmath>

#include "Alignment/Kalman/KAlignDerivatives.h"

using namespace emph::alignment;

namespace {

  // Exact (nonlinear) prediction after moving the sensor: the strip line is
  // rotated by rz about the sensor center, shifted by tn along the original
  // normal and tpar along the original strip direction, and the plane is
  // moved by tz along the beam. The track is a straight line.
  double ExactPrediction(const HitGeometry& hit, double d0,
                         const TrackAtPlane& track, double tn, double tpar,
                         double tz, double rz)
  {
    const double u = hit.u, v = hit.v;
    const double tX = -v, tY = u;
    // Point on the nominal line.
    const double px = d0 * u, py = d0 * v;
    const double c = std::cos(rz), s = std::sin(rz);
    const double rx = px - hit.centerX, ry = py - hit.centerY;
    const double nx = hit.centerX + c * rx - s * ry + tn * u + tpar * tX;
    const double ny = hit.centerY + s * rx + c * ry + tn * v + tpar * tY;
    const double un = c * u - s * v, vn = s * u + c * v;
    const double dNew = un * nx + vn * ny;
    const double x = track.x + track.tx * tz;
    const double y = track.y + track.ty * tz;
    return un * x + vn * y - dNew;
  }

}

BOOST_AUTO_TEST_CASE(derivatives_match_finite_differences)
{
  const double angles[] = {0.0, 0.7, 1.9, -2.4};
  for (double angle : angles) {
    HitGeometry hit;
    hit.u = std::cos(angle);
    hit.v = std::sin(angle);
    hit.centerX = 3.0;
    hit.centerY = -5.0;
    TrackAtPlane track{12.0, 8.0, 0.03, -0.02};
    const double d0 = 4.5;

    const auto analytic = PredictionDerivatives(hit, track);
    const double h = 1e-6;
    for (int dof = 0; dof < kNSensorDOF; ++dof) {
      double up[kNSensorDOF] = {0, 0, 0, 0}, down[kNSensorDOF] = {0, 0, 0, 0};
      up[dof] = h;
      down[dof] = -h;
      const double numeric =
        (ExactPrediction(hit, d0, track, up[0], up[1], up[2], up[3]) -
         ExactPrediction(hit, d0, track, down[0], down[1], down[2], down[3])) /
        (2 * h);
      BOOST_CHECK_SMALL(analytic[dof] - numeric, 1e-6);
    }
    // The prediction depends on the field translations only via the track.
    BOOST_CHECK_EQUAL(analytic[static_cast<int>(DOF::FieldX)], 0.0);
    BOOST_CHECK_EQUAL(analytic[static_cast<int>(DOF::FieldY)], 0.0);
  }
}

BOOST_AUTO_TEST_CASE(free_parameter_row_respects_fixed_and_other_sensors)
{
  KAlignParameters parameters;
  parameters.AddSensor({1, 0, 0});
  parameters.AddSensor({1, 1, 0});
  parameters.Fix("1:0:0:*");
  parameters.Fix("1:1:0:Tz");
  parameters.Finalize();

  HitGeometry hit;
  hit.u = 0.0;
  hit.v = 1.0;
  TrackAtPlane track{2.0, 1.0, 0.1, 0.2};

  const auto fixedRow = FreeParameterRow(parameters, {1, 0, 0}, hit, track);
  BOOST_REQUIRE_EQUAL(fixedRow.size(), 3u);
  for (double value : fixedRow) BOOST_CHECK_EQUAL(value, 0.0);

  const auto row = FreeParameterRow(parameters, {1, 1, 0}, hit, track);
  BOOST_CHECK_EQUAL(row[0], -1.0);                       // Tn
  BOOST_CHECK_EQUAL(row[1], 0.0);                        // Tpar
  BOOST_CHECK_CLOSE(row[2], -track.x, 1e-9);             // Rz: s = -x for v=1
}
