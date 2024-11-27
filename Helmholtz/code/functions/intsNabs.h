/*  L++ intsNabs
 *  Created by Leo Bellantoni on 11 May 2018.
 *  Copyright 2018 URA. All rights reserved.
 *
 *  Include macros for sign and aint (FORTRAN-style floor) here, and a 
 *  function to map arbitrary angles into the -pi to +pi range of atan2.
 *
 */
#pragma once



// FORTRAN-style truncation of integer; fint (Round towards zero, same as the
// FORTRAN INT function, and nint (Nearest integer).  The C++ way is in <cmath>
// this is here for easy documentation really.  *sheesh* there no std::sign?
template <typename T> int fint(T a) {return a<0.0 ? int(-floor(-a)) : int(floor(a));};
template <typename T> int nint(T a) {return int(floor((a)+0.5));};
template <typename T> int sign(T a) {return a<0 ? -1 : ( a>0 ? +1 : 0 );};



// Put any angle in the range (-pi, +pi]
#define nang(a)     ( (a)-2*pie*floor(((a)+pie)/(2*pie)) )
