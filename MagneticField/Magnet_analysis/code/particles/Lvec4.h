/*  L++ Lvec4
 *  Created by Leo Bellantoni on 11/19/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A 4-vector class, implemented as simply as possible on Lvecs.  But not
 *  inherited from Lvec, as an Lvec is of arbitrary dimension and can be
 *  complex.  Mass is derived, imaginary parts of Lvecs are ignored.
 *  Indexing is 1,2,3 for spacelike coordinates and 4 for the timelike.  Code
 *  uses the Qsub, Qadd methods for best precision.
 *
 *  As a rule, exceptions such as tachyonic conditions generate warnings and
 *  the member function suffering from them will return zero, or something
 *  that could pass for its inverse.
 *
 */
#pragma once



#include "Lvec.h"
#include "Lmat.h"



class Lvec4{
public:
    // Basic 4, but!  Force the non-trivial signature as the
    // replacement for Lvec4();
	Lvec4(double x=0, double y=0, double z=0, double e=0);
    Lvec4(Lvec4 const& inLvec4);
	Lvec4& operator=(Lvec4 const& rhs);
	~Lvec4();



	// Constructors for users
	// Different signature for p,m than for p,E.
	Lvec4(Lvec const p, double const m);

	// Getters and setters.  E = (*this)(4) is not required to be >=0 in setter
	// but implementation of Lvec4 members requires E>=0 if that seems sensible.
	double& operator() (int i);
	double  operator() (int i) const;
	double M() const;			// M = 0 if p>=E; exception if E<0
	void setM(double m);		// Will also change E
	double P() const;			// Length of the 3-vector
	Lvec threepart() const;		// Gets the 3-vector

	// Projections and simple kinematics
	double cosTheta() const;	// Get the 3-vector in spherical
	double phi() const;			// rho is l3 of course!
	double Pt() const;			// Magnitude of the (x,y) projection
	double Et() const;			// Pt by E/p; "Speed preserving" cf. arXiv
	double gamma() const;		// 1108.5182
	double beta() const;

	// Rapidity related
	double p_rap() const;						// The pseudo-rapidity
	double rap() const;							// The rapidity
	double deltaR(Lvec4 const& other) const;	// Uses the pseudo-rapidity

	// Operators
	Lvec4& operator*=(double a);                // Multiply by a scalar
	Lvec4  operator- () const;					// Unary op
	Lvec4  operator+ (Lvec4 const& rhs) const;	// Add 4-vectors 
	Lvec4& operator+=(Lvec4 const& rhs);
	double dot3(Lvec4 const& other) const;		// Euclidean metric in 3 D
	double operator*(Lvec4 const& other) const;	// Minkowski metric in 4 D

	// Boosts, decay angles, transverse mass, undetected fragments.
	// FindBoost will find the boost that puts this 4-vector into its own rest
	// frame.  Emits a warning for attempt to boost to lightspeed.  It returns a
	// 4x4 Lmat argument for use in ApplyBoost.  ApplyBoost will apply the boost
	// in its argument to this 4-vector.
	Lmat FindBoost() const;
	Lvec4 ApplyBoost(Lmat boost);

	// decang_0 returns the cosine of angle between this Lvec4, boosted to frame
	// of InitialState = (this+OtherProduct) system, and lab frame direction of
	// InitialState system.  So if OtherProduct is at rest in the lab frame,
	// decang_0 will return +1.
	// decang_I commutes the cosine of angle between this Lvec4, boosted to frame
	// of InitialState system, and lab frame direction of InitialState.  So if
	// this Lvec4 is at rest in the lab frame, will return -1.
	double decang_O(Lvec4 const& OtherProduct) const;
	double decang_I(Lvec4 const& InitialState) const;

	// Pinvis2 uses only the mass and Pt (relative to InitialState) of (*this),
	// and only the mass and direction of InitialState.  Assuming that the other
	// decay products have mass Mhypo, it returns the square of the longitudinal
	// momentum of other decay products, relative to InitialState.
    // For example, suppose a Ks was produced at a specific IP and there are a
	// pair of tracks from a decay point.  Create InitialState with spatial
	// components proportional to the direction vector from the two verticies,
	// and mass 497.6MeV.  The unseen decay product is a pi0 in the signal; set
	// Mhypo to 135.0MeV.  Construct an Lvec4 from the two tracks with assumed
	// masses of 139.6MeV.  Then the Pinvisi2 method of this Lvec4 will be
	// somewhere between 0 and the square of 132.0MeV; events outside this range
	// may be killed as background.
	double Pinvis2(Lvec4 const& InitialState, double const Mhypo) const;

private:
	Lvec* p_;
};
