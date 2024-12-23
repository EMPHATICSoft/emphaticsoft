/*  L++ example
 *  Created by Leo Bellantoni on 3/29/2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  C++14, and the addr type is 8 bytes long.
 */
#include "LppMain.h"
#include <utility>
using namespace std;

#include "Inputs.h"
#include "FieldMap.h"
#include "divHisto.h"

#include "Funcs.h"

#include "showXYscan.h"
#include "showBvsZ.h"
#include "sliceMag.h"
#include "sliceDiv.h"
#include "sliceVec.h"
#include "sliceBz.h"
#include "BdlHisto.h"
#include "mapChecker.h"
#include "findOffs.h"
#include "plotOverlapDiff.h"
#include "mapMerger.h"





int main (/*int argc, char* const argv[]*/) { try{
#ifndef raBOUND
    cout << "ra< > has NO bounds checking" << endl << endl;
#endif




//==============================================================================
//==============================================================================
//==============================================================================
// The plot routines which show one slice for each z want one field map each.
FieldMap* fieldMap;
FieldMap* cnMap;        FieldMap* upMap;        FieldMap* dnMap;
ra<double> off(3);

if (whichMagnet==CMSM) {
    cnMap = new FieldMap(dirName+CMSMfiles[central],FieldMap::CMSMdata);
    off.fillFromC(CMSMcentralOff,3);
    cnMap->setOffset(off);
    upMap = new FieldMap(dirName+CMSMfiles[upstream],FieldMap::CMSMdata);
    off.fillFromC(CMSMupInitOff,3);
    upMap->setOffset(off);
    dnMap = new FieldMap(dirName+CMSMfiles[downstream],FieldMap::CMSMdata);
    off.fillFromC(CMSMdownInitOff,3);
    dnMap->setOffset(off);
} else {
    cnMap = new FieldMap(dirName+SABRfiles[central],FieldMap::SABRdata);
    off.fillFromC(SABRcentralOff,3);
    cnMap->setOffset(off);
    upMap = new FieldMap(dirName+SABRfiles[upstream],FieldMap::SABRdata);
    off.fillFromC(SABRupInitOff,3);
    upMap->setOffset(off);
    dnMap = new FieldMap(dirName+SABRfiles[downstream],FieldMap::SABRdata);
    off.fillFromC(SABRdownInitOff,3);
    dnMap->setOffset(off);
}



switch (showThis) {
case central    :
    fieldMap = cnMap;
    break;
case upstream   :
    fieldMap = upMap;
    break;
case downstream   :
    fieldMap = dnMap;
}



// Opens & plot (x,y) distribution of scans in 3 maps
if (showXYscan) showXYscans::plotXYscanPoints(cnMap, upMap, dnMap);



// Opens & plot distribution of divergence for all 3 field maps
if (showDivHisto) divHists::plotDivergenceHistograms(cnMap, upMap, dnMap);



// Plot the field vs z at given (x,y)
if (showBxyzVsZ)    showBvsZ::plotFieldvsZ(fieldMap);


// Show the magnitude of the field in slices of z
if (showSliceMag) sliceMag::plotMagnitudeSlices(fieldMap);



// Plot the divergence of the field in slices of z
if (showSliceDiv) sliceDiv::plotDivergenceSlices(fieldMap);



// Plot the field as a (2-d) vector in slices of z
if (showSliceVec) sliceVec::plotVectorSlices(fieldMap);



// Plot Bz field in slices of z
if (showSliceBz) sliceBz::plotBzSlices(fieldMap);



// Plot the integral of B dot dL
if (showIntegrals) BdlHisto::plotBdlIntegral(fieldMap);



// Check the quality of the interpolation
if (checkUp) {
        mapChecker bounced(cnMap);
        bounced.checkMap();
}
if (checkCn) {
        mapChecker bounced(upMap);
        bounced.checkMap();
}
if (checkDn) {
        mapChecker bounced(dnMap);
        bounced.checkMap();
}



// Determine the optimal offsets for matching fringe to central maps.
// findOffs::findOffsets(...) not only finds the offsets, it sets them
// for the up/downstream map.  The offsets are the return value too,
// which isn't used here.  But you do need the offsets for any stitching
if (fitUpstream || makeSingleMap || plotDiffUp)   {
    findOffs::findOffsets(cnMap, upMap);
}
if (fitDownstream || makeSingleMap || plotDiffDown) {
    findOffs::findOffsets(cnMap, dnMap);
}



ra<double> overlapZ(4);
overlapZ(upStart)   = -( cnMap->getOffset()(z) ) +cnMap->zLow();
overlapZ(upStop)    = -( upMap->getOffset()(z) ) +upMap->zHigh();
overlapZ(downStart) = -( dnMap->getOffset()(z) ) +dnMap->zLow();
overlapZ(downStop)  = -( cnMap->getOffset()(z) ) +cnMap->zHigh();



if (plotDiffUp)   plotOverlap::plotOverlapDiff(upMap,cnMap,
                                               overlapZ(upStart),overlapZ(upStop), 5);
if (plotDiffDown) plotOverlap::plotOverlapDiff(cnMap,dnMap,
                                               overlapZ(downStart),overlapZ(downStop), 5);



string mapOnDesktop = (whichMagnet==CMSM)   ?
    CMSMoutMapOnDesktop                     :
    SABRoutMapOnDesktop                     ;

if (makeSingleMap) {
    mapMerger scotchTape(upMap,cnMap,dnMap, &overlapZ,
                         headerFileName, mapOnDesktop);

    scotchTape.mergeMaps(copySpacing, halfSpanX,halfSpanY,
                         lowestZout,highestZout,
                         stepX,stepY,stepZ);
}

if (plotIntegral || plotStitchedBvsZ || checkFull) {
    // Plot the integral B dl
    FieldMap* fullMap = new FieldMap(mapOnDesktop,FieldMap::RecoFormat);
    if (plotIntegral) BdlHisto::plotBdlIntegral(fullMap);

    // Probably should check it, eh?  Well it's not THAT stringent a test.
    mapChecker bounced(fullMap);
    if (checkFull) bounced.checkMap();

    // plot B vs Z, perhaps
    if (plotStitchedBvsZ) showBvsZ::plotFieldvsZ(fullMap);
}





//==============================================================================
//==============================================================================
//==============================================================================
// Catch exceptions by reference!
}catch(LppExcept& anLppException){	cout << anLppException.what() << endl;}
 catch(exception& aCppException) {	cout << aCppException.what() << endl;}
}
