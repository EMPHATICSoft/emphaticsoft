////////////////////////////////////
// \file SRTruth.cxx
// \brief Contains the default constructor for the StandardRecord Truth member
////////////////////////////////////

#include "SRTruth.h"
#include <vector>
#include <string>

namespace caf
{
	// default SRTruth constructor
	SRTruth::SRTruth(): pbeam{0,0,0}
	{
	 beampid = 2212,  // default beam is proton
	 trgt = "C",	 // default target is Carbon
	 trgt_dens = 1,  // default target density
	 trgt_thick = 20; // default target thickness 
	}
}
