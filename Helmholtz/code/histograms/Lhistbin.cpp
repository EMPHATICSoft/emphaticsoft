/*  L++ Lhistbin
 *  Created by Leo Bellantoni on 11/26/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A histogram bin - it's contents only, not it's position on any axis.
 *
 */
#include "Lhistbin.h"


// Constructors; default destructor is fine
Lhistbin::Lhistbin() : SumW(0.0), SumW2(0) {}
Lhistbin::Lhistbin(Lhistbin const& inBin)
    : SumW(0.0), SumW2(0) {
    *this = inBin;
}
Lhistbin& Lhistbin::operator=(Lhistbin const& rhs) {
    if (this != &rhs) {
        SumW   = rhs.SumW;
        SumW2  = rhs.SumW2;
    };
    return *this;
}


// Bin arithmetic, the basis of histogram math
Lhistbin  Lhistbin::operator+ (Lhistbin const& rhs) const{
	Lhistbin retval(*this);	    return (retval += rhs);
}
Lhistbin& Lhistbin::operator+=(Lhistbin const& rhs) {
    SumW  += rhs.SumW;
    SumW2 += rhs.SumW2;
    return *this;
}

Lhistbin  Lhistbin::operator- (Lhistbin const& rhs) const{
	Lhistbin retval(*this);	return (retval -= rhs);
}
Lhistbin& Lhistbin::operator-=(Lhistbin const& rhs) {
    SumW  -= rhs.SumW;
    SumW2 -= rhs.SumW2;
    return *this;
}

Lhistbin  Lhistbin::operator* (Lhistbin const& rhs) const{
	Lhistbin retval(*this);	return (retval *= rhs);
}
Lhistbin& Lhistbin::operator*=(Lhistbin const& rhs) {
    double relnumerr, reldenerr;
    if (SumW == 0) {
        relnumerr = 0.0;
    } else {
        relnumerr = sqrt(    SumW2)/    SumW;
    }
    if (rhs.SumW == 0) {
        reldenerr = 0.0;
    } else {
        reldenerr = sqrt(    SumW2)/    SumW;
    }
    SumW  *= rhs.SumW;
    SumW2  = SumW * Qadd(relnumerr, reldenerr);
    SumW2 *= SumW2;
    return *this;
}

void Lhistbin::Scale(double const& scale) {
    SumW  *= scale;
    SumW2 *= xp(scale,2);
}

Lhistbin  Lhistbin::operator/ (Lhistbin const& rhs) const{
	Lhistbin retval(*this);	return (retval /= rhs);
}
Lhistbin& Lhistbin::operator/=(Lhistbin const& rhs) {
    double relnumerr, reldenerr;
    if (SumW == 0) {
        relnumerr = 0.0;
    } else {
        relnumerr = sqrt(    SumW2)/    SumW;
    }
    if (rhs.SumW == 0) {
        LppUrk.LppIssue(1,"Lhistbin:operator/ or /=(Lhistbin)");
    }
    reldenerr = sqrt(rhs.SumW2)/rhs.SumW;
    SumW  /= rhs.SumW;
    SumW2  = SumW * Qadd(relnumerr, reldenerr);
    SumW2 *= SumW2;
    return *this;
}

Lhistbin  Lhistbin::operator% (Lhistbin const& rhs) const{
	Lhistbin retval(*this);	return (retval %= rhs);
}
Lhistbin& Lhistbin::operator%=(Lhistbin const& rhs) {
    // binerrW has checks about num/den not in range 0 to 1; but this
    // method should not have altered any values in either this or in
    // rhs if an (admittedly, Fatal) exception is thrown by binerrW
    double tmpW  = SumW / rhs.SumW;
    // binerrW is error on the ratio
    double tmpW2 = xp(binerrW(SumW,rhs.SumW,rhs.SumW2),2);
    SumW  = tmpW;
    SumW2 = tmpW2;
    return *this;
}


// Filling is the basic setter; value is sum of weights, error
// is square root of sum of weights squared
void   Lhistbin::Fill(double weight) {
    SumW  += weight;
    SumW2 += xp(weight,2);
}

double Lhistbin::GetValue() { return SumW; }
void   Lhistbin::SetValue(double value) {
    // Doesn't set SumW2!
    SumW = value;
}
double Lhistbin::GetError() { return sqrt(SumW2); }
void   Lhistbin::SetError(double error) {
    // Doesn't set SumW.
    SumW2 = xp(error,2);
}
