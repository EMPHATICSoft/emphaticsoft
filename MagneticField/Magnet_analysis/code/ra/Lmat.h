/*  L++ Lmat
 *  Created by Leo Bellantoni on 1/23/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A slow & large but high-level SQUARE matrix built out of complex<double> ra
 *  objects.  If you need to go fast, work in ra objects or valarrays.
 *  There's also BLAS and some sparse matrices in the Accelerate
 *  framework.
 *  Always indexed from 1!
 *
 */
#pragma once



//#include <Accelerate/Accelerate.h>
#include "../lapack_temp.h"

#include "ra.h"
#include "Lvec.h"
#include "../functions/intsNabs.h"
#include "../functions/minmax.h"
#include "../functions/xp.h"



// Need only declaration of Lvec here; no reference to members of, no attempt
// to create a, or to find the size of Lvec occurs in definition of Lmat.
class Lvec;


class Lmat{
public:
	int  N;		// Public dimension for range-checking



	// Default, copy, assignment & destructor
	Lmat();
    Lmat(Lmat const& inLmat);
	Lmat& operator=(Lmat const& rhs);
	~Lmat();



	// Construct from an ra using promotion of numeric types.
	// Implemented as a template in class definition.  This simple scheme won't
	// catch attempts to construct from an ra<char> but that is an unlikely
	// mistake indeed.   Only this constructor is a template.
	template <class T> explicit Lmat(ra<T>& inra) :
		LU(new ra<cmplx> (inra.ne1,inra.ne1)),
		Eval(new ra<cmplx> (inra.ne1)),
		Evec(new ra<cmplx> (inra.ne1,inra.ne1)),
		PIV(new ra<int> (inra.ne1)),
		hasTrace(false), hasCond(false), hasLU(false), hasEig(false),
		N(inra.ne1) {
		if (inra.ne3 != 0)
			LppUrk.LppIssue(7,"Lmat::Lmat(ra<T>)");
		if (inra.ne1 != inra.ne2)
			LppUrk.LppIssue(8,"Lmat::Lmat(ra<T>)");

		// Task is simplified via promotion, although one can't promote the .val
		// members of inra and then use operator=() to copy into (*RA) I guess.
		// I'll uniformly use (*RA){method} rather than RA->{method}.

   		RA = new ra<cmplx>(inra.ne1,inra.ne1);
        int iOff1,iOff2;                            // Should be good, needs testing
        inra.getbase(iOff1,iOff2);  --iOff1;    --iOff2;
		for (int i=1; i<=N; ++i) {
			for (int j=1; j<=N; ++j){
				(*RA)(i,j) = cmplx(inra(i +iOff1, j +iOff2));
			}
		}
		return;
	}

	// Construct an uninitialized matrix.  Also uses promotion, although not a 
	// template; don't construct from a char!
	explicit Lmat(int const dim) :
		RA(new ra<cmplx> (dim,dim)),
		LU(new ra<cmplx> (dim,dim)),
		Eval(new ra<cmplx> (dim)),
		Evec(new ra<cmplx> (dim,dim)),
		PIV(new ra<int> (dim)),
		hasTrace(false), hasCond(false), hasLU(false), hasEig(false),
		N(dim) {
	}

	// Construct a diagonal matrix.
	Lmat(cmplx const diag, int const dim) :
		RA(new ra<cmplx> (dim,dim)),
		LU(new ra<cmplx> (dim,dim)),
		Eval(new ra<cmplx> (dim)),
		Evec(new ra<cmplx> (dim,dim)),
		PIV(new ra<int> (dim)),
		hasTrace(false), hasCond(false), hasLU(false), hasEig(false),
		N(dim) {
		for (int i=1; i<=N; ++i) {
			for (int j=1; j<=N; ++j){
				i==j ? (*RA)(i,j) = diag : (*RA)(i,j) = 0.0;
			}
		}
		return;
	}




	// Indexing - just a wrapper to the ra indexing.
	cmplx& operator() (int i1, int i2) const;

	// High-level operations
	cmplx trace();
	cmplx det();
	double cond();
    double lost_digits();
	Lmat trans() const;                                 // Constructs new Lmat
	Lmat conj() const;                                  // Constructs new Lmat
	Lmat inverse();                                     // Constructs new Lmat

	// Operators
	Lmat  operator-() const;							// Unary op
	Lmat  operator+ (Lmat const& rhs) const;			// Simple binary ops
	Lmat& operator+=(Lmat const& rhs);
	Lmat  operator- (Lmat const& rhs) const;
	Lmat& operator-=(Lmat const& rhs);

	Lmat operator*(Lmat const& rhs) const;				// Multiply/divide ops
	Lvec operator*(Lvec const& rhs) const;
	Lmat operator%(Lmat const& rhs);					// Slightly tricky ops:
	Lvec operator%(Lvec const& rhs);					// inverse of 1st by 2nd

	Lmat& operator+=(cmplx const& rhs);                 // Ops with scalars
	Lmat& operator-=(cmplx const& rhs);
	Lmat& operator*=(cmplx const& rhs);

	Lvec eigval();
	Lvec eigvec(int mode);

private:
	bool hasTrace, hasCond, hasLU, hasEig;
	cmplx Trace, Det;	double Cond;
	// Can't get by with 1 splat after >; that's the legacy of C I guess.
	// Create these ras with new, index via (*RA)(i,j)
	ra<cmplx> *RA, *LU, *Eval, *Evec;
	ra<int> *PIV;

	// Routine to fill the LU and PIV elements
	void doLU();
	// Eigen-finding routines
	void doEig();
};
