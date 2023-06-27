///////////////////////////////////////////////////////////
// \file SRTrueParticle.cxx
// \brief Contains the default constructor for the SRTruth
// beam particle member
///////////////////////////////////////////////////////////

#include "SRTrueParticle.h"
#include <vector>

namespace caf
{
        // default SRTrueParticle constructor
        SRTrueParticle::SRTrueParticle():
                vrt{0,0,0},
                mom{0,0,0}
                {
                 pdg = 25;
                 energy = 125.0;
                }
}
