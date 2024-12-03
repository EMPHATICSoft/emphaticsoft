//
//  FieldMap.cpp
//  L++
//
//  Created by Leo Bellantoni on 3/29/23.
//

#include "quadd.h"
#include "Funcs.h"
#include "FieldMap.h"





FieldMap::FieldMap(string mapFile, inputFileFormat format) : offset(nullptr) {

    // This edition of FieldMap::FieldMap only good for the Oct 2023 data.
    double convert_X, convert_B;
    switch (format) {
    case CMSMdata:
        convert_X = 2.54/100.0;     // inches to meters
        convert_B = 1.0;            // Tesla to Tesla
        break;
    case RecoFormat:
        convert_X = 1.0/1000.0;     // mm to meters
        convert_B = 1.0/10.0;       // kGauss to Tesla
        break;
    case SABRdata:
        convert_X = 1.0/1000.0;     // mm to meters
        convert_B = 1.0;            // Tesla to Tesla
    }



    // Determine the dimensions for the ra<AtPoint> structure
    vector<double> xValues, yValues, zValues;

    inputtextfile mapData(mapFile);
    std::string makeWhite = ",";
    // Get the header lines
    while (mapData.fetchline(makeWhite) != -1) {
        if (mapData.wtfIN()[0]=='%') {
            headerLines.push_back(mapData.wtfIN());
        } else {
            break;
        }
    }

    do {
        double xMap  = convert_X * mapData.tokenasd(0);
        double yMap  = convert_X * mapData.tokenasd(1);
        double zMap  = convert_X * mapData.tokenasd(2);
        // Truncate to appropriate precision
        xMap = ndouble(xMap,posPrecise);
        yMap = ndouble(yMap,posPrecise);
        zMap = ndouble(zMap,posPrecise);
        if ( std::find(xValues.begin(),xValues.end(), xMap)==xValues.end() ) {
            xValues.push_back(xMap);
        }
        if ( std::find(yValues.begin(),yValues.end(), yMap)==yValues.end() ) {
            yValues.push_back(yMap);
        }
        if ( std::find(zValues.begin(),zValues.end(), zMap)==zValues.end() ) {
            zValues.push_back(zMap);
        }
    } while (mapData.fetchline(makeWhite) != -1);
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
    while (mapData.fetchline(makeWhite) != -1) {
        if (mapData.wtfIN()[0]!='%') break;
    }
    AtPoint thisLine;
    do {
        thisLine.valid = true;

        thisLine.x  = convert_X * mapData.tokenasd(0);
        thisLine.y  = convert_X * mapData.tokenasd(1);
        thisLine.z  = convert_X * mapData.tokenasd(2);

        // Truncate to appropriate precision
        thisLine.x = ndouble(thisLine.x,posPrecise);
        thisLine.y = ndouble(thisLine.y,posPrecise);
        thisLine.z = ndouble(thisLine.z,posPrecise);

        switch (format) {
        case CMSMdata:
            thisLine.Bx = convert_B * mapData.tokenasd(4);
            thisLine.By = convert_B * mapData.tokenasd(5);
            thisLine.Bz = convert_B * mapData.tokenasd(6);
            thisLine.Bz = -thisLine.Bz;         // Pretty sure that Mike got his signs wrong here.
            break;
        case RecoFormat:
            thisLine.Bx = convert_B * mapData.tokenasd(3);
            thisLine.By = convert_B * mapData.tokenasd(4);
            thisLine.Bz = convert_B * mapData.tokenasd(5);
            break;
        case SABRdata:
            thisLine.Bx = convert_B * mapData.tokenasd(5);
            thisLine.By = convert_B * mapData.tokenasd(4);
            thisLine.Bz = convert_B * mapData.tokenasd(3);
            thisLine.Bx = -thisLine.Bx;
            thisLine.By = -thisLine.By;
        }

        double binWidth = (_xHigh - _xLow)/(xValues.size());
        int indX = floor((thisLine.x - _xLow)/binWidth);
        binWidth = (_yHigh - _yLow)/(yValues.size());
        int indY = floor((thisLine.y - _yLow)/binWidth);
        binWidth = (_zHigh - _zLow)/(zValues.size());
        int indZ = floor((thisLine.z - _zLow)/binWidth);

        (*map)(indX,indY,indZ) = thisLine;
    } while (mapData.fetchline(makeWhite) != -1);
    
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





bool FieldMap::writeMeOut(string outFileName, bool overwrite) {
    if (!overwrite && !filehamna(outFileName)) {
        LppUrk.UsrIssue(-1, Fatal, "FieldMap::writeItOut","Output file already exists and overwrite flag not set.");
    }

    std::fstream* fileOUT = new std::fstream(outFileName.c_str(),std::ios_base::out);
    if (fileOUT->fail()) {
        cout << outFileName << endl;
        LppUrk.UsrIssue(-2, Fatal, "FieldMap::writeItOut","File could not opened for appendage.");
        fileOUT->close();       // Not sure what state fileOUT is in at this point
        Ldelete(fileOUT);
        return false;
    }

    vector<string>::iterator headerItr = headerLines.begin();
    while (headerItr != headerLines.end()) {
        *fileOUT << *headerItr << endl;
        ++headerItr;
    }

    FieldMap::FieldMapInd outInd;     char outLine[200];
    ra<double>  X(3);               ra<double> B(3);
    for (outInd.iZ=0; outInd.iZ<getNzBins(); ++outInd.iZ) {
        for (outInd.iY=0; outInd.iY<getNyBins(); ++outInd.iY) {
            for (outInd.iX=0; outInd.iX<getNxBins(); ++outInd.iX) {
                if (findX(X,outInd)) {
                    findB(B,outInd);
                } else {
                    // You are at some unvalid corner point.  Probably can recover
                    FieldMapItr deleteMe;
                    // Check for x,y,z total span
                    if (!findItr(outInd,deleteMe)) return false;
                    // deleteMe is FieldMap::invalid, so have to find X hardway
                    ra<double> origin(3);   origin = 0.0;
                    FieldMapInd originInd;
                    if (!insideMap(origin,originInd)) return false;
                    X(x) = (outInd.iX -originInd.iX)*(_xHigh-_xLow)/getNxBins();
                    X(y) = (outInd.iY -originInd.iY)*(_yHigh-_yLow)/getNyBins();
                    X(z) = (outInd.iZ -originInd.iZ)*(_zHigh-_zLow)/getNzBins();
                    B    = 0.0;
                }
                sprintf(outLine, "%-25g %-24g %-24g %-24g %-24g %-24g",
                    1000*X(x),1000*X(y), 1000*X(z), 10*B(x),10*B(y),10*B(z));
                *fileOUT << outLine << endl;
            }
        }
    }
    fileOUT->close();
    return false;
}





bool FieldMap::findItr(FieldMapInd ind, FieldMapItr& retval) const {
    if (ind.iX<0         || ind.iY<0         || ind.iZ<0)         return false;
    if (ind.iX>=map->ne1 || ind.iY>=map->ne2 || ind.iZ>=map->ne3) return false;
    retval = &((*map)(ind.iX,ind.iY,ind.iZ));
    return true;
}
bool FieldMap::findInd (FieldMapItr inPoint, FieldMapInd& retval) const {
    // inPoint must be in the map coordinates, not physical
    // space.  That's why it is a FieldMapItr!
    if (!(inPoint->valid)) return false;
    
    Zbinning Zbinz   = getZbinning();
    double dZ        = (Zbinz.zHigh -Zbinz.zLow)/Zbinz.nZbins;
    retval.iZ        = floor((inPoint->z -Zbinz.zLow)/dZ);      // Recall floor(-0.49) = -1

    XYbinning XYbinz = getXYbinning();
    double dX        = (XYbinz.xHigh -XYbinz.xLow)/XYbinz.nXbins;
    retval.iX        = floor((inPoint->x -XYbinz.xLow)/dX);
    double dY        = (XYbinz.yHigh -XYbinz.yLow)/XYbinz.nYbins;
    retval.iY        = floor((inPoint->y -XYbinz.xLow)/dY);

    if (retval.iX<0         || retval.iY<0         || retval.iZ<0)         return false;
    if (retval.iX>=map->ne1 || retval.iY>=map->ne2 || retval.iZ>=map->ne3) return false;
    return true;
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

    for (FieldMapItr itr=pBegin; itr<pEnd; ++itr) {
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

    for (FieldMapItr itr=pBegin; itr<pEnd; ++itr) {
        if (itr->valid) {
            double zMap = itr->z;
            if ( std::find(zValues.begin(),zValues.end(), zMap)==zValues.end() ) zValues.push_back(zMap);
        }
    }
    sort(zValues.begin(),zValues.end());
    return zValues;
}





bool FieldMap::findX(ra<double>& retval, FieldMapInd& inInd) const {
    AtPoint doink = (*this)(inInd);
    if (!doink.valid) {
        // You are trying to find X for a point outside of the map.  Within limits,
        // actually, it can be done.  But it shouldn't be.
        return false;
    } else {
        retval(x)  = doink.x;         retval(y)  = doink.y;          retval(z)  = doink.z;
    }
    /* Because C is return-by-value, retval(x) -= getOffset()(x) is slower as it must
    call the copy constructor of ra<double> to create a return value */
    retval(x) -= (*offset)(x);    retval(y) -= (*offset)(y);     retval(z) -= (*offset)(z);
    return true;
}
bool FieldMap::findB(ra<double>& retval, FieldMapInd& inInd) const {
    AtPoint doink = (*this)(inInd);
    retval(x)  = doink.Bx;        retval(y)  = doink.By;         retval(z)  = doink.Bz;
    return doink.valid;
}



bool FieldMap::insideMap(ra<double>& Xi, FieldMapInd& closestInd) const {
    // Quick hi-reject rate test first.
    if (Xi(z) +(*offset)(z) < _zLow) return false;
    if (Xi(z) +(*offset)(z) > _zHigh) return false;
    
    // Make a temporary FieldMapItr, in FieldMapItr space, to get matching index
    FieldMapItr tmp = new AtPoint;      tmp->valid = true; // (otherwise findInd can fail)
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
        findB(moreB, moreDind);    findB(lessB, lessDind);
        findX(moreXi,moreDind);    findX(lessXi,lessDind);
        retval += (moreB(x) -lessB(x)) / (moreXi(x) -lessXi(x));
        valid--;
    }
    
    if ( findNextY(inInd,moreDind) &&  findLastY(inInd,lessDind) ) {
        findB(moreB, moreDind);    findB(lessB, lessDind);
        findX(moreXi,moreDind);    findX(lessXi,lessDind);
        retval += (moreB(y) -lessB(y)) / (moreXi(y) -lessXi(y));
        valid--;
    }
    
    if ( findNextZ(inInd,moreDind) &&  findLastZ(inInd,lessDind) ) {
        findB(moreB, moreDind);    findB(lessB, lessDind);
        findX(moreXi,moreDind);    findX(lessXi,lessDind);
        retval += (moreB(z) -lessB(z)) / (moreXi(z) -lessXi(z));
        valid--;
    }

    if (valid==0) {
        return retval;
    } else {
        return Nana;
    }
}



ra<double> FieldMap::findGrad(FieldMapInd inInd, Lv3ax fieldComponent) {
    ra<double> retval(3);       retval = Nana;
    FieldMapInd moreDind, lessDind;
    ra<double> moreB(3);        ra<double> lessB(3);
    ra<double> moreXi(3);       ra<double> lessXi(3);

    if ( findNextX(inInd,moreDind) &&  findLastX(inInd,lessDind) ) {
        findB(moreB, moreDind);    findB(lessB, lessDind);
        findX(moreXi,moreDind);    findX(lessXi,lessDind);
        retval(x) = (moreB(fieldComponent) -lessB(fieldComponent))
                   /(moreXi(x) -lessXi(x));
    }
    
    if ( findNextY(inInd,moreDind) &&  findLastY(inInd,lessDind) ) {
        findB(moreB, moreDind);    findB(lessB, lessDind);
        findX(moreXi,moreDind);    findX(lessXi,lessDind);
        retval(y) = (moreB(fieldComponent) -lessB(fieldComponent))
                   /(moreXi(y) -lessXi(y));
    }
    
    if ( findNextZ(inInd,moreDind) &&  findLastZ(inInd,lessDind) ) {
        findB(moreB, moreDind);    findB(lessB, lessDind);
        findX(moreXi,moreDind);    findX(lessXi,lessDind);
        retval(z) = (moreB(fieldComponent) -lessB(fieldComponent))
                   /(moreXi(z) -lessXi(z));
    }

    return retval;
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
