////////////////////////////////////////////////////////////////////////
// \file    SRTrueParticleFiller.cxx
// \brief    
//          
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/SRTruthFiller.h"
#include "StandardRecord/SRParticle.h"
#include "Simulation/Particle.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
	void SRTruthFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec){
    		art::Handle< std::vector <sim::Particle> > pcal;

    		evt.getByLabel("geantgen", pcal);		// figure out what the label is here
		
    		std::vector<sim::Particle> particles;
    		if(!pcal.failedToGet()){particles = *pcal;}
	
    		sim::Particle beam = particles[0];		// grab the first particle
    		
		
    		std::vector <caf::SRTrueParticle> daughters;
		SRTrueParticle dummy;	

   		for (int i = 0; i < beam.NumberDaughters(); ++i){
			int daughter_trackid = beam.Daughter(i);
			
			for (size_t i = daughter_trackid; i < particles.size(); ++i){
				if (particles[i].TrackId() == daughter_trackid){
					dummy.ptcl = particles[i];
					daughters.push_back(dummy);

				}
			}
    		}
	
	    	stdrec.truth = caf::SRTruth();
		
		stdrec.truth.beam.ptcl = beam;		
		stdrec.truth.beam.daughters = daughters;		




  }

} // end namespace caf
