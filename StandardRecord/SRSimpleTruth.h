///////////////////////////////////////////////////////////////
// \file SRSimpleTruth.h
// \brief SRSimpleTruth will keep some 'true' particle information about
// simulated particles in a reco track
///////////////////////////////////////////////////////////////

#ifndef SRSIMPLETRUTH_H
#define SRSIMPLETRUTH_H

#include <vector>
#include <string>
#include "Math/Vector3D.h"

// Common Analysis Files
namespace caf
{
        class SRSimpleTruth
        {
        public:
                ROOT::Math::XYZVector   pos; // position vector
                ROOT::Math::XYZVector   mom; // momentum vector
                int                     G4trkId;       ///< G4 track Id
                int                     pdgCode;       ///< PDG code
                std::string             process;       ///< Detector-simulation physics process that created the particle

                SRSimpleTruth();
//		SRSimpleTruth(SRParticle& b);
                ~SRSimpleTruth(){};
                void setDefault();

        }; //end class>
}
#endif //SRTrueParticle_H

