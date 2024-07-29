/*  L++ fullim
 *  Created by Leo Bellantoni on 5/12/09.
 *  Copyright 2009 FRA. All rights reserved.
 *
 *  A class to set Baysian style limits for a single experiment where there
 *  are Gaussian background and sensitivity PDFs.
 *
 */
#include "fullim.h"
using namespace std;


// Helper classes for the PDFs -------------------------------------------------
// Constructors, destructors.  width <= 0 creates a Dirac delta function
// Default destructor OK, don't need assignment or copy constructors
Tgauss::Tgauss(double mean, double width) :
	mean_(mean),
	width_(width) {
	if (mean_ < 0) {
		LppUrk.LppIssue(0,"Tgauss::Tgauss(double,double)");
	}
	ra<double> integral(2);
	integral = integrateObj(fred_,-min(fullim_Nsig,mean_/width_),+fullim_Nsig);
	norm_ = integral(1)*width_;
	return;
}

// The evaluation method
double Tgauss::operator() (double x) {
	double X = (x - mean_) / width_;
	if (x > 0) {
		return fred_(X) / norm_;
	} else {
		return 0;
	}
}

// A couple of getter functions
double Tgauss::getmean() {
	return mean_;
}
double Tgauss::getwidth() {
	return width_;
}


// Helper classes for the integrated Poisson sums ------------------------------
// Integrand in denominator - one constructor, an evaluation function
DNint::DNint(int Nobs, Tgauss backPDF) :
	No_(Nobs),
	backPDF_(backPDF) {}
double DNint::operator()(double muB) {
	return backPDF_(muB)*poisum(No_,muB);
}
double DNint::get_bkg() {
	return backPDF_.getmean();
}
double DNint::get_d_bkg() {
	return backPDF_.getwidth();
}

// Outer integral in numerator, over xi
NMint::NMint(int Nobs, Tgauss backPDF, Tgauss sigPDF, double muS) :
	No_(Nobs),
	backPDF_(backPDF),
	sigPDF_(sigPDF),
	muS_(muS),
	inner_(Nobs,backPDF) {}

double NMint::operator()(double xi) {
	ra<double> integral(2);
	double lo = max(0.0, backPDF_.getmean() -fullim_Nsig*backPDF_.getwidth());
	double hi =			 backPDF_.getmean() +fullim_Nsig*backPDF_.getwidth();
	inner_.set_xiMu(muS_*xi);
	integral = integrateObj(inner_, lo,hi, fullim_Nloop);
	return sigPDF_(xi)*integral(1);
}

void NMint::set_muS(double mu) {
	muS_ = mu;
}
double NMint::get_bkg() {
	return backPDF_.getmean();
}
double NMint::get_d_bkg() {
	return backPDF_.getwidth();
}
double NMint::get_d_sig() {
	return sigPDF_.getwidth();
}

// Inner integral
NMinner::NMinner(int Nobs, Tgauss backPDF) :
	No_(Nobs),
	backPDF_(backPDF) {}
double NMinner::operator()(double muB) {
	return backPDF_(muB)*poisum(No_,xiMU_+muB);
}
void NMinner::set_xiMu(double xM) {
	xiMU_ = xM;
}


// Main class to set the limit -------------------------------------------------
// Constructor; copy, assignment constructors and destructors are default.
fullim::fullim(int Nobs, double CL, double bkg, double d_bkg, double d_sig) :
	CL_(CL),
	outer_(Nobs, Tgauss(bkg,d_bkg), Tgauss(1.0,d_sig)),
	dener_(Nobs, Tgauss(bkg,d_bkg)) {}

// Method to evaluate the limit
double fullim::setlim() {
	// retval is changed by delval on each iteration until muS converges
	double retval = 3.0, delval = 3.0;
	// Numerator and denominator in the basic formula	
	double numerator, denominator;
	ra<double> integral(2);
	double lo, hi;

	// Denominator is single integral over muB
	lo = max(0.0, dener_.get_bkg() -fullim_Nsig*dener_.get_d_bkg());
	hi =		  dener_.get_bkg() +fullim_Nsig*dener_.get_d_bkg();
	integral = integrateObj(dener_, lo,hi, fullim_Nloop);
	denominator = integral(1);

	while (delval/retval > 0.0001) {
		outer_.set_muS(retval);
		// Numerator is formed by (outer) integration over xi
		lo = max(0.0, 1.0 -fullim_Nsig*outer_.get_d_sig());
		hi =          1.0 +fullim_Nsig*outer_.get_d_sig();
		integral  = integrateObj(outer_, lo,hi, fullim_Nloop);
		numerator = integral(1);
		// This looks like a mis-typed binary search algorithm but it isn't.
		// For correct convergence values much above the initial guess, you
		// need to be able to increase delval somehow
		if (numerator/denominator < (1.0 - CL_)) {
			delval /= 2.0;
			retval -= delval;
		} else {
			delval *= 2.0;
			retval += delval;
		}
	}
	return retval;
}
