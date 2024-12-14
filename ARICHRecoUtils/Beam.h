////////////////////////////////////////////////////////////////////////
/////// \brief   Definition of BEAM class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#ifndef BEAM_INCLUDE
#define BEAM_INCLUDE

#include "TRandom3.h"
#include "TVector3.h"
#include "Particle.h"


    namespace ARICHRECO{

        class Beam {
        private:
          TVector3 pos0;
          TVector3 dir0;
          double beta;
          static constexpr double errDirX = 0.000; // beam direction error
          static constexpr double errDirY = 0.000;
          static constexpr double errX = 0.001; // beam position error
          static constexpr double errY = 0.001;
          std::shared_ptr<TRandom3> randomGenerate;

        public:
          Beam(TVector3 pos0, TVector3 dir0, double beta);
          ARICHRECO::Particle* generateParticle();
        };
    }
#endif
