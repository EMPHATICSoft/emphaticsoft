//
// - Event display service used with the WebDisplay module to start up the
//   allow forward, backward, and jump-to navigation of events
//   in the root input file.  This is a very much simplified version based on
//   Nova's event display service by Mark Messier.
//
// - Thank you Jon Paley for simplifying it for EMPHATIC.  THIS version
//   has been modified to cut out the TApplication stuff.

#include "EvtDisplayNavigatorService.h"

// ART includes
#include "art_root_io/RootInput.h"

// ROOT includes
#include "TApplication.h"
#include "TEveManager.h"

// C++ includes
#include <iostream>

namespace emph
{

  EvtDisplayNavigatorService::EvtDisplayNavigatorService(fhicl::ParameterSet const& pset,
                             art::ActivityRegistry& reg): fTargetRun(-1), fTargetSubrun(-1),
                                                          fTargetEvent(-1)
  {
    (void)pset;
    reg.sPostBeginJobWorkers.watch(this, &EvtDisplayNavigatorService::postBeginJobWorkers);
    reg.sPostProcessEvent.watch   (this, &EvtDisplayNavigatorService::postProcessEvent);
  }

  //......................................................................

  void EvtDisplayNavigatorService::postBeginJobWorkers(art::InputSource* input_source,
                                         std::vector<art::Worker*> const&)
  {
    fInputSource = input_source;
  }

  //......................................................................

  void EvtDisplayNavigatorService::setTarget(const int run, const int subrun, const int event)
  {
    fChangeEventFlow = true;
    fTargetRun = run;
    fTargetSubrun = subrun;
    fTargetEvent = event;    
  }


  void EvtDisplayNavigatorService::postProcessEvent(art::Event const& evt, art::ScheduleContext)
  {
    art::RootInput* rootInput = dynamic_cast<art::RootInput*>(fInputSource);
    if(!rootInput){
      throw cet::exception("EvtDisplayNavigatorService")
        << "Random access for the EvtDisplay requires a RootInput source for proper operation.\n";
    }

    // Figure out where to go in the input stream from here
    if(fChangeEventFlow && (fTargetRun != evt.id().run() || fTargetSubrun != evt.id().subRun() || fTargetEvent != evt.id().event()))
    {
      if(fTargetRun < 1 || fTargetSubrun < 1 || fTargetEvent < 0)
      {
        mf::LogWarning("EvtDisplayNavigator") << "Requested bad event ID: run " << fTargetRun << " subrun " << fTargetSubrun << " event " << fTargetEvent;
        rootInput->seekToEvent(evt.id());
      }
      else
      {
        art::EventID eid(fTargetRun, fTargetSubrun, fTargetEvent);
        if(!rootInput->seekToEvent(eid))
        {
          mf::LogWarning("EvtDisplayNavigator") << "Unable to find " << eid << ".  Reloading the current event.";
          rootInput->seekToEvent(evt.id()); 
        }
      }
    }
    fChangeEventFlow = false;
  }

}// end namespace emph

DEFINE_ART_SERVICE(emph::EvtDisplayNavigatorService)
