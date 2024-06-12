/*  L++ Lvec
 *  Created by Leo Bellantoni on 1/23/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A slow and large but high-level vector built out of square 
 *  complex<double> ra objects.  If you need to go fast, work in the ra objects
 *  or in valarrays directly.
 *
 */
#include "Lvec.h"
using namespace std;





Lvec::Lvec() : RA(nullptr), N(0),hasMag(false),Mag(0.0) {};

// Copy and assignment constructors
Lvec::Lvec(Lvec const& inLvec)
    : RA(nullptr) {
    N = inLvec.N;   // Prevent LppIssue 9 in operator=
    *this = inLvec;
    return;
}
Lvec& Lvec::operator=(Lvec const& rhs) {
	if (this != &rhs) {
        if (N != rhs.N)
            LppUrk.LppIssue(9, "Lvec::operator=(Lvec const&)");
        delete RA;
        if (rhs.RA) {
            RA = new ra<cmplx>(rhs.N);
            for (int i=1; i<=N; ++i) {
                (*RA)(i)   = (*rhs.RA)(i);
            }
        }
        hasMag = rhs.hasMag;
        Mag    = rhs.Mag;
    }
	return *this;
}

// Destructor.
Lvec::~Lvec() {
	delete RA;
    return;
}

// Indexing - just a wrapper to the ra indexing.
cmplx& Lvec::operator() (int i1) const { return (*RA)(i1); }



//==============================================================================
//===  High-level functionality ================================================
//==============================================================================
// Magnitude, with complex conjugation and careful addition in quadrature
double Lvec::mag() const {
	double retval = 0.0;
	ra<double> tmp(N);
	ra<bool> used(N);
	for (int i=1; i<=N; ++i) {
		tmp(i) = Qadd( real((*RA)(i)), imag((*RA)(i)) );
		used(i) = false;
	}
	int minent;
	for (int j=1; j<=N; ++j) {
		// Look for the smallest element not used so far
		double minval = Dbig;
		for (int i=1; i<=N; ++i) {
			if ( (tmp(i)<minval) && (!used(i)) ) {
				minval = tmp(i);	minent = i;
			}
		}
		// Add it in
		used(minent) = true;
		retval = Qadd(retval,minval);
	}
	return retval;
}

// Complex conjugate
Lvec Lvec::conj() const {
	Lvec retval(0.0, N);
	retval.hasMag = hasMag;
	(*retval.RA).val = (*RA).val.apply(std::conj);
	return retval;
}



//==============================================================================
//===  Operators  ==============================================================
//==============================================================================
// Unary operator
Lvec Lvec::operator-() const {
	Lvec retval(*this);
	(*retval.RA).val = -(*RA).val;
    // But the valarray::operator= can change the location of the valarray,
    // so we must also
    (*retval.RA).fixptr();
    return retval;
}

// Simple binary operators
Lvec Lvec::operator+(Lvec const& rhs) const {
	Lvec retval(*this);		return (retval += rhs);
}
Lvec& Lvec::operator+=(Lvec const & rhs) {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lvec::operator+=(Lvec)");
	hasMag = false;
	(*RA).val += (*rhs.RA).val;
	return *this;
}
Lvec Lvec::operator-(Lvec const& rhs) const {
	Lvec retval(*this);		return (retval -= rhs);
}
Lvec& Lvec::operator-=(Lvec const & rhs) {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lvec::operator-=(Lvec)");
	hasMag = false;
	(*RA).val -= (*rhs.RA).val;
	return *this;
}

// Dot and cross operators
cmplx Lvec::operator*(Lvec const& rhs) const {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lvec::operator*(Lvec)");
    // Can't use (*rhs.RA).val.apply(std::conj) - dunno why.
    cmplx retval(0,0);
    for (int i=1; i<=N; ++i) {
        retval += (*RA)(i) * std::conj((*rhs.RA)(i));
    }
	return retval;
}
Lvec Lvec::operator^(Lvec const& rhs) const {
    if (N != rhs.N) LppUrk.LppIssue(9,"Lvec::operator^(Lvec) [1]");
    if (N != 3)     LppUrk.LppIssue(9,"Lvec::operator^(Lvec) [2]");
    Lvec retval(3);
    retval(1) = (*RA)(2) * std::conj((*rhs.RA)(3)) - (*RA)(3) * std::conj((*rhs.RA)(2));
    retval(2) = (*RA)(3) * std::conj((*rhs.RA)(1)) - (*RA)(1) * std::conj((*rhs.RA)(3));
    retval(3) = (*RA)(1) * std::conj((*rhs.RA)(2)) - (*RA)(2) * std::conj((*rhs.RA)(1));
    return retval;
}


// Operations with scalars on the right hand side
Lvec& Lvec::operator+=(cmplx const& rhs) {
	hasMag = false;
	(*RA).val += rhs;
	return *this;
}
Lvec& Lvec::operator-=(cmplx const& rhs) {
	hasMag = false;
	(*RA).val -= rhs;
	return *this;
}
Lvec& Lvec::operator*=(cmplx const& rhs) {
	hasMag = false;
	(*RA).val *= rhs;
	return *this;
}
