/*  L++ meansig
 *  Created by Leo Bellantoni on 2/3/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  Extension to weighted samples would require the following changes to push(double):
 *      Add a sumW variable that is incremented by the weight right after N++;
 *      Scale increments to oldM and old S by the weight;
 *  It also requires:
 *      Prove to yourself that the algorithm still works
 *      Figure out what are the correct forms for stdev(), variance() and mean_err().
 *
 */
#include "meansig.h"


meansig::meansig() :
    N(0), oldM(0.0), oldS(0.0) {
}


void meansig::push(double x) {
    if (x!=x) return;         // NaN!
    N++;
    if (N == 1) {
        oldM = x;
        peak = pits = x;
    } else {
        double del = x -oldM;
        oldM += del/N;
        oldS += del*(x -oldM); // doesn't equal del*del any more!
        if (x<pits) pits = x;
        if (x>peak) peak = x;
    }
}

void meansig::reset() {
    N = 0;
    oldM = oldS = 0.0;
}

int    meansig::count() {return N;}
double meansig::mean() {
    if (N<1) {
        LppUrk.LppIssue(112,"meansig::mean()");
    }
    return oldM;
}
double meansig::mean_err() {
    return stdev()/sqrt(N);
}
double meansig::variance() {
    if (N<2) {
        LppUrk.LppIssue(112,"meansig::variance()");
    }
    return oldS/N;
}
double meansig::stdev() {
    if (N<2) {
        LppUrk.LppIssue(112,"meansig::stdev()");
    }
    return sqrt(oldS/(N-1));
}
double meansig::zenith() {
    if (N<1) {
        LppUrk.LppIssue(112,"meansig::zenith()");
    }
    return peak;
}
double meansig::nadir() {
    if (N<1) {
        LppUrk.LppIssue(112,"meansig::nadir()");
    }
    return pits;
}
