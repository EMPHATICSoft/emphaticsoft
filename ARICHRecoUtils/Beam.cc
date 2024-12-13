////////////////////////////////////////////////////////////////////////
/////// \brief   BEAM class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#include "Beam.h"

namespace ARICHRECO{

        Beam::Beam(TVector3 pos0, TVector3 dir0, double beta) {
          this->pos0 = pos0;
          this->dir0 = dir0;
          this->beta = beta;

          randomGenerate=std::make_shared<TRandom3>();
          randomGenerate->SetSeed(0);
        }

         ARICHRECO::Particle* Beam::generateParticle() {
          /*
          Return a particle with a randomly thrown direction and position
          */
          double paX = pos0(0) + randomGenerate->Gaus(0.0, errX);
          double paY = pos0(1) + randomGenerate->Gaus(0.0, errY);
          TVector3 pos = TVector3(paX, paY, pos0(2));
          double paDirX = dir0(0) + randomGenerate->Gaus(0.0, errDirX);
          double paDirY = dir0(1) + randomGenerate->Gaus(0.0, errDirY);
          double paDirZ =  sqrt(1 - paDirX*paDirX - paDirY*paDirY);
          TVector3 dir = TVector3(paDirX, paDirY, paDirZ);

          return new ARICHRECO::Particle(pos, dir, beta);
        }
    }
