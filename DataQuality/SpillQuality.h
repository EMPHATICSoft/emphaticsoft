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
      enum spillState {
	kGood,
	kBad,
	kQuestionable,
	kSpecial,
	kNotInList = -1
      };

      SpillQuality();
      ~SpillQuality() {};

      spillState goodRunStatus;     ///< What is the run status in the Good Runs List?
      
      //Add all quality metrics to overall spill metric here
      bool isSpillGood()  const { return (goodRunStatus==kGood);}

      friend std::ostream& operator << (std::ostream& o, const SpillQuality& sq);

    };

  }// end namespace dq
}// end namespace emph

#endif //SPILLQUALITY_H
///////////////////////////////////////////////////////////////////////////////
