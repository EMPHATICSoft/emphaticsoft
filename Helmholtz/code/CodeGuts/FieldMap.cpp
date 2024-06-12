//
//  FieldMap.cpp
//  L++
//
//  Created by Leo Bellantoni on 3/29/23.
//

#include "../functions/quadd.h"
#include "Funcs.h"
#include "FieldMap.h"





FieldMap::FieldMap(string mapFile, ProbeCalib calib, bool stdForm) : offset(nullptr) {
    // stdForm has 9 lines of comment at top; other form, from AP-STD's csv files,
    // has only one line of comment at the top.  AP-STD files are in inches, standard
    // files in mm.  There's an extra column of unused data in the AP-STD files.
    double inch2Meter = 25.4/1000.0;
    if (stdForm) inch2Meter = 0.001;
 
    // Determine the dimensions for the ra<AtPoint> structure
    inputtextfile mapData(mapFile);
    std::string makeWhite = ",";

    vector<double> xValues, yValues, zValues;
    mapData.fetchline(makeWhite);       if (!stdForm) mapData.EoL = '\r';
    if (stdForm) for (int i=0; i<8; ++i) mapData.fetchline(makeWhite);
    while (mapData.fetchline(makeWhite) != -1) {
        double xMap  = inch2Meter * mapData.tokenasd(0);
        double yMap  = inch2Meter * mapData.tokenasd(1);
        double zMap  = inch2Meter * mapData.tokenasd(2);
        if ( std::find(xValues.begin(),xValues.end(), xMap)==xValues.end() ) {
            xValues.push_back(xMap);
        }
        if ( std::find(yValues.begin(),yValues.end(), yMap)==yValues.end() ) {
            yValues.push_back(yMap);
        }
        if ( std::find(zValues.begin(),zValues.end(), zMap)==zValues.end() ) {
            zValues.push_back(zMap);
        }
    }
    mapData.rewind();
    map = new ra<AtPoint>(xValues.size(),yValues.size(),zValues.size());
    map->setbase(0, 0, 0);
    (*map) = invalid;
    
    double lowest    = *( min_element(xValues.begin(),xValues.end()) );
    double highest   = *( max_element(xValues.begin(),xValues.end()) );
    double halfWidth = (highest -lowest)/(2.0 * (xValues.size() -1));
    _xLow  = lowest  -halfWidth;
    _xHigh = highest +halfWidth;

    lowest    = *( min_element(yValues.begin(),yValues.end()) );
    highest   = *( max_element(yValues.begin(),yValues.end()) );
    halfWidth = (highest -lowest)/(2.0 * (yValues.size() -1));
    _yLow  = lowest  -halfWidth;
    _yHigh = highest +halfWidth;

    lowest    = *( min_element(zValues.begin(),zValues.end()) );
    highest   = *( max_element(zValues.begin(),zValues.end()) );
    halfWidth = (highest -lowest)/(2.0 * (zValues.size() -1));
    _zLow  = lowest  -halfWidth;
    _zHigh = highest +halfWidth;

    // Yuh, gotta read the map a 2nd time for the data itself
    mapData.fetchline(makeWhite);       // Ignore 1st lines again
    if (stdForm) for (int i=0; i<8; ++ i) mapData.fetchline(makeWhite);
    AtPoint thisLine;
    while (mapData.fetchline(makeWhite) != -1) {
        thisLine.valid = true;
        thisLine.x  = inch2Meter * mapData.tokenasd(0);
        thisLine.y  = inch2Meter * mapData.tokenasd(1);
        thisLine.z  = inch2Meter * mapData.tokenasd(2);
        if (!stdForm) {
            thisLine.Bx = calib.correctBx(mapData.tokenasd(4),stdForm);
            thisLine.By = calib.correctBy(mapData.tokenasd(5),stdForm);
            thisLine.Bz = calib.correctBz(mapData.tokenasd(6),stdForm);
        } else {
            // stdForm already has calibration in it.
            thisLine.Bx = calib.correctBx(mapData.tokenasd(3),stdForm);
            thisLine.By = calib.correctBy(mapData.tokenasd(4),stdForm);
            thisLine.Bz = calib.correctBz(mapData.tokenasd(5),stdForm);
        }

        double binWidth = (_xHigh - _xLow)/(xValues.size());
        int indX = floor((thisLine.x - _xLow)/binWidth);
        binWidth = (_yHigh - _yLow)/(yValues.size());
        int indY = floor((thisLine.y - _yLow)/binWidth);
        binWidth = (_zHigh - _zLow)/(zValues.size());
        int indZ = floor((thisLine.z - _zLow)/binWidth);

        (*map)(indX,indY,indZ) = thisLine;
    }
    
    offset  = new ra<double>(3);
    *offset = 0;
    
    pBegin = &(map->val[0]);
    int nTot = map->ne1 * map->ne2 * map->ne3;
    pEnd   = &(map->val[nTot]);
    return;
}


FieldMap::FieldMap(FieldMap const& inMap) {
    comcopy(inMap,true);
    return;
}
FieldMap& FieldMap::operator=(FieldMap const& inMap) {
    if (this != &inMap) comcopy(inMap,false);
    return *this;
}
void FieldMap::comcopy(FieldMap const& inMap, bool constructing) {
    if (!constructing) {
        delete map;
        delete offset;
    }
    map    = new ra<AtPoint>(inMap.map->ne1,inMap.map->ne2,inMap.map->ne3);
    (*map) = (*inMap.map);
    offset = new ra<double>(3);
    setOffset(*(inMap.offset));
    pBegin = &(map->val[0]);
    int nTot = map->ne1 * map->ne2 * map->ne3;
    pEnd   = &(map->val[nTot]);
    _xLow  = inMap._xLow;       _xHigh = inMap._xHigh;
    _yLow  = inMap._yLow;       _yHigh = inMap._yHigh;
    _zLow  = inMap._zLow;       _zHigh = inMap._zHigh;
    return;
}

FieldMap::~FieldMap() {
    delete map;
    delete offset;
}





void FieldMap::setOffset(ra<double> inOffset) {
    // All constructors must initialize offset to SOME ra<double>(3)
    *offset = inOffset;         // ra's operator= is deep copy
}

ra<double> FieldMap::getOffset() const {
    // Recall: C is return by value, so this return statement calls
    // the ra copy constructor, which does a deep copy.
    return *offset;
}





FieldMap::XYbinning FieldMap::getXYbinning() const {
    // Does assume magnet scan was based on uniform intervals in the
    // plane transverse to the main beam axis.
    XYbinning retval;
    retval.nXbins = getNxBins();
    retval.nYbins = getNyBins();
    retval.xLow   = _xLow;
    retval.xHigh  = _xHigh;
    retval.yLow   = _yLow;
    retval.yHigh  = _yHigh;

    return retval;
}

vector<std::pair<double,double>> FieldMap::getXYgrid() const {
    vector<std::pair<double,double>> retval;

    for (FieldMapItr itr=pBegin; itr<=pEnd; ++itr) {
        if (itr->valid) {
            std::pair<double,double> doink(itr->x,itr->y);
            if (retval.size()==0) {
                retval.push_back(doink);
            } else {
                bool foundBefore = false;
                for (std::pair<double,double> pushedBefore : retval) {
                    if (rEQ(doink.first, pushedBefore.first) &&
                        rEQ(doink.second,pushedBefore.second)) {
                        foundBefore = true;     break;
                    }
                }
                if (!foundBefore) retval.push_back(doink);
            }
        }
    }
    return retval;
}

FieldMap::Zbinning FieldMap::getZbinning() const {
    // Does assume magnet scan was based on uniform intervals in the
    // plane transverse to the main beam axis.
    Zbinning retval;
    retval.nZbins = getNzBins();
    retval.zLow   = _zLow;
    retval.zHigh  = _zHigh;

    return retval;
}

vector<double> FieldMap::getZgrid() const {
    vector<double> zValues;

    for (FieldMapItr itr=pBegin; itr<=pEnd; ++itr) {
        double zMap = itr->z;
        if ( std::find(zValues.begin(),zValues.end(), zMap)==zValues.end() ) zValues.push_back(zMap);
    }
    sort(zValues.begin(),zValues.end());
    return zValues;
}





ra<double> FieldMap::findX(FieldMapInd inInd) const {
    // Returns lab coordinates.  "doink" is in the field map coordinates; (*offset)
    // is subtracted from that to get a position in the lab frame.
    ra<double> retval(3);
    findX(retval, inInd);
    return retval;
}
ra<double> FieldMap::findB(FieldMapInd inInd) const {
    ra<double> retval(3);
    findB(retval, inInd);
    return retval;
}
void FieldMap::findX(ra<double>& retval, FieldMapInd& inInd) const {
    AtPoint doink = (*this)(inInd);
    retval(x)  = doink.x;         retval(y)  = doink.y;          retval(z)  = doink.z;
    /* Because C is return-by-value, retval(x) -= getOffset()(x) is slower as it must
    call the copy constructor of ra<double> to create a return value */
    retval(x) -= (*offset)(x);    retval(y) -= (*offset)(y);     retval(z) -= (*offset)(z);
    return;
}
void FieldMap::findB(ra<double>& retval, FieldMapInd& inInd) const {
    AtPoint doink = (*this)(inInd);
    retval(x)  = doink.Bx;        retval(y)  = doink.By;         retval(z)  = doink.Bz;
    return;
}



bool FieldMap::insideMap(ra<double>& Xi, FieldMapInd& closestInd) const {
    // Quick hi-reject rate test first.
    if (Xi(z) +(*offset)(z) < _zLow) return false;
    if (Xi(z) +(*offset)(z) > _zHigh) return false;
    
    // Make a temporary FieldMapItr, in FieldMapItr space, to get matching index
    FieldMapItr tmp = new AtPoint;
    /* Because C is return-by-value, retval(x) -= getOffset()(x) is slower as it must
    call the copy constructor of ra<double> to create a return value */
    tmp->x = Xi(x) +(*offset)(x);     tmp->y = Xi(y) +(*offset)(y);     tmp->z = Xi(z) +(*offset)(z);
    bool found = findInd(tmp,closestInd);
    delete tmp;
    if (!found) return false;
    return (*this)(closestInd).valid;
}



double FieldMap::findDiv(FieldMapInd inInd) const {
    double retval = 0.0;
    int valid     = 3;
    FieldMapInd moreDind, lessDind;
    ra<double> moreB(3);        ra<double> lessB(3);
    ra<double> moreXi(3);       ra<double> lessXi(3);

    if ( findNextX(inInd,moreDind) &&  findLastX(inInd,lessDind) ) {
        moreB  = findB(moreDind);   lessB  = findB(lessDind);
        moreXi = findX(moreDind);   lessXi = findX(lessDind);
        retval += (moreB(x) -lessB(x)) / (moreXi(x) -lessXi(x));
        valid--;
    }
    
    if ( findNextY(inInd,moreDind) &&  findLastY(inInd,lessDind) ) {
        moreB  = findB(moreDind);   lessB  = findB(lessDind);
        moreXi = findX(moreDind);   lessXi = findX(lessDind);
        retval += (moreB(y) -lessB(y)) / (moreXi(y) -lessXi(y));
        valid--;
    }
    
    if ( findNextZ(inInd,moreDind) &&  findLastZ(inInd,lessDind) ) {
        moreB  = findB(moreDind);   lessB  = findB(lessDind);
        moreXi = findX(moreDind);   lessXi = findX(lessDind);
        retval += (moreB(z) -lessB(z)) / (moreXi(z) -lessXi(z));
        valid--;
    }

    if (valid==0) {
        return retval;
    } else {
        return Dbig;
    }
}





ra<FieldMap::AtPoint> FieldMap::sameXY(FieldMapInd inInd) const {
    const gslice lineInZ = map->sliceRA(inInd.iX, inInd.iY, map->ALL);
    ra<FieldMap::AtPoint> retval(getNzBins()) ;
    retval.setbase(0);
    retval.val = (map->val)[lineInZ];
    return retval;
}

ra<FieldMap::AtPoint> FieldMap::sameXZ(FieldMapInd inInd) const {
    const gslice lineInY = map->sliceRA(inInd.iX, map->ALL, inInd.iZ);
    ra<FieldMap::AtPoint> retval(getNyBins()) ;
    retval.setbase(0);
    retval.val = (map->val)[lineInY];
    return retval;
}

ra<FieldMap::AtPoint> FieldMap::sameYZ(FieldMapInd inInd) const {
    const gslice lineInX = map->sliceRA(map->ALL, inInd.iY, inInd.iZ);
    ra<FieldMap::AtPoint> retval(getNxBins()) ;
    retval.setbase(0);
    retval.val = (map->val)[lineInX];
    return retval;
}





bool FieldMap::findNextX(FieldMapInd& inInd, FieldMapInd& nextInd) const {
    nextInd = inInd;
    if (nextInd.iX+1 > getNxBins()-1) return false;
    AtPoint doink = (*map)(++nextInd.iX,nextInd.iY,nextInd.iZ);
    return doink.valid;
}
bool FieldMap::findLastX(FieldMapInd& inInd, FieldMapInd& nextInd) const {
    nextInd = inInd;
    if (nextInd.iX-1 < 0) return false;
    AtPoint doink = (*map)(--nextInd.iX,nextInd.iY,nextInd.iZ);
    return doink.valid;
}
 
bool FieldMap::findNextY(FieldMapInd& inInd, FieldMapInd& nextInd) const {
    nextInd = inInd;
    if (nextInd.iY+1 > getNyBins()-1) return false;
    AtPoint doink = (*map)(nextInd.iX,++nextInd.iY,nextInd.iZ);
    return doink.valid;
}
bool FieldMap::findLastY(FieldMapInd& inInd, FieldMapInd& nextInd) const {
    nextInd = inInd;
    if (nextInd.iY-1 < 0) return false;
    AtPoint doink = (*map)(nextInd.iX,--nextInd.iY,nextInd.iZ);
    return doink.valid;
}

bool FieldMap::findNextZ(FieldMapInd& inInd, FieldMapInd& nextInd) const {
    nextInd = inInd;
    if (nextInd.iZ+1 > getNzBins()-1) return false;
    AtPoint doink = (*map)(nextInd.iX,nextInd.iY,++nextInd.iZ);
    return doink.valid;
}
bool FieldMap::findLastZ(FieldMapInd& inInd, FieldMapInd& nextInd) const {
    nextInd = inInd;
    if (nextInd.iZ-1 < 0) return false;
    AtPoint doink = (*map)(nextInd.iX,nextInd.iY,--nextInd.iZ);
    return doink.valid;
}

