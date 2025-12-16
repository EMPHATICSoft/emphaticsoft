//////////////////////////////////////////////////////////////////////////
/// \brief  Event level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#include "DataQuality/EventQuality.h"

namespace emph {
  namespace dq {

    //----------------------------------------------------------------------
    EventQuality::EventQuality() :
      hasTrigger(false),
      hasT0CAEN(false),
      hasLGCaloHits(false),
      hasBACkovHits(false),
      hasGasCkovHits(false),
      hasT0TRB3(false),
      hasARICHHits(false),
      hasRPCHits(false),
      hasSSDHits(false),
      trigCoincLevel(0)
    {
    }
    //----------------------------------------------------------------------
    std::ostream& operator << (std::ostream& o, const EventQuality& eq)
    {
      o << "Event Quality information:\n";
      o << std::boolalpha;
      o << "Has trigger?: " << eq.hasTrigger << std::endl;
      o << "Has T0 for CAEN?: " << eq.hasT0CAEN << std::endl;
      o << "Has LGCalo hits?: " << eq.hasLGCaloHits << std::endl;
      o << "Has BACkov hits?: " << eq.hasBACkovHits << std::endl;
      o << "Has GasCkov hits?: " << eq.hasGasCkovHits << std::endl;
      o << "Has T0 for TRB3?: " << eq.hasT0TRB3 << std::endl;
      o << "Has ARICH hits?: " << eq.hasARICHHits << std::endl;
      o << "Has RPC hits?: " << eq.hasRPCHits << std::endl;
      o << "Has SSD hits?: " << eq.hasSSDHits << std::endl;
      o << "Number of trigger PMTs above threshold: " << eq.trigCoincLevel << std::endl;
      o << "Event passes quality checks: " <<eq.isEventGood() <<std::endl;
      return o;
    }

  }// end namespace dq

}// end namespace emph
//////////////////////////////////////////////////////////////////////////////
