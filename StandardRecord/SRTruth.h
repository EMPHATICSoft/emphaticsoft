////////////////////////////////////////////
// \file SRTruth.h
// \brief SRTruth will keep the true information 
// from simulation 
////////////////////////////////////////////

#ifndef SRTRUTH_H
#define SRTRUTH_H

#include <vector>
#include <string>
#include "SRTrueParticle.h"

///Common Analysis Files
namespace caf
{
	class SRTruth
	{
	public:
	  SRTrueParticle beam;		// this will hold all the particles generated during simulation, beam and daughters

	  std::string trgt;
	  float trgt_thick;
	  float trgt_dens;
	 
	  SRTruth(); 			// Default constructor
	  ~SRTruth() {};
	}; //end class
}
#endif //SRTRUTH_H
