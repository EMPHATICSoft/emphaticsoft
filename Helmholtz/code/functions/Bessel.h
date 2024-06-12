/*  L++ Bessel functions of the 1st kind, 2nd kind,
 *  plain vanilla and modified too.  Integer order only.
 *  Created by Leo Bellantoni on 25 Oct 2023.
 *  Copyright 2023 FRA.  Well, those that exist I guess.
 *
 *  Good to only about 6 digits, so "double" is just for show.
 *  Well actually on some architectures double is faster than float.
 *  From Press, Flannery, Teukolsky & Vetterling.  I used the version
 *  in C for the base functions of order 0 & 1, because I found errors
 *  in I0 & I1 functions in the FORTRAN version.  Those errors
 *  mght go back to Abramowitz and Stegun or even their sources.
 *  I left the top level functions as I translated them from the
 *  FORTRAN version.   A more general package, presumably of greater
 *  greater accuracy, is entry 644 of netlib's TOMS repository at
 *  https://www.netlib.org/toms/.  That will do Bessel functions of
 *  a complex argument and nonnegative order H1, H2, I, J, K, and Y,
 *  as well as the Airy functions Ai, Bi, and their derivatives.
 *  But it's 19000 lines of FORTRAN.  There is also arXiv:1705.07820
 *  which requires 8.4 Gig of binary data.
 *
 *  Plain ol' functions to call, no class.  Complete trailer trash.
 *
 */
#pragma once



#include <cmath>
#include "LppGlobals.h"
#include "intsNabs.h"
#include "xp.h"



// The main functions
double BessJ(int n, double x);      // Integer order, 1st kind, vanilla
double BessY(int n, double x);      // Integer order, 2nd kind, vanilla
double BessI(int n, double x);      // Integer order, 1st kind, modified
double BessK(int n, double x);      // Integer order, 2nd kind, modified



// BessJ & BessY recurses to the following 4 functions.  You
// don't need to call them directly, although nothing will stop you.
double BessJ0(double x);
double BessJ1(double x);
double BessY0(double x);
double BessY1(double x);
// BessI & BessJ, likewise.
double BessI0(double x);
double BessI1(double x);
double BessK0(double x);
double BessK1(double x);
