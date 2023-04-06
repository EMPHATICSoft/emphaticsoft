//////////////////////////////////////////////////////////////////////////
/// \brief  Event level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#include "DataQuality/EventQuality.h"

namespace emph {
  namespace dq {

    //----------------------------------------------------------------------
    EventQuality::EventQuality() :
      hasSSDHits(false),
      trigCoincLevel(0)
    {
    }
    //----------------------------------------------------------------------
    std::ostream& operator << (std::ostream& o, const EventQuality& eq)
    {
      o << "Event Quality information:\n";
      o << std::boolalpha;
      o << "Has SSD hits?: " << eq.hasSSDHits << std::endl;
      o << "Number of trigger PMTs above threshold: " << eq.trigCoincLevel << std::endl;
      o << "Event passes quality checks: " <<eq.isEventGood() <<std::endl;
      return o;
    }

  }// end namespace dq

}// end namespace emph
//////////////////////////////////////////////////////////////////////////////
