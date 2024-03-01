/*  L++ plotfit
 *  Created by Leo Bellantoni on 3 Sep 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  Base class for plot fitting classes - mostly virtual, actually
 *
 */

#include "plotfit.h"
using namespace std;


// (Default) constructor
plotfit::plotfit() :
    FitFunc(nullptr), fitr(nullptr), minum_(nullptr), covar_(nullptr) {}

plotfit::plotfit(plotfit const& inPfit)
    : FitFunc(nullptr), fitr(nullptr), minum_(nullptr), covar_(nullptr) {
    *this = inPfit;
}
plotfit& plotfit::operator=(plotfit const& rhs) {
    if (this != &rhs) {
        if (rhs.FitFunc) {
            FitFunc = rhs.FitFunc;      // Effectively deep copy
        } else {
            FitFunc = nullptr;
        }
        delete minum_;
        delete covar_;
        delete fitr;
        if (rhs.minum_) minum_ = new ra<double>(*rhs.minum_);
        if (rhs.covar_) covar_ = new ra<double>(*rhs.covar_);
        if (rhs.fitr)   fitr = new fitter(*rhs.fitr);
    }
    return *this;
}


// Destructor
plotfit::~plotfit() {
    if (FitFunc)  FitFunc = nullptr;    // Don't delete or free a double (*func)(ra<double>*, x)
    delete covar_;
    delete minum_;
    delete fitr;
}


void plotfit::ResetFit(ra<double> restart) {
    delete minum_;
    minum_ = new ra<double>(restart);
}

void plotfit::PrintFit() {
    printf("\n");
    int Npara = fitr->Ndall();
    cout << "Chi2/NDoF: " << chi2() << " / " << NDoF() << 
        ", corresponding to pValue of " << pVal() << endl;

    // Print pinned variables, if any
    for (int i = 1; i <= Npara; ++i) {
        if (!fitr->isFree(i) || !covar_) printf("Fixed parameter %d : %g\n",i,minum(i));
    }
    if (!covar_) {printf("\n"); return;}

    // Print free variables, errors.
    for (int i = 1; i <= Npara; ++i) {
        if (fitr->isFree(i)) {
            printf("Parameter %d : %g +/- %g\n",i,minum(i),sqrt(covar(i,i)));
        }
    }
    printf("\nCorrelation coefficients:\n");
    if (fitr->forcedPositive) {
        cout << endl << "Hesse forced quadratic approximation to be positive definite."
            << endl << "These uncertainties dubious indeed." << endl << endl;
    }
    for (int i = 2; i <= Npara; ++i) {
        if (fitr->isFree(i)) {
            for (int j = 1; j <= (i-1); ++j) {
                // Take square root of covariances separately, until you understand
                // why they come out negative.  Because they might both be negative
                // in which case you still want to print a nan.
                if (fitr->isFree(j)) {
                    printf("%6.3f,  ",covar(i,j)/( sqrt(covar(i,i)) * sqrt(covar(j,j)) ));
                }
            }
            printf("\n");
        }
    }
    printf("\n");
    return;
}

void plotfit::makeGirl(int talkativeness) {
    if (!fitr) {
        LppUrk.LppIssue(301,"plotfit::makeGirl(int)");
    }
    fitr->makeGirl(talkativeness);
}
double plotfit::minum(int i) {
    return (*minum_)(i);
}
double plotfit::covar(int i,int j) {
    return (*covar_)(i,j);
}
double plotfit::chi2() {
    return (*this)(minum_);
}
double plotfit::pVal() {
    return chiprbQ(chi2(),NDoF());
}
void plotfit::useVar(int i) {
    fitr->useVar(i);
}
void plotfit::fixVar(int i) {
    fitr->fixVar(i);
}
bool plotfit::isFree(int i) {
    return fitr->isFree(i);
}
void plotfit::setVar(int i, double v) {
    fitr->setVar(i,v);
}
double plotfit::getVar(int i) {
    return fitr->getVar(i);
}
void plotfit::setErrStep(double v) {
    fitr->errStep = v;
}

void plotfit::setBounds(int i, double low, double high) {
    fitr->setBounds(i, low,high);
}
