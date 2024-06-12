/*  L++ ranpoi
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
ranpoigen::ranpoigen(double p) :
	engine(), param(p) {
}

// Construct a generator using a seed
ranpoigen::ranpoigen(double p, int46 seedy) :
	engine(seedy), param(p) {
}


// Pull another number from the sequence.  Much modified from the F77 version
// of Press, Flannery, Teukolsky & Vetterling.
int ranpoigen::ranpoi(int stilldum) {
	double em,t,y;
	static double alxm, g, sq;

	if (param < 12.0) {
		g    = exp(-param);
		em = -1.0;
		t  =  1.0;
		do {
			em = em +1.0;
			t  = t*engine.ranunl(0);
		} while (t > g);
	} else {
		g    = param*alxm - lngamma(param+1.0);
		sq   = sqrt(2.0*param);
		alxm = log(param);

		spagetti:
		do {
			y  = tan(pie*engine.ranunl(1));
			em = sq*y + param;
		} while (em <0.0);
		em = fint(em);
		t = 0.9 *(1.0 +y*y) *exp(em*alxm -lngamma(em+1.0) -g);
		if (engine.ranunl(2) > t) goto spagetti;
	}
	return fint(em);
}
