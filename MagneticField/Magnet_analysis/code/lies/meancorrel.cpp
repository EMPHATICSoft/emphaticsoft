/*  L++ meancorrel
 *  Created by Leo Bellantoni on 2/3/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 */
#include "meancorrel.h"
using namespace std;





// Three of the boilerplates
    meancorrel::meancorrel() : Ndim(0) {};
    meancorrel::meancorrel(meancorrel const& inMcorr) {
        *this = inMcorr;
    }
    meancorrel& meancorrel::operator=(meancorrel const& rhs) {
        if (this != &rhs) {
            Ndim = rhs.Ndim;
        }
        return *this;
    }





// Constructor for the masses
meancorrel::meancorrel(int inNdim) :
    Ndim(inNdim) {
    if (Ndim <= 0) {
        LppUrk.LppIssue(16,"meancorrel(int)");
    }
    for (int i = 0; i <= inNdim; ++i) {
        meansig* temp = new meansig();
        MeanSigs.push_back( *temp );        // That is a deep copy
    }
}



// A resetter seems helpful
void meancorrel::reset() {
    for (int i = 1; i <= Ndim; ++i) {
        MeanSigs[i].reset();
    }
    data.clear();
}



// Method to input the numbers.
void meancorrel::push(ra<double> x) {
    for (int i = 1; i <= Ndim; ++i) {
        MeanSigs[i].push(x(i));
    }
    data.push_back(x);
}


// And your output methods
addr meancorrel::count() {
    return data.size();
}
double meancorrel::mean(int dim) {
    if (dim <= 0 || Ndim < dim) {
        LppUrk.LppIssue(16,"meancorrel::mean(int)");
    }
    return MeanSigs[dim].mean();
}
double meancorrel::variance(int dim) {
    if (dim <= 0 || Ndim < dim) {
        LppUrk.LppIssue(16,"meancorrel::variance(int)");
    }
    return MeanSigs[dim].variance();
}
double meancorrel::stdev(int dim) {
    if (dim <= 0 || Ndim < dim) {
        LppUrk.LppIssue(16,"meancorrel::stdev(int)");
    }
    return MeanSigs[dim].stdev();
}
double meancorrel::rho(int dim1, int dim2) {
    if (dim1 <= 0 || Ndim < dim1) {
        LppUrk.LppIssue(16,"meancorrel::rho(int,int) [1]");
    }
    if (dim2 <= 0 || Ndim < dim2) {
        LppUrk.LppIssue(16,"meancorrel::rho(int,int) [2]");
    }
    double mean1 = MeanSigs[dim1].mean();
    double mean2 = MeanSigs[dim2].mean();
    double Psig1 = sqrt(MeanSigs[dim1].variance());
    double Psig2 = sqrt(MeanSigs[dim2].variance());
    double quad  = 0.0;
    
    vector< ra<double> >::iterator it = data.begin();
    for (; it<data.end(); ++it) {
        double dx = (*it)(dim1) -mean1;
        double dy = (*it)(dim2) -mean2;
        quad += dx*dy;
    }
    double retval = quad/count();
    return retval/(Psig1*Psig2);
}
