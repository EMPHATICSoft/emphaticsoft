/*  L++ integrator
 *  Created by Leo Bellantoni on 2/6/07.
 *  Copyright 2007 FRA. All rights reserved.  Well, those that exist I guess.
 *  This might be from John J Barton's book.
 *
 *  This template will integrate a C++ function object.  That is an object of a
 *  class in which the () operator has been overloaded to be a function call.
 *  The advantage of this over classical C style functions is that the function
 *  object can be created with internal data fields that may be used in the
 *  function evaluation.  (An example is in poiprob).  One can thereby do a
 *  multiple integration.
 *  The algorithm uses the extended midpoint rule in double precision; the 2nd
 *  Euler-Maclaurin summation formula is involved somehow.  The 1st element of
 *  the returned ra is the integral, and the 2nd is the absolute accuracy
 *  of the integral.
 *
 *  WARNING: the function object is call-by-value; no side effects of the
 *  integration will be retained in it!
 *
 */
#pragma once



#include "ra.h"





template <class Fo> ra<double> integrateObj(Fo f, double lo, double hi,
                                            int NiterMax=10, double WarnAcc=1e-4) {
    // Return value OK on stack - contains pointers to heap but we don't return
    // a pointer to this retval itself, which won't exist after return statement.
    ra<double> retval(2);

	double integral,accuracy, old_integral,old_accuracy;
    bool converged_once = false;

	old_accuracy = +Dbig/2.0;
	old_integral = -old_accuracy;
	for (int i=1; i<=NiterMax; ++i) {
		integral = evalmidObj(f,lo,hi,i);

		// Handle zero integrals here
		if (integral != 0.0) {
			// Beware! abs(double) returns an int cast into a double!  But for
			// magnitude of a complex number you need abs()!  *sheesh*
			accuracy = fabs( (integral - old_integral)/integral );
		} else if (old_integral != 0.0) {
			accuracy = fabs( (integral - old_integral)/old_integral );
		} else if (i > 5) {
			goto exit;
		}
		// Return if you've stopped converging twice
		if ((accuracy > old_accuracy) && (i != 1)) {
            if (converged_once) goto exit;
            converged_once = true;
		}
		// Return if you've converged perfectly
		if (accuracy == 0.0) {
			goto exit;
		}
		old_integral = integral;
		old_accuracy = accuracy;
	}
	if ( accuracy > WarnAcc ) {
        // std::cout << "Integral accurate to " << accuracy << std::endl;
		LppUrk.LppIssue(102,"integrator::integrateObj(Fo,double,double...)");
	}

	exit:		// Only one way out of this function
	retval(1) = integral;
	retval(2) = integral*accuracy;
	return retval;
}


template <class Fo> double evalmidObj(Fo f, double lo, double hi, int Np) {
	// Used by integrateObj to actually evaluate the integral
	double Tnm,del,Ddel,x,sum;
	static int j,itr;
	// Algorithm cleverly keeps sum of previous integrations from earlier calls
	static double retval;

	if (Np == 1) {
		retval = (hi-lo) * f(0.5*(hi+lo));
		itr = 1;
	} else {
		Tnm  = itr;
		del  = (hi-lo)/(3.0*Tnm);
		Ddel = 2.0*del;
		x    = lo + 0.5*del;
		sum  = 0.0;
		for (j=1; j<=itr; ++j) {
			sum += f(x);
			x   += Ddel;
			sum += f(x);
			x   += del;
		}
		retval = (retval +(hi-lo)*sum/Tnm) / 3.0;
		itr    = 3*itr;
	}
	return retval;
}
