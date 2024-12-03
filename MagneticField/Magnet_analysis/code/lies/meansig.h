/*  L++ meansig
 *  Created by Leo Bellantoni on 2/3/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A class to implement Don Knuth's recursion algorithm, reportedly on pg 232
 *  of Vol. 2, for a well-conditioned computation of the mean and sample
 *  variance of a sequence of numbers.  Knuth cites Welford (1962).  Extention
 *  to samples with weights is not obvious - see the .ccp file.
 *
 *  If you push a Nan, it will be ignored.
 *
 */
#pragma once



#include <cmath>
#include "LppGlobals.h"



class meansig {
public:
    // Only one constructor needs defined; compiler produced
    // copy constructor, destructor are fine
    // I don't imagine needing an assignment operator either
    meansig();
    // A resetter seems helpful
    void reset();

    // Method to input the numbers.  Do not casually extend to
    // weighted data; you will probably want a different mean_err()
    // in that case and the N==1 branch in push(double) may or may
    // not screw up the values of oldS - I just don't know.
    void push(double x);

    // And your output methods
    int    count();
    double mean();
    double mean_err();
    double variance();  // N
    double stdev();     // N-1
    double zenith();
    double nadir();
    
private:
    int N;
    double oldM, oldS;
    double peak, pits;
};
