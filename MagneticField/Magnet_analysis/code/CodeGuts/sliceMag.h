//  Created by Leo Bellantoni on 4/1/23.



namespace sliceMag {

    void plotMagnitudeSlices(FieldMap* fieldMap) {

        // Make table of spatial coordinates in (X,Y) and also Z
        FieldMap::XYbinning scanOver = fieldMap->getXYbinning();
        Lbins xBins(scanOver.nXbins, scanOver.xLow, scanOver.xHigh);
        Lbins yBins(scanOver.nYbins, scanOver.yLow, scanOver.yHigh);
        vector<double> zValues = fieldMap->getZgrid();
        double zeOffset = (fieldMap->getOffset())(z);

        addr iniPlot = 0;
        addr finPlot = zValues.size();
        for (addr iPlot=iniPlot; iPlot<finPlot; ++iPlot) {
            
            Lhist2 hMagSlice(xBins,yBins,
                    "{/Palatino:Bold |B|} [T] at z = " +stringify(zValues[iPlot] -zeOffset));
            
            FieldMap::FieldMapInd ind = {0,0,static_cast<int>(iPlot)};

            for (ind.iX=0; ind.iX<scanOver.nXbins; ++ind.iX) {
                for (ind.iY=0; ind.iY<scanOver.nYbins; ++ind.iY) {
                    ra<double> Bee(3);
                    fieldMap->findB(Bee, ind);
                    // Magnitude of B will be zero for an invalid location in map
                    FieldMap::FieldMapItr itr;
                    fieldMap->findItr(ind,itr);
                    hMagSlice.Fill(itr->x,itr->y, Qadd(Bee(x),Bee(y),Bee(z)));
                }
            }
            
            Lzcol pMagSlice(hMagSlice);
            pMagSlice.blankEmpties();
            pMagSlice.show();
     
     
     
            if (!cr2continue()) break;
        }

        return;
    }

} // end of namespace
