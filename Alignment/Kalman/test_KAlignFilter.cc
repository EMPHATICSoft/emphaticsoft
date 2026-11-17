#define BOOST_TEST_MODULE test_KAlignFilter
#include <boost/test/unit_test.hpp>

#include "Alignment/Kalman/KAlignFilter.h"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace {

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

Matrix inverse(Matrix matrix)
{
  const std::size_t n = matrix.size();
  Matrix result(n, Vector(n, 0.0));
  for (std::size_t i = 0; i < n; ++i) {
    if (matrix[i].size() != n)
      throw std::invalid_argument("Expected square test matrix");
    result[i][i] = 1.0;
  }

  for (std::size_t col = 0; col < n; ++col) {
    std::size_t pivot = col;
    for (std::size_t row = col + 1; row < n; ++row)
      if (std::abs(matrix[row][col]) > std::abs(matrix[pivot][col]))
        pivot = row;
    if (std::abs(matrix[pivot][col]) < 1.e-14)
      throw std::runtime_error("Singular test matrix");
    std::swap(matrix[col], matrix[pivot]);
    std::swap(result[col], result[pivot]);

    const double diagonal = matrix[col][col];
    for (std::size_t j = 0; j < n; ++j) {
      matrix[col][j] /= diagonal;
      result[col][j] /= diagonal;
    }
    for (std::size_t row = 0; row < n; ++row) {
      if (row == col) continue;
      const double scale = matrix[row][col];
      for (std::size_t j = 0; j < n; ++j) {
        matrix[row][j] -= scale * matrix[col][j];
        result[row][j] -= scale * result[col][j];
      }
    }
  }
  return result;
}

Vector multiply(const Matrix& matrix, const Vector& vector)
{
  Vector result(matrix.size(), 0.0);
  for (std::size_t i = 0; i < matrix.size(); ++i)
    for (std::size_t j = 0; j < vector.size(); ++j)
      result[i] += matrix[i][j] * vector[j];
  return result;
}

} // namespace

BOOST_AUTO_TEST_CASE(IndependentUpdatesRetainFullCorrelatedCovariance)
{
  emph::alignment::KAlignFilter filter(
    {0.0, 0.0},
    {{4.0, 1.2}, {1.2, 2.0}});

  filter.UpdateIndependentMeasurement({1.0, 0.0}, 0.8, 0.25);
  filter.UpdateIndependentMeasurement({0.0, 1.0}, -0.3, 0.4);
  filter.UpdateIndependentMeasurement({1.0, 1.0}, 0.1, 0.6);

  const auto& mean = filter.GetMean();
  const auto& covariance = filter.GetCovariance();
  BOOST_CHECK_CLOSE_FRACTION(mean[0], 0.6438223938223938, 1.e-10);
  BOOST_CHECK_CLOSE_FRACTION(mean[1], -0.3221235521235521, 1.e-10);
  BOOST_CHECK_CLOSE_FRACTION(covariance[0][0], 0.1814671814671815, 1.e-10);
  BOOST_CHECK_CLOSE_FRACTION(covariance[0][1], -0.05637065637065638, 1.e-10);
  BOOST_CHECK_CLOSE_FRACTION(covariance[1][0], -0.05637065637065636, 1.e-10);
  BOOST_CHECK_CLOSE_FRACTION(covariance[1][1], 0.2268725868725869, 1.e-10);
}

BOOST_AUTO_TEST_CASE(RejectsInvalidMeasurementVariance)
{
  emph::alignment::KAlignFilter filter({0.0}, {{1.0}});
  BOOST_CHECK_THROW(
    filter.UpdateIndependentMeasurement({1.0}, 0.0, 0.0),
    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(KalmanInnovationsWithProcessNoiseMatchBatchCovariance)
{
  constexpr std::size_t nHits = 4;
  constexpr std::size_t nTracks = 3;
  const double initialStateVariance = 1.7;
  const double processNoiseVariance = 0.35;
  const Vector alphaTrue{0.12, -0.08};
  const Matrix priorCovariance{{0.25, 0.06}, {0.06, 0.36}};
  const Vector priorMean{0.0, 0.0};

  emph::alignment::KAlignFilter sequential(priorMean, priorCovariance);
  Matrix information = inverse(priorCovariance);
  Vector score = multiply(information, priorMean);
  bool sawProcessNoiseInInnovationVariance = false;

  for (std::size_t track = 0; track < nTracks; ++track) {
    Vector alignment0(nHits), alignment1(nHits), measured(nHits), sigma2(nHits);
    Vector trueState(nHits);
    for (std::size_t hit = 0; hit < nHits; ++hit) {
      alignment0[hit] = (hit == 1 || hit == 3) ? 1.0 + 0.1 * track : 0.0;
      alignment1[hit] = (hit == 2 || hit == 3) ? 1.0 - 0.05 * track : 0.0;
      sigma2[hit] = 0.2 + 0.03 * hit;
      const double transition =
        hit == 0 ? 0.0 : 0.25 * std::sin((track + 1.0) * hit);
      trueState[hit] =
        (hit == 0 ? 0.3 * (static_cast<double>(track) - 1.0)
                  : trueState[hit - 1] + transition);
      measured[hit] = trueState[hit] +
        alignment0[hit] * alphaTrue[0] +
        alignment1[hit] * alphaTrue[1] +
        0.04 * std::cos((track + 1.0) * (hit + 1.0));
    }

    double filteredState = 0.0;
    double filteredVariance = initialStateVariance;
    Vector filteredStateDerivative(2, 0.0);

    Matrix trackCovariance(nHits, Vector(nHits, 0.0));
    Matrix design(nHits, Vector(2, 0.0));
    for (std::size_t i = 0; i < nHits; ++i) {
      design[i][0] = alignment0[i];
      design[i][1] = alignment1[i];
      for (std::size_t j = 0; j < nHits; ++j) {
        trackCovariance[i][j] = initialStateVariance +
          std::min(i, j) * processNoiseVariance;
      }
      trackCovariance[i][i] += sigma2[i];
    }

    const Matrix inverseTrackCovariance = inverse(trackCovariance);
    Matrix designT(2, Vector(nHits, 0.0));
    for (std::size_t i = 0; i < nHits; ++i) {
      designT[0][i] = design[i][0];
      designT[1][i] = design[i][1];
    }
    Matrix trackInformation(2, Vector(2, 0.0));
    for (std::size_t i = 0; i < 2; ++i)
      for (std::size_t j = 0; j < 2; ++j)
        for (std::size_t k = 0; k < nHits; ++k)
          for (std::size_t l = 0; l < nHits; ++l)
            trackInformation[i][j] +=
              designT[i][k] * inverseTrackCovariance[k][l] * design[l][j];
    Vector trackScore(2, 0.0);
    for (std::size_t i = 0; i < 2; ++i)
      for (std::size_t k = 0; k < nHits; ++k)
        for (std::size_t l = 0; l < nHits; ++l)
          trackScore[i] +=
            designT[i][k] * inverseTrackCovariance[k][l] * measured[l];
    for (std::size_t i = 0; i < 2; ++i) {
      for (std::size_t j = 0; j < 2; ++j)
        information[i][j] += trackInformation[i][j];
      score[i] += trackScore[i];
    }

    for (std::size_t hit = 0; hit < nHits; ++hit) {
      const double predictedVariance =
        filteredVariance + (hit == 0 ? 0.0 : processNoiseVariance);
      const double innovation = measured[hit] - filteredState;
      const double innovationVariance = predictedVariance + sigma2[hit];
      if (hit > 0 && innovationVariance > sigma2[hit])
        sawProcessNoiseInInnovationVariance = true;

      const Vector jacobian{
        alignment0[hit] + filteredStateDerivative[0],
        alignment1[hit] + filteredStateDerivative[1]
      };
      sequential.UpdateIndependentMeasurement(
        jacobian, innovation, innovationVariance);

      const double gain = predictedVariance / innovationVariance;
      for (std::size_t dof = 0; dof < 2; ++dof)
        filteredStateDerivative[dof] -= gain * jacobian[dof];
      filteredState += gain * innovation;
      filteredVariance =
        (1.0 - gain) * (1.0 - gain) * predictedVariance +
        gain * gain * sigma2[hit];
    }
  }

  const Matrix batchCovariance = inverse(information);
  const Vector batchMean = multiply(batchCovariance, score);
  BOOST_CHECK(sawProcessNoiseInInnovationVariance);
  for (std::size_t i = 0; i < 2; ++i) {
    BOOST_CHECK_SMALL(sequential.GetMean()[i] - batchMean[i], 1.e-10);
    for (std::size_t j = 0; j < 2; ++j)
      BOOST_CHECK_SMALL(sequential.GetCovariance()[i][j] -
                        batchCovariance[i][j], 1.e-10);
  }
  BOOST_CHECK(std::abs(batchCovariance[0][1]) > 1.e-5);
}
