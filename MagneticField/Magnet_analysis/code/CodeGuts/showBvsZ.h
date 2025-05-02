//  Created by Leo Bellantoni on 13 Apr 2023.



namespace showBvsZ {

    void plotFieldvsZ(FieldMap* fieldMap) {

        vector<std::pair<double,double>> xyGrid  = fieldMap->getXYgrid();

        for (addr iPloink=0; iPloink<xyGrid.size(); ++iPloink) {
            double alongX = xyGrid[iPloink].first;
            double alongY = xyGrid[iPloink].second;

            Ldata hFieldX("Bx");
            Ldata hFieldY("{/Palatino:Bold B} [T] vs. z at (x,y) of (" +stringify(alongX)
                            +", " +stringify(alongY) +")");
            Ldata hFieldZ("Bz");
            Ldata::Point doinkX;        Ldata::Point doinkY;         Ldata::Point doinkZ;
            // Find some point in the map at this (x,y) - there surely will be one -
            // & get a lineInZ along that line
            FieldMap::FieldMapItr itr=fieldMap->pBegin;
            for (; itr<=fieldMap->pEnd; ++itr) {
                if (!itr->valid) continue;
                if (rEQ(itr->x, alongX) &&
                    rEQ(itr->y, alongY)) break;
            }
            ra<double> offset(3);       offset = fieldMap->getOffset();
            FieldMap::FieldMapInd ind;        fieldMap->findInd(itr,ind);
            ra<FieldMap::AtPoint> lineInZ = fieldMap->sameXY(ind);
            for (int jPloink=0; jPloink < lineInZ.ne1; ++jPloink) {
                doinkX.x = doinkY.x = doinkZ.x = lineInZ(jPloink).z -offset(z);
                doinkX.y = lineInZ(jPloink).Bx;         hFieldX.push(doinkX);
                doinkY.y = lineInZ(jPloink).By;         hFieldY.push(doinkY);
                doinkZ.y = lineInZ(jPloink).Bz;         hFieldZ.push(doinkZ);
                // Print Y field on axis
                if (rEQ(alongX,0)&&rEQ(alongY,0)) cout << doinkY.y << endl;
            }
            
            // Having collected all the field points down this line, make the plot
            Lscatter pField(hFieldY);
            pField.AddSeries(Lscatter::ScatCol::Red, hFieldX);
            pField.AddSeries(Lscatter::ScatCol::Blue,hFieldZ);
            pField.show();

            if (!cr2continue()) break;

        }
        return;
    }

} // end of namespace
