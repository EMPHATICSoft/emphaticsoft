/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 10 Jan 2018.
 *  Copyright 2018 FRA. All rights reserved.
 *
 *  Polynomial interpolation using LaGrange's formula and Neville's time-saver.
 *  Algorithm reworked from Press, Flannery, Teukolsky & Vetterling.
*/



#include "polyQint.h"
using namespace std;





// Default, copy, assignment, destructor
polyQint::polyQint() : Xra(nullptr),Yra(nullptr), iBegin(0),iEnd(0) {}

polyQint::polyQint(polyQint const& inPoly)
    : Xra(nullptr), Yra(nullptr) {
    *this = inPoly;
    return;
}

polyQint& polyQint::operator=(polyQint const& rhs) {
    if (this != &rhs) {
        delete Xra;
        delete Yra;
        if (rhs.Xra) Xra = new ra<double>(*rhs.Xra);
        if (rhs.Yra) Yra = new ra<double>(*rhs.Yra);
        iBegin = rhs.iBegin;
        iEnd   = rhs.iEnd;
   }
    return *this;
}

polyQint::~polyQint() {
    delete Xra;
    delete Yra;
}






// Constructor for the masses
polyQint::polyQint(ra<double> Xarray, ra<double> Yarray) {
    if ( ( Xarray.ne2 != 0 ) || ( Yarray.ne2 != 0 ) )
        LppUrk.LppIssue(116, "polyQint::polyQint [1]");
    if ( Xarray.ne1 != Yarray.ne1 )
        LppUrk.LppIssue(116, "polyQint::polyQint [2]");
    Xra = new ra<double>(Xarray.ne1);
    Yra = new ra<double>(Yarray.ne1);
    Xra->getbase(iBegin);
    iEnd = iBegin +Xra->ne1 -1;        // FORTRAN style indexing
    // I believe Neville's algorithm requires that the entries be sorted
    // There surely is a better sort algorithm out there for someone
    // more clever than I; on the other hand, only a fool does polynomial
    // interpolation on a large number of points in the first place.
    ra<bool> sordid(Xra->ne1);  sordid = false;
    for (int iOut=iBegin; iOut<=iEnd; ++iOut) {
        int iMin;   double vMin = Dbig;
        for (int iFrom=iBegin; iFrom<=iEnd; ++iFrom) {
            if (!sordid(iFrom)) {
                if ((Xarray)(iFrom) < vMin) {
                    iMin = iFrom;
                    vMin = Xarray(iFrom);
                }
            }
        }
        (*Xra)(iOut) = Xarray(iMin);
        (*Yra)(iOut) = Yarray(iMin);
        sordid(iMin) = true;
    }
    return;
}



double polyQint::eval(double x, double& err) {
    // Find the index of entry closest to X
    int Near = 1;
    double diff = fabs( x - (*Xra)(iBegin) );
    for (int i = iBegin; i<=iEnd; ++i) {
        double diffTmp = fabs (x - (*Xra)(i) );
        if ( diffTmp < diff ) {
            Near = i;   diff = diffTmp;   
        }
    }
    // Initialize correction arrays and 1st guess of return value.
    ra<double> C(*Yra);
    ra<double> D(*Yra);
    double retval = (*Yra)(Near);
    --Near;     // Right, no underflow check.  I am not sure why though see below

//  Start looping over the correction tables C and D
    for (int m = iBegin; m<=iEnd-1; ++m) {
        for (int i=iBegin; i<=iEnd-m; ++i) {
            double Ho  = (*Xra)(i)   -x;
            double Hp  = (*Xra)(i+m) -x;
            double W   = C(i+1) -D(i);
            double den = Ho - Hp;
            if (den == 0.0)
                LppUrk.LppIssue(117, "polyQint::eval(double)");
            den = W/den;
            C(i) = Ho * den;
            D(i) = Hp * den;
        }
        // Add add the C or D correction term here
        if (2*Near < iEnd-m) {
            err = C(Near+1);
        } else {
            err = D(Near);
            --Near;
        }
        retval += err;
    }
    return retval;
}
