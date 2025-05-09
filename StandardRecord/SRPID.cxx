#include "StandardRecord/SRPID.h"

#include <cmath>
#include <limits>



namespace caf{


      SRPID::SRPID()
     {
	track_id = -1;
	for(int t=0; t < 5;t++)scores.push_back(-1);
	}
	


      SRPID::~SRPID(){}

	
      void SRPID::Reset(){
	scores.clear();
	}
}
