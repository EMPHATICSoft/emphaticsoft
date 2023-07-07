///////////////////////////////////////////////////////////////
// \file SRTrueParticle.h
// \brief SRTP will keep some 'true' particle information about
// simulated particles
///////////////////////////////////////////////////////////////

#ifndef SRTRUEPARTICLE_H
#define SRTRUEPARTICLE_H

#include <vector>
#include <string>
#include "StandardRecord/SRParticle.h"

// Common Analysis Files
namespace caf
{
        class SRTrueParticle : public SRParticle
        {
        public:
                std::vector <caf::SRTrueParticle> daughters;
		//caf::SRTrueParticle mother;

                SRTrueParticle();
                ~SRTrueParticle(){};
        }; //end class>
}
#endif //SRTrueParticle_H

