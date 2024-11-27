/*  L++ plotfit
 *  Created by Leo Bellantoni on 3 Sep 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  Base class for plot fitting - mostly virtual, actually.  Makes annoying
 *  but harmless compile time warnings.
 */
#pragma once


#include "ra.h"
#include "fitter.h"



// The fitting things of a plot are functionals, where the functionality is the
// "chi-squared" function for the fitter.
class plotfit : public functional {
public:
    // Constructors, destructors; assignment operator below
    plotfit();
    plotfit(plotfit const& inPfit);
    ~plotfit();
    
    // Every class derived from plotfit must override this definition
    double operator()(ra<double>* X){return 0;};
    
    // Create and run the fitter, adding a function to be fit, excluding points
    virtual void InitFit(double (*inFitFunc)(ra<double>* param,double x), ra<double> start) {}
    virtual void RunFit(bool trySIMPLEX = false) {}
    virtual void Exclude(double loExcl, double hiExcl) {}
    virtual void ResetExclude() {}
    virtual double RMSresiduals() {return 0.0;};    // Stifles compile warning
    void ResetFit(ra<double> restart);
    void PrintFit();
    
    // Wrappers to access fitr that perforce have common implementations
    // in all derived classes
    void makeGirl(int talkativeness);               // 0, 1 or 2.  More is chatty
    double minum(int i);
    double covar(int i,int j);
    double chi2();
    virtual int NDoF() {return 0.0;};               // Stifles compile warning
    double pVal();
    void useVar(int i);
    void fixVar(int i);
    bool isFree(int i);
    void setErrStep(double v);
    
    void setBounds(int i, double low, double high);
    void setVar(int i, double v);
    double getVar(int i);

protected:
    double (*FitFunc)(ra<double>* param, double x);
    // The fitter itself
    fitter* fitr;           // nullptr before InitFit
    ra<double>* minum_;     // nullptr before InitFit
    ra<double>* covar_;     // nullptr before RunFit()
    
    // Assignment operator not for mortal man
    plotfit& operator=(plotfit const& rhs);
};
