//
// - Event display service used with the EventDisplay3D module to start up the
//   TApplication and allow forward, backward, and jump-to navigation of events
//   in the root input file.  This is a very much simplified version based on
//   Nova's event visplay service by Mark Messier.
//

#ifndef EvtDisplayService_EvtDisplayService_hh
#define EvtDisplayService_EvtDisplayService_hh
#ifndef __CINT__
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Core/InputSource.h"
#include "art/Framework/Principal/Worker.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "art/Framework/Principal/Event.h"
#include "EventDisplay/EnsureTApplication.h"

namespace emph
{
  class EvtDisplayService
  {
  public:

    EvtDisplayService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

  private:

    // When this object is intialized, it will ensure that
    // interactive ROOT is properly initialized
    EnsureTApplication ensureTApp_;

    void postBeginJobWorkers(art::InputSource* inputs,
                             std::vector<art::Worker*> const& workers);
    void postProcessEvent(art::Event const&, art::ScheduleContext);

  private:
    art::InputSource* fInputSource; ///< Input source of events

  public:
  };
}
#endif // __CINT__
DECLARE_ART_SERVICE(emph::EvtDisplayService, LEGACY)
#endif // EvtDisplayService_EvtDisplayService_hh
