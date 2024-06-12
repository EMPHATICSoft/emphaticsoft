/*  L++ Lprofbin
 *  Created by Leo Bellantoni on 11/23/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A profile bin - its contents only, not its position on any axis.
 *
 */
#pragma once



#include "quadd.h"
#include "xp.h"



class Lprofbin{
public:
    // Constructors; default destructor is fine
    Lprofbin();
    Lprofbin(Lprofbin const& inBin);
    Lprofbin& operator=(Lprofbin const& rhs);
    
    // Combination, scaling the only well-defined mathematical ops
    Lprofbin  operator& (Lprofbin const& rhs) const;
    Lprofbin& operator&=(Lprofbin const& rhs);
    void Scale(double const& scale);

    // Filling is the basic setter; then 4 simple getters.
    void   Fill(double value, double weight=1.0);

    int    GetNent();
    double GetMean();
    double GetSDev();   // Thas' N-1 hearabouts, boy
    double GetErrMean();

private:
    int    Nent;
    double SumW, SumW2;
};
