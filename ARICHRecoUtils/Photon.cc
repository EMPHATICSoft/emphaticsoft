////////////////////////////////////////////////////////////////////////
/////// \brief   PHOTON class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#include "Photon.h"

namespace ARICHRECO{

        Photon::Photon(TVector3 pos, TVector3 dir, double wavelength) : ARICHRECO::Particle(pos, dir, 1.) {
          this->pos0 = pos;
          this->dir0 = dir;
          this->pos = pos;
          this->dir = dir;
          this->wav = wavelength;
          this->beta = 1;
        }

        double Photon::getWavelength() {
          return wav;
        }
}
