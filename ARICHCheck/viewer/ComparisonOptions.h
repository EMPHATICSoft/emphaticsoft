#ifndef OM_COMPARISONOPTIONS_H
#define OM_COMPARISONOPTIONS_H
#include <string>

namespace emph { 
  namespace onmon {
    class ComparisonOptions {
    public:
      enum _compare_to {
        kNone,
        kReference,
        kRecent,
        kMaxCompWhich
      };
      enum _compare_method {
        kOverlay,
        kDifference,
        kRatio,
        kAsymmetry,
        kChi,
        kShowRef,
        kMaxCompHow
      };
      enum _compare_normalize {
        kAbsolute,
        kArea,
        kPeak,
        // To return the following line, see also ComparisonOptions.cxx and
        // both the TH1F and TH2F sections of the UpdateCompare() function in
        // PlotViewer.cxx.
        //
        // kIntegral,
        kMaxCompNorm
      };

    public:
      ComparisonOptions();
      
      static const char* CompareTo(unsigned int i);
      static const char* CompareMethod(unsigned int i);
      static const char* CompareNormalize(unsigned int i);
      
    public:
      unsigned int fWhich;     ///< Which histo. to compare to?
      unsigned int fMethod;    ///< How to show the comparison?
      unsigned int fNormalize; ///< How to normalize the histos?
      unsigned int fLookBack;  ///< Which "look back" histogram to compare to?
      double       fX1; ///< Range to normalize
      double       fX2; ///< Range to normalize
      double       fY1; ///< Range to normalize
      double       fY2; ///< Range to normalize
      std::string  fReferenceFile; ///< Reference file for comparison histograms
    };
  } //end namespace onmon
} //end namespace emph
#endif
