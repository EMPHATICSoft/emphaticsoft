/*  L++ Probe calibration correction.h
 *  Created by Leo Bellantoni on 29 Mar 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 */
#ifndef LppProbeCalib_H
#define LppProbeCalib_H



#include "MagneticField/field_map_tools/ra.h"
#include "MagneticField/field_map_tools/inputtextfile.h"
using std::string;



class ProbeCalib {
public:
    ProbeCalib() = delete;
    ProbeCalib(string calibFileName);

    double correctBx(double inBx);
    double correctBy(double inBy);
    double correctBz(double inBz);

private:
    int const nPoints = 20;
    int const Bmag=1; int const correction=2;
    ra<double>* curveX;
    ra<double>* curveY;
    ra<double>* curveZ;
};



#endif
