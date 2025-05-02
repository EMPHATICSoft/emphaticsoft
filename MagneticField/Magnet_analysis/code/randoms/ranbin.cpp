/*  L++ ranbin
 *  Created by Leo Bellantoni on 1/13/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A Poisson distributed random number generator.
 *
 */
#include "randoms.h"


// Construct a generator using system clock for seed.  Gotta specify the
// parameter of the distribution too!  You may want to make the created 
// object static in some cases.  See ranunl.cpp, RAMBO.cpp.
ranbingen::ranbingen(double p, int N) :
	engine(), p_(p), N_(N) {
}

// Construct a generator using a seed
ranbingen::ranbingen(double p, int N, int46 seedy) :
	engine(seedy), p_(p), N_(N) {
}


// Pull another number from the sequence.  Much modified from the F77 version
// of Press, Flannery, Teukolsky & Vetterling.
int ranbingen::ranbin(int stilldum) {
	int retval;
	double q,em,t,y,sq;
	if (p_>0.5) {
		q = 1.0 - p_;
	} else {
		q = p_;
	}
	// Admittedly, it would make more sense to put this test, along with
	// calculation of sq in constructor; but there seems to be a bug where
	// somehow p_ is garbled.
	if ( (N_*p_)<0.01 || (N_*p_)<0.01 ) {
		LppUrk.LppIssue(104,"ranbingen::ranbin(int)");
	}
	
	if (N_<25) {
		// For small enough values, do it directly
		retval = 0;
		for (int j=1; j<=N_; ++j) {
			// stilldum prevents optimization into a single engine.ranul() call
			if (engine.ranunl(j)<q) ++retval;
		}
	} else {
		// Rejection method; skip NR's Poisson approximation
		sq = sqrt(2.0* N_*q *(1-q));
		do {
			do {
				y  = tan(pie*engine.ranunl(1));
				em = sq*y + N_*q;
			} while ( em<0 || em>=(N_+1) ); 
			em = (int) em; // Truncate to integer
			t  = 1.2*sq*(1+y*y) *exp(
				 lngamma(N_+1)      -lngamma(em+1.0)
				-lngamma(N_-em+1.0) +em*log(q) +(N_-em)*log(1-q)  );
		} while (engine.ranunl(2) > t);
		retval = em;
	}
	if (p_>0.5) {
		return N_-retval;
	} else {
		return retval;
	}
}
