/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 12 May 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Polynomial interpolation using LaGrange's formula and Neville's time-saver.
 *  Algorithm reworked from Press, Flannery, Teukolsky & Vetterling.
 *  Revised to use RA3.h for speed.  Only 3D interpolation!
 */



#include "poly3int.h"
using namespace std;



// Constructor for the masses
// Copy, assignment, destructor
poly3int::poly3int(poly3int const& inPoly) {
    comcopy(inPoly,true);
    return;
}

poly3int& poly3int::operator=(poly3int const& rhs) {
    if (this != &rhs) comcopy(rhs,false);
    return *this;
}
void poly3int::comcopy(poly3int const& inPoly, bool constructing) {
    Xra_1 = inPoly.Xra_1;
    Xra_2 = inPoly.Xra_2;
    Xra_3 = inPoly.Xra_3;
    Fra   = inPoly.Fra;
}



void poly3int::setUpGrid(RA_1<double> Xin_1, RA_1<double> Xin_2, RA_1<double> Xin_3) {
    Xra_1 = Xin_1;
    Xra_2 = Xin_2;
    Xra_3 = Xin_3;
}

void poly3int::fillValues(RA_3<double> Fin) {
    Fra = Fin;
}



double poly3int::eval(ra<double>& x) {
    int i1st, i2nd, i3rd;
    double retval = 0;
    save1st = 0;      save2nd = 0;

    // A note on indexing:  the input argument, x is indexed from 1 as that is the default for
    // ra<double>.  The arrays Xra_? were, in the original version of this code, which also
    // used ra<double>, indexed from 0; then the setUpGrid methods would call various
    // ra::setbase methods.  This poly3int class uses the RA_? templates which should be
    // faster but don't have the setbase functionality.  So here, Xra_? are indexed from 1.
    // This keeps the interface of the poly3int class the same as that of the polyMint class.
    // In keeping with that, the indexing of values is also from 1; hamna setbase(0,0,0,0) call
    // of polyMint::fillValues.

    // contract the 1st index
    i3rd = 1; do {
        i2nd = 1; do {
            i1st = 1; do {
                double prod1st = 1.0;
                for (int j1st=1; j1st<=3; ++j1st) {
                    if (j1st!=i1st) prod1st *= (x(1) -Xra_1(j1st)) / (Xra_1(i1st) -Xra_1(j1st));
                }
                save1st(i2nd,i3rd)      += Fra(i1st,i2nd,i3rd)      * prod1st;

            ++i1st;} while (i1st<=3);
        ++i2nd;} while (i2nd<=3);
    ++i3rd;} while (i3rd<=3);

    // Contract the 2nd index
    i3rd = 1; do {
        i2nd = 1; do {
            double prod2nd = 1.0;
            for (int j2nd=1; j2nd<=3; ++j2nd) {
                if (j2nd!=i2nd) prod2nd *= (x(2) -Xra_2(j2nd)) / (Xra_2(i2nd) -Xra_2(j2nd));
            }
            save2nd(i3rd)      += save1st(i2nd,i3rd)      * prod2nd;
        ++i2nd;} while (i2nd<=3);
    ++i3rd;} while (i3rd<=3);

    // Contract the 3rd index
    i3rd = 1; do {
        double prod3rd = 1.0;
        for (int j3rd=1; j3rd<=3; ++j3rd) {
            if (j3rd!=i3rd) prod3rd *= (x(3) -Xra_3(j3rd)) / (Xra_3(i3rd) -Xra_3(j3rd));
        }
        retval           += save2nd(i3rd)      * prod3rd;
    ++i3rd;} while (i3rd<=3);
    return retval;
}
