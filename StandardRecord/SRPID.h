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
	int trackID;
	//SRPID(float scores[5]);	
	void Reset();
	
	//float scores[5];
	std::vector<double> scores;
	float operator [](int n){return scores[n];};

};

}
#endif
