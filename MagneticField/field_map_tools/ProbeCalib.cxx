//
//  ProbeCalib.cpp
//  L++
//
//  Created by Leo Bellantoni on 3/29/23.
//

#include <iostream>
#include "MagneticField/field_map_tools/ProbeCalib.h"



ProbeCalib::ProbeCalib(string calibFileName) {
    curveX = new ra<double>(nPoints, 2);
    curveY = new ra<double>(nPoints, 2);
    curveZ = new ra<double>(nPoints, 2);

    inputtextfile calibData(calibFileName);
    std::string makeWhite = ",";
    calibData.fetchline(makeWhite);
    for (int iPoint = 1; iPoint <= nPoints; ++iPoint) {
        calibData.fetchline(makeWhite);
        (*curveX)(iPoint,Bmag)       =  calibData.tokenasd(0);
        (*curveX)(iPoint,correction) =  calibData.tokenasd(1);
        (*curveY)(iPoint,Bmag)       =  calibData.tokenasd(2);
        (*curveY)(iPoint,correction) =  calibData.tokenasd(3);
        (*curveZ)(iPoint,Bmag)       =  calibData.tokenasd(4);
        (*curveZ)(iPoint,correction) =  calibData.tokenasd(5);
    }
    return;
}



double ProbeCalib::correctBx(double inBx) {
    // Plain ol' linear interpolation, and we don't even have to
    // bother to check for edge of range 'cuz we know we aren't
    // near the edge of range.
    int iPoint;
    for (iPoint = 1; iPoint <= nPoints; ++iPoint) {
        if ( (*curveX)(iPoint,Bmag) <= inBx &&
             inBx <= (*curveX)(iPoint+1,Bmag) ) break;
    }
    double frak = (                   inBx  -(*curveX)(iPoint,Bmag)) /
                  ((*curveX)(iPoint+1,Bmag) -(*curveX)(iPoint,Bmag));
    double retval = (*curveX)(iPoint+1,correction) -(*curveX)(iPoint,correction);
    retval *= frak;
    retval += (*curveX)(iPoint,correction);
    retval = (1.0 +retval) * inBx;
    retval *= -1.0;     // "Motion system has postive X to the west, but the
                        //  probe is opposite..."
    return retval;
}

double ProbeCalib::correctBy(double inBy) {
    int iPoint;
    for (iPoint = 1; iPoint <= nPoints; ++iPoint) {
        if ( (*curveY)(iPoint,Bmag) <= inBy &&
             inBy <= (*curveY)(iPoint+1,Bmag) ) break;
    }
    double frak = (                   inBy  -(*curveY)(iPoint,Bmag)) /
                  ((*curveY)(iPoint+1,Bmag) -(*curveY)(iPoint,Bmag));
    double retval = (*curveY)(iPoint+1,correction) -(*curveY)(iPoint,correction);
    retval *= frak;
    retval += (*curveY)(iPoint,correction);
    retval = (1.0 +retval) * inBy;
    retval *= -1.0;     // "... Similarly, Y motion is positive going up but
                        //  the probe is opposite, with positive down"
    return retval;
}

double ProbeCalib::correctBz(double inBz) {
    int iPoint;
    for (iPoint = 1; iPoint <= nPoints; ++iPoint) {
        if ( (*curveZ)(iPoint,Bmag) <= inBz &&
             inBz <= (*curveZ)(iPoint+1,Bmag) ) break;
    }
    double frak = (                   inBz  -(*curveZ)(iPoint,Bmag)) /
                  ((*curveZ)(iPoint+1,Bmag) -(*curveZ)(iPoint,Bmag));
    double retval = (*curveZ)(iPoint+1,correction) -(*curveZ)(iPoint,correction);
    retval *= frak;
    retval += (*curveZ)(iPoint,correction);
    retval = (1.0 +retval) * inBz;
    return retval;
}
