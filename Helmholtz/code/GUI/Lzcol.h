/*  L++ Lzcol
 *  Created by Leo Bellantoni on 1/11/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  Class for zcol plots made with GNUplot
 */
#pragma once



#include "LppGlobals.h"
#include "Ldelete.h"
#include "plotbase.h"
#include "Lhist2.h"
#include "Lprof2.h"



class Lzcol : public plotbase {
public:
    // The 4 horsemen
    Lzcol();
    // Copy, assignment constructors
    Lzcol(Lzcol const& inPlot);
    Lzcol& operator=(Lzcol const& rhs);
    ~Lzcol();

    // Constructors for the masses: from Lhist2 and Lprof2
    explicit Lzcol(Lhist2 const& inHist);
    explicit Lzcol(Lprof2 const& inProf);
    

    // Wot you wan
    Lhist2 GetHisto();
    Lprof2 GetProfile();
    void trimedges();
    void show();

private:    
    // Could be an Lhist2 or an Lprof2 we are plotting
    bool isProfile;
    Lhist2* Histo;
    Lprof2* Profile;

    void maketempfile();
};
