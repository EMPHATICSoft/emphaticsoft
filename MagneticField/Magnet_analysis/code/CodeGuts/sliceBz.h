//  Created by Leo Bellantoni on 4/1/23.



namespace sliceBz {

    void plotBzSlices(FieldMap* fieldMap) {

        vector<double> zValues = fieldMap->getZgrid();
        double zeOffset = (fieldMap->getOffset())(z);
        FieldMap::XYbinning scanOver = fieldMap->getXYbinning();
        Lbins bBz_vsX(scanOver.nXbins,scanOver.xLow,scanOver.xHigh);
        Lbins bBz_vsY(scanOver.nYbins,scanOver.yLow,scanOver.yHigh);

        addr iniPlot = 0;
        addr finPlot = zValues.size();
        for (addr iPlot=iniPlot; iPlot<finPlot; ++iPlot) {


            Lhist2 hBz_vsXY(bBz_vsX,bBz_vsY, "{/Palatino:Bold B}_z [T] at z = " +stringify(zValues[iPlot] -zeOffset));

            FieldMap::FieldMapInd ind = {0,0,static_cast<int>(iPlot)};
            
            for (ind.iX=0; ind.iX<scanOver.nXbins; ++ind.iX) {
                for (ind.iY=0; ind.iY<scanOver.nYbins; ++ind.iY) {
                    FieldMap::FieldMapItr itr;
                    fieldMap->findItr(ind,itr);
                    if (itr->valid) {
                        hBz_vsXY.Fill(itr->x,itr->y, itr->Bz);
                    }
                }
            }

            Lzcol pBz_vsXY(hBz_vsXY);
            pBz_vsXY.show();
     
            if (!cr2continue()) break;
        }

        return;
    }

} // end of namespace
