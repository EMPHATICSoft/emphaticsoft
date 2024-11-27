/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 12 May 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Multivariable polynomial interpolation using LaGrange's formula.
 *  More general than Neville's algorithm as implemented in polyQIint
 *  but not as fast, nor can it be expanded to give the not-very-useful
 *  error estimate of the Neville algorithm.
 *
 */
#pragma once



#include "ra.h"
#include "Ldelete.h"



class polyMint{
public:
    // Default, copy, assignment, destructor
    polyMint();
    polyMint(polyMint const& inPoly);
    polyMint& operator=(polyMint const& rhs);
    ~polyMint();

    // Set up the interpolator
    void setUpGrid(ra<double> Xin_1, ra<double> Xin_2);
    void setUpGrid(ra<double> Xin_1, ra<double> Xin_2, ra<double> Xin_3);
    void setUpGrid(ra<double> Xin_1, ra<double> Xin_2, ra<double> Xin_3, ra<double> Xin_4);
    void fillValues(ra<double> Fin);

    double eval(ra<double> x);

private:
    ra<double>* Xra_1;
    ra<double>* Xra_2;
    ra<double>* Xra_3;
    ra<double>* Xra_4;
    ra<double>* Fra;
    int nDim;
};
