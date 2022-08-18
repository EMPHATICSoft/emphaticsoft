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

      bool HasSSDHits()   const { return fHasSSDHits; }
      friend std::ostream& operator << (std::ostream& o, const EventQuality& eq);

    private:
      bool fHasSSDHits;    ///< Does the event have SSD hits?

    };

  }// end namespace dq
}// end namespace emph

#endif //EVENTQUALITY_H
///////////////////////////////////////////////////////////////////////////////
