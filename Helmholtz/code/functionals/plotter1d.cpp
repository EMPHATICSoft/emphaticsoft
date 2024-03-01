/*  L++ function plot in 1D
 *  Created by Leo Bellantoni on 21 Dec 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *
 */
#include "plotter1d.h"





// The 4 horsemen
plotter1d::plotter1d() : plot(nullptr) {}

plotter1d::plotter1d(plotter1d const& inPlot)
    : plot(nullptr) {
    *this = inPlot;
    return;
}
plotter1d& plotter1d::operator=(plotter1d const& rhs) {
    if (this != &rhs) {
        delete plot;
        if (rhs.plot) plot = new Lscatter(*rhs.plot);
    }
    return *this;
}
plotter1d::~plotter1d() {
    delete plot;
}





// Constructor for the great unwashed.
plotter1d::plotter1d(double (*f)(double x), double lo, double hi, int Nsam) {
    if (lo >= hi) {
        LppUrk.LppIssue(0,"plotter1d::plotter1d(double(*f),double,double...");
    }
    Ldata  hBlack,hCurve;        Ldata::Point pushme;
    
    // Need 2 points in the hBlack before making the real curve; stifle the
    // black points when plotting by making them super small.
    pushme.x = lo;      pushme.y = f(lo);        pushme.dy = 0;
    hBlack.push(pushme);
    pushme.x = hi;      pushme.y = f(hi);
    hBlack.push(pushme);
    plot = new Lscatter(hBlack);
    
    double span = hi -lo;
    for (int i=0; i<Nsam+1; ++i) {
        double x  = span * (double(i)/double(Nsam));
               x += lo;
        pushme.x = x;      pushme.y = f(x);
        hCurve.push(pushme);
    }
    plot->AddSeries(Lscatter::Curve,hCurve);
    return;
}
plotter1d::plotter1d(functional& f, double lo, double hi, int Nsam) {
    if (lo >= hi) {
        LppUrk.LppIssue(0,"plotter1d::plotter1d(double(*f),double,double...");
    }
    Ldata  hBlack,hCurve;        Ldata::Point pushme;
    ra<double>* arg = new ra<double>(1);
    
    // Need 2 points in the hBlack before making the real curve; stifle the
    // black points when plotting by making them super small.
    (*arg)(1) = lo;
    pushme.x = lo;      pushme.y = f(arg);        pushme.dy = 0;
    hBlack.push(pushme);
    (*arg)(1) = hi;
    pushme.x = hi;      pushme.y = f(arg);
    hBlack.push(pushme);
    plot = new Lscatter(hBlack);
    
    double span = hi -lo;
    for (int i=0; i<Nsam+1; ++i) {
        double x  = span * (double(i)/double(Nsam));
               x += lo;
        (*arg)(1) = x;
        pushme.x = x;      pushme.y = f(arg);
        hCurve.push(pushme);
    }
    plot->AddSeries(Lscatter::Curve,hCurve);
    return;
}




void plotter1d::show() {
    plot->SetDotSize(-5);   // Pretty much invisible!
    plot->show();
}
