//////////////////////////////////////////////////////////////////////////
/// \brief  Spill level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#ifndef SPILLQUALITY_H
#define SPILLQUALITY_H

#include <iostream>

namespace emph {
  namespace dq {
    class SpillQuality
    {
    public:
      SpillQuality();
      ~SpillQuality() {};

      bool isInGoodRunsList;     ///< Does the subrun exist in the Good Runs list?
      
      //Add all quality metrics to overall spill metric here
      bool isSpillGood()  const { return isInGoodRunsList;}

      friend std::ostream& operator << (std::ostream& o, const SpillQuality& sq);

    };

  }// end namespace dq
}// end namespace emph

#endif //SPILLQUALITY_H
///////////////////////////////////////////////////////////////////////////////
