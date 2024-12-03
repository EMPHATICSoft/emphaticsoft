/*  L++ Lmat
 *  Created by Leo Bellantoni on 1/23/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A slow & large but high-level matrix built out of SQUARE complex<double> ra
 *  objects.  If you need to go fast, work in ra objects or valarrays.
 *
 */
#include "Lmat.h"
using namespace std;


// Default, copy and assignment constructors
Lmat::Lmat() :
    RA(nullptr),LU(nullptr),Eval(nullptr),Evec(nullptr),PIV(nullptr),
    hasTrace(false), hasCond(false), hasLU(false), hasEig(false),
    N(0),Trace(0),Det(0),Cond(0) {};
Lmat::Lmat(Lmat const& inLmat)
    :     RA(nullptr),LU(nullptr),Eval(nullptr),Evec(nullptr),PIV(nullptr) {
    N = inLmat.N;   // Prevent LppIssue 9 in operator=
	*this = inLmat;
    return;
}
Lmat& Lmat::operator=(Lmat const& rhs) {
	if (this != &rhs) {
        if (N != rhs.N)
            LppUrk.LppIssue(9, "Lmat::operator=(Lmat const&)");
		delete RA;
		delete LU;
		delete Eval;
		delete Evec;
		delete PIV;
        if (rhs.RA) {
            RA  = new ra<cmplx>(N,N);
            *RA = *rhs.RA;
        }
        if (rhs.LU) {
            LU  = new ra<cmplx>(N,N);
            *LU = *rhs.LU;
        }
        if (rhs.Eval) {
            Eval  = new ra<cmplx>(N);
            *Eval = *rhs.Eval;
        }
        if (rhs.Evec) {
            Evec  = new ra<cmplx>(N,N);
            *Evec = *rhs.Evec;
        }
        if (rhs.PIV) {
            PIV  = new ra<int>(N);
            *PIV = *rhs.PIV;
        }

        hasTrace = rhs.hasTrace;
        hasCond  = rhs.hasCond;
        hasLU    = rhs.hasLU;
        hasEig   = rhs.hasEig;
        Trace    = rhs.Trace;
        Det      = rhs.Det;
        Cond     = rhs.Cond;
	}
	return *this;
}



// Destructor.
Lmat::~Lmat() {
	delete RA;  	delete LU;
	delete Eval;	delete Evec;
	delete PIV;
    return;
}

// Indexing - just a wrapper to the ra indexing.
cmplx& Lmat::operator() (int i1, int i2) const { return (*RA)(i1,i2); }



//==============================================================================
//===  LU decomposition related functions ======================================
//==============================================================================
// Trace
cmplx Lmat::trace() {
	if (!hasTrace) {
		Trace = 0.0;
		for (int i=1; i<=N; ++i) {
			Trace += (*RA)(i,i);
		}
		hasTrace = true;
	}
	return Trace;
}

// Determinant via LU decomposition
cmplx Lmat::det() {
	if (!hasLU) {
        doLU();
		for (int i=1; i<=N; ++i) {
			Det *= (*LU)(i,i);
		}
	}
	return Det;
}
// Do not call doLU directly; future calls to det() will give 
// then erroneous result.  Call det() to ensure (*LU) is filled.
void Lmat::doLU() {
	/* LU is set to the LU decomposition of a row-wise permutation of the
	original matrix, arranged with the factors by which each row is multiplied
	in the elimination as the lower-left side of the array.  The diagonals for 
	the L are all 1 and are therefore omitted; the diagonal and the upper right
	triangle are the U matrix, which is the rows after the elimination process.
	PIV is set to a vector that records the row permutation effected by the 
	partial pivoting.  Row exchanges are considered in order; so if PIV(1)=3
	and PIV(2)=4 and PIV(3)=3 then first row 1 was swapped with row 3, then
	row 2 was swapped with row 4, and then row 3 was not swapped with anything.
	Sign of determinant is determined by number of row swaps*/

	int i,j,k,imax;
	double aamax,tmp;
	double* vv = new double[N+1];
	cmplx sum,dum;
	Det = 1.0;

	// In-place algorithm; so move RA into LU for starters
	(*LU).val = (*RA).val;
	// Loop over rows to get scaling information
	for (i=1; i<=N; ++i) {
		aamax = 0.0;
		for (j=1; j<=N; ++j) {
			tmp = Qadd( (*LU)(i,j).real(), (*LU)(i,j).imag() );
			aamax=max(aamax,tmp);
		}
		if (aamax <= 0.0 ) LppUrk.LppIssue(100,"Lmat::doLU()");
		vv[i] = 1.0/aamax;
	}

	// Loop over columns:
	for (j=1; j<=N; ++j) {
		for (i=1; i<=(j-1); ++i) {
			sum = (*LU)(i,j);
			for (k=1; k<=(i-1); ++k) {
				sum -= (*LU)(i,k) * (*LU)(k,j);
			}
			(*LU)(i,j) = sum;
		}
		aamax = 0.0;        imax = j;
		for (i=j; i<=N; ++i) {
			sum = (*LU)(i,j);
			for (k=1; k<=(j-1); ++k) {
				sum -= (*LU)(i,k) * (*LU)(k,j);
			}
			(*LU)(i,j) = sum;
			// Figure of merit for the pivot:
			tmp = vv[i]*abs(sum);
			if (tmp >= aamax) {
				imax  = i;		aamax = tmp;
			}
		}
		if (j != imax) {
			// Interchange rows
			for (k=1; k<=N; ++k) {
				dum           = (*LU)(imax,k);
				(*LU)(imax,k) = (*LU)(j,k);
				(*LU)(j,k)    = dum;
			}
			Det *= -1.0;
			vv[imax] = vv[j];
		}
		(*PIV)(j) = imax;
		if ( (*LU)(j,j) == 0.0 ) LppUrk.LppIssue(100,"Lmat::doLU()");

		if (j != N) {
			dum = 1.0/(*LU)(j,j);
			for (i=(j+1); i<=N; ++i) {
				(*LU)(i,j) = (*LU)(i,j) *dum;
			}
		}
	}
	delete [] vv;
	hasLU = true;
	return;
}

// Condition number via Frobenius norm, along with an estimate of digits lost to
// roundoff using same.  If this number is too big, you may be in for a 
// singular value decomposition, probably effected by calls to LAPACK.
double Lmat::cond() {
	if (!hasCond) {
		cmplx cnd(0),cndi(0);
		for (int i=1; i<=N; ++i) {
			for (int j=1; j<=N; ++j) {
				cnd  += xp( abs((*RA)(j,i)), 2);
			}
		}
		Lmat that = this->inverse();
		for (int i=1; i<=N; ++i) {
			for (int j=1; j<=N; ++j) {
				cndi += xp( abs( that(j,i)), 2);
			}
		}
        Cond = sqrt( real(cnd*cndi) );
		hasCond = true;
	}
	return Cond;
}
double Lmat::lost_digits() {
    return log10(cond()) + log10(10.0*N);
}

// Transpose, conjugate
Lmat Lmat::trans() const {
	Lmat retval(0, N);
	// Other than Trace, data members initialized by constructor above
	retval.hasTrace = hasTrace;
	retval.Trace    = Trace;
	for (int i=1; i<=N; ++i) {
		for (int j=1; j<=N; ++j){
			(*retval.RA)(i,j) = (*RA)(j,i);
		}
	}
	return retval;
}
Lmat Lmat::conj() const {
	Lmat retval(0, N);
	retval.hasTrace = hasTrace;
    retval.Trace    = std::conj(Trace);
	// Argument list for function passed to apply not needed?  Probably because
	// (modulo const-correctness issues) function's input and output must be the
	// same type as the valarray's parameterization, so argument list not needed
	// to fully specify the signature.
    // Can't use (*rhs.RA).val.apply(std::conj) - dunno why.  Needs testing.
    for (int i=0; i<N*N; ++i) (*retval.RA).val[i] = std::conj((*RA).val[i]);
	return retval;
}

// Inverse via LU decomp & solve against unit matrix
Lmat Lmat::inverse() {
	Lmat unit(1.0,N);
	Lmat retval(0, N);
    retval = (*this)%unit;
	return retval;
}


//==============================================================================
//===  Operators  ==============================================================
//==============================================================================
// Unary operator
Lmat Lmat::operator-() const {
	Lmat retval(*this);
	retval.Det   = -Det;
	retval.Trace = -Trace;
	retval.hasLU = false;
	(*retval.RA).val = -((*RA).val);
    // But the valarray::operator= can change the location of the valarray,
    // so we must also
    (*retval.RA).fixptr();
	return retval;
}

// Simple binary operators
Lmat Lmat::operator+(Lmat const& rhs) const {
	Lmat retval(*this);		return (retval += rhs);
}
Lmat& Lmat::operator+=(Lmat const& rhs) {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lmat::operator+ or +=(Lmat)");
	// In principle, one could add traces if they both exist
	hasCond = hasEig = hasLU = hasTrace = false;
	(*RA).val += (*rhs.RA).val;
	return *this;
}
Lmat Lmat::operator-(Lmat const& rhs) const {
	Lmat retval(*this);		return (retval -= rhs);
}
Lmat& Lmat::operator-=(Lmat const& rhs) {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lmat::operator+ or +=(Lmat)");
	// In principle, one could add traces if they both exist
	hasCond = hasEig = hasLU = hasTrace = false;
	(*RA).val -= (*rhs.RA).val;
	return *this;
}


// Multiply and divide-like operators
Lmat Lmat::operator*(Lmat const& rhs) const {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lmat::operator*(Lmat)");
	cmplx sum;
	Lmat retval(0,N);
	// In principle, one could multiply determinants (and maybe condition
	// numbers ?) if they both exist
	for (int i=1; i<=N; ++i) {
		for (int j=1; j<=N; ++j) {
			sum = 0;
			for (int k=1; k<=N; ++k) {
				sum += (*RA)(i,k) * (*rhs.RA)(k,j);
			}
		(*retval.RA)(i,j) = sum;
		}
	}
	return retval;
}
Lvec Lmat::operator*(Lvec const& rhs) const {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lmat::operator*(Lvec)");
	cmplx sum;
	Lvec retval(0,N);
	for (int i=1; i<=N; ++i) {
		sum = 0;
		for (int j=1; j<=N; ++j) {
			sum += (*RA)(i,j) * rhs(j);
		}
		(retval)(i) = sum;
	}
	return retval;
}

// Gaussian elimination with a matrix, i.e. (A^-1)*B.  Lmat A is effectively
// const - doLU is called via det() but the changes that creates are hidden
Lmat Lmat::operator%(Lmat const& rhs) {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lmat::operator%(Lmat)");
	Lmat retval(0,N);
	Lvec colmR(0,N), colmL(0,N);
	for (int colnum=1; colnum<=N; ++colnum) {
		for (int j=1; j<=N; ++j) {
			colmR(j) = rhs(j,colnum);
		}
		colmL = (*this)%colmR;
		for (int j=1; j<=N; ++j) {
			(*retval.RA)(j,colnum) = colmL(j);
		}
	}
	return retval;
}

// Gaussian elimination with a vector i.e. solve Ax = b.  Lmat is
// effectively const - doLU is called but this is hidden from the user.
Lvec Lmat::operator%(Lvec const& rhs) {
	if (N != rhs.N) LppUrk.LppIssue(9,"Lmat::operator%(Lvec)");
	int i,j,k,m;
	cmplx sum;

	Lvec b(rhs);
	if (!hasLU) det();
	m = 0;
	// Massage b to allow for the pivoting operation
	for (i=1; i<=N; ++i) {
		k    = (*PIV)(i);
		sum  = b(k);
		b(k) = b(i);
		if (m!=0) {
			for (j=m; j<=(i-1); ++j) {
				sum -= (*LU)(i,j) * b(j);
			}
		} else if (sum != 0.0) {
			m = i;
		}
		b(i) = sum;
	}
	// Unwind the elimination
	for (i=N; i>=1; i--) {
		sum = b(i);
		if (i<N) {
			for (j=(i+1); j<=N; ++j) {
				sum -= (*LU)(i,j)*b(j);
			}
		}
		b(i) = sum / (*LU)(i,i);
	}
	return b;
}

// Operations with scalars on the right hand side
Lmat& Lmat::operator+=(cmplx const& rhs) {
	// In principle, one could add something to the trace
	hasCond = hasEig = hasLU = hasTrace = false;
	(*RA).val += rhs;
	return *this;
}
Lmat& Lmat::operator-=(cmplx const& rhs) {
	// In principle, one could add something to the trace
	hasCond = hasEig = hasLU = hasTrace = false;
	(*RA).val -= rhs;
	return *this;
}
Lmat& Lmat::operator*=(cmplx const& rhs) {
	// In principle, one could multiply determinant (and maybe condition
	// numbers ?) by something
	hasCond = hasEig = hasLU = hasTrace = false;
	(*RA).val *= rhs;
	return *this;
}


//==============================================================================
//===  Eigenstuff  =============================================================
//==============================================================================
Lvec Lmat::eigval() {
	if (!hasEig) {
		doEig();
	}
	// Move the output to an Lvec from an ra
	Lvec retval(0,N);
	for (int i=1; i<=N; ++i) {
		retval(i) = (*Eval)(i);
	}
	return retval;
}

Lvec Lmat::eigvec(int mode) {
	if (!hasEig) {
		doEig();
	}
	// Move the output to an Lvec from an ra
	Lvec retval(0,N);
	for (int i=1; i<=N; ++i) {
		retval(i) = (*Evec)(i,mode);
	}
	return retval;	
}

void Lmat::doEig() {
	// An example of a call to LAPACK.  ALL arguments must be passed by
	// reference, including scalar arguments such as matrix dimensions.
	// The name of a C/C++ array alone is a reference.  See also
	// <http://www.netlib.org/clapack/readme>.
	int i,j,k;

	// Copy input array into LAPACK type defined in clapack.h.  I find wierd
	// problems if compiled for Release configuration.  It looks like (*RA)
	// is getting clobbered, but I am not sure at all.
	__CLPK_integer n=N;
	__CLPK_doublecomplex* A = new __CLPK_doublecomplex[N*N];
	for (j=1; j<=N; ++j) {
		for (i=1; i<=N; ++i) {
			k = (i-1) +N*(j-1);
			A[k].r = (__CLPK_doublereal) real((*RA)(i,j));
			A[k].i = (__CLPK_doublereal) imag((*RA)(i,j));
		}
	}

	// Eigenresults appear here
	__CLPK_doublecomplex* W  = new __CLPK_doublecomplex[N];
	__CLPK_doublecomplex* VR = new __CLPK_doublecomplex[N*N];
	// Other stuff that is created along the way
	__CLPK_doublecomplex* vl  = new __CLPK_doublecomplex[N];
	__CLPK_integer ilo,ihi;
	__CLPK_doublereal* scale  = new __CLPK_doublereal[N];
	__CLPK_doublereal abnrm;
	__CLPK_doublereal* rconde = new __CLPK_doublereal[N];
	__CLPK_doublereal* rcondv = new __CLPK_doublereal[N];
	__CLPK_doublereal* rwork  = new __CLPK_doublereal[2*N];
	// Workspace for enquiry
	__CLPK_doublecomplex WORKENQ[1];
	__CLPK_integer LWORK,INFO;

	// First call is to find optimal workspace allocation.
	LWORK = -1;
	zgeevx_("B","N","V","N",	&n,A,&n, W, vl,&n, VR,&n,
			&ilo,&ihi,scale,&abnrm,rconde,rcondv,
			WORKENQ,&LWORK, rwork, &INFO);
	LWORK = nint(WORKENQ[0].r);
	__CLPK_doublecomplex* WORK  = new __CLPK_doublecomplex[LWORK];


	// Do it for real this time
	zgeevx_("B","N","V","N",	&n,A,&n, W, vl,&n, VR,&n,
			&ilo,&ihi,scale,&abnrm,rconde,rcondv,
			WORK,&LWORK, rwork, &INFO);
	if (INFO != 0) LppUrk.LppIssue(101,"Lmat::doEig()");
	
	// Move the result into the Lmat variables.
	int mode;
	for (mode=1; mode<=N; ++mode) {
		(*Eval)(mode) = cmplx(W[mode-1].r,W[mode-1].i);
		for (j=1; j<=N; ++j) {
			k = (j-1) +N*(mode-1);
			(*Evec)(j,mode) = cmplx(VR[k].r,VR[k].i);
		}
	}
	hasEig = true;
	
	// Pick up after yourself
	delete [] A;		delete [] W;		delete [] VR;
	delete [] vl;		delete [] scale;	delete [] rconde;
	delete [] rcondv;	delete [] rwork;	delete [] WORK;

	return;
}
