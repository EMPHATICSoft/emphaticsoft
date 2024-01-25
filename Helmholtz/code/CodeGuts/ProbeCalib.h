/*  L++ Probe calibration correction.h
 *  Created by Leo Bellantoni on 29 Mar 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 */
#pragma once



#include "ra.h"
#include "inputtextfile.h"
using std::string;



class ProbeCalib {
public:
    ProbeCalib() = delete;
    ProbeCalib(string calibFileName);

    double correctBx(double inBx, bool stdForm=false);
    double correctBy(double inBy, bool stdForm=false);
    double correctBz(double inBz, bool stdForm=false);

private:
    int const nPoints = 20;
    int const Bmag=1; int const correction=2;
    ra<double>* curveX;
    ra<double>* curveY;
    ra<double>* curveZ;
};
