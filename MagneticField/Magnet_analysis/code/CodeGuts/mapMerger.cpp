//
//  mergeMaps.cpp
//  L++
//
//  Created by Leo Bellantoni on 13 Sep 2023.
//
#include "mapMerger.h"





mapMerger::mapMerger(FieldMap* upMap_, FieldMap* cnMap_, FieldMap* dnMap_,
                     ra<double>* overlapZ_,
                     string headerFileName_, string outputMapName_) :

          upMap(upMap_),cnMap(cnMap_),dnMap(dnMap_),
          overlapZ(*overlapZ_),
          headerFileName(headerFileName_), outputMapName(outputMapName_)
          {};





void mapMerger::mergeMaps(bool copySpacing, double halfSpanX,double halfSpanY,
                          double lowestZout,double highestZout,
                          double stepX,double stepY,double stepZ) {

    if (!filehamna(outputMapName)) {
        // Oh, the file is already there.  Go ahead and overwrite it!
        cout<< outputMapName + " already exists & will be overwritten." << endl;
    }

    ofstream outFile;
    outFile.open(outputMapName, ios::out);

    ifstream headerFile;
    headerFile.open(headerFileName, ios::in);
    if (!headerFile.is_open()) {
            LppUrk.UsrIssue(-5, Fatal, "mapMerger::mergeMaps(...)", "Could not open header file");
    }
    string line;
    while (getline(headerFile, line)) outFile << line << "\n";
    headerFile.close();



    if (copySpacing) {
        FieldMap::XYbinning XYbins = cnMap->getXYbinning();
        halfSpanX  = XYbins.xHigh -(XYbins.xHigh -XYbins.xLow)/(2*XYbins.nXbins);
        halfSpanX -= XYbins.xLow  +(XYbins.xHigh -XYbins.xLow)/(2*XYbins.nXbins);
        halfSpanX /= 2.0;
        halfSpanY  = XYbins.yHigh -(XYbins.yHigh -XYbins.yLow)/(2*XYbins.nYbins);
        halfSpanY -= XYbins.yLow  +(XYbins.yHigh -XYbins.yLow)/(2*XYbins.nYbins);
        halfSpanY /= 2.0;
        stepX      = (XYbins.xHigh -XYbins.xLow)/XYbins.nXbins;
        stepY      = (XYbins.yHigh -XYbins.yLow)/XYbins.nYbins;

        FieldMap::Zbinning Zbins = cnMap->getZbinning();
        stepZ       = (Zbins.zHigh -Zbins.zLow)/Zbins.nZbins;
        lowestZout  = Zbins.zLow  +(Zbins.zHigh -Zbins.zLow)/(2*Zbins.nZbins);
        highestZout = Zbins.zHigh -(Zbins.zHigh -Zbins.zLow)/(2*Zbins.nZbins);

        // Subtract the offsets to get lab coordinates
        ra<double> off(3);
        off = cnMap->getOffset();
        halfSpanX  -= off(x);           halfSpanY   -= off(y);
        lowestZout -= off(z);           highestZout -= off(z);
        
    } else {
        // Round to nearest half-millimeter
        double const trunc = 0.0005;
        halfSpanX   = ndouble(halfSpanX,  trunc);
        halfSpanY   = ndouble(halfSpanY,  trunc);
        stepX       = ndouble(stepX,      trunc);
        stepY       = ndouble(stepY,      trunc);

        // Set Z bounds to force z=0 plane in the map
        stepZ       = ndouble(stepZ,      trunc);
        lowestZout  = ndouble(lowestZout, stepZ);
        highestZout = ndouble(highestZout,stepZ);
    }



    ra<double> Xi(3);       ra<double> B(3);
    ra<double> Bc(3);       ra<double> Bf(3);       double fracCentral;
    char lineOut[160];
    for (Xi(z)=lowestZout; Xi(z)<=highestZout +stepZ/2.0; Xi(z)+=stepZ) {
        // Linear interpolation in the overlap regions.
        if (Xi(z)<overlapZ(upStart)) {
            fracCentral = 0.0;
        } else if (Xi(z)<overlapZ(upStop)) {
            fracCentral = (Xi(z) -overlapZ(upStart)) / (overlapZ(upStop) -overlapZ(upStart));
        } else if (Xi(z)<overlapZ(downStart)) {
            fracCentral = 1.0;
        } else if (Xi(z)<overlapZ(downStop)) {
            fracCentral = 1.0 - (Xi(z) -overlapZ(downStart)) / (overlapZ(downStop) -overlapZ(downStart));
        } else {
            fracCentral = 0.0;
        }
        
        for (Xi(y)=-halfSpanY; Xi(y)<=+halfSpanY +stepY/2.0; Xi(y)+=stepY) {
            for (Xi(x)=-halfSpanX; Xi(x)<=+halfSpanX +stepX/2.0; Xi(x)+=stepX) {

                // Advertisement is that B = 0 is result of missing measurement.
                // Also, handle a point valid in one map but not another
                if (Xi(z)<overlapZ(upStart)) {
                    if (!upMap->interpolate(Xi,B)) B = 0;

                } else if (Xi(z)<overlapZ(upStop)) {
                    bool frInterpolate = upMap->interpolate(Xi,Bf);
                    bool cnInterpolate = cnMap->interpolate(Xi,Bc);
                    if ( frInterpolate && cnInterpolate) {
                        for (int i=x; i<=z; ++i) B(i) = fracCentral*Bc(i) + (1-fracCentral)*Bf(i);
                    }
                    if ( frInterpolate &&!cnInterpolate) B = Bf;
                    if (!frInterpolate && cnInterpolate) B = Bc;
                    if (!frInterpolate &&!cnInterpolate) B =  0;

                } else if (Xi(z)<overlapZ(downStart)) {
                    if (!cnMap->interpolate(Xi,B)) B = 0;

                } else if (Xi(z)<overlapZ(downStop)) {
                    bool frInterpolate = dnMap->interpolate(Xi,Bf);
                    bool cnInterpolate = cnMap->interpolate(Xi,Bc);
                    if ( frInterpolate && cnInterpolate) {
                        for (int i=x; i<=z; ++i) B(i) = fracCentral*Bc(i) + (1-fracCentral)*Bf(i);
                    }
                    if ( frInterpolate &&!cnInterpolate) B = Bf;
                    if (!frInterpolate && cnInterpolate) B = Bc;
                    if (!frInterpolate &&!cnInterpolate) B =  0;

                } else {
                    if (!dnMap->interpolate(Xi,B)) B = 0;
                }
                // mm and kGauss
                double const clip = 0.000001;
                sprintf(lineOut, "%-25g %-24g %-24g %-24g %-24g %-24g",
                    ndouble(1000*Xi(x),clip),ndouble(1000*Xi(y),clip),ndouble(1000*Xi(z),clip),
                    10*B(x),10*B(y),10*B(z));
                // DEBUG cout << lineOut <<endl;
                outFile << lineOut << "\n";
            }
        }
    }
    outFile.close();
    return;
}

