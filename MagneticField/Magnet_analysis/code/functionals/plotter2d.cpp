/*  L++ function plot in 2D
 *  Created by Leo Bellantoni on 21 Dec 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *
 */
#include "plotter2d.h"





// The 4 horsemen
plotter2d::plotter2d() : plot(nullptr) {}

plotter2d::plotter2d(plotter2d const& inPlot)
    : plot(nullptr) {
    *this = inPlot;
    return;
}
plotter2d& plotter2d::operator=(plotter2d const& rhs) {
    if (this != &rhs) {
        delete plot;
        if (rhs.plot) plot = new Lzcol(*rhs.plot);
    }
    return *this;
}

plotter2d::~plotter2d() {
    delete plot;
}





// Constructor for the proletariat
plotter2d::plotter2d(double (*f)(double x,double Y), double loX, double hiX,
                                                     double loY, double hiY, int Nsam) {
    if (loX >= hiX || loY >= hiY) {
        LppUrk.LppIssue(0,"plotter2d::plotter2d(double(*f),double,double...");
    }
    
    Lbins  bPlotX(Nsam,loX,hiX);
    Lbins  bPlotY(Nsam,loY,hiY);
    Lhist2 hPlot(bPlotX,bPlotY);
    
    for (Lbins::bin bX=1; bX<=Nsam; ++bX) {
        double x = hPlot.GetXCenter(bX);
        for (Lbins::bin bY=1; bY<=Nsam; ++bY) {
            double y = hPlot.GetYCenter(bY);
            double v = f(x,y);
            hPlot.Fill(x,y,v);
        }
    }
    plot = new Lzcol(hPlot);
}
plotter2d::plotter2d(functional& f, double loX, double hiX,
                                    double loY, double hiY, int Nsam) {
    if (loX >= hiX || loY >= hiY) {
        LppUrk.LppIssue(0,"plotter2d::plotter2d(double(*f),double,double...");
    }
    
    Lbins  bPlotX(Nsam,loX,hiX);
    Lbins  bPlotY(Nsam,loY,hiY);
    Lhist2 hPlot(bPlotX,bPlotY);
    
    ra<double>* arg = new ra<double>(2);
    for (Lbins::bin bX=1; bX<=Nsam; ++bX) {
        double x = (*arg)(1) = hPlot.GetXCenter(bX);
        for (Lbins::bin bY=1; bY<=Nsam; ++bY) {
            double y = (*arg)(2) = hPlot.GetYCenter(bY);
            double v = f(arg);
            hPlot.Fill(x,y,v);
        }
    }
    delete arg;
    plot = new Lzcol(hPlot);
}






void plotter2d::show() {
    plot->show();
}
