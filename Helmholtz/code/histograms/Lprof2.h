/*  L++ Lprof2
 *  Created by Leo Bellantoni on 12/14/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 2D profile plot
 *
 *
 */
#pragma once



#include <fstream>
#include "filehamna.h"
#include "Lprofbin.h"
#include "Lbins.h"



class Lprof2{
public:
    // Basic constructors
    Lprof2();
    Lprof2(Lprof2 const& inProf);
    Lprof2& operator=(Lprof2 const& rhs);   // deep copy
    ~Lprof2();



    // Constructor for the masses
    Lprof2(Lbins inXbins, Lbins inYbins, std::string inInfo="");



    // Mathematical operators &, &= and Scale are available
    // from Lprofbin
    Lprof2  operator& (Lprof2 const& rhs) const;
    Lprof2& operator&=(Lprof2 const& rhs);
    void Scale(double const& scale);



    // Filling
    void Fill(double x, double y, double value, double weight=1.0);



    // Accessing bins
    Lprofbin&  GetBinPos(double x, double y) const;
    Lprofbin&  GetBinBin(Lbins::bin Bx, Lbins::bin By) const;
    Lprofbin   GetXunder() const;                 Lprofbin   GetYunder() const;
    Lprofbin   GetXover() const;                  Lprofbin   GetYover() const;
    Lbins::bin GetXNbins() const;                 Lbins::bin GetYNbins() const;
    double     GetXCenter(Lbins::bin b) const;    double     GetYCenter(Lbins::bin b) const;
    double     GetXWidth(Lbins::bin b) const;     double     GetYWidth(Lbins::bin b) const;
    Lbins      GetXbins() const;                  Lbins      GetYbins() const;



    // Integral.  No GetXsum, GetXave etc. because the &= operator of Lprofbin
    // makes these of non-obvious meaning.  Convert the Lprof to an Lhist first.
    Lprofbin SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi,
                     Lbins::bin Bylo, Lbins::bin Byhi) const;



    // Get upper/lower values for plotting
    double MinVal() const;
    double MaxVal() const;
    ra<Lbins::bin> MinBin() const;
    ra<Lbins::bin> MaxBin() const;



    // CSV output: watch out for text printing precision problems!
    void CSVout(std::string filename = "", bool RMS = true, bool overwrite = false);



    std::string info;    // Histogram title, whatever

private:
    ra<Lprofbin>* ProfArray;
    Lbins Xbins, Ybins;    
};
