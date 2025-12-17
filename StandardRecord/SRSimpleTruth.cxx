///////////////////////////////////////////////////////////
// \file SRSimpleTruth.cxx
// \brief Contains the default constructor for the SRTruth
// beam particle member
///////////////////////////////////////////////////////////

#include "SRSimpleTruth.h"
#include <vector>

namespace caf
{
        // default SRSimpleTruth constructor
  SRSimpleTruth::SRSimpleTruth() 
  {

  }

//  SRTrueParticle::SRTrueParticle(SRParticle& p) : SRParticle(p)
//  {

//  }

  void SRSimpleTruth::setDefault() {
      pos = ROOT::Math::XYZVector(0.,0.,0.);
      mom = ROOT::Math::XYZVector(0.,0.,0.);
      G4trkId = -1;
      pdgCode = 0;
      process = "unknown";
  }

}
