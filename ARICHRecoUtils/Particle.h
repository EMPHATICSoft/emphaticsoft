
////////////////////////////////////////////////////////////////////////
/////// \brief   Definition of PARTICLE class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_INCLUDE
#define PARTICLE_INCLUDE

#include <iostream>
#include "TVector3.h"



namespace arichreco{

        class Particle {
        public:
          Particle(TVector3 pos, TVector3 dir, double beta);
          TVector3 pos0;
          TVector3 dir0;
          TVector3 pos;
          TVector3 dir;
          double beta;
          double theta();
          double phi();
          double dist(double);
          void travelDist(double);
          void travelZDist(double);
        	
	  inline friend std::ostream& operator<<(std::ostream& os, const Particle& particle) {
		os << "Particle : ( dir:("  << particle.dir.X() << ", " <<  particle.dir.Y() << ", " <<  particle.dir.Z();
		os << "), pos:(" << particle.pos.X() << ", " << particle.pos.Y() << ", " << particle.pos.Z();
		os << "), beta: " << particle.beta << ")";
		return os;
      		}

	};
    }
#endif
