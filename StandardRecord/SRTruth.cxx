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
	SRTruth::SRTruth()
	{
		//setSRTruth(p,ptype,targetmaterial);
	}
	// SRTruth member function
	void SRTruth::setSRTruth(std::vector<float> p, std::string ptype, std::string targetmaterial)
	{
		T_p = p;
		T_ptype = ptype;
		T_targetmaterial = targetmaterial;
	}
	// SRTruth default constructor
	void SRTruth::setDefault()
	{
	}
}
