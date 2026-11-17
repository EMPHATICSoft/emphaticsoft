#define BOOST_TEST_MODULE test_KAlignParameters
#include <boost/test/unit_test.hpp>

#include <stdexcept>

#include "Alignment/Kalman/KAlignParameters.h"

using namespace emph::alignment;

namespace {
  KAlignParameters MakeThreePlanes()
  {
    KAlignParameters p;
    for (int plane = 0; plane < 3; ++plane) p.AddSensor({1, plane, 0});
    return p;
  }
}

BOOST_AUTO_TEST_CASE(all_free_by_default)
{
  auto p = MakeThreePlanes();
  p.Finalize();
  BOOST_CHECK_EQUAL(p.NFree(), 12);
  BOOST_CHECK_EQUAL(p.Index({1, 0, 0}, DOF::Tn), 0);
  BOOST_CHECK_EQUAL(p.Index({1, 2, 0}, DOF::Rz), 11);
  BOOST_CHECK_EQUAL(p.Index({9, 9, 9}, DOF::Tn), -1);
}

BOOST_AUTO_TEST_CASE(fix_specs_and_wildcards)
{
  auto p = MakeThreePlanes();
  p.Fix("1:0:0:*");
  p.Fix("1:*:*:Tz");
  p.Finalize();
  // Plane 0 entirely fixed; Tz fixed on planes 1 and 2.
  BOOST_CHECK_EQUAL(p.NFree(), 6);
  BOOST_CHECK(p.IsFixed({1, 0, 0}, DOF::Rz));
  BOOST_CHECK(p.IsFixed({1, 2, 0}, DOF::Tz));
  BOOST_CHECK(!p.IsFixed({1, 2, 0}, DOF::Tn));
  BOOST_CHECK_EQUAL(p.Index({1, 0, 0}, DOF::Tn), -1);
  BOOST_CHECK_EQUAL(p.Index({1, 1, 0}, DOF::Tn), 0);
  BOOST_CHECK_EQUAL(p.Index({1, 1, 0}, DOF::Tpar), 1);
  BOOST_CHECK_EQUAL(p.Index({1, 1, 0}, DOF::Tz), -1);
  BOOST_CHECK_EQUAL(p.Index({1, 1, 0}, DOF::Rz), 2);
  const bool keyMatches = p.Key(3).station == 1 && p.Key(3).plane == 2 &&
                          p.Key(3).sensor == 0;
  BOOST_CHECK(keyMatches);
  BOOST_CHECK_EQUAL(static_cast<int>(p.GetDOF(3)), static_cast<int>(DOF::Tn));
  BOOST_CHECK_EQUAL(p.Label(2), "st1_pl1_sn0_Rz");
}

BOOST_AUTO_TEST_CASE(bad_specs_throw)
{
  auto p = MakeThreePlanes();
  BOOST_CHECK_THROW(p.Fix("1:0:0"), std::invalid_argument);
  BOOST_CHECK_THROW(p.Fix("a:0:0:Tn"), std::invalid_argument);
  BOOST_CHECK_THROW(p.Fix("1:0:0:Tx"), std::invalid_argument);
  BOOST_CHECK_THROW(p.Fix("7:0:0:Tn"), std::invalid_argument);
  BOOST_CHECK_THROW(p.Index({1, 0, 0}, DOF::Tn), std::logic_error);
  p.Finalize();
  BOOST_CHECK_THROW(p.Fix("1:0:0:Tn"), std::logic_error);
}

BOOST_AUTO_TEST_CASE(prior_covariance_matches_free_parameters)
{
  auto p = MakeThreePlanes();
  p.SetPriorSigma(DOF::Tpar, 5.0);
  p.SetPriorSigma(DOF::Rz, 0.02);
  p.Fix("1:0:0:*");
  p.Finalize();
  const auto cov = p.PriorCovariance();
  BOOST_REQUIRE_EQUAL(cov.size(), 8u);
  BOOST_CHECK_EQUAL(p.PriorMean().size(), 8u);
  BOOST_CHECK_CLOSE(cov[1][1], 25.0, 1e-12);
  BOOST_CHECK_CLOSE(cov[3][3], 4e-4, 1e-9);
  BOOST_CHECK_EQUAL(cov[0][1], 0.0);
  BOOST_CHECK_THROW(p.SetPriorSigma(DOF::Tn, 0.0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(magnet_field_parameters)
{
  KAlignParameters p;
  p.AddSensor({1, 0, 0});
  p.AddMagnet();
  p.SetPriorSigma(DOF::FieldX, 3.0);
  p.Fix("magnet:FieldY");
  p.Finalize();
  BOOST_CHECK_EQUAL(p.NFree(), 5); // four sensor parameters plus FieldX
  BOOST_CHECK_EQUAL(p.Index(MagnetKey(), DOF::FieldX), 4);
  BOOST_CHECK_EQUAL(p.Index(MagnetKey(), DOF::FieldY), -1);
  BOOST_CHECK_EQUAL(p.Index(MagnetKey(), DOF::Tn), -1);
  BOOST_CHECK_EQUAL(p.Index({1, 0, 0}, DOF::FieldX), -1);
  BOOST_CHECK_EQUAL(p.Label(4), "magnet_FieldX");
  BOOST_CHECK_CLOSE(p.PriorCovariance()[4][4], 9.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(wildcards_do_not_fix_the_magnet)
{
  KAlignParameters p;
  p.AddSensor({1, 0, 0});
  p.AddMagnet();
  p.Fix("*:*:*:*");
  p.Finalize();
  BOOST_CHECK_EQUAL(p.NFree(), 2); // only the magnet parameters remain
  KAlignParameters q;
  q.AddSensor({1, 0, 0});
  BOOST_CHECK_THROW(q.Fix("magnet:FieldX"), std::invalid_argument);
}
