/*  L++ Lfit1
 *  Created by Leo Bellantoni on 2/17/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  Class for 1D histogram fit plots made with GNUplot.  Class holds a data
 *  histogram, (optionally) an MC histogram that can be scaled to match the
 *  data and (optionally) a C style function that can be used to fit to the
 *  data.  The function takes an ra<double> as its first argument list; that
 *  is the parameter list and the ra need not be indexed from 1.  The second
 *  argument is, well, the argument; it is the same variable as in the histogram.
 *  Plotting options include an attached ratio plot with vertical axis
 *  determined from plotbase's ZlowAuto, logZ, etc.
 *
 *  The value of the function at the center of the bin is compared to the 
 *  contents of the bin, not to the contents time the width ('area' fit).  There
 *  is more info about how to handle that problem in GlobalFuncs.h, where some
 *  commonly used functions are defined.  The functions, unfortunately, must be
 *  at global scope.  The fit is by default a chi squared type, using histogram
 *  uncertainties as 1 sigma probable errors in the bins.  That can introduce a
 *  bias, as shown in the sample fit to a gaussian; bins that fluctuate down have
 *  a smaller uncertainty if you are using root-N error bars and that will pull
 *  the curve fit down.  Perhaps you can get a good initial fit then re-fill the
 *  data points with uncertainties equal to root-F (the function, not the data)
 *  to try and fix this if you need to.
 *  Log-likelyhood fits can be obtained by setting the LogLike data field true.
 *  Over/underflow bins never in fit, although they will be written into the
 *  upper left corner of the plot.
 *
 */
#pragma once



#include "LppGlobals.h"
#include "problems/LppExcept.h"
#include "ra/ra.h"
#include "functions/xp.h"
#include "lies/meansig.h"
#include "lies/stats.h"
#include "functionals/functional.h"
#include "functionals/fitter.h"
#include "histograms/Lhist1.h"
#include "GUI/plotbase.h"
#include "GUI/plotfit.h"



class Lfit1 : public plotbase, public plotfit {
// Derivation from functional is needed even though functional only defines
// virtual operator() in the same way that plotfit does.  If plotfit derives
// from functional there is an ambiguous conversion from this derived class to
// the base class functional when this (the derived) class tries to initialize
// the fitr field which is in the base class plotfit.
public:
    // Default constructor.
    Lfit1();

    // Copy, assignment constructors
    Lfit1(Lfit1 const& inPlot);
    Lfit1& operator=(Lfit1 const& rhs);
    
    // Constructor for the masses.  The info field is from the Data Lhist1
    explicit Lfit1(Lhist1 const& inData);
    
    // Had to allocate space for the Lhist1s on the heap
    ~Lfit1();
    
    // Add an MC histogram
    void AddMC(Lhist1 const& inMC);
    
    // Getters for the histograms
    Lhist1 GetData();
    Lhist1 GetMonte();
    
    // The "chi-squared" function for the fitter
    double operator()(ra<double>* param);

    // Create and run the fitter, adding a function to be fit, excluding points
    void InitFit(double (*inFitFunc)(ra<double>* param,double x), ra<double> start);
    void ReinitFit(ra<double> restart);
    void RunFit(bool trySIMPLEX = false);
    void Exclude(double loExcl, double hiExcl); // if loExcl==hiExcl, 1 bin excluded
    void ResetExclude();
    double RMSresiduals();

    int NDoF();
 
    // Only the data dots change size
    void SetDotSize(int ns);
    // Can do binomial uncertainties on the Data / MC ratio
    bool BinomialErrs;
    // Do or not do a log-likelyhood fit instead
    bool LogLike;
    
    void trimedges();
    void show();

    
private:
    Lhist1* Data;
    Lhist1* Monte;
 
    Lhist1* unFit;          // Non-zero entry for excluded bin - U bin != 0
                            // if any bin is unFit.
    
    double DotSize;
    void maketempfile();
};
