////////////////////////////////////////////////////////////////////////
/////// \brief   PARTICLE class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////



#include "Particle.h"

    namespace ARICHRECO{
    

        Particle::Particle(TVector3 pos, TVector3 dir, double beta) {
          this->pos0 = pos;
          this->dir0 = dir;
          this->pos = pos;
          this->dir = dir;
          this->beta = beta;
        }

        double Particle::theta() {
          return atan(sqrt(dir[0]*dir[0] + dir[1]*dir[1]) / dir[2]);
        }

        double Particle::phi() {
          return atan(dir[0] / dir[1]);
        }

        double Particle::dist(double z) {
          // distance to some z position in direction of travel
          return (z - pos[2]) / dir[2];
        }

        void Particle::travelDist(double dist) {
          // Travel some distance in the direction of travel
          pos[0] = pos[0] + dist*dir[0];
          pos[1] = pos[1] + dist*dir[1];
          pos[2] = pos[2] + dist*dir[2];
        }

        void Particle::travelZDist(double zDist) {
          // Travel some distance downstream
          pos[0] = pos[0] + zDist*dir[0]/dir[2];
          pos[1] = pos[1] + zDist*dir[1]/dir[2];
          pos[2] = pos[2] + zDist;
        }
    }
