/*  L++ Lhist1
 *  Created by Leo Bellantoni on 12/16/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 1D histogram.
 *
 */
#pragma once



#include <fstream>
#include "filehamna.h"
#include "minmax.h"
#include "Lhistbin.h"
#include "Lbins.h"              // Includes ra
#include "Lprof1.h"



class Lhist1{
public:
    // Basic constructors
    Lhist1();
    Lhist1(Lhist1 const& inHist);
    Lhist1& operator=(Lhist1 const& rhs);   // deep copy
    ~Lhist1();



    // Constructor for the masses.  If there is an \n character in the inInfo
    // string, you will have trouble if you need to make an Lfit1.
    explicit Lhist1(Lbins inXbins, std::string inInfo="");
    explicit Lhist1(Lprof1 inProf, bool PlotRMS=true);

    // Observe:
    // Lfit1 FitVary1(Lhist1(Vary1));
    // will not compile, saying that the parentheses were interpreted as a
    // function declaration, and that you need to do
    // Lfit1 FitVary1((Lhist1(Vary1)));
    // to declare a variable (FitVary1, of course).  Not that you typically want
    // to make an Lfit1 without filling the histogram first, though.



    // Mathematical operators built on corresponding operations
    // in Lhistbin
    Lhist1  operator+ (Lhist1 const& rhs) const;
    Lhist1& operator+=(Lhist1 const& rhs);
    Lhist1  operator- (Lhist1 const& rhs) const;
    Lhist1& operator-=(Lhist1 const& rhs);
    
    Lhist1  operator* (Lhist1 const& rhs) const;
    Lhist1& operator*=(Lhist1 const& rhs);
    void Scale(double const& scale);
    
    Lhist1  operator/ (Lhist1 const& rhs) const;
    Lhist1& operator/=(Lhist1 const& rhs);
    Lhist1  operator% (Lhist1 const& rhs) const;
    Lhist1& operator%=(Lhist1 const& rhs);


    
    // Filling
    void Fill(double x, double weight=1.0);



    // Accessing bins.
    Lhistbin&  GetBinPos(double x) const;
    Lhistbin&  GetBinBin(Lbins::bin Bx) const;
    Lhistbin   GetXunder() const;
    Lhistbin   GetXover() const;
    Lbins::bin GetXNbins() const;
    double     GetXCenter(Lbins::bin b) const;
    double     GetXWidth(Lbins::bin b) const;
    Lbins      GetXbins() const;



    // Simple stats, integrals
    double GetXsum();
    double GetXave();
    double GetXrms();
    Lhistbin SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi) const;



    // Get upper/lower values
    double MinVal() const;
    double MaxVal() const;
    Lbins::bin MinBin() const;
    Lbins::bin MaxBin() const;



    // CSV output: bin center, value, bin width, error
    // watch out for text printing precision problems!
    void CSVout(std::string filename = "", bool overwrite = false);



    std::string info;    // Histogram title, whatever



private:
    ra<Lhistbin>* HistArray;
    Lbins Xbins;
};
