////////////////////////////////////////////////////////////////////////
// \file    SRTrueParticleFiller.cxx
// \brief    
//          
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/SRTrueParticleFiller.h"
#include "Simulation/Particle.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void SRTrueParticleFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    art::Handle< std::vector <sim::Particle> > pcal;

    evt.getByLabel("geantgen", pcal);		// figure out what the label is here

    std::vector<sim::Particle> particles;
    if(!pcal.failedToGet()) particles = *pcal;

    sim::Particle beam = particles[1];		// grab the first particle

    std::vector <sim::Particle> daughters;
	
    for (int i = 0; i < beam.NumberDaughters(); ++i){
	int daughter_trackid = beam.Daughter(i);
	
	for (size_t i = 0; i < particles.size(); ++i){
		if (particles[i].TrackId() == daughter_trackid){daughters.push_back(particles[i]);}
	}
    	
    }
	




  }

} // end namespace caf
