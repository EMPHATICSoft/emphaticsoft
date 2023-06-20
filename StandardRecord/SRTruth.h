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
	  std::vector<float> T_p = {};	// Maybe this should be a set size...
	  int  T_ptype = {};	// Do we use PDG particle numbering? The answer is yes, so make int
	  std::string T_targetmaterial = {};
	 
	  SRTruth(); 			// Default constructor
	}; //end class
}
#endif //SRTRUTH_H
