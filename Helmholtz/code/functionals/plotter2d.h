/*  L++ function plot in 2D
 *  Created by Leo Bellantoni on 21 Dec 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *
 */
#pragma once



#include "functional.h"
#include "Lzcol.h"



class plotter2d{
public:
    plotter2d();
    plotter2d(plotter2d const& inPlot);
    plotter2d& operator=(plotter2d const& rhs);
    ~plotter2d();
    
    // Sort of a constructor-getter architecture, except that the getter
    // function is a void (show()) with the side effect of showing the plot 
    // on the screen.
    // To use the constructor with a functional, see example in plotter1d.h
    plotter2d(double (*f)(double x,double y), double loX, double hiX,
                                              double loY, double hiY, int Nsam=100);
    plotter2d(functional& f,                  double loX, double hiX,
                                              double loY, double hiY, int Nsam=100);
    
    void show();
    
    // Plot object itself is public so logY and SetDot etc are easy.
    Lzcol* plot;
};
