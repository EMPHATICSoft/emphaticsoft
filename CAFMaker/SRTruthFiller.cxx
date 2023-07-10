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

    		evt.getByLabel("geantgen", pcal);		// figure out what the label is here
				
    		std::vector<sim::Particle> particles;
    		if(!pcal.failedToGet()){particles = *pcal;}
		
		
		auto beam_temp = SRParticle(particles[0]);
		SRTrueParticle beam = SRTrueParticle(beam_temp);

    		std::vector <caf::SRTrueParticle> daughters;
		
		std::cerr << "1 Got Here!!!! CAFMaker/SRTruthFiller" << std::endl;
   		for (int i = 0; i < particles[0].NumberDaughters(); ++i){
			int daughter_trackid = particles[0].Daughter(i);
			std::cerr << "2 Got Here!!!! CAFMaker/SRTruthFiller" << std::endl;
			
			for (size_t i = 0; i < particles.size(); ++i){
				int idHolder = particles[i].TrackId();
				if (idHolder == daughter_trackid){
					std::cerr << "4 Got Here!!!! CAFMaker/SRTruthFiller" << std::endl;
					auto dummy = SRParticle(particles[i]);
					auto dummy2 = SRTrueParticle(dummy);
					std::cerr << "5 Got Here!!!! CAFMaker/SRTruthFiller" << std::endl;
					daughters.push_back(dummy2);
					std::cerr << "6 Got Here!!!! CAFMaker/SRTruthFiller" << std::endl;	
				}
			}
    		}
		
	    	stdrec.truth = caf::SRTruth();
		stdrec.truth.beam = beam;		
		stdrec.truth.beam.daughters = daughters;		




  }

} // end namespace caf
