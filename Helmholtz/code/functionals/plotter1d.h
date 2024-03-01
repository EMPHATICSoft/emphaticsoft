/*  L++ function plot in 1D
 *  Created by Leo Bellantoni on 21 Dec 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  To plot a function that lives in an object with this signature you need
 *  to make the function static.  That means you have to create the object in
 *  global space, and create another function, also in global space, which returns
 *  the value of the function in the object.  That 2nd function is static and
 *  can be passed to the constructor here.  I.e.
 *
 *  double peakValue = 70;  double FWHMValue = 1.0;
 *  ranlandaugen generator(peakValue, FWHMValue);
 *  double staticIt(double x) { return generator.landau_pdf(x); };
 *
 *  int main() {
 *      plotter1d Landau_pdf(staticIt , lo,hi,nBins);
 *      Landau_pdf.show();
 *  }
 *
 *  That makes it hard to change the parameters peakValue and FWHMValue.  So 
 *  there is also a second signature where the argument is a functional.  Use 
 *  it like this:
 *
 *  class Lev : public functional {
 *  public:
 *      Lev(double peakValue, double FWHMValue) :
 *          mpv(peakValue), fwhm(FWHMValue) {
 *          gennie = new ranlandaugen(peakValue,FWHMValue);}
 *      double operator()(ra<double>* X) {
 *          double x = (*X)(1);
 *          return gennie->landau_pdf(x);
 *      };
 * private:
 *      ranlandaugen* gennie;  double mpv, fwhm;
 * };
 *
 * Lev LandHo(3,1);
 * plotter1d Landau_PDF(LandHo, lo, hi, nBins);
 * Landau_PDF.show();
 *
 */
#pragma once



#include "Lscatter.h"



class plotter1d {
public:
    plotter1d();
    plotter1d(plotter1d const& inPlot);
    plotter1d& operator=(plotter1d const& rhs);
    ~plotter1d();
    
    // Sort of a constructor-getter architecture, except that the getter
    // function, a.k.a. show(), is a void with the side effect of showing 
    // the plot on the screen.
    plotter1d(double (*f)(double x), double lo, double hi, int Nsam=100);
    plotter1d(functional& f,         double lo, double hi, int Nsam=100);
    
    void show();
    
    // Plot object itself is public so logY and SetDot etc are easy.
    Lscatter* plot;
};
