#ifndef SRPID_H
#define SRPID_H

#include <cmath>
#include <iostream>
#include <vector>

namespace caf 
{

 // making a class that handles PID 
	
 class SRPID {

   public:
	
	SRPID();
      virtual ~SRPID();
	
	
	SRPID(float scores[5]);	
	SRPID(std::vector<double> scores_);
	void Reset();
	
	float scores[5];
	std::vector<double> scores_vec;
	float operator [](int n){return scores[n];};

};

}
#endif