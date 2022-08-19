//////////////////////////////////////////////////////////////////////////
/// \brief  Event level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#include "DataQuality/EventQuality.h"

namespace emph {
  namespace dq {

    //----------------------------------------------------------------------
    EventQuality::EventQuality() :
      fHasSSDHits(false),
      fIsEventGood(false)
    {
    }
    //----------------------------------------------------------------------
    std::ostream& operator << (std::ostream& o, const EventQuality& eq)
    {
      o << "Event Quality information:\n";
      o << std::boolalpha;
      o << "Has SSD hits?: " << eq.HasSSDHits() << std::endl;
      o << "Event passes quality checks: " <<eq.IsEventGood() <<std::endl;
      return o;
    }

  }// end namespace dq

}// end namespace emph
//////////////////////////////////////////////////////////////////////////////
