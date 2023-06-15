////////////////////////////////////
// \file SRTruth.cxx
// \brief 
////////////////////////////////////

#include "SRTruth.h"
#include <vector>
#include <string>

namespace caf
{
	// SRTruth constructor
	SRTruth::SRTruth(std::vector<float> p, std::string ptype, std::string targetmaterial)
	{
		setSRTruth(p,ptype,targetmaterial);
	}
	// SRTruth member function
	void SRTruth::setSRTruth(std::vector<float> p, std::string ptype, std::string targetmaterial)
	{
		T_p = p;
		T_ptype = ptype;
		T_targetmaterial = targetmaterial;
	}
}
