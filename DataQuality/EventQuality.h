//////////////////////////////////////////////////////////////////////////
/// \brief  Event level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#ifndef EVENTQUALITY_H
#define EVENTQUALITY_H

#include <iostream>

namespace emph {
  namespace dq {
    class EventQuality
    {
    public:
      EventQuality();
      ~EventQuality() {};

      bool hasSSDHits;    ///< Does the event have SSD hits?
      
      //Add all quality metrics to overall event metric here
      bool isEventGood()  const { return hasSSDHits;}

      friend std::ostream& operator << (std::ostream& o, const EventQuality& eq);

    };

  }// end namespace dq
}// end namespace emph

#endif //EVENTQUALITY_H
///////////////////////////////////////////////////////////////////////////////
