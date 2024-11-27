/*  L++ BLUE
 *  Created by Leo Bellantoni on 2/5/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A class to implement the Best Linear Unbiased Estimator for a number of
 *  measurements, following the prescription of Lyons, Gibaut and
 *  Clifford in N.I.M., A270 (1988) 110-117.
 *
 *  Uses constructor-getter architecture.
 *
 */
#pragma once



#include "Lvec.h"
#include "Lmat.h"
#include "stats.h"
#include "Lbins.h"
#include "Lhist1.h"
#include "Lfit1.h"



class BLUE {
public:
    BLUE();
    BLUE(BLUE const& inBLUE);
    BLUE& operator=(BLUE const& rhs);
    ~BLUE();


    // Only one constructor to take inputs and do the computation;
    // values contains experimental results, N of them; and
    // covariances is perforce N x N
    BLUE(ra<double> inValues, ra<double> inCovariances);

    // Getters for the results of the computation
    double getEstimate();
    double getUncertainty();
    double getChi2();
    double getCL();             // NDoF is number of experiments -1

    // These two methods only valid in the case of diagonal covariances
    void   showPulls(bool printPulls=true);
    double PDGscale();
    

    
private:
    // Values for output
    double blue, u_blue, chi2;
    
    // Number of experiments
    int Nexp;

    // For making the plots
    ra<double>* values;     ra<double>* covariances;
};
