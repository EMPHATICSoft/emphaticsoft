/*  L++ Lprof1
 *  Created by Leo Bellantoni on 12/18/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 1D profile plot
 *
 *
 */
#pragma once



#include <fstream>
#include "filehamna.h"
#include "Lprofbin.h"
#include "Lbins.h"



class Lprof1{
public:
    // John, Paul, George, Ringo
    Lprof1();
    Lprof1(Lprof1 const& inProf);
    Lprof1& operator=(Lprof1 const& rhs);   // deep copy
    ~Lprof1();
    
    
    // Constructor for the masses
    explicit Lprof1(Lbins inXbins, std::string inInfo="");
    
    // Mathematical operators &, &= and Scale are available
    // from Lprofbin
    Lprof1  operator& (Lprof1 const& rhs) const;
    Lprof1& operator&=(Lprof1 const& rhs);
    void Scale(double const& scale);


    // Filling
    void Fill(double x, double value, double weight=1.0);


    // Accessing bins.
    Lprofbin&  GetBinPos(double x) const;
    Lprofbin&  GetBinBin(Lbins::bin Bx) const;
    Lprofbin   GetXunder() const;
    Lprofbin   GetXover() const;
    Lbins::bin GetXNbins() const;
    double     GetXCenter(Lbins::bin b) const;
    double     GetXWidth(Lbins::bin b) const;
    Lbins      GetXbins() const;
    
    
    // Integral.  No GetXsum, GetXave etc. because the &= operator of Lprofbin
    // makes these of non-obvious meaning.  Convert the Lprof to an Lhist first.
    Lprofbin SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi) const;


    // Get upper/lower values
    double MinVal() const;
    double MaxVal() const;
    Lbins::bin MinBin() const;
    Lbins::bin MaxBin() const;

    // CSV output: bin center, mean, bin width, RMS or error on mean
    // watch out for text printing precision problems!
    void CSVout(std::string filename = "", bool RMS = true, bool overwrite = false);
    

    std::string info;    // Histogram title, whatever
    
private:
    ra<Lprofbin>* ProfArray;
    Lbins Xbins; 
};
