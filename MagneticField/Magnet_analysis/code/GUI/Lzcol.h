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
    // trimedges sets the (X,Y)(low,high)Lim to the edges of the binning
    void trimedges();
    // blankEmpties sets virgin bins (histograms only!) to either 1/2
    // way between the largest and lowest entries, or to the value
    // sent as an argument... the intent is to make those bins white
    // on the plot.
    void blankEmpties(double blankVal=-Dbig);
    void show();

private:    
    // Could be an Lhist2 or an Lprof2 we are plotting
    bool isProfile;
    Lhist2* Histo;
    Lprof2* Profile;

    void maketempfile();
};
