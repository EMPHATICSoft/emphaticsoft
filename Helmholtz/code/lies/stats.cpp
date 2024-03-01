/*  L++ stats
 *  Created by Leo Bellantoni on 12/22/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  Function for statistics and probabilities
 *
 */
#include "stats.h"
using namespace std;



// Stirling's approximation with first 2 correction terms and lookup for n<=12
double stirling(int n) {
	double x, retval;
	// In most 32 bit implementations, this is as high as you can go!
	int fact[13]={1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600};
	if (n<0) {
		LppUrk.LppIssue(0,"stats::stirling(int)");
		return 0.0; // LppIssue will throw an exception, but compiler wants this 
	} else if (n <= 12) {
		return log(fact[n]);
	} else {
		x = n;
		retval = ::r2pi*( 1.0 + 1.0/(12.0*x) + 1.0/(288.0*x*x) );
		retval = (x+0.5)*log(x) -x +log(retval);
		return retval;
	}
}



// Computation of ln[gamma(z)] for Re(x) > 0.  Algorithm by C.Lanczos, SIAM
// Journal of Numerical Analysis B1 (1964), 86; accurate to better than 2e-10.
// Two signatures provided, for real & complex arguments
double lngamma(double x) {
	double const coeff[7] = {
		2.5066282746310005, 76.18009172947146, -86.50532032941677,
		24.01409824083091, -1.231739572450155, 0.1208650973866179e-2,
		-0.5395239384953e-5};

	if (x<=0) LppUrk.LppIssue(0,"stats::lngamma(double)");
	double y   = x;
	double ser = 1.000000000190015;
	double tmp = x +5.5;
	tmp = (x +0.5e0)*log(tmp) -tmp;
	for (int i=1; i<7; ++i) {
		y   += 1.0e0;
		ser += coeff[i]/y;
   }
   return tmp +log(coeff[0]*ser/x);
}

complex<double> lngamma(complex<double> x) {
	double const coeff[7] = {
		2.5066282746310005, 76.18009172947146, -86.50532032941677,
		24.01409824083091, -1.231739572450155, 0.1208650973866179e-2,
		-0.5395239384953e-5};

	if (real(x)<=0) LppUrk.LppIssue(0,"stats::lngamma(cmplx)");
	complex<double> y(x);
	complex<double> ser(1.000000000190015,0.0);
	complex<double> x5(x);
	x5  += 0.5;
	complex<double> tmp(x);
	tmp += 5.5;
	tmp  = x5*log(tmp) -tmp;
	for (int i=1; i<7; ++i) {
		y   += 1.0e0;
		ser += coeff[i]/y;
   }
   return tmp +log(coeff[0]*ser/x);
}



// The error function 2/sqrt(pi) times integral from 0 to X of exp(-t**2) dt;
// errf(X/sqrt(2.0)) is the probability of a normal distribution returning a
// result between -X and +X; also available is errfc(double X), below.

// Based upon the approximations from W. J. Cody, Math. Comp., (1969) 631-638.
// This transportable program theoretically achieves at least 18 significant 
// decimal digits.  The actual accuracy achieved is machine dependent however;
// for this implementation a particular set of machine-dependent precision
// constants corresponding to IEEE standard double precision have been used here.
double errf(double X) {
	double const A_[5] =
		{3.16112374387056560e00, 1.13864154151050156e+2, 3.77485237685302021e+2,
		 3.20937758913846947e+3, 1.85777706184603153e-1};
	double const B_[4] =
		{2.36012909523441209e+1, 2.44024637934444173e+2, 1.28261652607737228e+3,
		 2.84423683343917062e+3};							// Coeffs 1st interval
	double const C_[9] =
		{5.64188496988670089e-1, 8.88314979438837594e00, 6.61191906371416295e+1,
		 2.98635138197400131e+2, 8.81952221241769090e+2, 1.71204761263407058e+3,
		 2.05107837782607147e+3, 1.23033935479799725e+3, 2.15311535474403846e-8};
	double const D_[8] =
		{1.57449261107098347e+1, 1.17693950891312499e+2, 5.37181101862009858e+2,
		 1.62138957456669019e+3, 3.29079923573345963e+3, 4.36261909014324716e+3,
		 3.43936767414372164e+3, 1.23033935480374942e+3};	// Coeffs 2nd interval
	double const P_[6] =
		{3.05326634961232344e-1, 3.60344899949804439e-1, 1.25781726111229246e-1,
		 1.60837851487422766e-2, 6.58749161529837803e-4, 1.63153871373020978e-2};
	double const Q_[5] =
		{2.56852019228982242e00, 1.87295284992346047e00, 5.27905102951428412e-1,
		 6.05183413124413191e-2, 2.33520497626869185e-3};	// Coeffs 3rd interval
	double const Xsmall = 1.11e-16;      double const Xbig = 26.543;
	double del,Y,Xden,Xnum,Ysq,tmp;
	int i;

	Y = fabs(X);
	if (Y <= 0.46875) {
		// Evaluate in 1st interval
		Ysq = 0.0;
		if (Y > Xsmall) Ysq = Y*Y;
		Xnum = A_[4]*Ysq;
		Xden = Ysq;
		for (i=0; i<3; ++i) {
			Xnum = (Xnum +A_[i]) *Ysq;
			Xden = (Xden +B_[i]) *Ysq;
		}
		tmp = X * (Xnum +A_[3]) / (Xden +B_[3]);
		return tmp;
	} else if (Y <= 4.0) {
		// Evaluate in 2nd interval
		Xnum = C_[8]*Y;
		Xden = Y;
		for (i=0; i<7; ++i) {
			Xnum = (Xnum +C_[i]) *Y;
			Xden = (Xden +D_[i]) *Y;
		}
		tmp = (Xnum +C_[7]) / (Xden +D_[7]);
		Ysq = fint(Y*16.0)/16.0;
		del = (Y -Ysq)*(Y +Ysq);
		tmp = exp(-Ysq*Ysq) * exp(-del) * tmp;

	} else if (Y < Xbig) {
		// Evaluate in 3rd interval
		Ysq  = 1.0 / (Y*Y);
		Xnum = P_[5]*Ysq;
		Xden = Ysq;
		for (i=0; i<4; ++i) {
			Xnum = (Xnum +P_[i]) *Ysq;
			Xden = (Xden +Q_[i]) *Ysq;
		}
		tmp = Ysq *(Xnum +P_[4]) / (Xden +Q_[4]);
		tmp = (1.0/sqrt(pie) -  tmp) / Y;
		Ysq = fint(Y*16.0)/16.0;
		del = (Y -Ysq)*(Y +Ysq);
		tmp = exp(-Ysq*Ysq) * exp(-del) * tmp;
	} else {
		tmp = 0.0;
	}

	// Fix up for negative arguments, erf, etc.
	tmp = (0.5 -tmp) +0.5;
	if (X < 0.0) tmp = -tmp;
	return tmp;
}

double errfc(double X)   { return (1.0 - errf(X)); }

double intnorm(double X) {
    double sqrt2 = sqrt(2);
    double retval = X<0 ? 1-errf(-X/sqrt2) : 1+errf(X/sqrt2);
    return retval/2.0;
}



// Returns the probability that a random variable that is distributed as chi-
// squared of NDF degrees of freedom will occur with  a value greater than X
// (chiprbQ) or less than X (chiprbP).
// Rewritten from Press, Teukolsky, Vetterling & Flannery's concept of
// building chi-squared out of the incomplete gamma function, which in
// turn is either implemented as a series expansion or a continued fraction
double chiprbQ(double chi2, int NDoF) {return 1 - chiprbP(chi2,NDoF);}

double chiprbP(double chi2, int NDoF) {
    if (chi2<0 || NDoF<=0) LppUrk.LppIssue(0,"stats::chiprb(double,int)");
    
    double a = NDoF/2.0;    double x = chi2/2.0;         int itr;
    
    // Tightening these up didn't improve accuracy enough to make it worthwhile
    int const ITMAX = 100;  double const EPS = 1.0e-7;   double const FPMIN = 1.0e-30;
    if (x < a+1.0) {
        double ap  = a;
        double sum = 1.0 / a;
        double del = sum;
        for (itr=1; itr<=ITMAX; ++itr) {
            ap  +=1.0;
            del *= x/ap;
            sum += del;
            if (fabs(del) < fabs(sum)*EPS) break;
        }
        if (itr>ITMAX)  LppUrk.LppIssue(201,"stats::chiprb(double,int) [1]");
        return sum *exp(-x +a*log(x) -lngamma(a));
    } else {
        double b = x +1.0 -a;
        double z = 1.0 / FPMIN;
        double d = 1.0 / b;
        double h = d;
        for (itr = 1; itr <= ITMAX; ++itr) {
            double an = -itr * (itr -a);
            b +=  2.0;
            d  = an*d +b;  // not /b?
            if (abs(d) < FPMIN) d = FPMIN;
            z = b +an/z;
            if (abs(z) < FPMIN) z = FPMIN;
            d = 1.0 / d;
            double del = d * z;
            h  *= del;
            if (fabs(del-1.0) < EPS) break;
        }
        if (itr>ITMAX)  LppUrk.LppIssue(201,"stats::chiprb(double,int) [2]");
        return 1.0 - (exp(-x +a*log(x) -lngamma(a)) * h);
    }
}

// Helper classes for prbchi
class prbchifuncQ {
public:
    prbchifuncQ(double prob,int NDoF) : prob_(prob), NDoF_(NDoF) {}
    double operator()(double x) {
        if (x<=0) x = 0;
        return chiprbQ(x,NDoF_) - prob_;
    }
private:
    double prob_;    int    NDoF_;
};

class prbchifuncP {
public:
    prbchifuncP(double prob,int NDoF) : prob_(prob), NDoF_(NDoF) {}
    double operator()(double x) {
        if (x<=0) x = 0;
        return chiprbP(x,NDoF_) - prob_;
    }
private:
    double prob_;	int    NDoF_;
};

double prbchiQ(double prob, int NDoF) {
    prbchifuncQ wrapped_chiprb(prob,NDoF);
    double retval;
    double lo = 0.8*NDoF;        double hi = 1.2*NDoF;
    retval = solver(wrapped_chiprb, lo,hi, 0.00001);
    return retval;
}

double prbchiP(double prob, int NDoF) {
    prbchifuncP wrapped_chiprb(prob,NDoF);
    double retval;
    double lo = 0.8*NDoF;        double hi = 1.2*NDoF;
    retval = solver(wrapped_chiprb, lo,hi, 0.00001);
    return retval;
}



// Pretty exact uncertainty for Poisson-distributed variables.  First need to
// define the function object class for negative log of likelyhood to
// intercept 1/2.
class poierrfunc {
public:
	poierrfunc(double Ni) : N2(Ni) {}
	double operator() (double mu) const {
		// N.B. Stirling's approximation NOT used to derive this, which is
        // the negative of the difference between the log of the pdf
        // minus the log of the minimal point, mu==N2.
		return (mu-N2) +N2*(log(N2)-log(mu)) -0.5;
	}
private: double N2;
};

double poierr(char dir, int N) {
	double x1,x2;
	double Yacc = 5.0e-6*sqrt(N);

	// Garbage input
	if (N<0) LppUrk.LppIssue(0,"stats::poierr(char,N) [1]");
	if ((dir != '+') && (dir != '-')) LppUrk.LppIssue(0,"stats::poierr(char,N) [2]");

	// Special cases
	if (N == 0) {
		if (dir == '+') {
			return 0.5;
		} else {
			LppUrk.LppIssue(0,"stats::poierr(char,N) [3]");
		}
	}
	if ((N == 1) && (dir == '-')) {
			return 0.698290438;		// mu-ln(mu) = 1.5
	}

	poierrfunc Pf(N);

	if (dir == '+') {
		x1 = N +sqrt(N);
		x2 = x1 +0.5;
	} else {
		x1 = N -sqrt(N);
		x2 = x1 -0.5;
	}
	
	double solveit = solver(Pf, x1,x2,Yacc);
	return fabs(solveit-N);
}



// Exact uncertainty for binomial-distributed variables.  Uses solver to find
// where -ln() is minimum value + 1/2.
// First need to define the function object class for -ln()-0.5.
class binerrfun { public:
    binerrfun(double Ai, double Bi, double Ci) :A_(Ai),B_(Bi),C_(Ci) {}
	double operator() (double X) const {
		// Return large positive values for out-of-bounds inputs
		double Elim = 0.1/(A_+B_);
		Elim = -A_*log(Elim) -B_*log(1.0-Elim);
		/* wouldn't (0.1/N)**N work? */
		if		  (X <= 0.0) {
			return Elim*(1.0-X);
		} else if (X >= 1.0) {
			return Elim*X;
		} else {
			return -A_*log(X)-B_*log(1.0-X)+C_;
		}
	}
	private: double A_,B_,C_;
};

double binerr(char dir, int passed, int tested) {
	double x1,x2;
	double A = passed;
	double B = tested-passed;
	double P = A/tested;

	// Garbage input
	if ((passed<0)   || (tested<0))
		LppUrk.LppIssue(0,"stats::binerr(char,int,int) [1]");
	if ((dir != '+') && (dir != '-'))
		LppUrk.LppIssue(0,"stats::binerr(char,int,int) [2]");

	// Special cases
	if (tested == 0)  LppUrk.LppIssue(0,"stats::binerr(char,int,int) [3]");
	// Analytic solution to -ln(likelyhood) = 1/2 on one side
	if (passed == 0) {
		return (dir == '+') ? 1.0 -exp(-0.5/B) : 0.0;
	}
	if (passed == tested) {
		return (dir == '-') ? 1.0 -exp(-0.5/A) : 0.0;
	}

	// Construct the function object
	double ll = A*log(P) +B*log(1.0-P) -0.5;	// Minimum less 1/2
	binerrfun Bf(A,B,ll);

	// Initial guess for solver based on primitive earth technology
	double pet	= sqrt(A*B/tested) / tested;
	double Yacc = 5.0e-6*fabs(pet-P);
	if (dir == '+') {
		x1 = P +0.5*pet;
		x2 = P +1.5*pet;
	} else {
		x1 = P -1.5*pet;
		x2 = P -0.5*pet;
	}
	double solveit = solver(Bf, x1,x2,Yacc);
	return fabs(solveit-P);
}

// Primitive earth algorithm for uncertainty in binomial-distributed variables.
char* stringEffErr(int num, int den) {
    // Yea, its a memory leak.  Fight me.
    char* temp = new char[32];
    double rat = 1000.0*double(num)/double(den);
    double err = 1000.0*binerrPEA(num,den);
    // Assuming UTF-8 encoding on your terminal or whatever
    sprintf(temp,"%5.2f %c%c %5.2f m%c%c", rat, 0xC2,0xB1, err, 0xC2,0xB0);
    return temp;
}
double binerrPEA(double passed, double tested) {
    // Garbage input
    if ((passed<0)   || (tested<=0))
        LppUrk.LppIssue(0,"stats::binerrPEA(int,int)");
    return sqrt(passed*(tested-passed)/tested)/tested;
}


// Binomial uncertainty, primitive earth algorithm modified for use
// with weighted events.  Inputs are sum of weights in numerator, sum
// of weights in denominator, sum of square of weights in denominator
double binerrW(double num, double den, double den2) {
	if ( (den <= 0.0) || (den2 <= 0.0) || (num < 0.0) || (num > den) ) {
		LppUrk.LppIssue(0,"stats:binerrW(double,double,double)");
	}

	// 3 weeks I worked on this derivation and all I can say is
	// that I really HOPE it is right
	return sqrt( (num/den) *(den-num)/den *den2 )/ den;
}



double fishbin(int N, double fit) {
	double retval;
	if		  (fit <= 0.0) {
		// Return a negative large value; log(0) prevents any reasonable
		// smooth continuation of the function into this region.
		if (fit > N) {
			retval =		   (N - fit);
		} else {
			// There is a discontinuity in dL/d(fit)at fit == N but it
			// usually does not matter
			retval = -1000.0 * (N - fit);
		}
	} else if (N <= 0) {
		// This linear function connects smoothly to the correct function
		// but wants an additional branch so dL/dN > 1
		if (fit > exp(0.5)) {
			retval = N*(log(fit) +0.5) -fit;
		} else {
			retval = N				   -fit;
		}
	} else {
		retval = N*log(fit) -fit -stirling(N);  // log of Poisson distribution
	}
	return -2.0 * retval;
}
