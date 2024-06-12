/*  L++ Lvec
 *  Created by Leo Bellantoni on 1/23/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A slow and large but high-level vector built out of complex<double> ra
 *  objects.  If you need to go faster, work in the ra objects or valarrays
 *  directly.  There's also BLAS and some sparse matrices in the Accelerate
 *  framework.
 *  Always indexed from 1!
 *
 */
#pragma once



// LppGlobals.h and Lmat.h included implicitly
#include "ra/ra.h"
#include "functions/quadd.h"



// Need only declaration of Lmat here; no reference to members of, no attempt
// to create a, or to find the size of Lmat occurs in definition of Lvec.
class Lmat;


class Lvec{
public:
	int  N;		// Public dimension for range-checking



	// Default, copy, assignment & destructor
    Lvec();
	Lvec(Lvec const& inLvec);
	Lvec& operator=(Lvec const& rhs);
	~Lvec();



	// Construct from an ra using promotion of numeric types.  Implemented using
	// promotion: don't try to construct from an ra<char>!  Only this constructor
	// is a template.
	template <class T> explicit Lvec(ra<T>& inra) :
		hasMag(false), N(inra.ne1) {
		if (inra.ne2 != 0)
			LppUrk.LppIssue(7,"Lvec::Lvec(ra<T>)");
        RA = new ra<cmplx>(inra.ne1);
        int iOff;
        inra.getbase(iOff);     --iOff;
		for (int i=1; i<=N; ++i) {
			(*RA)(i) = cmplx(inra(i +iOff));
		}
		return;
	}

	// Construct an uninitialized Lvec.  Promotion again!
	explicit Lvec(int const dim) :
		RA(new ra<cmplx> (dim)),
		hasMag(false), N(dim) {
	}

	// Construct from a number; all values the same.
	Lvec(cmplx const inVal, int const dim) :
		RA(new ra<cmplx> (dim)),
		hasMag(false), N(dim) {
		for (int i=1; i<=N; ++i) {
			(*RA)(i) = inVal;
		}
		return;
	}

	// Indexing - just a wrapper to the ra indexing.
	cmplx& operator() (int i1) const;

	// High-level functionality
	double mag() const;
	Lvec conj() const;

	// Operators
	Lvec  operator-() const;								// Unary op
	Lvec  operator+ (Lvec const& rhs) const;				// Simple binary ops
	Lvec& operator+=(Lvec const& rhs);
	Lvec  operator- (Lvec const& rhs) const;				// Simple binary ops
	Lvec& operator-=(Lvec const& rhs);

	cmplx operator* (Lvec const& rhs) const;			// Dot product w/ cmplx conjugate of rhs
	Lvec  operator^ (Lvec const& rhs) const;			// Cross product w/ cmplx conjugate of rhs

    // Ops with scalars - don't do operator*(cmplx) because then you
    // won't be able to write (cmplx)*(Lvec) which is dumb.
	Lvec& operator+=(cmplx const& rhs);                     
	Lvec& operator-=(cmplx const& rhs);
	Lvec& operator*=(cmplx const& rhs);

private:
	bool hasMag;
	cmplx Mag;
	// Create this ra with new, index via (*RA)(i)
	ra<cmplx> *RA;
};
