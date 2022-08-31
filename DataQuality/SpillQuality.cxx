//////////////////////////////////////////////////////////////////////////
/// \brief  Spill level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#include "DataQuality/SpillQuality.h"

namespace emph {
  namespace dq {

    //----------------------------------------------------------------------
    SpillQuality::SpillQuality() :
      goodRunStatus(kNotInList)
    {
    }
    //----------------------------------------------------------------------
    std::ostream& operator << (std::ostream& o, const SpillQuality& sq)
    {
      o << "Spill Quality information:\n";
      o << std::boolalpha;
      o << "Spill is in Good Runs list: " << sq.goodRunStatus << std::endl;
      o << "Spill passes quality checks: " <<sq.isSpillGood() <<std::endl;
      return o;
    }

  }// end namespace dq

}// end namespace emph
//////////////////////////////////////////////////////////////////////////////
