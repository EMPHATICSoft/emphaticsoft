#include "TrackTools.h"
#include <math.h>
#include <stdlib.h>
using namespace TrackTools;

double Cluster::GetPosition(Coordinate c) const{
	/*if(i < 0 || i > 2){
		cerr << __FUNCTION__ << ": Bad position index " << i << ". Index can be 0, 1, 2! Exiting..." << endl;
		exit(EXIT_FAILURE);
	}*/
	
	return fPos[c];
}

double Cluster::GetError(CorError c) const{
	/*if(i < 0 || i > 1){
		cerr << __FUNCTION__ << ": Bad error index " << i << ". Index can be 0, 1! Exiting..." << endl;
		exit(EXIT_FAILURE);
	}*/
	
	return fError[c];
}
