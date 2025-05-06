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

      //SRPID::SRPID(float _scores[5]){
	//for(int t=0; t < 5;t++){scores[t] = _scores[t];}
      //}
	
      void SRPID::Reset(){
	scores.clear();
	}
}
