////////////////////////////////////
// \file SRTruth.cxx
// \brief 
////////////////////////////////////

#include "SRTruth.h"
#include <vector>
#include <string>

namespace caf
{
	// default SRTruth constructor, won't do anything
	SRTruth::SRTruth(): pbeam{0,0,0} // defaultly construct the beam particle momentum to be zero
	{
	 beampid = 2212,  // defaultly construct the beam particle id to be a proton
	 trgt = "C",	 // default target, not sure whether to use "" like that?
	 trgt_dens = 1;  // default target density 
	}
}
