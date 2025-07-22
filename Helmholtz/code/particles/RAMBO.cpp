/*  L++ RAMBO
 *  Created by Leo Bellantoni on 8/10/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  Translation & simplification of the venerable RAMBO algorithm.
 *
 *  Generates weighted N-body events in the CoM frame using Kleiss,Stirling and
 *  Ellis, Comp.Phys.Commun. 40 (1986) 359.
 *
 *  Inputs:
 *        Np			int			Number of outgoing particles
 *        Ecm			double		Total energy in the CoM frame.
 *        M(Np)			ra<double>	Mass of outgoing particle
 *
 *    Outputs:
 *        Pcm(4,Np)		ra<double>*	(p,E) of generated particles; E is index 4
 *        wt			double*		Weight of event
 *
 *  To generate unweighted events, use the principle that (if the maximum event
 *  weight is 1) than accepting events with weight wt a fraction of the time
 *  equal to wt removes the effect of the weight.  So:
 *		ranunlgen foo;
 *		for (int i=0; i<Nthrow; ++i) {
 *			RAMBO(Np,Ecm,M,wt,Pcm);
 *			element = matrix_element(*Pcm);
 *			Wt = *wt;
 *			// Assuming real matrix element:
 *			Wt      = Wt*xp(element,2) / max(Wt*xp(element,2);  
 *			if (foo.ranunl(0) < wt) ACCEPT EVENT;
 *		}
 *
 *     This code only makes weighted events which have the property that their
 *     mean value is proportional to the integrated phase space of the PDG
 *     convention for particle decay:
 *         [1] Take the 3-d differential of momentum space for
 *             out going particle i
 *         [2] Divide by (2Ei), where Ei is the energy of the ith
 *             outgoing particle
 *         [3] Take the combined product for this quantity for all
 *             1..i..n products
 *         [4] Write the 4-dimensional Dirac delta function, with
 *             argument (Pin - sum[Pi]), the difference between the
 *             initial momentum and the sum of the outgoing momenta
 *         [5] Multiply the results of step 3 and 4.
 *         [6] Ask yourself, "why do we still have to use these
 *             limited ASCII character sets in the 21st century?"
 *
 *		Then one may calculate the partial width for a decay with unit matrix
 *		element like so:
 *
 *		PhaseSpace = 0.0;
 *		for (int i=1; i<Nthrow; ++i) {
 *			RAMBO(Nbod,Ecm,M,wt,Pcm);
 *			PhaseSpace += *wt;
 *		}
 *		PhaseSpace  = PhaseSpace / Nthrow
 *		PhaseSpace /= xp( (2*pie), (3*Nbod) )
 *		PartialWidth = ( xp( (2*pie), 4) / (2*Ecm) ) * PhaseSpace
 *
 *		Modification history:
 *		13 Aug 2007	Translation from LEOPAK version, with reference to original
 *
 */
#include "RAMBO.h"
using namespace std;


void RAMBO(int Np, double Ecm, ra<double> M, double* wt, ra<double>* Pcm) {
	static double Dbiggy=0.0;
	int Nmassive, iter;
	// If rambogen is not static, it will be recreated at each RAMBO call and
	// with typical timing, will be given substantially the same seed on each
	// call resulting in a very non-random distribution.  Here, outside of a 
	// class declaration or definition, static has the same meaning as in C.
	static ranunlgen rambogen;
	double Mtot, d, s, F,F0, G,G0, A, X,Xmax, wt2,wt3,wtM, Rmas, Po2log, bq;
	ra<double> q(4,Np), z(Np), r(4),  b(3), Pcm2(Np), m2(Np), E(Np), v(Np);

	// Check for enough energy, count != zero masses, require >2 particles
	if (Np < 2) LppUrk.LppIssue(0,"RAMBO::RAMBO [1]");
	Mtot     = 0.0;
	Nmassive = 0;
	for (int j=1; j<=Np; ++j) { 
		if (M(j) <  0.0) LppUrk.LppIssue(0,"RAMBO::RAMBO [2]");
		if (M(j) != 0.0) Nmassive += 1;
		Mtot += M(j);
	}
	if (Mtot >= Ecm) LppUrk.LppIssue(0,"RAMBO::RAMBO [3]");

	if (Dbiggy <= 1.0) {
		// Find big number Dbiggy
		Dbiggy = Dbig;
		Dbiggy = log(Dbiggy) -1.0;
	}

	// Initialize factorials for phase space weights
	Po2log = log(pie/2.0);
	z(2)   = Po2log;
	for (int i=3; i<=Np; ++i) z(i)  = z(i-1) +Po2log -2.0*log(double(i-2));
	for (int i=3; i<=Np; ++i) z(i) -= log(double(i-1));

	// Generate N massless momenta in infinite phase space
	for (int j=1; j<=Np; ++j) {
		d = 2.0*rambogen.ranunl(0) -1.0;
		s = sqrt(1.0 - d*d);
		F = 2.0*pie*rambogen.ranunl(1);
		q(4,j) = -log(rambogen.ranunl(2)*rambogen.ranunl(3));
		q(1,j) = q(4,j)*s*sin(F);
		q(2,j) = q(4,j)*s*cos(F);
		q(3,j) = q(4,j)*d;
	}

	// Calculate the parameters of the conformal transformation
	for (int i=1; i<=4; ++i) r(i) = 0.0;
	for (int j=1; j<=Np; ++j) {
		for (int i=1; i<=4; ++i) {
			r(i) += q(i,j);
		}
	}
	Rmas = Qsub( r(4), Qadd( r(1),r(2),r(3)) );
	// B has indeed just spatial dimensions
	for (int i=1; i<=3; ++i) b(i) = -r(i)/Rmas;
	G = r(4)/Rmas;
	A = 1.0/(1.0+G);
	X = Ecm/Rmas;

	// Transform the Q's conformally into the P's (in CoM)
	for (int j=1; j<=Np; ++j) {
		bq = b(1)*q(1,j) +b(2)*q(2,j) +b(3)*q(3,j);
		for (int i=1; i<=3; ++i) (*Pcm)(i,j) = X*( q(i,j) +b(i)*(q(4,j) +A*bq) );
		(*Pcm)(4,j) = X*( G*q(4,j) +bq );
	}

	// Calculate weights.  Original generated warning but continued apace for
	// either potential overflow or potential underflow conditions.  This 
	// code aborts on overflow, lets (presumably infrequent) underflow through.
	*wt = Po2log;
	if (Np != 2) *wt = (2.0*Np -4.0)*log(Ecm) +z(Np);
	if (*wt > Dbiggy) LppUrk.LppIssue(200,"RAMBO::RAMBO [1]");

	// Return if all particles massless
	if (Nmassive == 0) {
		*wt = exp(*wt);
		return;
	}

	// If some particles are massive, rescale momenta by "A FACTOR X"
	Xmax = sqrt ( 1.0 -xp(Mtot/Ecm,2) );
	for (int j=1; j<=Np; ++j) {
		m2(j)   = xp(M(j),2);
		Pcm2(j) = xp((*Pcm)(4,j),2);
	}
	iter = 0;
	X    = Xmax;

	ORIGINALINE_302:		// Too hard to clean this up!
	F0 = -Ecm;
	G0 = 0.0;
	for (int j=1; j<=Np; ++j) {
		E(j) = sqrt( m2(j) +X*X*Pcm2(j) );
		F0   = F0 +E(j);
		G0   = G0 +Pcm2(j)/E(j);
	}
	// The accuracy criteria 1.0e-14 is from the original which is in REAL*8,
	// which is the double type in most C implementations.
	double const ACC = 1.0e-14;
	if (fabs(F0) > ACC*Ecm) {
		iter += 1;
		if (iter > 6) {
			LppUrk.LppIssue(201,"RAMBO::RAMBO");
			goto ORIGINALINE_305;
		}
		X -= F0/(X*G0);
		goto ORIGINALINE_302;
	}

	ORIGINALINE_305:
	for (int j=1; j<=Np; ++j) {
		v(j) = X *( (*Pcm)(4,j) );
		for (int i=1; i<=3; ++i) {
			(*Pcm)(i,j) *= X;
		}
		(*Pcm)(4,j) = E(j);
	}

	// Calculate the mass-effect weight factor
	wt2 = 1.0;			wt3 = 0.0;
	for (int j=1; j<=Np; ++j) {
		wt2 *=    v(j)    /E(j);
		wt3 += xp(v(j),2) /E(j);
	}
	wtM = (2.0*Np-3.0) *log(X) +log( Ecm*(wt2/wt3) );
	*wt  += wtM;
	if (*wt > Dbiggy) LppUrk.LppIssue(200,"RAMBO::RAMBO [2]");
	*wt = exp(*wt);
	return;
}
