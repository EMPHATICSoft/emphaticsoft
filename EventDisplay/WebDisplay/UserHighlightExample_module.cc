////////////////////////////////////////////////////////////////////////
// Class:       UserHighlightExample
// Plugin Type: producer (Unknown Unknown)
// File:        UserHighlightExample_module.cc
//
// Generated at Thu Sep 12 16:48:59 2024 by Andrew Olivier using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////

//emphaticsoft includes
#include "Simulation/Particle.h"
#include "EventDisplay/WebDisplay/UserHighlight.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <memory>

namespace evd {
  class UserHighlightExample;
}

//Reads sim::Particles from the event and flags particles that are
//not children of the primary particle for highlighting and extra comments in the event display.
//Creates evd::UserHighlight and associations between
//evd::UserHighlight and sim::Particle.

class evd::UserHighlightExample : public art::EDProducer {
public:
  struct Config
  {
    using Name=fhicl::Name;
    using Comment=fhicl::Comment;
    fhicl::Atom<art::InputTag> mcPartLabel{Name("mcPartLabel"), Comment("Name of the module that produced sim::Particles.  Usually the GEANT simulation."), "geantgen"};
  };

  typedef art::EDProducer::Table<Config> Parameters;

  explicit UserHighlightExample(Parameters const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  UserHighlightExample(UserHighlightExample const&) = delete;
  UserHighlightExample(UserHighlightExample&&) = delete;
  UserHighlightExample& operator=(UserHighlightExample const&) = delete;
  UserHighlightExample& operator=(UserHighlightExample&&) = delete;

  // Required functions.
  void produce(art::Event& e) override;

private:

  // Declare member data here.
  Config fConfig;
};


evd::UserHighlightExample::UserHighlightExample(Parameters const& p)
  : EDProducer{p}, fConfig(p())
{
  produces<std::vector<evd::UserHighlight>>();
  produces<art::Assns<evd::UserHighlight, sim::Particle>>();
  consumes<std::vector<sim::Particle>>(fConfig.mcPartLabel());
}

void evd::UserHighlightExample::produce(art::Event& e)
{
  const auto particles = e.getValidHandle<std::vector<sim::Particle>>(fConfig.mcPartLabel());
  auto highlightCol = std::make_unique<std::vector<evd::UserHighlight>>();
  auto partToHighlight = std::make_unique<art::Assns<evd::UserHighlight, sim::Particle>>();

  art::PtrMaker<sim::Particle> makePartPtr(e, particles.id());
  art::PtrMaker<evd::UserHighlight> makeHighlightPtr(e);

  const int brightBlue = 0x0096ffff;
  for(size_t whichPart = 0; whichPart < particles->size(); ++whichPart)
  {
    const auto& part = (*particles)[whichPart];
    if(part.fmother != 0)
    {
      highlightCol->emplace_back(brightBlue, "foo"); //part.fprocess);
      partToHighlight->addSingle(makeHighlightPtr(highlightCol->size()-1), makePartPtr(whichPart));
    }
  }

  e.put(std::move(highlightCol));
  e.put(std::move(partToHighlight));
}

DEFINE_ART_MODULE(evd::UserHighlightExample)
