////////////////////////////////////////////
// \file SRTruth.h
// \brief SRTruth will keep the true information 
// from simulation 
////////////////////////////////////////////

#ifndef SRTRUTH_H
#define SRTRUTH_H

#include <vector>
#include <string>
#include "Math/Vector3D.h"
#include "SRTrueParticle.h"
#include "SRTrueSSDHitsBranch.h"


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
      ROOT::Math::XYZVector   posUSTarget; // position vector upstream of target
      ROOT::Math::XYZVector   momUSTarget; // momentum vector upstream of target
      std::vector<ROOT::Math::XYZVector>   posDSTarget; // position vector downstream of target
      std::vector<ROOT::Math::XYZVector>   momDSTarget; // momentum vector downstream of target

//	  SRTrueSSDHitsBranch truehits; ///< SSD True Hits branch: 
	 
	  SRTruth(); 			// Default constructor
	  ~SRTruth() {};
	}; //end class
}
#endif //SRTRUTH_H
