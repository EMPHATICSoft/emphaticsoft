#include "Hit.h"

namespace ssd {
  //--------------------------------------------------------------------
  Hit::Hit() : data   (0)
	     ,detId  (0) 
	     ,station(0)
	     ,plaq   (0)  
	     ,cid    (0)   
	     ,row    (0)   
	     ,col    (0)    
	     ,adc    (0)   
	     ,trig   (0)  
	     ,bco    (0)  
	       
  {
  }
  
  //--------------------------------------------------------------------
  Hit::~Hit()
  {
  }
  
} // end namespace ssd
