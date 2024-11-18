/*  L++ fullim
 *  Created by Leo Bellantoni on 5/12/09.
 *  Copyright 2009 FRA. All rights reserved.
 *
 *  A class to set Baysian style limits for a single experiment where there
 *  are Gaussian background and sensitivity PDFs.  The algorithm combines the
 *  methods of Cousins & Highland, NIM A320 (1992) 331 and Zech, NIM A277
 *  (1989) 608.  This is your basic Baysian analysis:
 *  Prob(x|y)*Prob(y) = Prob(x&y) where x = "signal is above such & such" and
 *  y = "background contributed such & such".  The following expression has to
 *  be solved for Ms:
 *
 *	            Prob that Poisson of (Mb+Ms) sees <= Nobs events
 *     1 - CL = ------------------------------------------------
 *	             Prob that Poisson of (Mb) sees <= Nobs events
 *
 *  Where Mb and Ms are the Poisson parameters for the background and signal
 *  processes.  When there is a distribution for the background estimate, both
 *  numerator and denominator must be multiplied by the and integrated over the
 *  probability density function for Mb; when there is uncertainty in the
 *  sensitivity, Ms must be written as xi*Mo, where xi is a number with some
 *  probability density function that has a mean of 1.  The numerator again
 *  must then be integrated over xi, weighted by its probability density.
 *  Then Mo is the quantity to be solved for, rather than Ms.
 *
 */
#pragma once



#include "stats.h"
#include "integrator.h"		// has ra.h
#include "poisum.h"
// Should really restructure helper classes into main class so that these
// numbers, which are needed by the helper classes can become double const
// in public scope of fullim class.

// How many sigma to run the numeric integrals out to?
double const fullim_Nsig = 3.5;
// How many iterative loops in the numeric integration
int const   fullim_Nloop = 7;
    




// Helper classes and functions ------------------------------------------------
class unorm {
public:
	double operator()(double x) { return exp(-0.5*x*x); };
};

class Tgauss {
public:
	// Default destructor OK, don't need assignment or copy constructors
	Tgauss(double mean, double width);
	double operator() (double x);
	double getmean();
	double getwidth();
private:
	double mean_, width_, norm_;
	unorm fred_;
};

// Integrand in denominator
class DNint {
public:
	DNint(int Nobs, Tgauss backPDF);
	double operator() (double muB);
	double get_bkg();
	double get_d_bkg();
private:
	int No_;
	Tgauss backPDF_;
};

// Inner integral in numerator is over muB
class NMinner {
public:
	NMinner(int Nobs, Tgauss backPDF);
	double operator() (double muB);
	void set_xiMu(double xM);
private:
	int No_;
	Tgauss backPDF_;
	double xiMU_;
};

// Integrand in numerator.  Outer integral is over xi
class NMint {
public:
	NMint(int Nobs, Tgauss backPDF, Tgauss sigPDF, double muS = 0.0);
	double operator() (double xi);
	void   set_muS(double mu);
	double get_bkg();
	double get_d_bkg();
	double get_d_sig();
private:
	int No_;
	Tgauss backPDF_, sigPDF_;
	double muS_;
	NMinner inner_;
};


// Main class to set the limit -------------------------------------------------
class fullim {
public:
	// Constructor; copy, assign constructors and destructors are default.
	fullim(int Nobs, double CL, double bkg,
		   double d_bkg=0.0001, double d_sig=0.0001);
	// Method to evaluate the limit in terms of max number of events
	double setlim();
 
private:
	double CL_;
	NMint outer_;
	DNint dener_;
};
