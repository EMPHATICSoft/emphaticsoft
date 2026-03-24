////////////////////////////////////////////////////////////////////////
/// \brief  Unit tests for kalman::State and kalman::Propagator
///
/// Pure-math layer: no art services, no geometry, no magnetic-field service.
/// Dependencies: KalmanReco (ROOT::Matrix for SVector/SMatrix).
///
/// Run with:  ctest -R test_propagator  (from the build directory)
////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE test_propagator
#include <boost/test/unit_test.hpp>

#include <cmath>
#include "KalmanReco/KState.h"
#include "KalmanReco/Propagator.h"

// ── helpers ─────────────────────────────────────────────────────────────────

static KPar zeroPar()
{
  KPar p;
  for (int i = 0; i < 5; ++i) p[i] = 0.;
  return p;
}

static K5x5 zeroCov()
{
  K5x5 c;
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j)
      c[i][j] = 0.;
  return c;
}

// ── kalman::State ────────────────────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(StateTests)

BOOST_AUTO_TEST_CASE(DefaultConstructorZeroesAll)
{
  kalman::State s;
  BOOST_CHECK_EQUAL(s.GetZ(), 0.);
  for (int i = 0; i < 5; ++i)
    BOOST_CHECK_EQUAL(s.GetPar(i), 0.);
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j)
      BOOST_CHECK_EQUAL(s.GetCov(i, j), 0.);
}

BOOST_AUTO_TEST_CASE(TwoArgConstructorSetsZAndPar)
{
  KPar par = zeroPar();
  par[0] = 1.5;   // x
  par[2] = 0.1;   // tx
  par[4] = 0.125; // q/p  (8 GeV/c, positive)

  kalman::State s(100.0, par);

  BOOST_CHECK_EQUAL(s.GetZ(), 100.0);
  BOOST_CHECK_EQUAL(s.GetPar(0), 1.5);
  BOOST_CHECK_EQUAL(s.GetPar(2), 0.1);
  BOOST_CHECK_EQUAL(s.GetPar(4), 0.125);
  // Covariance was not supplied — must be zero
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j)
      BOOST_CHECK_EQUAL(s.GetCov(i, j), 0.);
}

BOOST_AUTO_TEST_CASE(ThreeArgConstructorStoresCov)
{
  KPar par = zeroPar();
  K5x5 cov = zeroCov();
  cov[0][0] = 1.0e-4;
  cov[1][1] = 2.0e-4;
  cov[2][2] = 5.0e-6;

  kalman::State s(50.0, par, cov);

  BOOST_CHECK_EQUAL(s.GetCov(0, 0), 1.0e-4);
  BOOST_CHECK_EQUAL(s.GetCov(1, 1), 2.0e-4);
  BOOST_CHECK_EQUAL(s.GetCov(2, 2), 5.0e-6);
  BOOST_CHECK_EQUAL(s.GetCov(0, 1), 0.);
}

BOOST_AUTO_TEST_CASE(AssignmentCopiesZParCov)
{
  KPar par = zeroPar();
  par[0] = 3.0;
  K5x5 cov = zeroCov();
  cov[3][3] = 7.0e-5;

  kalman::State s1(200.0, par, cov);
  kalman::State s2;
  s2 = s1;

  BOOST_CHECK_EQUAL(s2.GetZ(), 200.0);
  BOOST_CHECK_EQUAL(s2.GetPar(0), 3.0);
  BOOST_CHECK_EQUAL(s2.GetCov(3, 3), 7.0e-5);
}

BOOST_AUTO_TEST_CASE(SettersOverwriteValues)
{
  kalman::State s;

  KPar par = zeroPar();
  par[1] = 9.9;
  s.SetPar(par);
  BOOST_CHECK_EQUAL(s.GetPar(1), 9.9);

  K5x5 cov = zeroCov();
  cov[4][4] = 3.3e-3;
  s.SetCov(cov);
  BOOST_CHECK_EQUAL(s.GetCov(4, 4), 3.3e-3);

  s.SetZ(777.0);
  BOOST_CHECK_EQUAL(s.GetZ(), 777.0);
}

BOOST_AUTO_TEST_SUITE_END()

// ── kalman::Propagator::Extrapolate ─────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(PropagatorExtrapolate)

BOOST_AUTO_TEST_CASE(ZeroFieldZeroSlopeStraightLine)
{
  // tx = ty = 0, B = 0 → x and y must not change; z advances by st.
  KPar par = zeroPar();
  par[0] = 5.0;    // x
  par[1] = 3.0;    // y
  par[4] = 0.125;  // q/p
  kalman::State s(500.0, par);

  double B[3] = {0., 0., 0.};
  double st   = 50.0;

  kalman::Propagator prop;
  double ds = prop.Extrapolate(s, st, B);

  BOOST_CHECK_CLOSE(s.GetPar(0), 5.0,   1e-10);
  BOOST_CHECK_CLOSE(s.GetPar(1), 3.0,   1e-10);
  BOOST_CHECK_CLOSE(s.GetPar(2), 0.0,   1e-10);  // tx unchanged
  BOOST_CHECK_CLOSE(s.GetPar(3), 0.0,   1e-10);  // ty unchanged
  BOOST_CHECK_CLOSE(s.GetPar(4), 0.125, 1e-10);  // q/p unchanged
  BOOST_CHECK_CLOSE(s.GetZ(), 550.0,    1e-10);

  // Pathlength for zero slope = st * sqrt(1 + 0 + 0) = st
  BOOST_CHECK_CLOSE(ds, st, 1e-10);
}

BOOST_AUTO_TEST_CASE(ZeroFieldNonzeroSlopeAdvancesX)
{
  // tx = 0.1, ty = 0, B = 0 → x should advance by tx*st.
  KPar par = zeroPar();
  par[2] = 0.1;   // tx
  par[4] = 0.125;
  kalman::State s(0.0, par);

  double B[3] = {0., 0., 0.};
  double st   = 100.0;

  kalman::Propagator prop;
  prop.Extrapolate(s, st, B);

  BOOST_CHECK_CLOSE(s.GetPar(0), 0.1 * 100.0, 1e-10);
  BOOST_CHECK_SMALL(s.GetPar(1), 1e-12);
  BOOST_CHECK_CLOSE(s.GetPar(2), 0.1, 1e-10);  // slope unchanged (B=0)
  BOOST_CHECK_CLOSE(s.GetZ(), 100.0, 1e-10);
}

BOOST_AUTO_TEST_CASE(ZeroFieldNonzeroBothSlopes)
{
  // tx = 0.05, ty = -0.03 — both advance independently
  KPar par = zeroPar();
  par[2] = 0.05;
  par[3] = -0.03;
  par[4] = 0.08;   // q/p ~ 12.5 GeV/c
  kalman::State s(0.0, par);

  double B[3] = {0., 0., 0.};
  double st   = 200.0;

  kalman::Propagator prop;
  prop.Extrapolate(s, st, B);

  BOOST_CHECK_CLOSE(s.GetPar(0),  0.05 * 200.0, 1e-10);
  BOOST_CHECK_CLOSE(s.GetPar(1), -0.03 * 200.0, 1e-10);
  BOOST_CHECK_CLOSE(s.GetPar(2),  0.05, 1e-10);
  BOOST_CHECK_CLOSE(s.GetPar(3), -0.03, 1e-10);
}

BOOST_AUTO_TEST_CASE(ZeroFieldPathlengthWithSlope)
{
  // ds = st * sqrt(1 + tx^2 + ty^2)
  KPar par = zeroPar();
  par[2] = 0.1;   // tx
  par[3] = 0.1;   // ty
  par[4] = 1.0;
  kalman::State s(0.0, par);

  double B[3] = {0., 0., 0.};
  double st   = 100.0;
  kalman::Propagator prop;
  double ds = prop.Extrapolate(s, st, B);

  // For zero-B straight line the average slope equals the initial slope
  double expected = st * std::sqrt(1. + 0.01 + 0.01);
  BOOST_CHECK_CLOSE(ds, expected, 1e-4);  // 0.01% — numerical from averaging
}

BOOST_AUTO_TEST_CASE(NonzeroByFieldDeflectsTrackInNegX)
{
  // Positive charge (+q/p > 0) moving in +z in By > 0:
  //   F = q v×B = q(+ẑ)×(+ŷ B) = q B(-x̂)  →  tx becomes negative.
  //
  // Use a short 1 mm step so higher-order terms are negligible (< 0.05%).
  KPar par = zeroPar();
  par[4] = 0.125;   // q/p = 1/8 GeV⁻¹  (8 GeV/c, +ve)
  kalman::State s(0.0, par);

  double B[3] = {0., 5.0, 0.};  // By = 5 kG (typical EMPHATIC magnet)
  double st   = 1.0;             // 1 mm — higher-order corrections < 0.05%

  kalman::Propagator prop;
  prop.Extrapolate(s, st, B);

  // First-order analytical: tx_new = h * dtx1 * st
  // where h = kappa * qdp, dtx1 = -(1+0)*By = -By
  constexpr double kappa = 2.997925e-4;
  double h       = kappa * 0.125;
  double tx_1st  = h * (-5.0) * st;

  BOOST_CHECK_LT(s.GetPar(2), 0.);           // direction: definitely negative
  BOOST_CHECK_CLOSE(s.GetPar(2), tx_1st, 0.05);  // within 0.05%
}

BOOST_AUTO_TEST_CASE(QdpSignFlipsDeflectionDirection)
{
  // Negative charge (qdp < 0) should deflect in +x (opposite to positive).
  KPar par = zeroPar();
  par[4] = -0.125;   // q/p negative
  kalman::State s(0.0, par);

  double B[3] = {0., 5.0, 0.};
  double st   = 1.0;

  kalman::Propagator prop;
  prop.Extrapolate(s, st, B);

  BOOST_CHECK_GT(s.GetPar(2), 0.);  // deflected in +x (opposite sign)
}

BOOST_AUTO_TEST_CASE(QdpUnchangedAfterPropagation)
{
  // q/p is a constant of motion (no energy loss modelled here)
  KPar par = zeroPar();
  par[4] = 0.125;
  kalman::State s(0.0, par);

  double B[3] = {0., 5.0, 0.};
  double st   = 100.0;

  kalman::Propagator prop;
  prop.Extrapolate(s, st, B);

  BOOST_CHECK_CLOSE(s.GetPar(4), 0.125, 1e-10);
}

BOOST_AUTO_TEST_CASE(ZeroFieldJacobianLeavesInitialCovUnchanged)
{
  // With B=0 and tx=ty=0, the Jacobian is the identity → cov unchanged.
  KPar par = zeroPar();
  par[4] = 0.125;
  K5x5 cov = zeroCov();
  cov[0][0] = 1.0e-4;
  cov[1][1] = 2.0e-4;
  cov[2][2] = 5.0e-6;
  cov[3][3] = 5.0e-6;
  cov[4][4] = 1.0e-6;

  kalman::State s(0.0, par, cov);

  double B[3] = {0., 0., 0.};
  kalman::Propagator prop;
  prop.Extrapolate(s, 50.0, B);

  // J = identity when B=0 and tx=ty=0; C' = J C J^T = C
  BOOST_CHECK_CLOSE(s.GetCov(0, 0), 1.0e-4, 1e-8);
  BOOST_CHECK_CLOSE(s.GetCov(1, 1), 2.0e-4, 1e-8);
  BOOST_CHECK_CLOSE(s.GetCov(2, 2), 5.0e-6, 1e-8);
}

BOOST_AUTO_TEST_SUITE_END()

// ── kalman::Propagator::AddNoise ─────────────────────────────────────────────

BOOST_AUTO_TEST_SUITE(PropagatorAddNoise)

BOOST_AUTO_TEST_CASE(ZeroCovarianceGrowsAfterNoise)
{
  // Starting from zero covariance, noise must increase diagonal tx/ty elements.
  KPar par = zeroPar();
  par[4] = 0.125;
  K5x5 cov = zeroCov();
  kalman::State s(100.0, par, cov);

  kalman::Propagator prop;
  prop.AddNoise(s, 10.0, 0.01);  // 10 mm step, 1% X/X0

  BOOST_CHECK_GT(s.GetCov(2, 2), 0.);  // Var(tx) > 0
  BOOST_CHECK_GT(s.GetCov(3, 3), 0.);  // Var(ty) > 0
}

BOOST_AUTO_TEST_CASE(PositionCovarianceGrowsWithNoise)
{
  // C[0][0] and C[1][1] also get contributions proportional to dz^2.
  KPar par = zeroPar();
  par[4] = 0.125;
  K5x5 cov = zeroCov();
  kalman::State s(100.0, par, cov);

  kalman::Propagator prop;
  prop.AddNoise(s, 10.0, 0.01);

  BOOST_CHECK_GT(s.GetCov(0, 0), 0.);  // Var(x) > 0
  BOOST_CHECK_GT(s.GetCov(1, 1), 0.);  // Var(y) > 0
}

BOOST_AUTO_TEST_CASE(CrossCovarianceNonzeroWithSlopedTrack)
{
  // With tx != 0, ty != 0, the mixed term C(tx,ty) must be non-zero.
  KPar par = zeroPar();
  par[2] = 0.1;    // tx
  par[3] = 0.05;   // ty
  par[4] = 0.125;
  K5x5 cov = zeroCov();
  kalman::State s(100.0, par, cov);

  kalman::Propagator prop;
  prop.AddNoise(s, 10.0, 0.01);

  BOOST_CHECK_NE(s.GetCov(2, 3), 0.);  // C(tx, ty) non-zero
}

BOOST_AUTO_TEST_CASE(NoiseMagnitudeScalesWithRadiationLength)
{
  // Thicker material → more scattering → larger covariance growth.
  KPar par = zeroPar();
  par[4] = 0.125;
  double dz = 10.0;

  K5x5 thinCov = zeroCov();
  kalman::State thin(0.0, par, thinCov);
  kalman::Propagator prop;
  prop.AddNoise(thin, dz, 0.005);   // 0.5% X/X0

  K5x5 thickCov = zeroCov();
  kalman::State thick(0.0, par, thickCov);
  prop.AddNoise(thick, dz, 0.050);  // 5% X/X0

  BOOST_CHECK_GT(thick.GetCov(2, 2), thin.GetCov(2, 2));
  BOOST_CHECK_GT(thick.GetCov(3, 3), thin.GetCov(3, 3));
}

BOOST_AUTO_TEST_SUITE_END()
