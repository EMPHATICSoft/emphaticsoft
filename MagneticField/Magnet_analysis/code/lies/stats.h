/*  L++ stats
 *  Created by Leo Bellantoni on 12/22/06.
 *  Copyright 2006 URA.  Well, those that exist I guess.
 *
 *  Function for statistics and probabilities.  To simplify their use, don't
 *  create a stats namespace.
 *
 */
#pragma once



#include <complex>
#include "intsNabs.h"
#include "xp.h"
#include "solver.h"



// Stirling's approximation for ln(n!)
double stirling(int n);



// Natural logarithm of the gamma function for Re(x) > 0
double lngamma(double x);
std::complex<double> lngamma(std::complex<double> x);



// errf(X/sqrt(2)) is the probability of a normal distribution returning a
// result between -X and +X; errfc is 1-errf.  intnorm is integral of normal
// distribution from -infinity to X.  Errf and errfc are now in <cmath> so
// you should probably re-write these when you get the chance.
double errf(double X);
double errfc(double X);
double intnorm(double X);



// Convert Chi2/NDoF to a probability & back. 'P' is for integral from 0 to X
// and 'Q' is for integral of chi2 from X to infinity.  The chiprb functions
// should be good to about 1 part in 10^7; the inverse functions are limited
// to about 1 in 10^5 by the solver.
double chiprbP(double chi2, int NDoF);
double chiprbQ(double chi2, int NDoF);
double prbchiP(double prob, int NDoF);
double prbchiQ(double prob, int NDoF);



// Pretty exact uncertainty for Poisson-distributed variables.  Uses the Poisson
// distribution as likelyhood, asks where the negative log of it goes up by
// 1/2.  dir is either '+' or '-' (single quotes).  To the 1 - 2% level, for
// 5 < N one could use the rules
//			positive error = sqrt{(N + 1.471)/0.942}
//			negative error = sqrt{(N - 1.130)/1.069}
// For N = 1, the positive error is 1.358 and the negative one is 0.698.  For
// N close to but just above 0 gives 1/2 for the positive-going uncertainty.
//
// A frequentist would disagree sharply with this Baysian method at N = 0.  In
// that case, you would estimate the Poisson parameter mu = 0; then a large 
// number of hypothetical repeats of the experiment would all give N = 0 so that
// 100% of the trials would produce an interval - to wit, from 0 to 1/2 - that
// contains the true mu of zero.  A frequentist would want 68.27%; but in fact
// there is no assignable uncertainty that doesn't give 100% uncertainty.
double poierr(char dir, int N);



// Binomial uncertainty, same method as poierr.  The primitive earth algorithm 
// is sqrt(n*(m-n)/m)/m.  For n==0 || n==m, you could use 0.4 (m)^-0.9 and be
// right to ~1%.
char* stringEffErr(int num, int den);
double binerr(char dir, int passed, int tested);
double binerrPEA(double passed, double tested);

// Binomial uncertainty, primitive earth algorithm modified for use
// with weighted events.  Inputs are sum of weights in numerator, sum
// of weights in denominator, sum of square of weights in denominator
double binerrW(double num, double den, double den2);



// -2 times the per-bin contribution to a binned least-likelyhood fit;
// the -2 (times the log of the likelihood) means this can be directly
// substituted in place of a chi-squared minimization.
double fishbin(int N, double fit);
