////////////////////////////////////////////
// \file SRTruth.h
// \brief SRTruth will keep the true information 
// from simulation 
////////////////////////////////////////////

#ifndef SRTRUTH_H
#define SRTRUTH_H

#include <vector>
#include <string>
#include "SRParticle.h"

///Common Analysis Files
namespace caf
{
	class SRTruth
	{
	public:
	  SRParticle beam;		// this will hold all the particles generated during simulation
	  std::vector<caf::SRParticle> particles;

	  std::string trgt;
	  float trgt_thick;
	  float trgt_dens;
	 
	  SRTruth(); 			// Default constructor
	  ~SRTruth() {};
	}; //end class
}
#endif //SRTRUTH_H
