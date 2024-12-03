/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 10 Jan 2018.
 *  Copyright 2018 FRA.  Well, those that exist I guess.
 *
 *  Polynomial interpolation using LaGrange's formula and Neville's time-saver.
 *  Algorithm reworked from Press, Flannery, Teukolsky & Vetterling.  The "error"
 *  estimate presented there is the difference between the full n-point
 *  polynomial and what you would get with the n-1 polynomials.  In real life,
 *  this is hardly ever a meaningful quantity.
 *
 */
#pragma once



#include "ra.h"



class polyQint{
public:
    // Default, copy, assignment, destructor
    polyQint();
    polyQint(polyQint const& inPoly);
    polyQint& operator=(polyQint const& rhs);
    ~polyQint();


    // Constructor-getter architecture.  Usually construct with (x,y) points
    polyQint(ra<double> Xarray, ra<double> Yarray);
    
    double eval(double x, double& err);

private:
    ra<double>* Xra;
    ra<double>* Yra;
    int iBegin, iEnd;
};
