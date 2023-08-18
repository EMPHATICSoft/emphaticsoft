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
  SRTrueParticle::SRTrueParticle() : SRParticle()
  {

  }

  SRTrueParticle::SRTrueParticle(SRParticle& p) : SRParticle(p)
  {

  }

}
