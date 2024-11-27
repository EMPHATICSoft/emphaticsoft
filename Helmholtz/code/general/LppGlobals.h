/*  L++ Global header file
 *  Created by Leo Bellantoni on 1/11/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A few globals made available in many places
 *
 */
#pragma once



#include <complex>
typedef std::complex<double> cmplx;
typedef size_t addr;
#define  tabl  <<"\t"<<
#define  coml  <<", "<<
#define  noop  static_assert(true,"NO OP");



#include "../problems/LppError.h"
#include "LppGNUpipe.h"



extern LppError   LppUrk;
extern LppGNUpipe LppPlot;
extern double const pie, r2pi, one, hbar, gold, oil, Ibig, Dbig, Deps;
