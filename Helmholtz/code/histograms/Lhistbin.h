/*  L++ Lhistbin
 *  Created by Leo Bellantoni on 11/26/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A histogram bin - its contents only, not its position on any axis.
 *  Not implemented yet: asymmetric errors
 *
 */
#pragma once



#include "quadd.h"
#include "stats.h"



class Lhistbin{
public:
    // Constructors; default destructor is fine
    Lhistbin();
    Lhistbin(Lhistbin const& inBin);
    Lhistbin& operator=(Lhistbin const& rhs);

    // Bin arithmetic, the basis of histogram math
    Lhistbin  operator+ (Lhistbin const& rhs) const;
    Lhistbin& operator+=(Lhistbin const& rhs);
    Lhistbin  operator- (Lhistbin const& rhs) const;
    Lhistbin& operator-=(Lhistbin const& rhs);
    // Multiply with errors in quad, and by double (scaling)
    Lhistbin  operator* (Lhistbin const& rhs) const;
    Lhistbin& operator*=(Lhistbin const& rhs);
    void Scale(double const& scale);
    // Divide operator for when error propagated in
    // quadrature, Modulo operator for efficiency
    Lhistbin  operator/ (Lhistbin const& rhs) const;
    Lhistbin& operator/=(Lhistbin const& rhs);
    Lhistbin  operator% (Lhistbin const& rhs) const;
    Lhistbin& operator%=(Lhistbin const& rhs);

    // Filling is the basic setter; value is sum of weights, error
    // is square root of sum of weights squared.  This formula can
    // be derived if we assume the numerator (i.e. the bin content)
    // and denominator (the sample from which certain weighted events
    // are selected to be in the bin) are uncorrelated and calcuate a
    // weighted average of the passing and non-passing events.  I can't
    // get this formula as the limit of a binomial distribution on a
    // constrained sum of events.
    void   Fill(double weight=1.0);
    double GetValue();
    void   SetValue(double value);  // Doesn't set SumW2!
    double GetError();
    void   SetError(double error);  // Doesn't set SumW.

    
private:
    double SumW, SumW2;
};
