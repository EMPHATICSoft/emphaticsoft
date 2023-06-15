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
	  std::string T_ptype = {};	// Do we use PDG particle numbering?
	  std::string T_targetmaterial = {};
	
	  SRTruth(std::vector<float> p, std::string ptype,std::string targetmaterial);		// Construct an SRTruth object
	  void setSRTruth(std::vector<float> p, std::string ptype, std::string targetmaterial); 	// SRTruth member function
										// TODO: add functions to get the individual components of SRTruth
	}; //end class
}
#endif //SRTRUTH_H
