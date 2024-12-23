/*  L++ Field map for EMPHATIC magnet.
 *  Created by Leo Bellantoni on 29 Mar 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Explicit assumption is that map is given on a rectangular grid.
 *
 */
#pragma once



#include "Ldelete.h"
#include "LppGlobals.h"
#include "inputtextfile.h"
#include "ra.h"
#include "intsNabs.h"

#include <fstream>
#include <vector>
using std::vector;
using std::cout;
using std::endl;
#include <string>
using std::string;



class FieldMap {
public:

    // FieldMap is an ra<AtPoint>, an offset position & helper code such as interators.
    // Also it contains a vector<string> which is the header information.
    // Constructor/getter architecture, with a write-it-out method



    struct AtPoint {bool valid; double x; double y; double z; double Bx; double By; double Bz;};
    AtPoint invalid = {false, 0.0,0.0,0.0, 0.0,0.0,0.0};



    // Constructor for user inputs file.  Format of file is as enumerated:
    enum inputFileFormat {CMSMdata,RecoFormat,SABRdata};
    // CMSMdata is what we got from AP-STD when they measured the CMSM magnet.  The file
    //  had to be hand-edited to remove some extraneous data points and the first line
    //  commented out.  There is an extraneous column of data as well.
    //  Units are inches and Tesla, but the polarity of Bx and By is right
    // RecoFormat is what the EMPHATIC analysis software uses.  It has a lengthy
    //  descriptive header, and the units are mm and kiloGauss.  The polarity of
    //  Bx and By better be right!
    // SABRdata is what  we got from AP-STD when they measured the SABR magnet.
    //  Units are mm and Tesla, but they got the polarity of Bx and By wrong.
    //  Also, the Bx and Bz columns are swapped.
    // Units inside this code is meters and Tesla with correct polarity, which
    //  are always converted to RecoFormat for the collaboration at output.
    
    FieldMap() = delete;
    FieldMap(string mapFileName, inputFileFormat format);
    FieldMap(FieldMap const& inMap);                // Deep copy.
    FieldMap& operator=(FieldMap const& inMap);     // Deep copy.
    ~FieldMap();

    // Output is in mm and kGauss, with whatever header lines the map had when it
    // was read in.  The only map in the scheme which was created in this code and
    // not read in is the merged map, which is not created using this method and
    // gets its header from OutputHeader.txt.
    bool writeMeOut(string outFileName, bool overwrite=false);



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
    bool findItr(FieldMapInd ind,     FieldMapItr& retval) const;
    bool findInd(FieldMapItr inPoint, FieldMapInd& retval) const;



    // Convention is: spatial position is position in measured map (as found
    // from a dereferenced FieldMapInd/FieldMapItr) minus offset.
    void setOffset(ra<double> inOffset);
    ra<double> getOffset() const;
    // These return bounds on the map in the field map frame, not in real space.
    // They extend 1/2 bin outward from the extreme values in each dimension.
    double xLow() const {return _xLow;};    double xHigh() const {return _xHigh;};
    double yLow() const {return _yLow;};    double yHigh() const {return _yHigh;};
    double zLow() const {return _zLow;};    double zHigh() const {return _zHigh;};



    // Binning methods for easy histogramming; grid methods for precise point lists.
    // Grid methods do not include points with !valid.  x/y Low/High in field map
    // coordinates not real space.
    struct XYbinning {int nXbins, nYbins; double xLow, xHigh, yLow, yHigh;};
    XYbinning getXYbinning() const;
    vector<std::pair<double,double>> getXYgrid() const;
    struct Zbinning  {int nZbins; double zLow, zHigh;};
    Zbinning getZbinning()const;
    vector<double> getZgrid() const;



    // Returns lab coordinates, not FieldMapItr coordinates
    bool findX(ra<double>& retval, FieldMapInd& inInd) const;    // Same thing, different
    bool findB(ra<double>& retval, FieldMapInd& inInd) const;    // signature for speed.

    // insideMap asks if there is a nearby valid mapped point in map &
    // BTW gives the FieldMap::FieldMapInd for that point.
    bool   insideMap(ra<double>& Xi, FieldMapInd& closestInd) const;

    // Interpolator needs more than nearby valid point; it needs 3^3 contiguous
    // nearby valid points.  Input argument Xi is in lab coordinate system.
    bool   interpolate(ra<double>& Xi, ra<double>& B) const;

    // And some derivatives for ya!
    double findDiv(FieldMapInd inInd) const;
    ra<double> findGrad(FieldMapInd inInd, Lv3ax fieldComponent);

    ra<AtPoint> sameXY(FieldMapInd inInd) const;
    ra<AtPoint> sameXZ(FieldMapInd inInd) const;
    ra<AtPoint> sameYZ(FieldMapInd inInd) const;



private:
    ra<AtPoint>* map;  // rather than vector to make sort efficient.
    ra<double>* offset;
    // These are in index space, not physical space.
    double _xLow,_xHigh, _yLow, _yHigh, _zLow,_zHigh;
    vector<string> headerLines;
    
    // How many digits in location precision (meters).  Must be 1.0d-? format.
    // Data from the map is truncated to this level.
    double const posPrecise = 1.0e-5;

    bool findNextX(FieldMapInd& inInd, FieldMapInd& nextInd) const;
    bool findLastX(FieldMapInd& inInd, FieldMapInd& lastInd) const;
    bool findNextY(FieldMapInd& inInd, FieldMapInd& nextInd) const;
    bool findLastY(FieldMapInd& inInd, FieldMapInd& lastInd) const;
    bool findNextZ(FieldMapInd& inInd, FieldMapInd& nextInd) const;
    bool findLastZ(FieldMapInd& inInd, FieldMapInd& lastInd) const;

    void comcopy(FieldMap const& inMap, bool constructing);
 };
