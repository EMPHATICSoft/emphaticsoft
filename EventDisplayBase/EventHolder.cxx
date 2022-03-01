///
/// \file    EventHolder.h
/// \brief   Singleton to hold the current art::Event for the event display
/// \author  brebel@fnal.gov
/// \version $Id: EventHolder.cxx,v 1.2 2011-01-19 16:44:59 p-nusoftart Exp $
///
#include <iostream>
#include "EventDisplayBase/EventHolder.h"

namespace evdb{

  static EventHolder* gInstance = 0;

  //---------------------------------------------------------
  EventHolder* EventHolder::Instance() { 
    if(!gInstance) gInstance = new EventHolder();
    return gInstance; 
  }

  //---------------------------------------------------------
  EventHolder::EventHolder() : 
    fEvent(0)
  {
  }

  //---------------------------------------------------------
  EventHolder::~EventHolder()
  {
  }

  //---------------------------------------------------------
  void EventHolder::SetEvent(art::Event const* evt)
  {
    if(fEvent){
//       std::cerr << "event is already loaded, resetting it" << std::endl;
      fEvent = 0;
    }
    fEvent = evt;

//     std::cout << "run is " << fEvent->run() << " event is " << fEvent->id().event() << std::endl;

  }
 
  //---------------------------------------------------------
  const art::Event* EventHolder::GetEvent() const
  {
//     if(!fEvent){
//       std::cerr << "No art::Event set to return, handing back null pointer" << std::endl;
//     }
    return fEvent;
  }

}//namespace 
