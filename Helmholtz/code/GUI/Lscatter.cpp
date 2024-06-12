/*  L++ Lscatter
 *  Created by Leo Bellantoni on 1/19/14.
 *  Copyright 2014 Leo Bellantoni. All rights reserved.
 *
 *  Class for scatter plots made with GNUplot
 *
 */
#include "Lscatter.h"
using namespace std;


// Constructors
Lscatter::Lscatter() :
    Black_(nullptr),Red_(nullptr),Blue_(nullptr),Curve_(nullptr),
    y2Red(false), y2Blue(false), y2Curve(false),Nexcl(0) 
    {DotSize[Black] = DotSize[Red] = DotSize[Blue] = 1.0;}

Lscatter::Lscatter(Lscatter const& inPlot)
    : Black_(nullptr),Red_(nullptr),Blue_(nullptr),Curve_(nullptr),
    plotfit(inPlot), plotbase(inPlot) {
    *this = inPlot;
}
Lscatter& Lscatter::operator=(Lscatter const& rhs) {
    if (this != &rhs) {
        delete Black_;
        delete Red_;
        delete Blue_;
        delete Curve_;
        if (rhs.Black_) Black_ = new Ldata(*rhs.Black_);
        if (rhs.Red_)   Red_  = new Ldata(*rhs.Red_);
        y2Red = rhs.y2Red;
        if (rhs.Blue_)  Blue_ = new Ldata(*rhs.Blue_);
        y2Blue = rhs.y2Blue;
        if (rhs.Curve_) Curve_= new Ldata(*rhs.Curve_);
        y2Curve = rhs.y2Curve;
        for (int i=0; i<3; ++i) DotSize[i] = rhs.DotSize[i];

        Nexcl   = rhs.Nexcl;
        plotfit::operator=(rhs);
        plotbase::operator=(rhs);
    }
    return *this;
}

Lscatter::~Lscatter() {
    delete Black_;      delete Red_;
    delete Blue_;       delete Curve_;
    return;
}



// Constructors for the masses
Lscatter::Lscatter(Ldata const& inData) :
    Red_(nullptr), Blue_(nullptr), Curve_(nullptr), y2Red(false), y2Blue(false),
    y2Curve(false), Nexcl(0) {
    DotSize[Black] = DotSize[Red] = DotSize[Blue] = 1.0;
    Black_  = new Ldata(inData);
}


// Add a data series
void Lscatter::AddSeries(Lscatter::ScatCol col, Ldata inData) {
    switch (col) {
    case Red:       Red_   = new Ldata(inData);        return;
    case Blue:      Blue_  = new Ldata(inData);        return;
    case Black:     Black_ = new Ldata(inData);        return;
    case Curve:     Curve_ = new Ldata(inData);        return;
    }
}


// Fitting a function - least squares fit only implemented so far.
double Lscatter::operator()(ra<double>* param) {
    double retval = 0;
    LdataPointItr itr = Black_->begin();
    for (; itr != Black_->end(); ++itr) {
        if (itr->inFit) {
            if (itr->dy <=0) {
                LppUrk.LppIssue(307,"Lscatter::operator()");
            }
            double WTF = FitFunc(param,itr->x);
            retval += xp( (itr->y -WTF) / itr->dy, 2);
        }
    }
    return retval;
}

void Lscatter::InitFit(double (*inFitFunc)(ra<double>* param, double x), ra<double> start) {
    if (Curve_ || y2Red || y2Blue) {
        // Is fatal to fit function to Black_ in presence of Curve_ or if
        // plotting Red_ or Blue_ on 2nd y axis which is needed for residual
        LppUrk.LppIssue(308,"Lscatter::InitFit");
    }
    fitr    = new fitter( (*this), &start );
    FitFunc = inFitFunc;
    minum_  = new ra<double>(start);

    Black_->Sort();
    
    // Create initial Curve_ to show the starting status of the fit
    Curve_ = new Ldata();
    // Step through the fitted & sorted data points to get x values for Curve_
    Ldata::Point pushme;
    LdataPointItr itr = Black_->begin();
    for (; itr != Black_->end(); ++itr) {
        if (itr->inFit) {
            pushme.x = itr->x;
            pushme.y = FitFunc(minum_,itr->x);
            Curve_->push(pushme);
        }
    }
    return;
}

void Lscatter::RunFit(bool trySIMPLEX) {
    if (!fitr) {
        LppUrk.LppIssue(301,"Lscatter::RunFit(bool)");
    }
    int Ndim = fitr->Ndfree();
    ra<double> answer(Ndim);        ra<double> saveAnswer(Ndim);
    ra<double> errors(Ndim,Ndim);
    ra<double> step(Ndim);

    if (trySIMPLEX) {
        step = (*minum_);
        step /= 20.0;
        fitr->SIMPLEX(step);
        answer = fitr->getMinimum();
        saveAnswer = answer;
    }

    try {
        fitr->gradient();
        answer = fitr->getMinimum();
        saveAnswer = answer;
    } catch (LppExcept& anLppException){
        cout << anLppException.what() << endl;
        errors = 0;
        answer = saveAnswer;
    }
        
    try {
        fitr->hesse();
        errors = fitr->getCovariance();
        answer = fitr->getMinimum();
    } catch (LppExcept& anLppException){
        cout << anLppException.what() << endl;
        errors = 0;
        answer = saveAnswer;
    }
    minum_ = new ra<double>(answer);
    covar_ = new ra<double>(errors);
    
    // RE-create Curve_ to show the results of the fit
    Curve_ -> clear();
    // Step through the fitted & sorted data points to get x values for Curve_
    LdataPointItr itr = Black_->begin();
    Ldata::Point pushme;
    for (; itr != Black_->end(); ++itr) {
        if (itr->inFit) {
            pushme.x = itr->x;
            pushme.y = FitFunc(minum_,itr->x);
            Curve_->push(pushme);
        }
    }
}

void Lscatter::Exclude(double loExcl, double hiExcl) {
    LdataPointItr itr = Black_->begin();
    for (; itr != Black_->end(); ++itr) {
        if (loExcl <= itr->x  &&  itr->x <= hiExcl) {
            ++Nexcl;
            itr->inFit = false;
        }
    }
    return;
}
void Lscatter::ResetExclude() {
    LdataPointItr itr = Black_->begin();
    for (; itr != Black_->end(); ++itr) {
        itr->inFit = true;
    }
    Nexcl = 0;
    return;
}

double Lscatter::RMSresiduals() {
    meansig residz;
    LdataPointItr itr = Black_->begin();
    for (; itr != Black_->end(); ++itr) {
        if (itr->inFit) {
            residz.push(itr->y -FitFunc(minum_,itr->x));
        }
    }
    return residz.stdev();
}

int Lscatter::NDoF() {
    int usedData = 0;
    LdataPointItr itr = Black_->begin();
    for (; itr != Black_->end(); ++itr) {
        if (itr->inFit) ++usedData;
    }
    return usedData -fitr->Ndfree();
}



void Lscatter::SetDotSize(double ns) {
    DotSize[Black] = DotSize[Red] = DotSize[Blue] = ns;
}
void Lscatter::SetDotSize(double nsBlack, double nsRed, double nsBlue) {
    DotSize[Black] = nsBlack;
    DotSize[Red]   = nsRed;
    DotSize[Blue]  = nsBlue;
}




void Lscatter::trimedges() {
    // A no-op
    return;
}



// The working part
void Lscatter::show() {
    plotbase::show();
    
    // Get the key setup
    fprintf(LppPlot.ThePipe, "%s\n", "set title \"\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set key default");
    fprintf(LppPlot.ThePipe, "%s\n", "set key horiz out bottom center");
    fprintf(LppPlot.ThePipe, "%s\n", "set key font \"Times,13\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set key autotitle columnheader");
    fprintf(LppPlot.ThePipe, "%s\n", "set bars small");
    fflush(LppPlot.ThePipe);


 
    // Must construct a temporary datafile, getting x, y & y2 range estimates as you go
    ra<double> corners(6);
    corners = maketempfile();

    // GNUPLOT autoranging on scatter plots is poor
    if (!XlowAuto)  corners(sXlo) = XlowLim;
    if (!YlowAuto)  corners(sYlo) = YlowLim;
    if (!XhighAuto) corners(sXhi) = XhighLim;
    if (!YhighAuto) corners(sYhi) = YhighLim;
    if (!ZhighAuto) corners(s2hi) = ZhighLim;
    if (!ZhighAuto) corners(s2hi) = ZhighLim;
    if (logX && corners(sXlo)<=0) LppUrk.LppIssue(303, "Lscatter::show() [1]");
    if (logY && corners(sYlo)<=0) LppUrk.LppIssue(303, "Lscatter::show() [2]");
    if (logZ && corners(s2lo)<=0) LppUrk.LppIssue(303, "Lscatter::show() [3]");
    fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set  xrange[",corners(sXlo),":",corners(sXhi),"] writeback");
    fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set  yrange[",corners(sYlo),":",corners(sYhi),"]");
    fflush(LppPlot.ThePipe);

    // No title in scatter plot; in other plot types, the title is in the data
    // structure, but Lscatter has 3 different Ldata in it.

    fprintf(LppPlot.ThePipe, "%s\n", "unset y2tics");
    //fprintf(LppPlot.ThePipe, "%s\n", "set ytics nomirror");   
    if (y2Red || y2Blue || y2Curve) {
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set y2range[",corners(s2lo),":",corners(s2hi),"]");
    } else {
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set y2range[",corners(sYlo),":",corners(sYhi),"]");
    }
    fprintf(LppPlot.ThePipe, "%s\n", "set xlabel \"\"");
    fflush(LppPlot.ThePipe);



    char stats[40];
    if (covar_) {
        fprintf(LppPlot.ThePipe, "%s\n", "set multiplot");
        fprintf(LppPlot.ThePipe, "%s\n", "set bmargin at screen 0.30");
        fprintf(LppPlot.ThePipe, "%s\n", "set lmargin at screen 0.05");
        fprintf(LppPlot.ThePipe, "%s\n", "set rmargin at screen 0.95");
        fprintf(LppPlot.ThePipe, "%s\n", "unset xtics");
        // Put the stats on the screen too.
        if (fitr->forcedPositive) {
            sprintf(stats,"%s","Dubious uncertainties stifled");
        } else {
            double CL = pVal();
            sprintf(stats,"%s%g","Fit C.L. ",CL);
        }
        addText(stats, 0.08, 0.88, 101);
        fflush(LppPlot.ThePipe);
    }

    // Marker & color codes for x11 can be found with gnuplot's (interactive) test command
    // Finally, construct the complicated plot command!
    string tmp = "plot \"/tmp/LscatBlack.dat\" binary format=\"%4f\" with xyerr ti \"";
    ostringstream stupid[3];
    tmp += Black_->info + "\" pt 5 lc -1 ps ";
    stupid[Black] << DotSize[Black];
    tmp += stupid[Black].str();
    if (Red_) {
        tmp += ",";
        tmp += " \"/tmp/LscatRed.dat\" binary format=\"%4f\" with xyerr ti \"";
        tmp += Red_->info + "\" pt 5 lc 15 ps ";
        stupid[Red] << DotSize[Red];
        tmp += stupid[Red].str();
        if (y2Red) tmp += " axes x1y2";
    }
    if (Blue_) {
        tmp += ",";
        tmp += " \"/tmp/LscatBlue.dat\" binary format=\"%4f\" with xyerr ti \"";
        tmp += Blue_->info + "\" pt 11 lc 22 ps ";
        stupid[Blue] << DotSize[Blue];
        tmp += stupid[Blue].str();
        if (y2Blue) tmp += " axes x1y2";
    }
    if (Curve_) {
        tmp += ",";
        tmp += " \"/tmp/LscatCurve.dat\" binary format=\"%4f\" with lines lc 3 lw 2 smoo unique";
        if (y2Curve) tmp += " axes x1y2";

    }
    fprintf(LppPlot.ThePipe, "%s\n", tmp.c_str());
    fflush(LppPlot.ThePipe);
    
    // Now do the ratio window
    if (covar_) {
        fprintf(LppPlot.ThePipe, "%s\n", "set tmargin at screen 0.30");
        fprintf(LppPlot.ThePipe, "%s\n", "set lmargin at screen 0.05");
        fprintf(LppPlot.ThePipe, "%s\n", "set rmargin at screen 0.95");
        //fprintf(LppPlot.ThePipe, "%s\n", "set bmargin -1");
        fprintf(LppPlot.ThePipe, "%s\n", "set bmargin at screen 0.10");
        fprintf(LppPlot.ThePipe, "%s\n", "set title \"\"");
        fprintf(LppPlot.ThePipe, "%s\n", "set xtics font \"Times,14\"");
        fprintf(LppPlot.ThePipe, "%s\n", "unset y2tics");
        fprintf(LppPlot.ThePipe, "%s\n", "unset ytics");
        fprintf(LppPlot.ThePipe, "%s\n", "set ytics font \"Times,14\"");
        fprintf(LppPlot.ThePipe, "%s%g\n", "set pointsize ",DotSize[Black]);
        fprintf(LppPlot.ThePipe, "%s\n", "set bars small");
        fprintf(LppPlot.ThePipe, "%s\n", "set xrange restore");
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set yrange[",corners(s2lo),":",corners(s2hi),"]");
        fprintf(LppPlot.ThePipe, "%s\n", "unset logscale y");
        if (logZ) {
            fprintf(LppPlot.ThePipe, "%s\n", "set logscale 2");
        }
        tmp = "plot \"/tmp/Lscat_dF.dat\" binary format=\"%3f\" with yerr pt 13 lc -1";
        fprintf(LppPlot.ThePipe, "%s\n", tmp.c_str());
        fprintf(LppPlot.ThePipe, "%s\n", "unset multiplot");
        fflush(LppPlot.ThePipe);
    }
    if (mouseClose) {
        fprintf(LppPlot.ThePipe, "%s\n", "pause mouse close");
        fflush(LppPlot.ThePipe);
    }
    sleep(1);
    return;
}


ra<double> Lscatter::maketempfile() {
    FILE* fout=nullptr;
    typedef float REAL32;   // GNUplot wants 32 bit reals in binary files
    REAL32 lineout[4];
    LdataPointItr itr;

    ra<double> retval(6);
    retval(sXlo) = +Dbig;
    retval(sYlo) = +Dbig;
    retval(s2lo) = +Dbig;
    retval(sXhi) = -Dbig;
    retval(sYhi) = -Dbig;
    retval(s2hi) = -Dbig;
    double retmp;

    // First series, which will be there, is Black
    // Delete any existing file via new shell that flushes right now
    system("rm /tmp/LscatBlack.dat");
    char buf1[32] = "/tmp/LscatBlack.dat";
    if ( !(fout = fopen(buf1, "wb")) ) {
        LppUrk.LppIssue(14,"Lscatter::maketempfile() [1]");
    }
    for (itr=Black_->begin(); itr!=Black_->end(); ++itr) {
        lineout[0] = itr-> x;    lineout[1] = itr-> y;
        lineout[2] = itr->dx;    lineout[3] = itr->dy;
        if ( fwrite( lineout, sizeof(REAL32), 4, fout) == 0) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [2]");
        }
        retmp = itr->x - itr->dx; if (retmp < retval(sXlo)) retval(sXlo) = retmp;
        retmp = itr->x + itr->dx; if (retmp > retval(sXhi)) retval(sXhi) = retmp;
        retmp = itr->y - itr->dy; if (retmp < retval(sYlo)) retval(sYlo) = retmp;
        retmp = itr->y + itr->dy; if (retmp > retval(sYhi)) retval(sYhi) = retmp;
    }
    retval(s2lo) = 0.0; retval(s2hi) = 1.0;
    if (EOF == fclose(fout)) {
        LppUrk.LppIssue(14,"Lscatter::maketempfile() [3]");
    }

    
    if (Red_) {
        system("rm /tmp/LscatRed.dat");
        char buf2[32] = "/tmp/LscatRed.dat";
        if ( !(fout = fopen(buf2, "wb")) ) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [4]");
        }
        for (itr=Red_->begin(); itr!=Red_->end(); ++itr) {
            lineout[0] = itr-> x;    lineout[1] = itr-> y;
            lineout[2] = itr->dx;    lineout[3] = itr->dy;
            if ( fwrite( lineout, sizeof(REAL32), 4, fout) == 0) {
                LppUrk.LppIssue(14,"Lscatter::maketempfile() [5]");
            }
            retmp = itr->x - itr->dx; if (retmp < retval(sXlo)) retval(sXlo) = retmp;
            retmp = itr->x + itr->dx; if (retmp > retval(sXhi)) retval(sXhi) = retmp;
            retmp = itr->y - itr->dy; if (y2Red) {
                if (retmp < retval(s2lo)) retval(s2lo) = retmp;
            } else {
                if (retmp < retval(sYlo)) retval(sYlo) = retmp;
            }
            retmp = itr->y + itr->dy;  if (y2Red) {
                if (retmp > retval(s2hi)) retval(s2hi) = retmp;
            } else {
                if (retmp > retval(sYhi)) retval(sYhi) = retmp;
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [6]");
        }
    }
    
    if (Blue_) {
        system("rm /tmp/LscatBlue.dat");
        char buf3[32] = "/tmp/LscatBlue.dat";
        if ( !(fout = fopen(buf3, "wb")) ) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [7]");
        }
        for (itr=Blue_->begin(); itr!=Blue_->end(); ++itr) {
            lineout[0] = itr-> x;    lineout[1] = itr-> y;
            lineout[2] = itr->dx;    lineout[3] = itr->dy;
            if ( fwrite( lineout, sizeof(REAL32), 4, fout) == 0) {
                LppUrk.LppIssue(14,"Lscatter::maketempfile() [8]");
            }
            retmp = itr->x - itr->dx; if (retmp < retval(sXlo)) retval(sXlo) = retmp;
            retmp = itr->x + itr->dx; if (retmp > retval(sXhi)) retval(sXhi) = retmp;
            retmp = itr->y - itr->dy; if (y2Blue) {
                if (retmp < retval(s2lo)) retval(s2lo) = retmp;
            } else {
                if (retmp < retval(sYlo)) retval(sYlo) = retmp;
            }
            retmp = itr->y + itr->dy; if (y2Blue) {
                if (retmp > retval(s2hi)) retval(s2hi) = retmp;
            } else {
                if (retmp > retval(sYhi)) retval(sYhi) = retmp;
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [9]");
        }
    }

    if (Curve_) {
        system("rm /tmp/LscatCurve.dat");
        char buf4[32] = "/tmp/LscatCurve.dat";
        if ( !(fout = fopen(buf4, "wb")) ) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [10]");
        }
        for (itr=Curve_->begin(); itr!=Curve_->end(); ++itr) {
            lineout[0] = itr-> x;    lineout[1] = itr-> y;
            lineout[2] = itr->dx;    lineout[3] = itr->dy;
            if ( fwrite( lineout, sizeof(REAL32), 4, fout) == 0) {
                LppUrk.LppIssue(14,"Lscatter::maketempfile() [11]");
            }
            retmp = itr->x - itr->dx; if (retmp < retval(sXlo)) retval(sXlo) = retmp;
            retmp = itr->x + itr->dx; if (retmp > retval(sXhi)) retval(sXhi) = retmp;
            retmp = itr->y - itr->dy; if (y2Curve) {
                if (retmp < retval(s2lo)) retval(s2lo) = retmp;
            } else {
                if (retmp < retval(sYlo)) retval(sYlo) = retmp;
            }
            retmp = itr->y + itr->dy; if (y2Curve) {
                if (retmp > retval(s2hi)) retval(s2hi) = retmp;
            } else {
                if (retmp > retval(sYhi)) retval(sYhi) = retmp;
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [12]");
        }
    }
    
    if (covar_) {
        // Plot only the residuals.  Skip plotting the fit variation as it
        // involves a big mess for a large set of unordered points rather than
        // as smallish number of sequenced bins.
        system("rm /tmp/Lscat_dF.dat");
        char buf7[32] = "/tmp/Lscat_dF.dat";
        if ( !(fout = fopen(buf7, "wb")) ) {
            LppUrk.LppIssue(14,"Lscatter::maketempfile() [13]");
        }
        for (itr=Black_->begin(); itr!=Black_->end(); ++itr) {
            if (itr->inFit) {    // Residuals only exist for fitrted points!
                lineout[0] = itr-> x;    lineout[1] = itr-> y;
                lineout[1] -= FitFunc(minum_,itr->x);
                lineout[2] = itr->dy;
            
                if ( fwrite( lineout, sizeof(REAL32), 3, fout) == 0) {
                    LppUrk.LppIssue(14,"Lscatter::maketempfile() [14]");
                }
                retmp = lineout[1] -itr->dy;
                if (retmp < retval(s2lo)) retval(s2lo) = retmp;
                retmp = lineout[1] +itr->dy;
                if (retmp > retval(s2hi)) retval(s2hi) = retmp;
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lscatter1::maketempfile() [15]");
        }
    }

    // Add some margins to the corners, but don't go negative on log scale
    double del = 0.10*(retval(sXhi) - retval(sXlo));
    if (del<=0) del = 10.0*FLT_EPSILON;
    if (!logX) retval(sXlo) -= del;         retval(sXhi) += del;
    del = 0.10*(retval(sYhi) - retval(sYlo));
    if (del<=0) del = 10.0*FLT_EPSILON;
    if (!logY) retval(sYlo) -= del;         retval(sYhi) += del;
    del =0.10*(retval(s2hi) - retval(s2lo));
    if (del<=0) del = 10.0*FLT_EPSILON;
    if (!logZ) retval(s2lo) -= del;         retval(s2hi) += del;
    return retval;
}
