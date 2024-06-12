/*  L++ ranorm
 *  Created by Leo Bellantoni on 1/11/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A normally distributed random number generator.
 *
 */
#include "randoms.h"


// Construct a generator using system clock for seed.  You may want to
// make the created object static in some cases.  See ranunl.cpp, RAMBO.cpp.
ranormgen::ranormgen() :
	engine(), dun(false) {
}

// Construct a generator using a seed
ranormgen::ranormgen(int46 seedy) :
	engine(seedy), dun(false) {
}


// Pull another number from the sequence; stilldum is to foil optimization of
// e.g. xp(Rx*ranorm(1),2) + xp(Ry*ranorm(2),2) into a single call to ranorm.
// Public domain algorithm
double ranormgen::ranorm(int stilldum) {
	double fac, V1, V2, r;
	if (dun) {
		dun = false;
		return onhand;
	}
	do {
		V1 = 2.0*engine.ranunl(0) -1.0;
		V2 = 2.0*engine.ranunl(1) -1.0;
		r  = V1*V1 + V2*V2;
	} while (r >= 1.0);
	fac = sqrt(-2.0*log(r)/r);
	onhand = fac*V1;
	dun = true;
	return   fac*V2;
}
