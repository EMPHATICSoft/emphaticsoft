/*  L++ Ldata
 *  Created by Leo Bellantoni on 1/10/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  A set of data points for scatter plots.
 *
 */
#include "Ldata.h"
using namespace std;

// Basic constructors
Ldata::Ldata() : info("") {}
Ldata::Ldata(Ldata const& inData) {
    *this = inData;
}
Ldata& Ldata::operator=(Ldata const& rhs) {
    if (this != &rhs) {
        Points = rhs.Points;
        info   = rhs.info;
    }
    return *this;
}



// User interface
Ldata::Ldata(string inInfo) :
    info(inInfo) {}

// Deque operations for manipulating the points of an Ldata
void Ldata::push(Point datum) {
    Points.push_back(datum);
}
void Ldata::erase(LdataPointItr ptr) {
    Points.erase(ptr);
}
void Ldata::clear() {
    Points.clear();
}
Ldata::Point& Ldata::operator[] (addr i) {
    #ifdef raBOUND
    return Points.at(i);
    #endif
    #ifndef raBOUND
    return Points[i];
    #endif
}

addr Ldata::size() const {
    return Points.size();
}
LdataPointItr Ldata::begin() {
    return Points.begin();
}
LdataPointItr Ldata::end() {
    return Points.end();
}

// Finding and scaling data
LdataPointItr Ldata::find(double x) {
    // Easier to write the loop than the predicate ;)
    LdataPointItr itr;
    for (itr=Points.begin(); itr!=Points.end(); ++itr) {
        if (itr->x == x) return itr;
    }
    return Points.end();  // Check this for fail-to-find
}
LdataPointItr Ldata::find(double x, double y) {
    LdataPointItr itr;
    for (itr=Points.begin(); itr!=Points.end(); ++itr) {
        if ( itr->x == x && itr->y == y) return itr;
    }
    return Points.end();}
void Ldata::Scale(double const& scale) {
    LdataPointItr itr;
    for (itr=Points.begin(); itr!=Points.end(); ++itr) {
        itr->y  *= scale;
        itr->dy *= scale;
    }
    return;
}

// Sort the vector of Points
void Ldata::Sort() {
    std::sort(begin(),end());
}


double Ldata::Wstats(stat s) {
    // Don't save statistics in this object.  Erase/deletes will make them
    // invalid, and you won't know about it.
    ra<double> statistics(10);
    LdataPointItr itr;
    double sW,wX,wXX,wY,wXY,wYY;
    sW = wX =wXX = wY = wXY = wYY = 0;
    for (itr = Points.begin(); itr < Points.end(); ++itr) {
        double w = xp(itr->dy,-2);
        sW  += w;
        wX  += w * itr->x;
        wY  += w * itr->y;
        wXX += w * itr->x * itr ->x;
        wXY += w * itr->x * itr ->y;
        wYY += w * itr->y * itr ->y;
    }
    (statistics)(SumW)  = sW;
    double mX,mY;
    mX = (statistics)(MeanX) = wX / sW;
    mY = (statistics)(MeanY) = wY / sW;
    (statistics)(RmsX)  = sqrt((wXX -mX*mX*sW) / sW);
    (statistics)(RmsY)  = sqrt((wYY -mY*mY*sW) / sW);
    (statistics)(Rho) = ((wXY -mX*wY -mY*wX + mX*mY*sW) / sW)
                         /((statistics)(RmsX) * (statistics)(RmsY));

    double invMat_11, invMat_12, invMat_22, det;
    det = sW*wXX -wX*wX;
    invMat_11 = wXX/det;    invMat_12 = -wX/det;    invMat_22 = sW/det;
    (statistics)(B)  = invMat_11*wY + invMat_12*wXY;
    (statistics)(M)  = invMat_12*wY + invMat_22*wXY;
    (statistics)(dB) = sqrt(invMat_11);
    (statistics)(dM) = sqrt(invMat_22);
    return statistics(s);
}



// CSV output: x, dx, y, dy
void Ldata::CSVout(string filename) {
    string fullname = string(getenv("HOME")) +string("/Desktop");
    if (filename == "") {
        fullname += string("/Ldata");
    } else {
        fullname += "/" + filename;
    }
    fullname += string(".csv");
    
    if (!filehamna(fullname)) {
        cout << fullname << endl;
        LppUrk.LppIssue(13,"Ldata::CSVout(string) [1]");
    }
    
    fstream file(fullname.c_str(),ios_base::out);
    if (file.fail()) {
        cout << filename << endl;
        LppUrk.LppIssue(14,"Ldata::CSVout(string) [2]");
    }
    
    file << info << endl;
    if (file.bad()) {      // Wish they'd created exceptions for << operator by default
        file.close();
        cout << filename << endl;
        LppUrk.LppIssue(14,"Ldata::CSVout(string) [3]");
    }
    
    file << " x,dx,y,dy " << endl;
    if (file.bad()) {      // Wish they'd created exceptions for << operator by default
        file.close();
        cout << filename << endl;
        LppUrk.LppIssue(14,"Ldata::CSVout(string) [4]");
    }
    
    for (LdataPointItr itr=Points.begin(); itr!=Points.end(); ++itr) {
        file << itr->x << "," << itr->dx << "," << itr->y << "," << itr->dy << endl;
        if (file.bad()) {
            file.close();
            cout << filename << endl;
            LppUrk.LppIssue(14,"Ldata::CSVout(string) [5]");
        }
    }
    file.close();
    return;
}
