/*  L++ intsNabs
 *  Created by Leo Bellantoni on 11 May 2018.
 *  Copyright 2018 URA. All rights reserved.
 *
 *  Include macros for sign and aint (FORTRAN-style floor) here, and a 
 *  function to map arbitrary angles into the -pi to +pi range of atan2.
 *
 */
#pragma once



#include <cmath>
#include <type_traits>
#include "LppGlobals.h"



// FORTRAN-style truncation of integer; fint (Round towards zero, same as the
// FORTRAN INT function, and nint (Nearest integer).  The C++ way is in <cmath>
// this is here for easy documentation really.  *sheesh* there no std::sign?
template <typename T> int fint(T x) {return x<0.0 ? int(-floor(-x)) : int(floor(x));};
template <typename T> int nint(T x) {return int(floor((x)+0.5));};
template <typename T> int sign(T x) {return x<0 ? -1 : ( x>0 ? +1 : 0 );};

// Round a real number to the specified precision
template <typename T> float       nfloat(T x, T dx)   {return dx * nint(x/dx);};
template <typename T> double      ndouble(T x, T dx)  {return dx * nint(x/dx);};
template <typename T> long double nLdouble(T x, T dx) {return dx * nint(x/dx);};


// Put any angle in the range (-pi, +pi]
// #define nang(a)     ( (a)-2*pie*floor(((a)+pie)/(2*pie)) )
template <typename T> T nang(T x) {return x-2*pie*floor((x+pie)/(2*pie));};
