#include "OnlineMonitoring/viewer/ComparisonOptions.h"
using namespace emph::onmon;

//......................................................................

const char* ComparisonOptions::CompareTo(unsigned int i) 
{
  switch (i) {
  case ComparisonOptions::kNone:      return "None";
  case ComparisonOptions::kReference: return "Reference";
  case ComparisonOptions::kRecent:    return "Recent";
  }
  return 0;
}

//......................................................................

const char* ComparisonOptions::CompareMethod(unsigned int i) {
  switch (i) {
  case ComparisonOptions::kOverlay:    return "Overlay";
  case ComparisonOptions::kDifference: return "Difference";
  case ComparisonOptions::kRatio:      return "Ratio";
  case ComparisonOptions::kAsymmetry:  return "Asymmetry";
  case ComparisonOptions::kChi:        return "Chi";
  case ComparisonOptions::kShowRef:    return "Show reference histogram";
  };
  return 0;
}

//......................................................................

const char* ComparisonOptions::CompareNormalize(unsigned int i) {
  switch (i) {
  case ComparisonOptions::kAbsolute: return "Absolute";
  case ComparisonOptions::kArea:     return "Area";
  case ComparisonOptions::kPeak:     return "Peak";
    // case ComparisonOptions::kIntegral: return "Integral";
  };
  return 0;
}

//......................................................................

ComparisonOptions::ComparisonOptions() :
  fWhich(kNone),
  fMethod(kOverlay),
  fNormalize(kAbsolute),
  fLookBack(0),
  fX1(0),
  fX2(0),
  fY1(0),
  fY2(0),
  fReferenceFile("")
{ }

////////////////////////////////////////////////////////////////////////
