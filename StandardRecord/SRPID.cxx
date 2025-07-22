#include "StandardRecord/SRPID.h"

#include <cmath>
#include <limits>



namespace caf{


      SRPID::SRPID()
     {
	trackID = -1;
	for(int t=0; t < 5;t++)scores.push_back(-1);
	}
	


      SRPID::~SRPID(){}

	
      void SRPID::Reset(){
	scores.clear();
	}
}
