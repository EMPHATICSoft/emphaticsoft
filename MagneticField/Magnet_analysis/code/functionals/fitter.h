/*  L++ fitter
 *  Created by Leo Bellantoni on 11/2/09.
 *  Copyright 2009 FRA. All rights reserved.
 *
 *  To find the minimum of a function, and (under the assumption that it is
 *  quadratic near the minimum) also the covariance matrix.  Convention is that
 *  the function represents a chi-squared or -2*ln(likelyhood).  Limits on
 *  parameter ranges, non-quadratic minima or functions that are very expensive
 *  to evaluate are not handled here; for those cases you might do better with
 *  the ROOT package MINUIT (not MINUIT2, which is not ready for prime time even
 *  yet) or some other fitter.
 *
 *  A gradient search, a SIMPLEX method and a minimum-of-quadratic method are
 *  provided.  The gradient search uses with numeric derivatives after
 *  attempting to work out the optimum step size for finding the derivative;
 *  then it just steps along the gradient looking for a minimum.  The SIMPLEX
 *  algorithm is that of Nelder and Mead.  Both gradient and SIMPLEX track
 *  samples in a deque called prior; this is used to estimate the initial step
 *  size followed by finding the quadratic approximation for the minimum.  The
 *  gradient search can observe parameter bounds created with the setBounds
 *  method; but bounds testing only occurs in the step-along-gradient stage.
 *  Finally, the results of finding the parabolic uncertainties can be used to
 *  improve the found minimum.
 *
 *  The value of the covariance matrix can depend on the size of the region
 *  over which the chi-squared function is sampled around the minimum.  This
 *  routine samples over a region close in size to the volume spanned by
 *  +/- 0.01 sigma uncertainties.  This is close to what MINUIT does, which
 *  seems to reduce occurances of negative definite covariances from nonquadratic
 *  terms.  That value, 0.01, can be changed by altering fitter::errStep.
 *  The intent of the algorithm to find the covariances is to obtain, in the
 *  perfectly parabolic case, a resolution of 1 part in 33.
 *
 *  The function to be minimized must be implemented as the 
 *  operator()(ra<double>* X) method of a class that derives publically from the
 *  abstract base class functional.
 *
 */
#pragma once



#include <deque>
#include "ra.h"
#include "Lmat.h"
#include "minmax.h"
#include "stats.h"
#include "stringify.h"
#include "functional.h"





class fitter{
public:
	// Constructor takes function to be minimized and start point
    // Function is passed by reference to prevent slicing
	fitter(functional& chi2in, ra<double>* start);
	
	// Default, copy and assignment constructors
    fitter();
	fitter(fitter const& inFit);
	fitter& operator=(fitter const& rhs);

	// Destructor
	~fitter();
	
	// Individual variables may be fixed or frozen
	void useVar(int i);
	void fixVar(int i);
    bool isFree(int i);
    int  inBounds(int i, double v);     // -1(+1) for outside low(high) bound, 0 for in bounds

	// Set (or tweak) a new start point.
	void reset(ra<double>* Xin);
    void setVar(int i, double v);
	
	// Set verbosity; 0, 1 and 2 are valid.  Also a getter
	void makeGirl(int talkativeness);
    int getGirl() const;      // Who, you?  Fat chance.
    
    // plotfit classes need to look at number of both fixed and unfixed variables
    int Ndall();
    int Ndfree();
	
	// Set a new value for the UP parameter. The meaning of UP is that when
	// the chi-squared goes up to UP over the minimum, then you have the
	// assigned uncertainty.  For UP = 1.0, the default, your uncertainty is
	// 1 sigma confidence level in each parameter, regardless of (i.e. after
	// integrating over the pdfs of) the other parameters.  The setNsigma
	// methods and setCL methods allow you to set UP so that the enclosed
	// regions of the ellipse given by the covariance matrix contain some
	// number of standard deviation equivalents to, or some specific fraction
	// of the integrated p.d.f.
	void setUP(double UPin);
	void setCL(double CLin);
	void setNsigma(int Nsigin);
    
    // Getter functions for the results computed below.  getMinimum simply
    // returns the current estimate of the minimum; but getCovariance might
    // hurl if you didn't actually execute hesse().
    ra<double> getMinimum();
    double     getMinValue();
    ra<double> getCovariance();
    double     getVar(int i);

    // Setter/getters for bounds; values at low & high exactly are in bounds.
    void setBounds(int i, double  low, double  high);
    bool getBounds(int i, double& low, double& high);


	// Find minimum using SIMPLEX search
	void SIMPLEX(ra<double> initialErr);
    int const    maxNeval = 5000;         // Also used in hesse()
    double finalTolerance = 0.01;   // Converge chi2 to 0.01 absolute val



	// Find minimum using the gradient search
	void gradient();
    int const    maxStep = 10000;
    int const    maxScan = 400;



    // Compute covariance matrix - unused dimensions will be suppressed.
    // If called from a place where the chi2 function is not quadratic,
    // will throw an LppExcept which you could catch if you wanted to.
    // Will repeatedly use the results of the covariance calculation to
    // tweak up the estimate of the minimum.
	void hesse();
    // If the steps taken to determine the quadratic approximation to the
    // minimized function are comparable to the variation in the fit
    // parameters, one has in principle the best approximation to that
    // quadratic.  Because the function often isn't really quadratic, the
    // resulting approximation can result in a quadratic which is not
    // positive definite; this is unphysical, and the inverse of the
    // Hessian matrix, the covariance, will have negatives on the diagonal.
    // diagonal of the covariance matrix, i.e. imaginary uncertainties.
    // By adding a number just a little larger than the most negative
    // eigenvalue of the quadratic, one minimally pertubs the system and
    // gets physically possible uncertainties.  Unfortunately, they are
    // usually huge; fitter::forcedPositive is the flag for this unfortunate
    // state of affairs.
    // By making the steps much smaller, e.g. 1% of the full step, this
    // can be avoided.  To an extent.  Public, so that it can be tweaked.
    double errStep = 0.01;
    bool   forcedPositive;



    // epsD is 10x the smallest number eps such that 1+eps and 1-eps are both
    // represented as different from 1 when implemented in double precision.
    // epsP is a small but more practially sized number for determining
    // convergence and step sizes; it is basically a number on which scale
    // two quantities are physically (rather than computationally) the same.
    // These are used in gradient and hesse_once.
    double const epsD = 10*Deps;
    double const epsP = 1.0e-4;



private:
	int Ndim;
	functional& chi2;
	ra<double>* Xcurrent;
	ra<bool>*   Xuse;
    bool        boundsExist;
    ra<double>* XlowBounds;
    ra<double>* XhighBounds;

	ra<double>* covar;
	double		UP;
	int			verbose;
 

    // Method to flip a SIMPLEX point
    double amoeba(ra<double>* p, ra<double>* y, ra<double>* psum, int hi, double scale);

    // Method to find the hessian and invert it; retval means the stepsizes used
    // actually do span the minimum.
    bool hesse_once();
    // Method to tweak up the minimum in hesse; returns false if there was a problem
	bool improve();

	// Tracking previous samples requires a helper class
	class sample {
	public:
		double value;
		ra<double> point;
        sample(double v, ra<double> p) : value(v), point(p) {}
	};
	// Repeated calls to minimum() and covariance() should not erase prior
	std::deque<sample> prior;

	// Save the quadratic approximation to the chi-square between covariance()
	// and improve().  These will be Ndim - sized, regardless of Xuse.
	ra<double>* qwd;		ra<double>* lin;
    
    // In at least one constructor, I need an instance of a functional
    class derivedFunctional : public functional {
        double operator()(ra<double>* X) {return 0;}
    };
    derivedFunctional nullFunctional;
};
