#include "StandardRecord/SRPID.h"

#include <cmath>
#include <limits>



namespace caf{


      SRPID::SRPID()
     {
	trackID = -1;
	for(int t=0; t < 3;t++){scoresLL.push_back(-1); scoresML.push_back(-1);}
	}
	


      SRPID::~SRPID(){}

	
      void SRPID::Reset(){
	scoresLL.clear();
	scoresML.clear();
	}
}
