/*  L++ Lvec4
 *  Created by Leo Bellantoni on 11/19/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  A 4-vector class, implemented as simply as possible.
 *
 */
#include "Lvec4.h"
using namespace std;





// The fab four
//Lvec4::Lvec4() : p_(nullptr) {};
Lvec4::Lvec4(Lvec4 const& inLvec4)
    : p_(nullptr) {
	*this = inLvec4;
}
Lvec4& Lvec4::operator=(Lvec4 const& rhs) {
	if (this != &rhs) {
        delete p_;
        if (rhs.p_) {
            p_ = new Lvec(0.0,4);
            for (int i=1; i<=4; ++i) (*p_)(i) = (*rhs.p_)(i);
        }
    }
	return *this;
}
Lvec4::~Lvec4() {
	delete p_;
}


// Constructors, destructors.  Refer to p_ rather than this, i.e. don't use
// the () operator of Lvec4, for clarity if nothing else
Lvec4::Lvec4(double x, double y, double z, double e) :
	p_(new Lvec(0.0,4)) {
	(*p_)(1) = x;		(*p_)(2) = y;		(*p_)(3) = z;
	(*p_)(4) = e;
}
Lvec4::Lvec4(Lvec const p, double const m) :
	p_(new Lvec(0.0,4)) {
	if (p.N != 3) LppUrk.LppIssue(0,"Lvec4::Lvec4(Lvec,double)");
	(*p_)(1) = p(1);	(*p_)(2) = p(2);	(*p_)(3) = p(3);
	double pmag = (*p_).mag();
	(*p_)(4) = Qadd(pmag,m);
}



//==============================================================================
//===  Indexing, angles, transverse & other simple kinematics ==================
//==============================================================================
// Getters and setters
double& Lvec4::operator() (int i) {
    // Indexing is a wrapper to Lvec indexing, in turn a wrapper to ra
    // indexing.  Can't use real() in complex<_Tp> as an l-value in recent
    // compiler versions.
    if ((i<1)||(i>4)) LppUrk.LppIssue(0,"Lvec4::operator(int)");
    
    // Well, it seems to work
    std::complex<double>* temp = &(*p_)(i);
    double* retval = (double*)(temp);
    return *retval;
}
double Lvec4::operator() (int i) const {
	// const-correct r-value form
	if ((i<1)||(i>4)) LppUrk.LppIssue(0,"Lvec4::operator(int) const");
	return real((*p_)(i));
}	
	
double Lvec4::P() const {  return Qadd((*this)(1),(*this)(2),(*this)(3));	}
double Lvec4::M() const {
	double e = (*this)(4);
	if (e<0) LppUrk.LppIssue(204,"Lvec4::M()");
	double p = P();
	return e>=p ? +Qsub(e,p) : -Qsub(p,e);
}
void Lvec4::setM(double m) {
    double temp;
	if (m>=0) {
        temp = Qadd(P(),m);
	} else {
		temp = Qsub(P(),m);
	}
    (*this)(4) = temp;
	return;
}
Lvec Lvec4::threepart() const {  return (*p_);  }



//==============================================================================
//===  Projections, transverse quantities, angles & simple kinematics ==========
//==============================================================================
// Projections and simple kinematics
double Lvec4::cosTheta() const {
	double p = P();
    if (p==0) LppUrk.LppIssue(203,"Lvec4::cosTheta()");
	return (*this)(3)/p;
}
double Lvec4::phi() const	   {  return atan2((*this)(2),(*this)(1));  }

double Lvec4::Pt() const	   {  return Qadd ((*this)(1),(*this)(2));  }
double Lvec4::Et() const	   {  
	double pmag = P();
	double e    = (*this)(4);
	if (e<0) LppUrk.LppIssue(204,"Lvec4::Et()");
	return pmag>0.0 ? (e/pmag)*Pt() : 0.0;
}

double Lvec4::gamma() const {  
	double b = beta();
    if ( b<0 || b>=1 ) LppUrk.LppIssue(204,"Lvec4::gamma()");
    return 1.0/Qsub(1.0,b);
}
double Lvec4::beta() const {
	double e = (*this)(4);
	if (e<0) LppUrk.LppIssue(204,"Lvec4::beta() [1]");
	if (e==0) LppUrk.LppIssue(204,"Lvec4::beta() [2]");
	return P()/e;
}

// Rapidity related
double Lvec4::p_rap() const {
	// The pseudo-rapidity is -ln(tan(theta/2)); for theta = 0, pi (i.e.,
	// along the +z, -z axis) return a signed, large (but not too large!) result
	double cosT = cosTheta();
	if (SQR(cosT)!=1.0) {
		return -0.5*log( (1.0-cosT)/(1.0+cosT) );
	} else {
		LppUrk.LppIssue(203,"Lvec4::p_rap()");
		return cosT*0.5*Dbig;
    }
}
double Lvec4::rap() const {
	// The rapidity is not defined for Pz > E
	double e = (*this)(4);
	if (e<0) LppUrk.LppIssue(204,"Lvec4::rap() [1]");
	double z = (*this)(3);
	if (e>z) {
		return -0.5*log( (e-z)/(e+z) );
	} else {
		LppUrk.LppIssue(203,"Lvec4::rap() [2]");
		return cosTheta()*0.5*Dbig;
	}
}
double Lvec4::deltaR(Lvec4 const& other) const {
	// Uses the pseudo-rapidity
	double deta = p_rap() -other.p_rap();
	double dphi = phi()   -other.phi();
	if (dphi >  +pie) dphi -= 2*pie;		// phi() uses atan2
	if (dphi <= -pie) dphi += 2*pie;
	return Qadd(deta,dphi);
}



//==============================================================================
//===  Operators ===============================================================
//==============================================================================
Lvec4  Lvec4::operator-() const {
	Lvec4 retval(*this);
	// Use the unary operator of Lvec
	(*retval.p_) = -(*p_);
	return retval;
}

Lvec4& Lvec4::operator*=(double a) {
	(*p_) *= a;
	return *this;
}

Lvec4  Lvec4::operator+ (Lvec4 const& rhs) const {
	// Add 4 vectors
	Lvec4 retval(*this);	return (retval += rhs);
}
Lvec4& Lvec4::operator+=(Lvec4 const& rhs) {
	(*p_) += (*rhs.p_);
	return *this;
}

double Lvec4::dot3(Lvec4 const& other) const {
	// Euclidean metric in 3 D
	return (*this)(1)*other(1) +(*this)(2)*other(2) +(*this)(3)*other(3);
}
double Lvec4::operator*(Lvec4 const& other) const {
	// Minkowski metric in 4 D
	double pp = (*this)(1)*other(1) +(*this)(2)*other(2) +(*this)(3)*other(3);
	double ee = (*this)(4)*other(4);
	return ee-pp;
}


//==============================================================================
//===  Boosts, decay angles  ===================================================
//==============================================================================
Lmat Lvec4::FindBoost() const {
	// Uses the formulas on pg. 34 of R. Hagedorn's "Relativistic Kinematics"
	// (1973) W.A. Benjamin, Inc.  These are equivalent to the forms in
	// ROOT's Boost method for its TLorentzVector class.
	if (M() <= 0.0) LppUrk.LppIssue(204,"Lvec4::FindBoost() [1]");
	Lmat retval(0,4);

	Lvec vbeta(0,3);
	double e = -(*this)(4);		// Minus sign to get BACK into own rest frame
	if (e>0) LppUrk.LppIssue(203,"Lvec4::FindBoost() [2]");
	for (int i=1; i<=3; ++i) {
		vbeta(i) = (*this)(i) / e;
	}
	double GAMMA = gamma();
	double g2g1 = xp(GAMMA,2) / (GAMMA +1);

	retval(4,4) =          GAMMA;
	retval(1,4) = vbeta(1)*GAMMA;
	retval(2,4) = vbeta(2)*GAMMA;
	retval(3,4) = vbeta(3)*GAMMA;

	retval(1,1) = 1.0 + vbeta(1)*vbeta(1)*g2g1;
	retval(2,2) = 1.0 + vbeta(2)*vbeta(2)*g2g1;
	retval(3,3) = 1.0 + vbeta(3)*vbeta(3)*g2g1;

	retval(1,2) =       vbeta(1)*vbeta(2)*g2g1;
	retval(1,3) =       vbeta(1)*vbeta(3)*g2g1;
	retval(2,3) =       vbeta(2)*vbeta(3)*g2g1;

	retval(4,1) = retval(1,4);
	retval(4,2) = retval(2,4);
	retval(4,3) = retval(3,4);
	retval(2,1) = retval(1,2);
	retval(3,1) = retval(1,3);
	retval(3,2) = retval(2,3);

	return retval;
}
Lvec4 Lvec4::ApplyBoost(Lmat boost) {
	Lvec4 retval;
	for (int i=1; i<=4; ++i) {
		retval(i) = 0.0;
		for (int j=1; j<=4; ++j) {
			retval(i) += real( boost(i,j) * (*this)(j) );
		}
	}
	return retval;
}

double Lvec4::decang_O(Lvec4 const& OtherProduct) const {
	Lvec4 InitialState = (*this)+OtherProduct;
	return decang_I(InitialState);
 }
double Lvec4::decang_I(Lvec4 const& InitialState) const {
	double Bframe = InitialState.beta();
	if (Bframe >= 1.0) {
		if (Bframe > 1.00001) {
			// It ain't round-off.  Fatal!
			LppUrk.LppIssue(204,"Lvec4::decang_I(Lvec4)");
		} else {
			// Let 'em off with a warning.
			LppUrk.LppIssue(203,"Lvec4::decang_I(Lvec4)");
			return 0.0;			// beta=1 => xx=0
		}
	}

	double ppar = dot3(InitialState) / InitialState.P();
	double pp   = ppar - Bframe*(*this)(4);
	double xx   = xp(pp,2) + (P()-ppar)*(P()+ppar)*(1.0-Bframe)*(1.0+Bframe);
	return pp/sqrt(xx);
}

double Lvec4::Pinvis2(Lvec4 const& InitialState, double const Mhypo) const {
	double Min2    = xp(InitialState.M(),2);
	double Mrec2   = xp(             M(),2);
	double Minvis2 = xp(           Mhypo,2);
	double Pt2;
	
	Pt2 = dot3(InitialState) / InitialState.P();
	Pt2 = xp(P(),2) - xp(Pt2,2);

	double NUMER = xp( Min2 +Minvis2 -Mrec2, 2)  -4.0*Min2*(Minvis2 +Pt2);
	return NUMER / (4.0*Min2);
}
