////////////////////////////////////////////////////////////////////////
/// \brief   Module that kips a configurable number of events between 
///          each that it allows through. Note that this module really 
///          skips (N-1) events, it uses a simple modular division as 
///          its critera. This module will cut down the data sample to 
///          1/N of its original size.
/// \author  rocco@fnal.gov
/// \date    August 2011
////////////////////////////////////////////////////////////////////////
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"

namespace filter{
  class SkipNEvents : public art::EDFilter {
  public:
    explicit SkipNEvents(fhicl::ParameterSet const &pset);
    ~SkipNEvents();
    bool filter(art::Event &e);
    void reconfigure(const fhicl::ParameterSet &pset);		

  private:
    int skipCounter;
    int nSkip;
  };
}

filter::SkipNEvents::SkipNEvents(fhicl::ParameterSet const &pset)
  : EDFilter(pset),
  skipCounter(0),
  nSkip(0)
{  
  reconfigure(pset);
}

filter::SkipNEvents::~SkipNEvents() {}

bool filter::SkipNEvents::filter(art::Event &/*e*/) {
  return !(skipCounter++ % nSkip);
}

void filter::SkipNEvents::reconfigure(const fhicl::ParameterSet &pset){
  nSkip = pset.get<int>("SkipNumber");
}

DEFINE_ART_MODULE(filter::SkipNEvents)
