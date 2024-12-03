/*  L++ poiprob
 *  Created by Leo Bellantoni on 10/29/10.
 *  Copyright 2010 FRA. All rights reserved.
 *
 *  A class to compute the probability of observing Nobs or more events for
 *  a Poisson distributed variable with parameter X that might be known only
 *  to a Gaussian-distributed uncertainty dX.
 *
 */
#include "poiprob.h"
using namespace std;


// Helper classes --------------------------------------------------------------
PPint::PPint(int Nobs, double X, double dX) :
	X__(X),	dX__(dX),	Nobs__(Nobs) {}

double PPint::operator()(double x) {
	double poipart;
	if (Nobs__ >= 1) {
		poipart = 1.0 - poisum(Nobs__ -1, x);
	} else
	if (Nobs__ == 0) {
		poipart = 1.0;	// Trying to sum the whole distribution
	} else {
		LppUrk.LppIssue(0,"PPint::operator(double)");
	}
	double gaupart = exp( -0.5 *xp((x-X__)/dX__,2) )/ (r2pi*dX__);
	return poipart*gaupart;
}


// Main class to set the limit -------------------------------------------------
// Constructor; copy, assignment constructors and destructors are default.
poiprob::poiprob(int Nobs, double X, double dX) :
	X_(X),	dX_(dX),	Nobs_(Nobs)	{}


// Method to evaluate the limit
double poiprob::findOdds() {
	if (dX_ == 0.0) {
		// Do the straight evaluation
		return 1.0 - poisum(Nobs_ -1,X_);
	} else {
		ra<double> integral(2);
		double lo, hi;
		lo = max(0.0, X_ -Nsig*dX_);
		hi = X_ +Nsig*dX_;
		PPint WolfMan(Nobs_,X_,dX_);
		integral = integrateObj(WolfMan, lo,hi, Nloop);
		return integral(1);
	}
}
