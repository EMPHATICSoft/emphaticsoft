#include "Alignment/Kalman/KAlignFilter.h"

#include <cmath>
#include <stdexcept>

namespace emph {
namespace alignment {

  namespace {

    bool IsFinite(double value)
    {
      return std::isfinite(value);
    }

  } // namespace

  KAlignFilter::KAlignFilter(
    const std::vector<double>& priorMean,
    const std::vector<std::vector<double>>& priorCovariance)
    : fMean(priorMean),
      fCovariance(priorCovariance)
  {
    const std::size_t n = fMean.size();
    if (n == 0 || fCovariance.size() != n)
      throw std::invalid_argument("Alignment prior dimensions must be nonzero and square");

    for (std::size_t i = 0; i < n; ++i) {
      if (fCovariance[i].size() != n || !IsFinite(fMean[i]))
        throw std::invalid_argument("Alignment prior has invalid dimensions or non-finite values");
      for (double value : fCovariance[i]) {
        if (!IsFinite(value))
          throw std::invalid_argument("Alignment prior covariance must be finite");
      }
    }

    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        const double scale = 1.0 + std::abs(fCovariance[i][j]);
        if (std::abs(fCovariance[i][j] - fCovariance[j][i]) > 1.e-12 * scale)
          throw std::invalid_argument("Alignment prior covariance must be symmetric");
      }
    }

    // Require a positive-definite prior; fixed parameters are not included
    // in the filter state.
    std::vector<std::vector<double>> lower(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j <= i; ++j) {
        double value = fCovariance[i][j];
        for (std::size_t k = 0; k < j; ++k)
          value -= lower[i][k] * lower[j][k];
        if (i == j) {
          if (!(value > 0.0))
            throw std::invalid_argument("Alignment prior covariance must be positive definite");
          lower[i][j] = std::sqrt(value);
        }
        else {
          lower[i][j] = value / lower[j][j];
        }
      }
    }
  }

  void KAlignFilter::UpdateIndependentMeasurement(
    const std::vector<double>& jacobian,
    double observation,
    double variance)
  {
    const std::size_t n = fMean.size();
    if (jacobian.size() != n)
      throw std::invalid_argument("Alignment measurement Jacobian has the wrong dimension");
    if (!IsFinite(observation) || !IsFinite(variance) || !(variance > 0.0))
      throw std::invalid_argument("Alignment observation and variance must be finite; variance must be positive");
    for (double value : jacobian) {
      if (!IsFinite(value))
        throw std::invalid_argument("Alignment measurement Jacobian must be finite");
    }

    std::vector<double> covarianceTimesJacobian(n, 0.0);
    double innovation = observation;
    double innovationVariance = variance;
    for (std::size_t i = 0; i < n; ++i) {
      innovation -= jacobian[i] * fMean[i];
      for (std::size_t j = 0; j < n; ++j)
        covarianceTimesJacobian[i] += fCovariance[i][j] * jacobian[j];
    }
    for (std::size_t i = 0; i < n; ++i)
      innovationVariance += jacobian[i] * covarianceTimesJacobian[i];
    if (!IsFinite(innovationVariance) || !(innovationVariance > 0.0))
      throw std::runtime_error("Alignment innovation variance is not positive and finite");

    std::vector<double> gain(n);
    for (std::size_t i = 0; i < n; ++i) {
      gain[i] = covarianceTimesJacobian[i] / innovationVariance;
      fMean[i] += gain[i] * innovation;
    }

    std::vector<std::vector<double>> updatedCovariance(
      n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i) {
      for (std::size_t j = 0; j < n; ++j) {
        // Algebraically expand the scalar Joseph update to avoid cubic work
        // in the number of alignment parameters.
        updatedCovariance[i][j] =
          fCovariance[i][j] -
          gain[i] * covarianceTimesJacobian[j] -
          covarianceTimesJacobian[i] * gain[j] +
          innovationVariance * gain[i] * gain[j];
      }
    }

    for (std::size_t i = 0; i < n; ++i)
      for (std::size_t j = i + 1; j < n; ++j) {
        const double symmetric =
          0.5 * (updatedCovariance[i][j] + updatedCovariance[j][i]);
        updatedCovariance[i][j] = symmetric;
        updatedCovariance[j][i] = symmetric;
      }
    fCovariance.swap(updatedCovariance);
  }

} // namespace alignment
} // namespace emph
