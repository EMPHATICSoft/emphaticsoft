/*  L++ meancorrel
 *  Created by Leo Bellantoni on 2 Dec 2014.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  A class to compute mean, sigma, and correlation coefficients for lists of
 *  Ndim doubles.  Mean and sigma based on meansig class.  Dimensions count
 *  from 1 to Ndim for the user of this class.
 *
 */
#pragma once



#include <cmath>
#include <vector>

#include "LppGlobals.h"
#include "ra.h"
#include "meansig.h"



class meancorrel {
public:
    // Three of the boilerplates
    meancorrel();
    meancorrel(meancorrel const& inMcorr);
    meancorrel& operator=(meancorrel const& rhs);



    // Constructor for the masses
    meancorrel(int inNdim);

    // A resetter seems helpful
    void reset();
    
    // Method to input the numbers.
    void push(ra<double> x);
    
    // And your output methods; first variable is dim == 1
    addr  count();
    double mean(int dim);
    double variance(int dim);           // N
    double stdev(int dim);              // N-1
    double rho(int dim1, int dim2);     // N
    
private:
    int Ndim;
    std::vector<meansig> MeanSigs;      // A vector, but indexed from 1 because
    std::vector< ra<double> > data;      // ra<meansig> has some issue.  Data is
                                        // saved in data; running stats are in
                                        // MeanSigs.
};

