/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 12 May 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Multivariable polynomial interpolation using LaGrange's formula.
 *  More general than Neville's algorithm as implemented in polyQint
 *  but not as fast, nor can it be expanded to give the not-very-useful
 *  error estimate of the Neville algorithm.
 *  Revised to use RA3.h for speed.  Only 3D interpolation!
 *
 */
#pragma once


#include "ra.h"
#include "RA3.h"



class poly3int{
public:
    // Default, copy, assignment, destructor
    poly3int() {};
    poly3int(poly3int const& inPoly);
    poly3int& operator=(poly3int const& rhs);
    ~poly3int() {};

    // Set up the interpolator
    void setUpGrid (RA_1<double> Xin_1, RA_1<double> Xin_2, RA_1<double> Xin_3);
    void fillValues(RA_3<double> Fin);

    double eval(ra<double>& x);

private:
    RA_1<double> Xra_1;
    RA_1<double> Xra_2;
    RA_1<double> Xra_3;
    RA_3<double> Fra;

    RA_2<double> save1st;        RA_1<double> save2nd;

    void comcopy(poly3int const& inPoly, bool constructing);
};
