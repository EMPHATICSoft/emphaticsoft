/*  L++ Lscatter
 *  Created by Leo Bellantoni on 1/19/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  Class for scatter plots made with GNUplot
 */
#pragma once



#include "LppGlobals.h"
#include "LppExcept.h"
#include "xp.h"
#include "ra.h"
#include "meansig.h"
#include "fitter.h"
#include "plotbase.h"
#include "plotfit.h"
#include "Ldata.h"



class Lscatter : public plotbase, public plotfit {
public:
    // Default, copy, assignment and destructor
    Lscatter();
    // Copy, assignment constructors
    Lscatter(Lscatter const& inPlot);
    Lscatter& operator=(Lscatter const& rhs);
    ~Lscatter();

    // Constructor for the masses: from JUST ONE Ldata, plotted on left
    explicit Lscatter(Ldata const& inData);
    
    
    // AddSeries will add Red, Blue, Curve only.  Black usually exists,
    // except if only the default constructor without a call to AddSeries
    // to create that series.
    // Only Red, Blue & Curve can be plotted on the y2 (right side) axis
    // Curve will be created if you do a fit
    enum ScatCol {Black=0,Red=1,Blue=2,Curve=3};
    void AddSeries(Lscatter::ScatCol col, Ldata inData);
    bool y2Red, y2Blue, y2Curve;
    
    // The "chi-squared" function for the fitter
    double operator()(ra<double>* param);
    
    // Create and run the fitter, adding a function to be fit, excluding points
    void InitFit(double (*inFitFunc)(ra<double>* param,double x), ra<double> start);
    void RunFit(bool trySIMPLEX = false);
    void Exclude(double loExcl, double hiExcl);
    void ResetExclude();
    double RMSresiduals();
    
    int NDoF();

    // All dots change size at the same time.  Default is 1.0
    void SetDotSize(double ns);
    // Change Black, Red, Blue dots
    void SetDotSize(double nsBlack, double nsRed, double nsBlue);

    void trimedges();
    void show();
    // '2' is y2, the right hand side axis
    enum ScatCorn {sXlo=1,sYlo=2,sXhi=3,sYhi=4,s2lo=5,s2hi=6};



private:
    Ldata* Black_;
    Ldata* Red_;
    Ldata* Blue_;
    Ldata* Curve_;

    int Nexcl;

    double DotSize[3];
    ra<double> maketempfile();
};
