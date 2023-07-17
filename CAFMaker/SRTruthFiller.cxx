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
#include <typeinfo>

namespace caf
{
  	void SRTruthFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec){
    	art::Handle< std::vector <sim::Particle> > pcal;

    	try {
      		evt.getByLabel("geantgen", pcal);		// get the particle list from art
				
      		std::vector<sim::Particle> particles;		
      		if(!pcal.failedToGet()){particles = *pcal;}
		
		  
      		auto beam_temp = SRParticle(particles[0]);		// grab the first beam particle, do an upcast and then a downcast from
      		SRTrueParticle beam = SRTrueParticle(beam_temp);	// sim::Particle to caf::SRParticle and then from caf::SRParticle to 
      		std::vector <caf::SRTrueParticle> daughters;		// caf::SRTrueParticle
      		stdrec.truth = caf::SRTruth();
		  		
      		
		for (int i = 0; i < particles[0].NumberDaughters(); ++i){	// record every daughter of the beam particle
	  	int daughter_trackid = particles[0].Daughter(i);
		      	
	  		for (size_t i = 0; i < particles.size(); ++i){
	    		int idHolder = particles[i].TrackId();
					
	    			if (idHolder == daughter_trackid){
	      			auto dummy = SRParticle(particles[i]);		// again this is some maneuvering around class inheritances 
	      			auto dummy2 = SRTrueParticle(dummy);
	      			daughters.push_back(dummy2);
	    			}
	  		}							// potential to extend this to include more generations
		}	  
	      	stdrec.truth.beam = beam;				// record the beam particle into the CAF
		stdrec.truth.beam.daughters = daughters;		// record the vector of daughter particles into the CAF
    	}
    	catch(...) {}
  }

} // end namespace caf
