////////////////////////////////////////////////////////////////////////
// \file    SRARing.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRARing.h"

namespace caf
{
  SRARing::SRARing()
  {
  	center[0] = 0;
	center [1] = 0;
	radius = -1;
  }

  //--------------------------------------------------------------------
  SRARing::~SRARing()
  {
  }

  //--------------------------------------------------------------------
  void SRARing::setDefault()
  {
        center[0] = 0;  
        center [1] = 0;
	radius = -1;
	
  }
 

} // end namespace
