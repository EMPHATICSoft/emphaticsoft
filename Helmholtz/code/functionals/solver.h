/*  L++ root finder
 *  Created by Leo Bellantoni on 2/6/07.
 *  Copyright 2007 FRA. All rights reserved.  Well, those that exist I guess.
 *  This might be from John J Barton's book.
 *
 * This template is a reasonably idiot-proof way to find the zero of a C++
 * function object, i.e. an object of a class in which the () operator has
 * been overloaded to be a function call.  Uses the secant method.
 * INPUTS:
 *		lo,hi   	- Initial guesses for root.
 *			    	  If lo=hi, sets hi to lo +0.001
 *		Yacc        - Convergence is when function is limited to +/-Yacc,
 *			    	  or if you run out of iterations, 100 by default)
 *      verbosity   - 1 for initial values only; 2 for all samples
 *
 * It is not true that lo < hi at all points in execution.  However,
 * retval is typically the best estimate of the root.
 *
 * Example:
 * class Gold {
 *    public: double operator()(double x) { return x*x -x -1; };
 * };
 * Gold fools;
 * cout << solver(fools, 1.6, 1.13, 1.0e-6) << endl;
 *
 * WARNING: the function object is call-by-value; no side effects of the
 * solve process will be retained in it!
 *
 */
#pragma once



#include "LppGlobals.h"
#include "ra.h"
#include "minmax.h"





template<class Fo> double solver(Fo f, double lo,double hi, double Yacc,
                                 int verbosity=0, int NiterMax=100) {
	int Ntry;
	double fL,fH,fR, df,retval;

	if (lo==hi) hi = lo +0.001;
	for (int Niter=0; Niter<NiterMax; ++Niter) {
		// Intersecting secant might take a few iterations; counter is Ntry
		Ntry = 0;
		fL = f(lo);   // <======  Here is where the function is 1st called
		nudgehi:
        fH = f(hi);
        if (verbosity>0 && Niter==0) {
            std::cout << "solver: Initial samples, possibly with nudges" << std::endl;
            printf("% 20.12e\t% 20.12e\n",lo,fL);
            printf("% 20.12e\t% 20.12e\n",hi,fH);
        }
        if (verbosity>1 && !(verbosity>0 && Niter==0)) {
            printf("solver: Sample at % 20.12e\t% 20.12e\n",lo,fL);
            printf("solver: Sample at % 20.12e\t% 20.12e\n",hi,fH);
        }
		// Check your lucky guesses first
		if (fabs(fL) < Yacc) {
            if (verbosity>0) {
                printf("solver: Sample at % 20.12e\t% 20.12e\n",retval,fR);
            }
            return lo;
        }
		if (fabs(fH) < Yacc) {
        if (verbosity>0) {
            printf("solver: Sample at % 20.12e\t% 20.12e\n",retval,fR);
            }
            return hi;
        }
		df = (fH-fL)/(hi-lo);       // What to do about roundoff here?
		if ((df != 0.0) && (fabs(fH-fL) > Yacc)) {
			retval = lo - (fL/df) / gold;
		} else if (Ntry>NiterMax) {
			LppUrk.LppIssue(201,"solver::solver(Fo,double,double,double...) [1]");
			return 0.5*(lo+hi);
		} else {
			Ntry +=    1;
			hi   +=0.001;
			goto nudgehi;
		}

		// If derivative df is small, could wind up far far away.  Fix that
		// before checking convergence.
		if ( fabs(retval-lo) > 100.0*fabs(hi-lo) ) {
			// that was just too big a step!
			retval = lo + 0.1*(retval-lo);
		}

		// Check convergence
		if ( fabs(fR=f(retval)) < Yacc) {
            if (verbosity>0) {
                printf("solver: Sample at % 20.12e\t% 20.12e\n",retval,fR);
            }
			return retval;		// Happiness returning
		}
        if (verbosity>1) {
            printf("solver: Sample at % 20.12e\t% 20.12e\n",retval,fR);
        }


		// Decision branches for next secant computation
		if ( fabs(fR) < fmin(fabs(fL),fabs(fH)) ) {
			// fR = f(retval) is closest.
			if (fabs(fL) < fabs(fH)) {
				// Toss hi, since it is further away.
				hi = retval;
            } else {
				// Toss lo
				lo = retval;
            }
         } else {
			// fR is NOT closest - you are not making progress!  Fit a
			// parabola, and look at the points that it predicts.
			double A_ = fL*(retval-hi) +fH*(lo-retval) +fR*(hi-lo);
			double B_ = fL*(hi+retval)*(hi-retval) +fH*(retval+lo)*(retval-lo)
												   +fR*(lo+hi)*(lo-hi);
			double C_ = fL*hi*retval*(retval-hi) +fH*retval*lo*(lo-retval)
												 +fR*lo*hi*(hi-lo);
			// Assuming that it does predict a point, ha-ha
            double DIS = xp(B_,2) - 4.0 *A_ *C_;
            if (DIS < 0.0) {
				LppUrk.LppIssue(207,"solver::solver(Fo,double,double,double...) [1]");
				return fabs(fL)<fabs(fH) ? lo : hi;
            }
			DIS = sqrt(DIS);
            double Xhope1 = (-B_ +DIS)/(2*A_);		// The intercepts of the
            double Xhope2 = (-B_ -DIS)/(2*A_);		// parabola and the
            double Fhope1 = f(Xhope1);			    // function values there
            double Fhope2 = f(Xhope2);
            if (verbosity>1) {
                printf("solver: Quadratic % 20.12e\t% 20.12e\n",Xhope1,Fhope1);
                printf("solver: Quadratic % 20.12e\t% 20.12e\n",Xhope2,Fhope2);
             }
            if (fabs(Fhope1) < fabs(Fhope2)) {
               retval = Xhope1;
               fR	  = Fhope1;
            } else {
               retval = Xhope2;
               fR	  = Fhope2;
            }
			// There is no guarantee that the parabola will help though!
            if (fabs(fR) > 2.0*fmin(fabs(fH),fabs(fL))) {
				LppUrk.LppIssue(207,"solver::solver(Fo,double,double,double...) [2]");
				return fabs(fL)<fabs(fH) ? lo : hi;
			}
			// Need values of hi and lo for next iteration.  Take the Fhope
			// that was best and the other point that is closest to Xhope
			// in X - look for proximity in the domain not the range.
			if ( fabs(lo-retval) < fabs(hi-retval) ) {
				hi = retval;
			} else {
				lo = retval;
			}
		} // Decision branches for next secant computation
	}	// Loop over Niter

	// Another possible excessive iteration return point
	LppUrk.LppIssue(201,"solver::solver(Fo,double,double,double...) [2]");
	return fabs(fL)<fabs(fH) ? lo : hi;
}
