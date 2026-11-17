////////////////////////////////////////////////////////////////////////
/// \brief Toy linear-Gaussian test for recursive alignment updates.
///
/// Each track has two local line parameters and two global alignment
/// parameters. The test compares sequential updates on independent
/// residual contrasts with the batch solution after eliminating the
/// local track parameters by a Schur complement.
////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE test_alignment_linear_algebra
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace {

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

Matrix zeros(std::size_t rows, std::size_t cols)
{
  return Matrix(rows, Vector(cols, 0.0));
}

Matrix identity(std::size_t n)
{
  Matrix result = zeros(n, n);
  for (std::size_t i = 0; i < n; ++i) result[i][i] = 1.0;
  return result;
}

Matrix transpose(const Matrix& a)
{
  Matrix result = zeros(a.front().size(), a.size());
  for (std::size_t i = 0; i < a.size(); ++i)
    for (std::size_t j = 0; j < a[i].size(); ++j)
      result[j][i] = a[i][j];
  return result;
}

Matrix multiply(const Matrix& a, const Matrix& b)
{
  if (a.empty() || b.empty() || a.front().size() != b.size())
    throw std::runtime_error("Invalid matrix dimensions in toy alignment test");
  Matrix result = zeros(a.size(), b.front().size());
  for (std::size_t i = 0; i < a.size(); ++i)
    for (std::size_t k = 0; k < b.size(); ++k)
      for (std::size_t j = 0; j < b[k].size(); ++j)
        result[i][j] += a[i][k] * b[k][j];
  return result;
}

Vector multiply(const Matrix& a, const Vector& x)
{
  if (a.empty() || a.front().size() != x.size())
    throw std::runtime_error("Invalid matrix/vector dimensions in toy alignment test");
  Vector result(a.size(), 0.0);
  for (std::size_t i = 0; i < a.size(); ++i)
    for (std::size_t j = 0; j < x.size(); ++j)
      result[i] += a[i][j] * x[j];
  return result;
}

Matrix add(const Matrix& a, const Matrix& b, double bScale = 1.0)
{
  if (a.size() != b.size())
    throw std::runtime_error("Invalid matrix dimensions in toy alignment test");
  Matrix result = a;
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (a[i].size() != b[i].size())
      throw std::runtime_error("Invalid matrix dimensions in toy alignment test");
    for (std::size_t j = 0; j < a[i].size(); ++j)
      result[i][j] += bScale * b[i][j];
  }
  return result;
}

double dot(const Vector& a, const Vector& b)
{
  if (a.size() != b.size())
    throw std::runtime_error("Invalid vector dimensions in toy alignment test");
  double result = 0.0;
  for (std::size_t i = 0; i < a.size(); ++i) result += a[i] * b[i];
  return result;
}

Matrix inverse(Matrix a)
{
  const std::size_t n = a.size();
  if (n == 0 || a.front().size() != n)
    throw std::runtime_error("Expected square matrix in toy alignment test");
  Matrix inv = identity(n);

  for (std::size_t col = 0; col < n; ++col) {
    std::size_t pivot = col;
    for (std::size_t row = col + 1; row < n; ++row)
      if (std::abs(a[row][col]) > std::abs(a[pivot][col])) pivot = row;
    if (std::abs(a[pivot][col]) < 1.e-14)
      throw std::runtime_error("Singular matrix in toy alignment test");
    std::swap(a[col], a[pivot]);
    std::swap(inv[col], inv[pivot]);

    const double scale = a[col][col];
    for (std::size_t j = 0; j < n; ++j) {
      a[col][j] /= scale;
      inv[col][j] /= scale;
    }

    for (std::size_t row = 0; row < n; ++row) {
      if (row == col) continue;
      const double factor = a[row][col];
      for (std::size_t j = 0; j < n; ++j) {
        a[row][j] -= factor * a[col][j];
        inv[row][j] -= factor * inv[col][j];
      }
    }
  }
  return inv;
}

std::size_t numericalRank(Matrix a)
{
  const std::size_t rows = a.size();
  const std::size_t cols = a.front().size();
  double scale = 0.0;
  for (const auto& row : a)
    for (double value : row) scale = std::max(scale, std::abs(value));
  const double tolerance = std::max(1.0, scale) * 1.e-10;

  std::size_t rank = 0;
  for (std::size_t col = 0; col < cols && rank < rows; ++col) {
    std::size_t pivot = rank;
    for (std::size_t row = rank + 1; row < rows; ++row)
      if (std::abs(a[row][col]) > std::abs(a[pivot][col])) pivot = row;
    if (std::abs(a[pivot][col]) <= tolerance) continue;
    std::swap(a[rank], a[pivot]);
    for (std::size_t row = rank + 1; row < rows; ++row) {
      const double factor = a[row][col] / a[rank][col];
      for (std::size_t j = col; j < cols; ++j)
        a[row][j] -= factor * a[rank][j];
    }
    ++rank;
  }
  return rank;
}

struct Track {
  Matrix local;
  Matrix alignment;
  Vector sigma;
  Vector measured;
};

std::vector<Track> makeTracks()
{
  constexpr std::size_t nHits = 6;
  const double z[nHits] = {-2., -1., 0., 1., 2., 3.};
  const Vector alphaTrue{0.22, -0.13};
  std::vector<Track> tracks;

  for (int itrack = 0; itrack < 4; ++itrack) {
    Track track{zeros(nHits, 2), zeros(nHits, 2),
                Vector(nHits), Vector(nHits)};
    const Vector localTrue{1.0 + 0.4 * itrack, 0.015 * (itrack - 1)};
    for (std::size_t hit = 0; hit < nHits; ++hit) {
      track.local[hit][0] = 1.0;
      track.local[hit][1] = z[hit];
      track.sigma[hit] = 0.7 + 0.07 * hit + 0.02 * itrack;
      const int sensor = hit % 3;
      if (sensor == 1)
        track.alignment[hit][0] = 1.0 + 0.03 * itrack * (hit + 1);
      if (sensor == 2)
        track.alignment[hit][1] = 1.0 - 0.02 * itrack * (hit + 1);

      const double noise = track.sigma[hit] *
        0.12 * std::sin((itrack + 1) * (hit + 1) * 0.7);
      track.measured[hit] =
        track.local[hit][0] * localTrue[0] +
        track.local[hit][1] * localTrue[1] +
        track.alignment[hit][0] * alphaTrue[0] +
        track.alignment[hit][1] * alphaTrue[1] + noise;
    }
    tracks.push_back(track);
  }
  return tracks;
}

struct WhitenedTrack {
  Matrix local;
  Matrix alignment;
  Vector measured;
};

WhitenedTrack whiten(const Track& track)
{
  WhitenedTrack result{track.local, track.alignment, track.measured};
  for (std::size_t i = 0; i < track.sigma.size(); ++i) {
    for (double& value : result.local[i]) value /= track.sigma[i];
    for (double& value : result.alignment[i]) value /= track.sigma[i];
    result.measured[i] /= track.sigma[i];
  }
  return result;
}

std::vector<Vector> residualContrasts(const Matrix& local)
{
  const std::size_t n = local.size();
  const std::size_t p = local.front().size();
  std::vector<Vector> localBasis;
  for (std::size_t col = 0; col < p; ++col) {
    Vector v(n);
    for (std::size_t row = 0; row < n; ++row) v[row] = local[row][col];
    for (const auto& basis : localBasis) {
      const double projection = dot(v, basis);
      for (std::size_t i = 0; i < n; ++i) v[i] -= projection * basis[i];
    }
    const double length = std::sqrt(dot(v, v));
    if (length <= 1.e-12)
      throw std::runtime_error("Rank-deficient local model in toy alignment test");
    for (double& value : v) value /= length;
    localBasis.push_back(v);
  }

  std::vector<Vector> contrasts;
  for (std::size_t seed = 0; seed < n; ++seed) {
    Vector v(n, 0.0);
    v[seed] = 1.0;
    for (const auto& basis : localBasis) {
      const double projection = dot(v, basis);
      for (std::size_t i = 0; i < n; ++i) v[i] -= projection * basis[i];
    }
    for (const auto& basis : contrasts) {
      const double projection = dot(v, basis);
      for (std::size_t i = 0; i < n; ++i) v[i] -= projection * basis[i];
    }
    const double length = std::sqrt(dot(v, v));
    if (length <= 1.e-10) continue;
    for (double& value : v) value /= length;
    contrasts.push_back(v);
  }
  return contrasts;
}

struct Posterior {
  Vector mean;
  Matrix covariance;
};

Posterior batchSchurPosterior(const std::vector<Track>& tracks,
                              const Vector& priorMean,
                              const Matrix& priorCovariance)
{
  Matrix information = inverse(priorCovariance);
  Vector score = multiply(information, priorMean);

  for (const auto& track : tracks) {
    const WhitenedTrack w = whiten(track);
    const Matrix at = transpose(w.local);
    const Matrix bt = transpose(w.alignment);
    const Matrix localInformation = multiply(at, w.local);
    const Matrix localCovariance = inverse(localInformation);
    const Matrix projection = multiply(
      multiply(w.local, localCovariance), at);
    const Matrix residualProjector = add(identity(w.local.size()), projection, -1.0);
    const Matrix projectedAlignment = multiply(residualProjector, w.alignment);
    const Vector projectedMeasured = multiply(residualProjector, w.measured);
    information = add(information,
      multiply(bt, projectedAlignment));
    const Vector trackScore = multiply(bt, projectedMeasured);
    for (std::size_t i = 0; i < score.size(); ++i) score[i] += trackScore[i];
  }

  Matrix covariance = inverse(information);
  return {multiply(covariance, score), covariance};
}

Posterior sequentialContrastPosterior(const std::vector<Track>& tracks,
                                     const Vector& priorMean,
                                     const Matrix& priorCovariance)
{
  Vector mean = priorMean;
  Matrix covariance = priorCovariance;
  for (const auto& track : tracks) {
    const WhitenedTrack w = whiten(track);
    const auto contrasts = residualContrasts(w.local);
    for (const auto& q : contrasts) {
      Vector h(2, 0.0);
      for (std::size_t i = 0; i < q.size(); ++i)
        for (std::size_t j = 0; j < h.size(); ++j)
          h[j] += q[i] * w.alignment[i][j];
      const double observation = dot(q, w.measured);
      const Vector covarianceTimesH = multiply(covariance, h);
      const double innovationVariance = 1.0 + dot(h, covarianceTimesH);
      Vector gain = covarianceTimesH;
      for (double& value : gain) value /= innovationVariance;
      const double innovation = observation - dot(h, mean);
      for (std::size_t i = 0; i < mean.size(); ++i)
        mean[i] += gain[i] * innovation;

      Matrix kh = zeros(mean.size(), mean.size());
      for (std::size_t i = 0; i < mean.size(); ++i)
        for (std::size_t j = 0; j < mean.size(); ++j)
          kh[i][j] = gain[i] * h[j];
      const Matrix ikh = add(identity(mean.size()), kh, -1.0);
      covariance = add(
        multiply(multiply(ikh, covariance), transpose(ikh)),
        multiply(Matrix{{gain[0]}, {gain[1]}},
                 Matrix{{gain[0], gain[1]}}));
    }
  }
  return {mean, covariance};
}

} // namespace

BOOST_AUTO_TEST_CASE(ResidualCovarianceIsSingularAfterTrackFit)
{
  const Track track = makeTracks().front();
  const WhitenedTrack w = whiten(track);
  const auto contrasts = residualContrasts(w.local);
  const Matrix localCovariance = inverse(
    multiply(transpose(w.local), w.local));
  const Matrix rawResidualCovariance = add(
    [&]() {
      Matrix r = zeros(track.sigma.size(), track.sigma.size());
      for (std::size_t i = 0; i < track.sigma.size(); ++i)
        r[i][i] = track.sigma[i] * track.sigma[i];
      return r;
    }(),
    multiply(multiply(track.local, localCovariance), transpose(track.local)),
    -1.0);

  BOOST_CHECK_EQUAL(numericalRank(rawResidualCovariance), 4U);
  BOOST_CHECK_EQUAL(contrasts.size(), 4U);
  for (std::size_t i = 0; i < contrasts.size(); ++i) {
    for (std::size_t col = 0; col < w.local.front().size(); ++col) {
      Vector localColumn(w.local.size());
      for (std::size_t row = 0; row < w.local.size(); ++row)
        localColumn[row] = w.local[row][col];
      BOOST_CHECK_SMALL(dot(contrasts[i], localColumn), 1.e-10);
    }
    for (std::size_t j = 0; j < contrasts.size(); ++j)
      BOOST_CHECK_SMALL(dot(contrasts[i], contrasts[j]) -
                        (i == j ? 1.0 : 0.0), 1.e-10);
  }
}

BOOST_AUTO_TEST_CASE(SequentialContrastsMatchBatchSchurMeanAndCovariance)
{
  const auto tracks = makeTracks();
  const Vector priorMean{0.0, 0.0};
  const Matrix priorCovariance{{0.09, 0.025}, {0.025, 0.16}};

  const Posterior batch =
    batchSchurPosterior(tracks, priorMean, priorCovariance);
  const Posterior recursive =
    sequentialContrastPosterior(tracks, priorMean, priorCovariance);

  for (std::size_t i = 0; i < batch.mean.size(); ++i) {
    BOOST_CHECK_SMALL(recursive.mean[i] - batch.mean[i], 1.e-10);
    for (std::size_t j = 0; j < batch.mean.size(); ++j)
      BOOST_CHECK_SMALL(recursive.covariance[i][j] -
                        batch.covariance[i][j], 1.e-10);
  }
  BOOST_CHECK(std::abs(batch.covariance[0][1]) > 1.e-5);
}
