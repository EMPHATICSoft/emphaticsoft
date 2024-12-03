//
//  plotOverlapDiff.h
//  L++
//
//  Created by Leo Bellantoni on 27 Feb 2024.
//


#include "integrands.h"


namespace plotOverlap {

    double xLoLim,xHiLim,yLoLim,yHiLim;
    double zVal,discard,mean,stdev,peak;
    double margin = 0.006;      // A bit over the grid step size to not
                                // waste time on uninterpolatable volume

    void plotOverlapDiff(FieldMap* upMap, FieldMap* dnMap,
                         double overStart, double overStop, int nBins) {
    
        cout << "Overlap is from " << overStart << " to " << overStop <<
            " meters;\n with margin of " << margin << " m, plot using " <<
            nBins << " bins, each of z span " << (overStop -overStart)/nBins
            << " meters" << endl;

        Lbins  zAxis(nBins, overStart+margin, overStop-margin);
        Lhist1 upDiffMean    (zAxis,"Mean field difference, upstream overlap");
        Lhist1 upDiffPeak    (zAxis,"Peak field difference, upstream overlap");

        xLoLim = max(dnMap->xLow() +margin, upMap->xLow() +margin);
        xHiLim = min(dnMap->xHigh()-margin, upMap->xHigh()-margin);
        yLoLim = max(dnMap->yLow() +margin, upMap->yLow() +margin);
        yHiLim = min(dnMap->yHigh()-margin, upMap->yHigh()-margin);



        integralOverXY mapSlice1(upMap, dnMap, xLoLim,xHiLim, yLoLim,yHiLim);


        for (int iBin=1; iBin<=nBins; ++iBin) {
            mapSlice1.countAtZ.reset();
            zVal = zAxis.Center(iBin);
            discard = mapSlice1(zVal);        // Only use integral to sample via the counter in integrandOverX
            mean    = mapSlice1.countAtZ.mean();
            stdev   = mapSlice1.countAtZ.stdev();
            upDiffMean.Fill(zVal,mean);
            upDiffMean.GetBinPos(zVal).SetError(stdev);
            peak    = mapSlice1.countAtZ.zenith();
            upDiffPeak.Fill(zVal,peak);
            upDiffPeak.GetBinPos(zVal).SetError(0);
            cout << "That is " << plusORminus::formatted(mean,stdev,plusORminus::micro) << ", based on " <<
                mapSlice1.countAtZ.count() << " samples at z = " << zVal << endl;
        }



        Lfit1 pUpMean(upDiffMean);
        pUpMean.show();

        //Lfit1 pUpPeak(upDiffPeak);
        //pUpPeak.show();
        
        return;
    }
}



    
