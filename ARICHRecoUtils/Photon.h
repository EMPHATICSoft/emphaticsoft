////////////////////////////////////////////////////////////////////////
/////// \brief   Definition of PHOTON class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#ifndef PHOTON_INCLUDE
#define PHOTON_INCLUDE

#include "Particle.h"
    namespace ARICHRECO{

        class Photon : public ARICHRECO::Particle {
        public:
          Photon(TVector3, TVector3, double);
          double wav;
          double getWavelength();
          int numScatters = 0;
        };
}
#endif
