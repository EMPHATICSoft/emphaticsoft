#ifndef ALIGNMENT_KALIGNFILTER_H
#define ALIGNMENT_KALIGNFILTER_H

#include <vector>

namespace emph {
namespace alignment {

  class KAlignFilter {
  public:
    KAlignFilter(const std::vector<double>& priorMean,
                 const std::vector<std::vector<double>>& priorCovariance);

    const std::vector<double>& GetMean() const { return fMean; }
    const std::vector<std::vector<double>>& GetCovariance() const
    {
      return fCovariance;
    }

    // observation = jacobian * alignment + independent Gaussian noise.
    void UpdateIndependentMeasurement(const std::vector<double>& jacobian,
                                     double observation,
                                     double variance);

  private:
    std::vector<double> fMean;
    std::vector<std::vector<double>> fCovariance;
  };

} // namespace alignment
} // namespace emph

#endif
