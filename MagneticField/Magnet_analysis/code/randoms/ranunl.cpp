/*  L++ ranunl
 *  Created by Leo Bellantoni on 1/10/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A simple random number generator, using the "minimal standard"
 *  multiplicative congruential algorithm of Park & Miller.  See PTV&F, pg 269.
 *  For the full-glorious RANLUX, see CLHEP.
 *
 */
 #include <cassert>
#include "randoms.h"
using namespace std;



// Construct a generator object using system clock for seed; you may want to
// make the created object static in some cases because system time has poor
// resolution and frequent repeated calls to the constructor here can produce 
// identical random sequences.  See RAMBO.cpp.
ranunlgen::ranunlgen() {
	setseed( (int46) time(nullptr) );
    assert( 8*sizeof(int46) > 46 );
}

// Construct a generator object using a seed.  Default destructor should be OK.
ranunlgen::ranunlgen(int46 seedy) {
	setseed ( seedy );
}



// Get next number from the sequence; stilldum is to foil optimization of
// e.g. xp(Rx*ranunl(1),2) + xp(Ry*ranunl(2),2) into a single call to ranunl.
double ranunlgen::ranunl(int stilldum) {
	int46 dumfk = a*lastRN;
	lastRN = (dumfk)%m;
	return double(lastRN)/dm;
}



void ranunlgen::setseed(int46 seedy) {
	// These values for a,m are the default generator in many F90/F95
	// implementations
	lastRN = seedy;
	a      = 16807;
	m      = (int46(1) << 31) - 1;
	dm     = m;
	return;
}
