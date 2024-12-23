/*  L++ Global header file
 *  Created by Leo Bellantoni on 2/20/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A few globals to be made available in many places via LppGlobals.h
 *
 */
#pragma once



#include <cfloat>
#include <cmath>
#include <climits>
#include "../problems/LppExcept.h"
#include "../GUI/LppGNUpipe.h"



LppError   LppUrk;
LppGNUpipe LppPlot;
// Without extern, the compiler will not allocate space for these numbers,
// causing problems later.  const implies internal linkage, meaning that the
// declared/defined object is not visible outside the translation unit in
// which it is defined.

// El Capitan & later define pi somewhere in CarbonCore as used by Accelerate
extern double const pie  = M_PI;
extern double const r2pi = sqrt(2*M_PI);
extern double const one  = 299792458.0;         // TMinuit has lower-case c variable
extern double const hbar = 6.58211899e-22;		// Mev-sec
extern double const gold = 1.6180339887498949025;
extern double const oil  = 0.5772156649015328606;
extern double const Ibig = INT_MAX;
extern double const Dbig = min( DBL_MAX,1.0/DBL_MIN );
extern double const Deps = DBL_EPSILON;
extern double const Nana = std::nan("");        // To check for a Nan, try X==X; it will be false!
