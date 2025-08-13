////////////////////////////////////////////////////////////////////////
// Class:       HasSSDLineSegs
// Plugin Type: filter (Unknown Unknown)
// File:        HasSSDLineSegs_module.cc
//
// Generated at Wed Sep 25 15:58:00 2024 by Andrew Olivier using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////

//Gets rid of all events that don't have any rb::LineSegments.
//Useful for looking at data with the event display.

#include "RecoBase/LineSegment.h"

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace emph
{
  class HasSSDLineSegs;
}

class emph::HasSSDLineSegs : public art::EDFilter {
public:
  struct Config
  {
    using Name=fhicl::Name;
    using Comment=fhicl::Comment;
    fhicl::Atom<art::InputTag> lineSegLabel{Name("lineSegLabel"), Comment("Name of the module that produced SSD LineSegments.  Usually the cluster module."), "maketracksegments"};
  };

  typedef art::EDFilter::Table<Config> Parameters;

  explicit HasSSDLineSegs(Parameters const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  HasSSDLineSegs(HasSSDLineSegs const&) = delete;
  HasSSDLineSegs(HasSSDLineSegs&&) = delete;
  HasSSDLineSegs& operator=(HasSSDLineSegs const&) = delete;
  HasSSDLineSegs& operator=(HasSSDLineSegs&&) = delete;

  // Required functions.
  bool filter(art::Event& e) override;

private:

  // Declare member data here.
  Config fConfig;

};


emph::HasSSDLineSegs::HasSSDLineSegs(Parameters const& p)
  : EDFilter{p}, fConfig(p())
{
  consumes<std::vector<rb::LineSegment>>(p().lineSegLabel());
}

bool emph::HasSSDLineSegs::filter(art::Event& e)
{
  art::Handle<std::vector<rb::LineSegment>> lineSegHandle;
  e.getByLabel(fConfig.lineSegLabel(), lineSegHandle);

  //You know, we can produce things in EDFilters too!  It would be interesting
  //to write an EDFilter that produces Assns<evd::UserHighlight, PROD>
  //to highlight e.g. the sim::Particles that caused an event to pass the filter!

  return !lineSegHandle->empty();
}

DEFINE_ART_MODULE(emph::HasSSDLineSegs)
