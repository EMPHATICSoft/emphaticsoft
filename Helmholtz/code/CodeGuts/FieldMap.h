/*  L++ Field map for EMPHATIC magnet.
 *  Created by Leo Bellantoni on 29 Mar 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Explicit assumption is that map is given on a rectangular grid.
 *
 */
#pragma once


#include "../general/LppGlobals.h"
#include "../io/inputtextfile.h"
#include <vector>
using std::vector;
using std::cout;
using std::endl;
#include "ProbeCalib.h"



class FieldMap {
public:

    // FieldMap is an ra<AtPoint>, an offset position & helper code such as interators.
    // Constructor/getter architecture.



    struct AtPoint {bool valid; double x; double y; double z; double Bx; double By; double Bz;};
    AtPoint invalid = {false, 0.0,0.0,0.0, 0.0,0.0,0.0};



    FieldMap() = delete;
    FieldMap(string mapFileName, ProbeCalib calibration, bool stdForm=false);
    FieldMap(FieldMap const& inMap);
    FieldMap& operator=(FieldMap const& inMap);
    ~FieldMap();



    // There are 2 index techniques, and you can also index with (*map)(int,int,int)
    // N.B. not (*map)(size_t,size_t,size_t) - generally, an ra<T> can index from
    // anything, not just zero.  Although, indexing is from zero here, and the upper
    // range is getNxBins()-1, getNyBins()-1, getNzBins()-1 as in C++
    struct FieldMapInd {int iX; int iY; int iZ;};
    AtPoint operator()(FieldMapInd ind) const {return (*map)(ind.iX,ind.iY,ind.iZ);};
    int getNxBins() const {return map->ne1;};
    int getNyBins() const {return map->ne2;};
    int getNzBins() const {return map->ne3;};

    // Iterators are just bare pointers; important ones are begin and end iterators
    typedef AtPoint* FieldMapItr;
    FieldMapItr pBegin;
    FieldMapItr pEnd;

    // 2 methods to convert indices to iterators and back, with bounds check only
    bool findItr(FieldMapInd ind, FieldMapItr& retval) const {
        if (ind.iX<0         || ind.iY<0         || ind.iZ<0)         return false;
        if (ind.iX>=map->ne1 || ind.iY>=map->ne2 || ind.iZ>=map->ne3) return false;
        retval = &((*map)(ind.iX,ind.iY,ind.iZ));
        return true;
    }
    bool findInd (FieldMapItr inPoint, FieldMapInd& retval) const {
        // inPoint must be in the map coordinates, not physical
        // space.  That's why it is a FieldMapItr!
        
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



    // Convention is: spatial position is position in measured map (as found
    // from a dereferenced FieldMapInd/FieldMapItr) minus offset.
    void setOffset(ra<double> inOffset);
    ra<double> getOffset() const;
    // These return bounds on the map in the field map frame, not in real space.
    // They extend 1/2 bin outward from the extreme values in each dimension.
    double xLow() const {return _xLow;};    double xHigh() const {return _xHigh;};
    double yLow() const {return _yLow;};    double yHigh() const {return _yHigh;};
    double zLow() const {return _zLow;};    double zHigh() const {return _zHigh;};



    // Binning methods for easy histogramming; grid methods for precise point lists
    struct XYbinning {int nXbins, nYbins; double xLow, xHigh, yLow, yHigh;};
    XYbinning getXYbinning() const;
    vector<std::pair<double,double>> getXYgrid() const;
    struct Zbinning  {int nZbins; double zLow, zHigh;};
    Zbinning getZbinning()const;
    vector<double> getZgrid() const;



    ra<double>  findX(FieldMapInd inInd) const;                 // Returns lab coordinates
    ra<double>  findB(FieldMapInd inInd) const;
    void findX(ra<double>& retval, FieldMapInd& inInd) const;    // Same thing, different
    void findB(ra<double>& retval, FieldMapInd& inInd) const;    // signature for speed.

    // insideMap asks if there is a nearby valid point in map
    bool   insideMap   (ra<double>& Xi, FieldMapInd& closestInd) const;
    double findDiv     (FieldMapInd inInd) const;

    // Interpolator needs more than nearby valid point; it needs 3^3
    // contiguous nearby valid points.  Input argument Xi is in lab
    // coordinate system.
    bool   interpolate (ra<double>& Xi, ra<double>& B) const;



    ra<AtPoint> sameXY(FieldMapInd inInd) const;
    ra<AtPoint> sameXZ(FieldMapInd inInd) const;
    ra<AtPoint> sameYZ(FieldMapInd inInd) const;



private:
    ra<AtPoint>* map;  // rather than vector to make sort efficient.
    ra<double>* offset;
    // These are in index space, not physical space.
    double _xLow,_xHigh, _yLow, _yHigh, _zLow,_zHigh;


    bool findNextX(FieldMapInd& inInd, FieldMapInd& nextInd) const;
    bool findLastX(FieldMapInd& inInd, FieldMapInd& lastInd) const;
    bool findNextY(FieldMapInd& inInd, FieldMapInd& nextInd) const;
    bool findLastY(FieldMapInd& inInd, FieldMapInd& lastInd) const;
    bool findNextZ(FieldMapInd& inInd, FieldMapInd& nextInd) const;
    bool findLastZ(FieldMapInd& inInd, FieldMapInd& lastInd) const;

    void comcopy(FieldMap const& inMap, bool constructing);
 };
