///
/// \file    EventHolder.h
/// \brief   Singleton to hold the current art::Event for the event display
/// \author  brebel@fnal.gov
/// \version $Id: EventHolder.h,v 1.3 2011-10-31 14:41:40 greenc Exp $
///
#ifndef EVDB_EVENTHOLDER_H
#define EVDB_EVENTHOLDER_H
#ifndef __CINT__ // root 5
#ifndef __ROOTCLING__ // root 6

#include "art/Framework/Principal/Event.h"

namespace evdb {
  
  class EventHolder {
    
  public:
    static EventHolder* Instance();

    void SetEvent(art::Event const* evt);
    const art::Event* GetEvent() const;

  private:

    EventHolder();
    ~EventHolder();

    const art::Event* fEvent; ///< the Event
  };

}
#endif // root 6 - dangerous, to be used sparingly
#endif // __CINT__
#endif // EVDB_EVENTHOLDER_H
