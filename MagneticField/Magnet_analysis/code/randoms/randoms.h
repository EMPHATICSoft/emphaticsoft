/*  L++  randoms
 *  Created by Leo Bellantoni on 1/13/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  Header file for ranunl, ranorm, ranpoi, ranbin, ranlandau.  Shared as all
 *  but ranunl contain a ranunlgen.
 *
 *  All three classes are OK with default destructors, copy and assignment
 *  constructors - so they are not defined.
 *
 *	HISTORY:
 *	2 Sep 2008	Added a binomial distribution generator.  Be aware that poisson,
 *				normal, and binomial generators in particular will be limited in
 *				their ability to model tails of these distributions simply
 *				because they (by definition of their functionality) sample those
 *				tails infrequently.
 *  24 Oct 2016 Added a Landau distribution generator, based on the ROOT
 *              implementation of the old CERNLIB code dislan, taken from
 *              K.S.Kolbig and B.Schorr, "A program package for the Landau
 *              distribution", Computer Phys.Comm., 31 (1984), 97-111.  Also,
 *              maybe even more importantly, a Landau distribution function for
 *              fitting or whatever.
 *
 */
#pragma once



#include <cmath>
#include "minmax.h"
#include "stats.h"



typedef int64_t int46;    // Must cover 0 to 2^46
class ranunlgen{			    // Uniform random numbers
public:
	// Construct a generator using system clock for seed.
	ranunlgen();
	// Construct a generator using a seed.
	explicit ranunlgen(int46 seedy);

	// Get next number from the sequence; stilldum is to foil optimization of
	// e.g. xp(Rx*ranunl(1),2) + xp(Ry*ranunl(2),2) into a single call to ranunl.
	double ranunl(int stilldum);

private:
	// Seed and algorithm values in here
	int46 lastRN, a, m;         // a and m are in fact, 32 bit numbers but are
	double dm;                  // implemented as int46
	void setseed(int46 seedy);
};



class ranormgen{			// Normal random numbers
public:
	// Construct a generator using system clock for seed
	ranormgen();
	// Construct a generator using a seed
	explicit ranormgen(int46 seedy);

	// Pull another number from the sequence
	double ranorm(int stilldum);

private:
	ranunlgen  engine;
	bool       dun;
	double     onhand;
};



class ranpoigen{			// Poisson-distributed random numbers
public:
	// Construct a generator using system clock for seed
	explicit ranpoigen(double param);
	// Construct a generator using a seed
	ranpoigen(double param, int46 seedy);

	// Pull another number from the sequence
	int ranpoi(int stilldum);

private:
	ranunlgen  engine;
	double     param;
};



class ranbingen{			// Binomial-distributed random numbers
public:
	// Construct a generator using system clock for seed
	explicit ranbingen(double p, int N);
	// Construct a generator using a seed
	ranbingen(double p, int N, int46 seedy);

	// Pull another number from the sequence
	int ranbin(int stilldum);

private:
	ranunlgen  engine;
	double     p_;
	int		   N_;
};



class ranlandaugen{			// Landau-distributed random numbers
public:
    // Construct a generator using system clock for seed
    explicit ranlandaugen(double mpv, double FWHM=hiHalf-loHalf);
    // Construct a generator using a seed
    ranlandaugen(double mpv, double FWHM, int46 seedy);
    
    // Pull another number from the sequence
    double ranlandau(int stilldum);
    
    // A Landau distribution itself
    double landau_pdf(double z);
    
    // A few important parameters:  the most probable value of the raw distribution
    double static constexpr rawMPV = -0.22278;
    // The lower and upper FWHM points for a distribution with MPV = 0
    double static constexpr loHalf = -1.36376;
    double static constexpr hiHalf = +2.65489;
    
private:
    ranunlgen  engine;
    double     inter_;
    double	   slope_;
    double     landau_quantile(double x);
};
