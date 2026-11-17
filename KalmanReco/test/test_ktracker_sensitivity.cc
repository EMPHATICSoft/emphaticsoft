////////////////////////////////////////////////////////////////////////
/// \brief Finite-difference check of KTracker alignment sensitivities.
///
/// Straight-line (field-free) tracks are filtered through planes whose
/// lines are displaced along their normals by combinations of a few
/// global parameters. The innovation Jacobians returned by KTracker are
/// compared with central finite differences of the innovations.
////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE test_ktracker_sensitivity
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

#include "KalmanReco/KLSMeasurement.h"
#include "KalmanReco/KState.h"
#include "KalmanReco/KTracker.h"
#include "RecoBase/LineSegment.h"

namespace {

struct Plane {
  double z;
  double nu, nv;     // strip normal, matches KLSMeasurement conventions
  double position;   // nominal line position along the normal (mm)
  std::vector<double> weights; // displacement per unit of each parameter
};

const double kRoot2 = std::sqrt(0.5);

std::vector<Plane> MakePlanes()
{
  return {
    {100., 1., 0., 0.30, {1., 0., 0.}},
    {150., 0., 1., -0.20, {0., 1., 0.}},
    {200., kRoot2, kRoot2, 0.45, {0., 0., 1.}},
    {250., -kRoot2, kRoot2, 0.10, {1., 0.5, 0.}},
    {300., 1., 0., -0.35, {0.3, 0., -0.7}},
    {350., 0., 1., 0.25, {-0.4, 1., 0.2}},
    {400., kRoot2, kRoot2, -0.15, {0., 0.6, 1.}},
  };
}

struct Result {
  std::vector<double> innovations;
  std::vector<double> variances;
  std::vector<std::vector<double>> jacobians;
};

// Analytic stand-in for the magnet: a field with transverse gradients in a
// window between the planes, in kGauss.
void ToyField(const double* pos, double* B)
{
  B[0] = B[1] = B[2] = 0.;
  if (pos[2] < 180. || pos[2] > 320.) return;
  B[1] = 20. * (1. + 0.01 * pos[0] - 0.006 * pos[1]);
  B[0] = 1.0 * (1. - 0.005 * pos[0] + 0.01 * pos[1]);
}

// alpha holds one entry per sensor parameter; when withField is set, the two
// entries after them are the x and y translations of the field.
Result Run(const std::vector<Plane>& planes, const std::vector<double>& alpha,
           bool withDerivatives, bool withField = false)
{
  kalman::KTracker tracker;
  const size_t nSensor = planes.front().weights.size();
  if (withField) {
    tracker.SetFieldFunction(ToyField, 50., 450.);
    tracker.SetRK4Parameters(10000, 1.0);
    tracker.SetFieldOffset(alpha[nSensor], alpha[nSensor + 1]);
    if (withDerivatives) tracker.SetFieldShiftParameters(nSensor, nSensor + 1, 0.5);
  }
  else {
    tracker.SetFieldFree(true);
  }

  std::vector<std::vector<double>> derivatives;
  for (size_t i = 0; i < planes.size(); ++i) {
    const Plane& plane = planes[i];
    double shift = plane.position;
    for (size_t p = 0; p < nSensor; ++p) shift += plane.weights[p] * alpha[p];
    // Line direction is perpendicular to the normal.
    double cx = shift * plane.nu, cy = shift * plane.nv;
    double x0[3] = {cx + 50. * plane.nv, cy - 50. * plane.nu, plane.z};
    double x1[3] = {cx - 50. * plane.nv, cy + 50. * plane.nu, plane.z};
    rb::LineSegment line(x0, x1);
    line.SetSigma(0.01);
    line.SetSSDInfo(i, i, 0, 0);
    kalman::KLSMeasurement meas(line);
    BOOST_REQUIRE_CLOSE(meas.GetU(), plane.nu, 1e-6);
    BOOST_REQUIRE_CLOSE(meas.GetV(), plane.nv, 1e-6);
    tracker.AddMeasurement(meas);
    tracker.AddRadLength(0.01);
    // m = u*x + v*y - d, so a displacement w*alpha of d gives -w.
    std::vector<double> row(alpha.size(), 0.);
    for (size_t p = 0; p < nSensor; ++p) row[p] = -plane.weights[p];
    derivatives.push_back(row);
  }
  if (withDerivatives) tracker.SetMeasurementPredictionDerivatives(derivatives);

  KStateVec par;
  par[0] = 0.1; par[1] = -0.2; par[2] = 0.001; par[3] = 0.002; par[4] = 0.2;
  KStateCov cov;
  for (int i = 0; i < 5; ++i) cov[i][i] = 1.0;
  cov[2][2] = cov[3][3] = 1.e-2;
  cov[4][4] = 1.e-2;
  kalman::KState initial(par, cov, 0.);
  tracker.SetInitialState(initial);

  BOOST_REQUIRE(tracker.RunForwardFilter());
  Result result;
  result.innovations = tracker.GetForwardInnovations();
  result.variances = tracker.GetForwardInnovationVariances();
  result.jacobians = tracker.GetForwardInnovationJacobians();
  return result;
}

} // namespace

BOOST_AUTO_TEST_CASE(innovation_jacobian_matches_finite_difference)
{
  const auto planes = MakePlanes();
  const std::vector<double> alpha0 = {0.02, -0.03, 0.01};
  const Result base = Run(planes, alpha0, true);

  BOOST_REQUIRE_EQUAL(base.innovations.size(), planes.size());
  BOOST_REQUIRE_EQUAL(base.jacobians.size(), planes.size());

  const double h = 1.e-3;
  for (size_t p = 0; p < alpha0.size(); ++p) {
    auto up = alpha0, down = alpha0;
    up[p] += h;
    down[p] -= h;
    const Result rUp = Run(planes, up, false);
    const Result rDown = Run(planes, down, false);
    for (size_t i = 0; i < planes.size(); ++i) {
      double numeric = (rUp.innovations[i] - rDown.innovations[i]) / (2. * h);
      // The analytic form holds the gain fixed, while the finite difference
      // lets it vary through the slope-dependent process noise; the
      // difference is O(1e-5) relative, far below any real derivative error.
      BOOST_CHECK_SMALL(base.jacobians[i][p] - numeric,
                        1.e-4 * (1. + std::abs(numeric)));
    }
  }
}

BOOST_AUTO_TEST_CASE(sensitivities_do_not_change_filter_output)
{
  const auto planes = MakePlanes();
  const std::vector<double> alpha = {0.02, -0.03, 0.01};
  const Result with = Run(planes, alpha, true);
  const Result without = Run(planes, alpha, false);
  for (size_t i = 0; i < planes.size(); ++i) {
    BOOST_CHECK_SMALL(with.innovations[i] - without.innovations[i], 1.e-12);
    BOOST_CHECK_SMALL(with.variances[i] - without.variances[i], 1.e-12);
  }
  BOOST_CHECK(without.jacobians.empty());
}

BOOST_AUTO_TEST_CASE(field_translation_jacobian_matches_finite_difference)
{
  const auto planes = MakePlanes();
  // three sensor parameters, then the field x and y translations
  const std::vector<double> alpha0 = {0.02, -0.03, 0.01, 0.4, -0.3};
  const Result base = Run(planes, alpha0, true, true);
  BOOST_REQUIRE_EQUAL(base.jacobians.size(), planes.size());

  const double h = 0.5;
  double largest = 0.;
  for (size_t p = 3; p < 5; ++p) {
    auto up = alpha0, down = alpha0;
    up[p] += h;
    down[p] -= h;
    const Result rUp = Run(planes, up, false, true);
    const Result rDown = Run(planes, down, false, true);
    for (size_t i = 0; i < planes.size(); ++i) {
      const double numeric = (rUp.innovations[i] - rDown.innovations[i]) / (2. * h);
      largest = std::max(largest, std::abs(numeric));
      BOOST_CHECK_SMALL(base.jacobians[i][p] - numeric,
                        2.e-3 * (1. + std::abs(numeric)));
    }
  }
  // The shift must actually matter, otherwise the check is vacuous.
  BOOST_CHECK_GT(largest, 1.e-3);
}
