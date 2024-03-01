/*  L++ Lprofbin
 *  Created by Leo Bellantoni on 11/30/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A histogram bin - it's contents only, not it's position on any axis.
 *
 */
#include "Lprofbin.h"


// Constructors; default destructor is fine
Lprofbin::Lprofbin() :
    Nent(0), SumW(0.0), SumW2(0.0) {
}
Lprofbin::Lprofbin(Lprofbin const& inBin)
    :Nent(0), SumW(0.0), SumW2(0.0) {
    *this = inBin;
}
Lprofbin& Lprofbin::operator=(Lprofbin const& rhs) {
    if (this != &rhs) {
        Nent   = rhs.Nent;
        SumW   = rhs.SumW;
        SumW2  = rhs.SumW2;
    }
    return *this;
}



// Bin arithmetic, the basis of histogram math
Lprofbin  Lprofbin::operator& (Lprofbin const& rhs) const{
	Lprofbin retval(*this);	return (retval &= rhs);
}
Lprofbin& Lprofbin::operator&=(Lprofbin const& rhs) {
    // Not addition in the sense of adding two functions
    // that are represented by profiles; but combination
    // in the sense of combining two bins.
    Nent  += rhs.Nent;
    SumW  += rhs.SumW;
    SumW2 += rhs.SumW2;
    return *this;
}
void Lprofbin::Scale(double const& scale) {
    SumW  *= scale;
    SumW2 *= xp(scale,2);
}


// The setter and 4 getters
void Lprofbin::Fill(double value, double weight) {
    ++Nent;
    SumW  +=    value   *weight;
    SumW2 += xp(value,2)*weight;
}

int    Lprofbin::GetNent() { return Nent; }
double Lprofbin::GetMean() {
    // Profiles differ from histograms in that histograms will
    // not have divide-by-zero in their Getter functions; only if
    // you actually divide by zero in the histogram itself.  A Fatal
    // exception for some bin that never got filled is too dire a
    // remedy.  So one needs a Warning level exception, in this class.
    if (Nent > 0) {
        return SumW / double(Nent);
    } else {
        LppUrk.LppIssue(109,"Lprofbin::GetMean()");
        return 0.0;
    }
}
double Lprofbin::GetSDev() {
    if (Nent > 1) {
        double tmp = SumW2 - Nent*xp(GetMean(),2);
        if (tmp < 0.0) {
            LppUrk.LppIssue(110,"Lprofbin::GetSDev()");
            return 0.0;
        }
        return sqrt(tmp/(Nent-1));
    } else {
        LppUrk.LppIssue(109,"Lprofbin::GetSDev()");
        return 0.0;
    }
}
double Lprofbin::GetErrMean() {
    if (Nent > 0) {
        return GetSDev()/sqrt(Nent);
    } else {
        LppUrk.LppIssue(109,"Lprofbin::GetErrMean()");
        return 0.0;
    }
}
