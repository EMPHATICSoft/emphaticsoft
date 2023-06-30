///////////////////////////////////////////////////////////////
// \file SRTrueParticle.h
// \brief SRTP will keep some 'true' particle information about
// simulated particles
///////////////////////////////////////////////////////////////

#ifndef SRTRUEPARTICLE_H
#define SRTRUEPARTICLE_H

#include <vector>
#include <string>

// Common Analysis Files
namespace caf
{
        class SRTrueParticle
        {
        public:
                float vrt[3];
                float mom[3];
                int pdg;
                float energy;
                std::vector <caf::SRTrueParticle> daughters;
		//caf::SRTrueParticle mother;
		std::string process;

                SRTrueParticle();
                ~SRTrueParticle(){};
        }; //end class>
}
#endif //SRTrueParticle_H

