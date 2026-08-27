////////////////////////////////////////////////////////////////////////
/// \brief  Unit tests for ru::RecoUtils geometry functions
///
/// Tests cover getMomentum, findRecoAngle, ClosestApproach, and findLine.
/// findTrackIntersection is omitted here: rb::TrackSegment pulls in canvas.
///
/// These tests exercise only valid inputs, so none of the
/// mf::LogError error branches are triggered — no art/MF initialisation needed.
///
/// Run with:  ctest -R test_recoutils  (from the build directory)
////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE test_recoutils
#include <boost/test/unit_test.hpp>

#include <cmath>
#include "Math/Vector3D.h"   // ROOT::Math::XYZVector
#include "RecoUtils/RecoUtils.h"

using XYZ = ROOT::Math::XYZVector;

// ── getMomentum ──────────────────────────────────────────────────────────────
//
// Formula: p = 64.649 / (theta_rad * 1000)  [GeV/c]

BOOST_AUTO_TEST_SUITE(GetMomentum)

BOOST_AUTO_TEST_CASE(KnownAngleGivesCorrectMomentum)
{
  ru::RecoUtils ru;
  // theta = 1 mrad → p = 64.649 GeV/c
  BOOST_CHECK_CLOSE(ru.getMomentum(0.001), 64.649, 1e-6);
}

BOOST_AUTO_TEST_CASE(LargerAngleGivesLowerMomentum)
{
  ru::RecoUtils ru;
  double p_small = ru.getMomentum(0.005);
  double p_large = ru.getMomentum(0.010);
  BOOST_CHECK_GT(p_small, p_large);
}

BOOST_AUTO_TEST_CASE(InverseRelationship)
{
  ru::RecoUtils ru;
  double theta = 0.002;
  // p * theta * 1000 == 64.649
  BOOST_CHECK_CLOSE(ru.getMomentum(theta) * theta * 1000., 64.649, 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()

// ── findRecoAngle ────────────────────────────────────────────────────────────
//
// Takes four 3D points defining two line segments and returns the angle
// between their direction vectors.

BOOST_AUTO_TEST_SUITE(FindRecoAngle)

BOOST_AUTO_TEST_CASE(ParallelSegmentsReturnZero)
{
  ru::RecoUtils ru;
  // Both segments along +z
  double p1[3] = {0., 0., 0.};
  double p2[3] = {0., 0., 1.};
  double p3[3] = {1., 0., 0.};
  double p4[3] = {1., 0., 1.};
  BOOST_CHECK_SMALL(ru.findRecoAngle(p1, p2, p3, p4), 1e-12);
}

BOOST_AUTO_TEST_CASE(PerpendicularSegmentsReturnHalfPi)
{
  ru::RecoUtils ru;
  // Segment 1 along +z, segment 2 along +x
  double p1[3] = {0., 0., 0.};
  double p2[3] = {0., 0., 1.};
  double p3[3] = {0., 0., 0.};
  double p4[3] = {1., 0., 0.};
  BOOST_CHECK_CLOSE(ru.findRecoAngle(p1, p2, p3, p4), M_PI / 2., 1e-8);
}

BOOST_AUTO_TEST_CASE(AntiParallelSegmentsReturnPi)
{
  ru::RecoUtils ru;
  double p1[3] = {0., 0., 0.};
  double p2[3] = {0., 0., 1.};
  double p3[3] = {0., 0., 1.};
  double p4[3] = {0., 0., 0.};  // opposite direction
  BOOST_CHECK_CLOSE(ru.findRecoAngle(p1, p2, p3, p4), M_PI, 1e-8);
}

BOOST_AUTO_TEST_CASE(FortyFiveDegreesReturnsPiOverFour)
{
  ru::RecoUtils ru;
  double p1[3] = {0., 0., 0.};
  double p2[3] = {0., 0., 1.};
  double p3[3] = {0., 0., 0.};
  double p4[3] = {1., 0., 1.};  // 45° in the xz-plane
  BOOST_CHECK_CLOSE(ru.findRecoAngle(p1, p2, p3, p4), M_PI / 4., 1e-8);
}

BOOST_AUTO_TEST_SUITE_END()

// ── ClosestApproach (SSD mode) ───────────────────────────────────────────────
//
// Finds the midpoint F of the closest-approach pair between two 3D line
// segments. The "SSD" mode requires both parameter values s,t ∈ [0,1].

BOOST_AUTO_TEST_SUITE(ClosestApproach)

BOOST_AUTO_TEST_CASE(PerpendicularSegmentsCrossAtOrigin)
{
  // AB: x-axis from (-1,0,0) to (1,0,0)
  // CD: y-axis from (0,-1,0) to (0,1,0)
  // The segments intersect at (0,0,0) with s=t=0.5.
  ru::RecoUtils ru;
  XYZ A(-1., 0., 0.);
  XYZ B( 1., 0., 0.);
  XYZ C( 0.,-1., 0.);
  XYZ D( 0., 1., 0.);

  double F[3], l1[3], l2[3];
  ru.ClosestApproach(A, B, C, D, F, l1, l2, "SSD", false);

  BOOST_CHECK_SMALL(F[0], 1e-12);
  BOOST_CHECK_SMALL(F[1], 1e-12);
  BOOST_CHECK_SMALL(F[2], 1e-12);
}

BOOST_AUTO_TEST_CASE(IntersectionPointsAreAtOrigin)
{
  ru::RecoUtils ru;
  XYZ A(-1., 0., 0.);
  XYZ B( 1., 0., 0.);
  XYZ C( 0.,-1., 0.);
  XYZ D( 0., 1., 0.);

  double F[3], l1[3], l2[3];
  ru.ClosestApproach(A, B, C, D, F, l1, l2, "SSD", false);

  BOOST_CHECK_SMALL(l1[0], 1e-12);
  BOOST_CHECK_SMALL(l1[1], 1e-12);
  BOOST_CHECK_SMALL(l1[2], 1e-12);

  BOOST_CHECK_SMALL(l2[0], 1e-12);
  BOOST_CHECK_SMALL(l2[1], 1e-12);
  BOOST_CHECK_SMALL(l2[2], 1e-12);
}

BOOST_AUTO_TEST_CASE(SegmentsCrossingAtKnownOffOriginPoint)
{
  // AB along x, centred at x=2: (1,0,0) to (3,0,0)
  // CD along y, centred at y=0 but at x=2: (2,-1,0) to (2,1,0)
  // They should intersect at (2,0,0) → F = (2,0,0).
  ru::RecoUtils ru;
  XYZ A( 1., 0., 0.);
  XYZ B( 3., 0., 0.);
  XYZ C( 2.,-1., 0.);
  XYZ D( 2., 1., 0.);

  double F[3], l1[3], l2[3];
  ru.ClosestApproach(A, B, C, D, F, l1, l2, "SSD", false);

  BOOST_CHECK_CLOSE(F[0], 2.0, 1e-10);
  BOOST_CHECK_SMALL(F[1], 1e-12);
  BOOST_CHECK_SMALL(F[2], 1e-12);
}

BOOST_AUTO_TEST_CASE(SkewSegmentsReturnMidpointOfDOCA)
{
  // Two skew lines: AB along z at x=0,y=0; CD along x at z=1,y=0.
  // Closest approach: l1=(0,0,1), l2=(0,0,1) → F=(0,0,1).
  ru::RecoUtils ru;
  XYZ A(0., 0., 0.);
  XYZ B(0., 0., 2.);
  XYZ C(-1., 0., 1.);
  XYZ D( 1., 0., 1.);

  double F[3], l1[3], l2[3];
  ru.ClosestApproach(A, B, C, D, F, l1, l2, "SSD", false);

  BOOST_CHECK_SMALL(F[0], 1e-10);
  BOOST_CHECK_SMALL(F[1], 1e-10);
  BOOST_CHECK_CLOSE(F[2], 1.0, 1e-10);
}

BOOST_AUTO_TEST_SUITE_END()

// ── findLine ─────────────────────────────────────────────────────────────────
//
// PCA-based best-fit line through a set of 3D points. Returns the line
// endpoints at the z-positions of the first and last input points.

BOOST_AUTO_TEST_SUITE(FindLine)

BOOST_AUTO_TEST_CASE(PointsOnZAxisRecoverEndpoints)
{
  // Four points exactly on z-axis: (0,0,0), (0,0,10), (0,0,20), (0,0,30)
  ru::RecoUtils ru;
  std::vector<std::vector<double>> v = {
    {0., 0.,  0.},
    {0., 0., 10.},
    {0., 0., 20.},
    {0., 0., 30.},
  };

  double lfirst[3], llast[3];
  ru.findLine(v, lfirst, llast);

  // z-coordinates are fixed by definition
  BOOST_CHECK_CLOSE(lfirst[2],  0., 1e-8);
  BOOST_CHECK_CLOSE(llast[2],  30., 1e-8);
  // x and y should be ~0 (all input points at x=y=0)
  BOOST_CHECK_SMALL(lfirst[0], 1e-10);
  BOOST_CHECK_SMALL(lfirst[1], 1e-10);
  BOOST_CHECK_SMALL(llast[0],  1e-10);
  BOOST_CHECK_SMALL(llast[1],  1e-10);
}

BOOST_AUTO_TEST_CASE(DiagonalLineRecoversSlopeInX)
{
  // Points on the line  x = z/10, y = 0  →  slope dx/dz = 0.1
  ru::RecoUtils ru;
  std::vector<std::vector<double>> v = {
    {0.,  0.,  0.},
    {1.,  0., 10.},
    {2.,  0., 20.},
    {3.,  0., 30.},
  };

  double lfirst[3], llast[3];
  ru.findLine(v, lfirst, llast);

  BOOST_CHECK_CLOSE(lfirst[2],  0., 1e-8);
  BOOST_CHECK_CLOSE(llast[2],  30., 1e-8);
  BOOST_CHECK_CLOSE(lfirst[0],  0., 1e-6);
  BOOST_CHECK_CLOSE(llast[0],   3., 1e-6);  // x = slope * z_last
  BOOST_CHECK_SMALL(lfirst[1], 1e-10);
  BOOST_CHECK_SMALL(llast[1],  1e-10);
}

BOOST_AUTO_TEST_CASE(DiagonalLineInYRecoversSlopeInY)
{
  // Points on  y = z/20, x = 0
  ru::RecoUtils ru;
  std::vector<std::vector<double>> v = {
    {0.,  0.,  0.},
    {0.,  0.5, 10.},
    {0.,  1.0, 20.},
    {0.,  1.5, 30.},
  };

  double lfirst[3], llast[3];
  ru.findLine(v, lfirst, llast);

  BOOST_CHECK_SMALL(lfirst[0], 1e-10);
  BOOST_CHECK_SMALL(lfirst[1], 1e-8);   // y_first = 0
  BOOST_CHECK_CLOSE(llast[1], 1.5, 1e-6);
}

BOOST_AUTO_TEST_CASE(MinimumTwoPointsRequired)
{
  // With fewer than 2 points the function should return gracefully (no crash).
  ru::RecoUtils ru;
  std::vector<std::vector<double>> v = {{0., 0., 0.}};  // only 1 point

  double lfirst[3] = {9., 9., 9.};
  double llast[3]  = {9., 9., 9.};
  ru.findLine(v, lfirst, llast);  // should not crash; outputs initialised to 0

  BOOST_CHECK_EQUAL(lfirst[0], 0.);
  BOOST_CHECK_EQUAL(llast[0],  0.);
}

BOOST_AUTO_TEST_SUITE_END()
