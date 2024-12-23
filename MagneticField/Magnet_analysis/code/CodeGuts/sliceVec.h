//  Created by Leo Bellantoni on 4/1/23.



namespace sliceVec {

    void plotVectorSlices(FieldMap* fieldMap) {

        vector<double> zValues = fieldMap->getZgrid();
        double zeOffset = (fieldMap->getOffset())(z);
        FieldMap::XYbinning scanOver = fieldMap->getXYbinning();

        addr iniPlot = 0;
        addr finPlot = zValues.size();
        for (addr iPlot=iniPlot; iPlot<finPlot; ++iPlot) {

            Ldata hVectorsThisZ("{/Palatino:Bold B} [T] at z = " +stringify(zValues[iPlot] -zeOffset));
            Ldata::Point doink;

            FieldMap::FieldMapInd ind = {0,0,static_cast<int>(iPlot)};
            
            for (ind.iX=0; ind.iX<scanOver.nXbins; ++ind.iX) {
                for (ind.iY=0; ind.iY<scanOver.nYbins; ++ind.iY) {
                    FieldMap::FieldMapItr itr;
                    fieldMap->findItr(ind,itr);
                    if (itr->valid) {
                        doink.x  = itr->x;              doink.y  = itr->y;
                        doink.dx = itr->Bx;             doink.dy = itr->By;
                        hVectorsThisZ.push(doink);
                    }
                }
            }

            Lfield pVectorsThisZ(hVectorsThisZ);
            if (showThis==central) {
                pVectorsThisZ.SetArrowLength(0.01);
            } else {
                pVectorsThisZ.SetArrowLength(0.01);
            }
            pVectorsThisZ.show();
     
            if (!cr2continue()) break;
        }

        return;
    }

} // end of namespace
