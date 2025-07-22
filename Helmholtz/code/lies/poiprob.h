/*  L++ poiprob
 *  Created by Leo Bellantoni on 10/29/10.
 *  Copyright 2010 FRA. All rights reserved.
 *
 *  A class to compute the probability of observing Nobs or more events for
 *  a Poisson distributed variable with parameter X that might be known only
 *  to a Gaussian-distributed uncertainty dX.
 *
 */
#pragma once



#include "stats.h"
#include "integrator.h"
#include "poisum.h"





// Helper classes and functions ------------------------------------------------
// Integrand class
class PPint {
public:
	PPint(int Nobs, double X, double dX);
	double operator()(double x);
private:
	double X__,dX__;
	int Nobs__;
};


// Main class ------------------------------------------------------------------
class poiprob {
public:
	// Constructor; copy, assign constructors and destructors are default.
	poiprob(int Nobs, double X, double dX=0.0);
	// Method to compute the probability
	double findOdds();
private:
	double X_, dX_;
	int Nobs_;
    // How many sigma to run the numeric integrals out to?
    double const Nsig = 3.5;
    // How many iterative loops in the numeric integration
    double const Nloop = 7;
};
