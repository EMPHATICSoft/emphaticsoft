#include "StandardRecord/SRPID.h"

#include <cmath>
#include <limits>



namespace caf{

      SRPID::SRPID(){}
      SRPID::~SRPID(){}

      SRPID::SRPID(float _scores[5]){
	for(int t=0; t < 5;t++){scores[t] = _scores[t];}
      }

      SRPID::SRPID(std::vector<double> scores_){
        for(size_t l=0; l < scores_.size();l++){scores_vec.push_back(scores_[l]);}	
      }
	
      void SRPID::Reset(){
	for(int t=0; t < 5;t++){scores[t] = -1.;}	
	}
}
