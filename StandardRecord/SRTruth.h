////////////////////////////////////////////
// \file SRTruth.h
// \brief SRTruth will keep the true information 
// from simulation 
////////////////////////////////////////////

#ifndef SRTRUTH_H
#define SRTRUTH_H

#include <vector>
#include <string>

///Common Analysis Files
namespace caf
{
	class SRTruth
	{
	public:
	  float pbeam[3];
	  float xbeam[3];
	  int beampid;
	  std::string trgt;

	  float trgt_thick;
	  float trgt_dens;
	 
	  SRTruth(); 			// Default constructor
	  ~SRTruth() {};
	}; //end class
}
#endif //SRTRUTH_H
