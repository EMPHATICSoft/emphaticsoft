/*  L++   SFT.cpp
 *  Created by Leo Bellantoni on 29 Dec 2012.
 *  Copyright 2012 FRA. All rights reserved.
 *
*/

#include "SFT.h"


// Simple constructor but it can hurl.  Default constructor OK
// Copy constructor built to facilitate assignment operator
// Default destructor should be fine
SFT::SFT(int inSign) {
    if (abs(inSign) != 1) LppUrk.LppIssue(0,"SFT::SFT(int)");
    Sign = inSign;
}

SFT::SFT() : Sign(+1) {};
SFT::SFT(SFT const& inSFT) {
	*this = inSFT;
}
SFT& SFT::operator=(SFT const& rhs) {
	if (this != &rhs) {
        Sign = rhs.Sign;
    }
	return *this;
}


ra<cmplx> SFT::transform(ra<cmplx> data) {
    // Make sure you got a 1D ra
    if (data.ne2 != 0) LppUrk.LppIssue(7,"SFT::transform");
    // Of even number of elements
    int N = data.ne1;       /// int, not addr, to save compiler warnings
    if (N%2 != 0) LppUrk.LppIssue(111,"SFT::transform");


    ra<cmplx> retval = data;    retval = cmplx(0,0);
    // Force indexing from 0 for a transform, -N/2 for an inverse
    if (Sign > 0) {
        data.setbase(0);
        retval.setbase(-N/2);
    } else {
        data.setbase(-N/2);
        retval.setbase(0);
    }
    
    // The Fourier transform itself and its inverse
    for (int n=-N/2; n<+N/2; ++n) {
        for (int k=0; k<N; ++k) {
            double theta  = 2*Sign*pie * double(n)*double(k)/double(N);
            cmplx xptheta(0.0,theta);
            if (Sign>0) {
                retval(n) += data(k) * exp(xptheta);
            } else {
                retval(k) += data(n) * exp(xptheta);
            }
        }
    }
    
    // Inverse transform gets factor 1/M
    if (Sign<0) retval.val /= N;

    return retval;
}


ra<double> SFT::power(ra<cmplx> atad) {
    // Make sure you got a 1D ra
    if (atad.ne2 != 0) LppUrk.LppIssue(7,"SFT::power");
    // Of even number of elements
    int N = atad.ne1;
    if (N%2 != 0) LppUrk.LppIssue(111,"SFT::power");
    // Force indexing from -N/2 'cuz it better be an inverse
    atad.setbase(-N/2);
    ra<double> retval(N/2 + 1);     retval.setbase(0);
    retval(0) = abs(atad(0));
    for (int i=1; i<N/2; ++i) {
        retval(i) = abs(atad(i)) +abs(atad(-i));
    }
    retval(N/2) = abs(atad(-N/2));
    retval.val /= N;
    retval.val *= retval.val;
    return retval;
}

