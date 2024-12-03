//  Created by Leo Bellantoni on 4/1/23.



namespace sliceDiv {

    void plotDivergenceSlices(FieldMap* fieldMap) {

        // Make table of spatial coordinates in (X,Y) and also Z
        FieldMap::XYbinning scanOver = fieldMap->getXYbinning();
        Lbins xBins(scanOver.nXbins, scanOver.xLow, scanOver.xHigh);
        Lbins yBins(scanOver.nYbins, scanOver.yLow, scanOver.yHigh);
        vector<double> zValues = fieldMap->getZgrid();
        double zeOffset = (fieldMap->getOffset())(z);

        addr iniPlot = 1;
        addr finPlot = zValues.size() -1;
        for (addr iPlot=iniPlot; iPlot<finPlot; ++iPlot) {
            
            Lhist2 hDivSlice(xBins,yBins,
                    "{/Symbol \321\267}{/Palatino:Bold B} [T/m] at z = " +stringify(zValues[iPlot] -zeOffset));

            FieldMap::FieldMapInd ind = {0,0,static_cast<int>(iPlot)};

            for (ind.iX=1; ind.iX<scanOver.nXbins-1; ++ind.iX) {
                for (ind.iY=1; ind.iY<scanOver.nYbins-1; ++ind.iY) {
                    double div  = fieldMap->findDiv(ind);
                    FieldMap::FieldMapItr itr;
                    fieldMap->findItr(ind,itr);
                    hDivSlice.Fill(itr->x,itr->y, div);
                }
            }

            Lzcol pDivSlice(hDivSlice);
            pDivSlice.show();
     
            if (!cr2continue()) break;
        }

        return;
    }

} // end of namespace
