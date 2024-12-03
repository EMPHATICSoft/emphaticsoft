//
//  Created by Leo Bellantoni on 13 Sep 2023.
//

#include "mapChecker.h"





mapChecker::mapChecker(FieldMap* aMap_in) : aMap(aMap_in) {};





void mapChecker::checkMap() {
    ra<double> Xi(3);   ra<double> Bmeas(3);    ra<double> Bmap(3);
    FieldMap::FieldMapInd ind;
    
    string plotLabel;

    double histXspan = 0.00004;    int nBins = 100;
    double bippy = histXspan / (2*nBins);
    Lbins  bMagDiff(nBins, -bippy, histXspan-bippy);
    Lhist1 hMagDiff(bMagDiff,
        plotLabel +" |{/Palatino:Bold B_{interpolated}} - {/Palatino:Bold B_{measured}}|");
    
    // Loop through the map and compare interpolation to measurement
    double closestDist = Dbig;
    for (FieldMap::FieldMapItr itr=aMap->pBegin; itr<aMap->pEnd; ++itr) {
        if (itr->valid) {
            // Xi in iterator space at first
            Xi(x) = itr->x;    Xi(y) = itr->y;    Xi(z) = itr->z;
            if (aMap->findInd (itr, ind)) {
                aMap->findB(Bmeas,ind);
                // Shift Xi into lab space
                for (int i=x; i<=z; ++i) Xi(i) -= aMap->getOffset()(i);
                if (aMap->interpolate(Xi, Bmap)) {
                    double errX = Bmap(x) -Bmeas(x);
                    double errY = Bmap(y) -Bmeas(y);
                    double errZ = Bmap(z) -Bmeas(z);
                    double magErr = Qadd(errX,errY,errZ);
                    hMagDiff.Fill(magErr);
                }
            }
        }
    }
    
    Lfit1 pMagDiff(hMagDiff);
    pMagDiff.logY = true;
    pMagDiff.show();
    cout << "Average:" tabl hMagDiff.GetXave() tabl "\tRMS:" tabl hMagDiff.GetXrms() << endl;
    
    return;
}
