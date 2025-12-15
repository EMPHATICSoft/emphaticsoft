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
	std::vector<double> scoresLL;
	std::vector<double> scoresML;
	
	double GetScoreLL(int n){return scoresLL[n];};
	double GetScoreML(int n){return scoresML[n];};
};

}
#endif
