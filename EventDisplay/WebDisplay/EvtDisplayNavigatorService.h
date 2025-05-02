//
// - Event display service used with the EventDisplay3D module to start up the
//   TApplication and allow forward, backward, and jump-to navigation of events
//   in the root input file.  This is a very much simplified version based on
//   Nova's event visplay service by Mark Messier.
//

#ifndef EvtDisplayService_EvtDisplayNavigatorService_hh
#define EvtDisplayService_EvtDisplayNavigatorService_hh
#ifndef __CINT__
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Core/InputSource.h"
#include "art/Framework/Principal/Worker.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "art/Framework/Principal/Event.h"

namespace emph
{
  class EvtDisplayNavigatorService
  {
  public:

    EvtDisplayNavigatorService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    //Interfaces for the event display module to use 
    void setTarget(const int run, const int subrun, const int event);

  private:

    void postBeginJobWorkers(art::InputSource* inputs,
                             std::vector<art::Worker*> const& workers);
    void postProcessEvent(art::Event const&, art::ScheduleContext);

    bool fChangeEventFlow; //Set to true when a module has requested a different event than the one that's next in order
    int fTargetRun;
    int fTargetSubrun;
    int fTargetEvent;

  private:
    art::InputSource* fInputSource; ///< Input source of events
  };
}
#endif // __CINT__
DECLARE_ART_SERVICE(emph::EvtDisplayNavigatorService, LEGACY)
#endif // EvtDisplayService_EvtDisplayService_hh
