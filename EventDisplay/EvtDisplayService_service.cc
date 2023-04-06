//
// - Event display service used with the EventDisplay3D module to start up the
//   TApplication and allow forward, backward, and jump-to navigation of events
//   in the root input file.  This is a very much simplified version based on
//   Nova's event display service by Mark Messier.
//

#include "EvtDisplayService.h"
#include "NavState.h"

// ART includes
#include "art_root_io/RootInput.h"

// ROOT includes
#include "TApplication.h"
#include "TEveManager.h"

// C++ includes
#include <iostream>

namespace emph
{

  EvtDisplayService::EvtDisplayService(fhicl::ParameterSet const& pset,
                             art::ActivityRegistry& reg):
    ensureTApp_()
  {
    (void)pset;
    reg.sPostBeginJobWorkers.watch(this, &EvtDisplayService::postBeginJobWorkers);
    reg.sPostProcessEvent.watch   (this, &EvtDisplayService::postProcessEvent);
  }

  //......................................................................

  void EvtDisplayService::postBeginJobWorkers(art::InputSource* input_source,
                                         std::vector<art::Worker*> const&)
  {
    fInputSource = input_source;
  }

  //......................................................................

  void EvtDisplayService::postProcessEvent(art::Event const& evt, art::ScheduleContext)
  {
    if(gEve){
      gEve->Redraw3D(kFALSE,kTRUE);
    }else{
      mf::LogWarning("EvtDisplayService") << "TeveManager is not properly initialized.";
      gApplication->SetReturnFromRun(kFALSE);
      gApplication->Terminate(0);
    }

    gApplication->Run(kTRUE);
    if(!gEve){
      gApplication->SetReturnFromRun(kFALSE);
      gApplication->Terminate(0);
    }

    art::RootInput* rootInput = dynamic_cast<art::RootInput*>(fInputSource);
    if(!rootInput){
      throw cet::exception("EvtDisplayService")
        << "Random access for the EvtDisplay requires a RootInput source for proper operation.\n";
    }

    // Figure out where to go in the input stream from here
    switch (NavState::Which()) {
    case kNEXT_EVENT: {
      // Contrary to appearances, this is *not* a NOP: it ensures run and
      // subRun are (re-)read as necessary if we've been doing random
      // access. Come the revolution ...
      //
      // 2011/04/10 CG.
      art::EventID eid(evt.id().run(),evt.id().subRun(),evt.id().event()+1);      
      rootInput->seekToEvent(eid);
      break;
    }
    case kPREV_EVENT: {
      art::EventID eid(evt.id().run(),evt.id().subRun(),evt.id().event()-1);
      rootInput->seekToEvent(eid);
      break;
    }
    case kRELOAD_EVENT: {
      rootInput->seekToEvent(evt.id());
      break;
    }
    case kGOTO_EVENT: {
      int targRun = NavState::TargetRun();
      int targEvt = NavState::TargetEvent();
      if(targRun<0 || targEvt<0){
        mf::LogWarning("EvtDisplayService") << "Negative Run or Event number specified -- reloading current event.";
        // Reload current event.
        rootInput->seekToEvent(evt.id());
      } else {
        art::EventID id(art::SubRunID::invalidSubRun(art::RunID(targRun)),targEvt);
        if (!rootInput->seekToEvent(id)) { // Couldn't find event
          mf::LogWarning("EvtDisplayService") << "Unable to find "
                                             << id
                                             << " -- reloading current event.";
          // Reload current event.
          rootInput->seekToEvent(evt.id());
        }
      }
      break;
    }
    case kRELOAD_FHICL: {
      rootInput->seekToEvent(evt.id());
      std::cout << "Reload FHICL button not working yet..." << std::endl;
      break;
    }
    default: {
      throw art::Exception(art::errors::LogicError)
        << "EvtDisplayService in unhandled state "
        << NavState::Which()
        << ".\n";
    }
    }

  }

}// end namespace emph

DEFINE_ART_SERVICE(emph::EvtDisplayService)
