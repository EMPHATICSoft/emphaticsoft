/*  L++ Lfield
 *  Created by Leo Bellantoni on 3 APr 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Class for 2D vector plots made with GNUplot, using Ldata for input
 */
#include "Lfield.h"
//using namespace std;




// Constructors
Lfield::Lfield() :
    Vector_(nullptr), arrowScale(1.0) {}

Lfield::Lfield(Lfield const& inPlot)
    : Vector_(nullptr) {
    *this = inPlot;
}
Lfield& Lfield::operator=(Lfield const& rhs) {
    if (this != &rhs) {
        delete Vector_;
        if (rhs.Vector_) Vector_ = new Ldata(*rhs.Vector_);
        arrowScale = rhs.arrowScale;
        plotbase::operator=(rhs);
    }
    return *this;
}



// Constructors for the masses
Lfield::Lfield(Ldata const& inData) {
    Vector_    = new Ldata(inData);
    arrowScale = 1.0;
}

// Had to allocate space for the Lhist or Lprof on the heap
Lfield::~Lfield() {
    delete Vector_;
}



void Lfield::SetArrowLength(double inScale) {
    arrowScale = inScale;
}



double Lfield::MinVal() {
    double retval = +Dbig;      double temp;
    LdataPointItr itr = Vector_->begin();
    for (; itr!=Vector_->end(); ++itr) {
        temp = Qadd(itr->dx,itr->dy);
        if (temp<retval) retval = temp;
    }
    return retval;
}
double Lfield::MaxVal() {
    double retval = -Dbig;      double temp;
    LdataPointItr itr = Vector_->begin();
    for (; itr!=Vector_->end(); ++itr) {
        temp = Qadd(itr->dx,itr->dy);
        if (temp>retval) retval = temp;
    }
    return retval;
}



void Lfield::trimedges() {
    // A no-op
    return;
}



// The working part
void Lfield::show(bool showScale) {
    plotbase::show();
    
    // Get the key setup
    fprintf(LppPlot.ThePipe, "%s\n", "set title \"\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set key default");
    fprintf(LppPlot.ThePipe, "%s\n", "set key horiz out bottom center");
    fprintf(LppPlot.ThePipe, "%s\n", "set key font \"Times,13\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set key autotitle columnheader");
    // An Lfield has no key but it has a title, which is the info field of the Ldata
    string tmp = Vector_->info;
    fprintf(LppPlot.ThePipe, "%s%s%s\n", "set title \"", tmp.c_str(),"\"");

    fflush(LppPlot.ThePipe);


 
    // Must construct a temporary datafile, getting x & y range estimates as you go
    ra<double> corners(4);
    corners = maketempfile();   // scaleFactor is applied here.

    // GNUPLOT autoranging on scatter plots is poor
    if (!XlowAuto)  corners(sXlo) = XlowLim;
    if (!YlowAuto)  corners(sYlo) = YlowLim;
    if (!XhighAuto) corners(sXhi) = XhighLim;
    if (!YhighAuto) corners(sYhi) = YhighLim;
    if (logX && corners(sXlo)<=0) LppUrk.LppIssue(303, "Lfield::show() [1]");
    if (logY && corners(sYlo)<=0) LppUrk.LppIssue(303, "Lfield::show() [2]");
    fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set  xrange[",corners(sXlo),":",corners(sXhi),"]");
    fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set  yrange[",corners(sYlo),":",corners(sYhi),"]");
    fflush(LppPlot.ThePipe);
 
    fprintf(LppPlot.ThePipe, "%s\n", "set xlabel \"\"");
    fflush(LppPlot.ThePipe);

    char message[60];
    if (showScale) {
        sprintf(message,"%s%g","Arrow scale factor: ", arrowScale);
    } else {
        double maxLen = MaxVal();
        sprintf(message,"%s%g","Longest: ", maxLen);
    }
    addText(message, 0.12, 0.86, 101);



    // Marker & color codes for x11 can be found with gnuplot's (interactive) test command
    // Finally, construct the complicated plot command!
    tmp = "plot \"/tmp/Lvector.dat\" binary format=\"%4f\" with vectors filled lc -1";
    fprintf(LppPlot.ThePipe, "%s\n", tmp.c_str());
    fflush(LppPlot.ThePipe);

    if (mouseClose) {
        fprintf(LppPlot.ThePipe, "%s\n", "pause mouse close");
        fflush(LppPlot.ThePipe);
    }
    sleep(1);
    return;
}


ra<double> Lfield::maketempfile() {
    FILE* fout=nullptr;
    typedef float REAL32;   // GNUplot wants 32 bit reals in binary files
    REAL32 lineout[4];
    LdataPointItr itr;

    ra<double> retval(4);
    retval(sXlo) = +Dbig;
    retval(sYlo) = +Dbig;
    retval(sXhi) = -Dbig;
    retval(sYhi) = -Dbig;
    double retmp;

    // First series, which will be there, is Black
    // Delete any existing file via new shell that flushes right now
    system("rm /tmp/Lvector.dat");
    char buf1[32] = "/tmp/Lvector.dat";
    if ( !(fout = fopen(buf1, "wb")) ) {
        LppUrk.LppIssue(14,"Lfield::maketempfile() [1]");
    }
    for (itr=Vector_->begin(); itr!=Vector_->end(); ++itr) {
        lineout[0] = itr-> x;                   lineout[1] = itr-> y;
        lineout[2] = arrowScale * itr->dx;      lineout[3] = arrowScale * itr->dy;
        if ( fwrite( lineout, sizeof(REAL32), 4, fout) == 0) {
            LppUrk.LppIssue(14,"Lfield::maketempfile() [2]");
        }
        retmp = min(itr->x, itr->x +arrowScale*itr->dx);     if (retmp < retval(sXlo)) retval(sXlo) = retmp;
        retmp = max(itr->x, itr->x +arrowScale*itr->dx);     if (retmp > retval(sXhi)) retval(sXhi) = retmp;
        retmp = min(itr->y, itr->y +arrowScale*itr->dy);     if (retmp < retval(sYlo)) retval(sYlo) = retmp;
        retmp = max(itr->y, itr->y +arrowScale*itr->dy);     if (retmp > retval(sYhi)) retval(sYhi) = retmp;
    }
    if (EOF == fclose(fout)) {
        LppUrk.LppIssue(14,"Lfield::maketempfile() [3]");
    }
    
    // Give yourself a little margin
    double const margin = 0.05;
    double xMargin = margin *(retval(sXhi) -retval(sXlo));
    double yMargin = margin *(retval(sYhi) -retval(sYlo));
    retval(sXhi) += xMargin;        retval(sXlo) -= xMargin;
    retval(sYhi) += yMargin;        retval(sYlo) -= yMargin;

    return retval;
}

