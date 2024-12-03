/*  L++ Lhist2
 *  Created by Leo Bellantoni on 12/14/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 2D histogram.
 *
 */
#pragma once



#include <fstream>
#include "filehamna.h"
#include "Lhistbin.h"
#include "Lbins.h"
#include "Lprof2.h"
using namespace std;



class Lhist2{
public:
    // The fab 4
    Lhist2();
    Lhist2(Lhist2 const& inHist);
    Lhist2& operator=(Lhist2 const& rhs);   // deep copy
    ~Lhist2();


    
    // Constructor for the masses
    Lhist2(Lbins inXbins, Lbins inYbins, string inInfo="");
    explicit Lhist2(Lprof2 inProf, bool PlotRMS=true);
    


    // Mathematical operators built on corresponding operations
    // in Lhistbin
    Lhist2  operator+ (Lhist2 const& rhs) const;
    Lhist2& operator+=(Lhist2 const& rhs);
    Lhist2  operator- (Lhist2 const& rhs) const;
    Lhist2& operator-=(Lhist2 const& rhs);
 
    Lhist2  operator* (Lhist2 const& rhs) const;
    Lhist2& operator*=(Lhist2 const& rhs);
    void Scale(double const& scale);

    Lhist2  operator/ (Lhist2 const& rhs) const;
    Lhist2& operator/=(Lhist2 const& rhs);
    // % is division with uncertainties for binomials
    Lhist2  operator% (Lhist2 const& rhs) const;
    Lhist2& operator%=(Lhist2 const& rhs);

    // Fetch errors into new histogram.  'cuz Zcol has no error bars
    Lhist2 GetErrorHisto() const;



    // Filling. weight of Nan will be ignored.
    // x or y of Nan goes into overflow bin.
    void Fill(double x, double y, double weight=1.0);



    // Accessing bins.
    Lhistbin&  GetBinPos(double x, double y) const;
    Lhistbin&  GetBinBin(Lbins::bin Bx, Lbins::bin By) const;
    Lhistbin   GetXunder() const;                 Lhistbin   GetYunder() const;
    Lhistbin   GetXover() const;                  Lhistbin   GetYover() const;
    Lbins::bin GetXNbins() const;                 Lbins::bin GetYNbins() const;
    double     GetXCenter(Lbins::bin b) const;    double     GetYCenter(Lbins::bin b) const;
    double     GetXWidth(Lbins::bin b) const;     double     GetYWidth(Lbins::bin b) const;
    Lbins      GetXbins() const;                  Lbins      GetYbins() const;


    // Simple stats, integrals
    double GetXYsum();
    double GetXave();                       double GetYave();
    double GetXrms();                       double GetYrms();
    // Can't include over/underflow bins
    Lhistbin SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi,
                     Lbins::bin Bylo, Lbins::bin Byhi) const;
 
 

    // Get upper/lower values  Returns +/-Dbig or Lbins::U if all bins virgins
    double MinVal() const;
    double MaxVal() const;
    ra<Lbins::bin> MinBin() const;
    ra<Lbins::bin> MaxBin() const;



    // CSV output: watch out for text printing precision problems!
    void CSVout(string filename = "", bool overwrite = false);



    string info;    // Histogram title, whatever
    
private:
    ra<Lhistbin>* HistArray;
    Lbins Xbins, Ybins;
};
