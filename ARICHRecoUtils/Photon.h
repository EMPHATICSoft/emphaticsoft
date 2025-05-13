////////////////////////////////////////////////////////////////////////
/////// \brief   Definition of PHOTON class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#ifndef PHOTON_INCLUDE
#define PHOTON_INCLUDE

#include "ARICHRecoUtils/Particle.h"

namespace arichreco{

        class Photon : public arichreco::Particle {
        public:
          Photon(TVector3, TVector3, double);
          double wav;
          double getWavelength();
          int numScatters = 0;
        };
}
#endif
