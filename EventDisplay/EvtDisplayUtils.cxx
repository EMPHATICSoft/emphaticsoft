//
// - Serves as the link between ROOT "events" (e.g. mouse-clicks) and the ART
//   event display service by providing a receiver slot for signals generated
//   by the ROOT events.  A ROOT dictionary needs to be generated for this.
//

#include "EventDisplay/NavState.h"
#include "EventDisplay/EvtDisplayUtils.h"
#include <string>

namespace emph {
  EvtDisplayUtils::EvtDisplayUtils():fTbRun(0),fTbSubrun(0),fTbEvt(0){}
  void EvtDisplayUtils::PrevEvent(){
    NavState::Set(kPREV_EVENT);
  }
  void EvtDisplayUtils::NextEvent(){
    NavState::Set(kNEXT_EVENT);
  }
  void EvtDisplayUtils::GotoEvent(){
    int run = std::stoi(fTbRun->GetString());
    int subrun = std::stoi(fTbSubrun->GetString());
    int event = std::stoi(fTbEvt->GetString());
    NavState::SetTarget(run, subrun, event);
    NavState::Set(kGOTO_EVENT);
  }
  void EvtDisplayUtils::ReloadFHICL(){
    NavState::Set(kRELOAD_FHICL);
  }
}
