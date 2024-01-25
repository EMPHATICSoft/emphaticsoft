//
//  interpolate.cpp
//  L++
//
//  Created by Leo Bellantoni on 23 May 2023.
//  This version modified to use the RA3 classes
//

#include "../functions/quadd.h"
#include "../CodeGuts/poly3int.h"
#include "../CodeGuts/Funcs.h"
#include "../CodeGuts/FieldMap.h"





bool FieldMap::interpolate(ra<double>& Xi, ra<double>& B) const {
    // Interpolate a 3 d quadratic with cross terms.
    FieldMapInd closestInd;
    if (!insideMap(Xi,closestInd)) return false;

    // 1st index is x direction, then y & z.  Create interpolator & set its grid
    poly3int BinterpX,BinterpY,BinterpZ;
    RA_1<double> Xgrid;        RA_1<double> Ygrid;        RA_1<double> Zgrid;
    FieldMapInd indL, indN, indTmp;     ra<double> tempX(3);

    // Assume that the map is not so small that a scan in z is two opposite edges.
    // A corner would be on 2 edges though.  N.B. these next few lines do NOT change closestInd.
    bool leftEdge   = !findLastX(closestInd,indTmp);    bool rightEdge = !findNextX(closestInd, indTmp);
    bool bottomEdge = !findLastY(closestInd,indTmp);    bool topEdge   = !findNextY(closestInd, indTmp);
    // The points on the next diagonal row in the corners need 2 side-steps also
    if (!leftEdge && !rightEdge && !bottomEdge && !topEdge) {
        // Cast to void in order to stifle compiler warning.
        if ((void)findLastX(closestInd,indTmp), !findLastY(indTmp,indTmp)) leftEdge  = bottomEdge = true;
        if ((void)findNextX(closestInd,indTmp), !findLastY(indTmp,indTmp)) rightEdge = bottomEdge = true;
        if ((void)findLastX(closestInd,indTmp), !findNextY(indTmp,indTmp)) leftEdge  = topEdge    = true;
        if ((void)findNextX(closestInd,indTmp), !findNextY(indTmp,indTmp)) rightEdge = topEdge    = true;
    }


    if (leftEdge) {
        // If on low-X edge, get the next index up (better be there!) & slide
        findNextX(closestInd, closestInd);
    } else if (rightEdge) {
        // Similarly at high-X edge
        findLastX(closestInd, closestInd);
    }
    findLastX(closestInd,indL);         findNextX(closestInd,indN);
    findX(tempX,indL);          Xgrid(1) = tempX(x);
    findX(tempX,closestInd);    Xgrid(2) = tempX(x);
    findX(tempX,indN);          Xgrid(3) = tempX(x);

    // By here, closestInd may have been shifted in X; but we remember if we need
    // to also shift in Y at a corner from earlier
    if (bottomEdge) {
        findNextY(closestInd, closestInd);
    } else if (topEdge) {
        findLastY(closestInd, closestInd);
    }
    findLastY(closestInd,indL);         findNextY(closestInd,indN);
    findX(tempX,indL);          Ygrid(1) = tempX(y);
    findX(tempX,closestInd);    Ygrid(2) = tempX(y);
    findX(tempX,indN);          Ygrid(3) = tempX(y);
    
    bool foundLast = findLastZ(closestInd,indL);        bool foundNext = findNextZ(closestInd, indN);
    if (!foundLast && foundNext) {
        findNextZ(closestInd, closestInd);
    } else if (foundLast && !foundNext) {
        findLastZ(closestInd, closestInd);
    }
    findLastZ(closestInd,indL);         findNextZ(closestInd,indN);
    findX(tempX,indL);          Zgrid(1) = tempX(z);
    findX(tempX,closestInd);    Zgrid(2) = tempX(z);
    findX(tempX,indN);          Zgrid(3) = tempX(z);

    BinterpX.setUpGrid(Xgrid, Ygrid, Zgrid);
    BinterpY.setUpGrid(Xgrid, Ygrid, Zgrid);
    BinterpZ.setUpGrid(Xgrid, Ygrid, Zgrid);


    RA_3<double> Bx;   RA_3<double> By;       RA_3<double> Bz;
    B = 0;          // Invalid result if can't interpolate
    FieldMapInd tmp;        FieldMapItr itr;
    // N. B. at this point, closestInd may have been shifted in X, Y or Z
    for (int jX=-1; jX<=+1; ++jX) {
        tmp.iX = closestInd.iX +jX;
        for (int jY=-1; jY<=+1; ++jY) {
            tmp.iY = closestInd.iY +jY;
            for (int jZ=-1; jZ<=+1; ++jZ) {
                tmp.iZ = closestInd.iZ +jZ;
                if (!findItr(tmp, itr) || !itr->valid) return false;
                Bx(jX+2,jY+2,jZ+2) = itr->Bx;
                By(jX+2,jY+2,jZ+2) = itr->By;
                Bz(jX+2,jY+2,jZ+2) = itr->Bz;
            }
        }
    }
    
    BinterpX.fillValues(Bx);    BinterpY.fillValues(By);    BinterpZ.fillValues(Bz);

    B(x) = BinterpX.eval(Xi);   B(y) = BinterpY.eval(Xi);   B(z) = BinterpZ.eval(Xi);

    return true;
}
