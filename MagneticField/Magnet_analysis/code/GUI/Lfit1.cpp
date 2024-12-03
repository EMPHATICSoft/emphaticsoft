/*  L++ Lfit1
 *  Created by Leo Bellantoni on 2/17/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  Class for 1D histogram fit plots made with GNUplot
 *
 */
#include "Lfit1.h"
using namespace std;


// Constructors
Lfit1::Lfit1() :
    Data(nullptr), Monte(nullptr), unFit(nullptr),
    BinomialErrs(false), LogLike(false), DotSize(gold) {}

Lfit1::Lfit1(Lfit1 const& inPlot)
    : Data(nullptr),Monte(nullptr),unFit(nullptr),
    plotfit(inPlot), plotbase(inPlot)  {
    *this = inPlot;
}
Lfit1& Lfit1::operator=(Lfit1 const& rhs) {
    if (this != &rhs) {
        delete Data;
        delete Monte;
        delete unFit;
        if (rhs.Data)  Data  = new Lhist1(*rhs.Data);
        if (rhs.Monte) Monte = new Lhist1(*rhs.Monte);
        if (rhs.unFit) unFit = new Lhist1(*rhs.unFit);

        DotSize = rhs.DotSize;
        LogLike = rhs.LogLike;
        BinomialErrs = rhs.BinomialErrs;
        plotfit::operator=(rhs);
        plotbase::operator=(rhs);
    }
    return *this;
}
// Had to allocate space for the Lhist1s on the heap
Lfit1::~Lfit1() {
    delete Data;
    delete unFit;
    delete Monte;
    // Destructors for base class automatically called
    // after above code executes.
}



// Constructor for the masses
Lfit1::Lfit1(Lhist1 const& inData)
    : plotfit(), plotbase() {
    Data    = new Lhist1(inData);
    unFit   = new Lhist1(inData);
    *unFit -= *unFit;
    Monte   = nullptr;
    BinomialErrs = false;
    LogLike = false;
    DotSize = gold;
    return;
}



// Add an MC histogram
void Lfit1::AddMC(Lhist1 const& inMC) {
    if (fitr) {
        LppUrk.LppIssue(302,"Lfit1::AddMC");
    }
    Lbins::bin thisNbins = Data->GetXbins().Nbins();
    Lbins::bin themNbins = (inMC.GetXbins()).Nbins();
    if (thisNbins != themNbins) {
        LppUrk.LppIssue(300,"Lfit1::AddMC");
    }
    Monte = new Lhist1(inMC);
}



// Fitting a function
double Lfit1::operator()(ra<double>* param) {
    double retval = 0;
    double x,F;
    Lbins::bin Nbins = Data->GetXbins().Nbins();
    for (Lbins::bin i=1; i<=Nbins; ++i) {
        x = Data->GetXCenter(i);
        // Don't factor bin width into function (good for lines, bad for
        // Gaussians) here.  Do it in the function definition or something
        F = FitFunc(param,x);
        if (LogLike) {
            // Do a log-likelyhood fit.  Be sure you have -2*ln(likelyhood)
            if (unFit->GetBinBin(i).GetValue() != 0) continue;
            int D = nint(Data->GetBinBin(i).GetValue());
            retval += fishbin(D, F);
        } else {
            // Minimize the chi-squared based on the error bars in the bins
            if (unFit->GetBinBin(i).GetValue() != 0) continue;
            double  D = Data->GetBinBin(i).GetValue();
            double dD = Data->GetBinBin(i).GetError();
            if (dD > 0) retval += SQR( (D-F)/dD );
        }
    }
    return retval;
}



void Lfit1::InitFit(double (*inFitFunc)(ra<double>* param, double x), ra<double> start) {
    if (Monte) {
        LppUrk.LppIssue(302,"Lfit1::InitFit");
    }
    fitr = new fitter( (*this), &start );
    FitFunc = inFitFunc;
    minum_ = new ra<double>(start);
}
void Lfit1::ReinitFit( ra<double> restart) {
    if (!fitr) {
        LppUrk.LppIssue(301,"Lfit1::ReinitFit");
    }
    if (Monte) {
        LppUrk.LppIssue(302,"Lfit1::ReinitFit");
    }
    minum_ = new ra<double>(restart);
    fitr->reset(&restart);
}

void Lfit1::RunFit(bool trySIMPLEX) {
    if (!fitr) {
        LppUrk.LppIssue(301,"Lfit1::RunFit(bool)");
    }
    int Ndim = fitr->Ndall();
    ra<double> answer(Ndim);        ra<double> saveAnswer(Ndim);
    ra<double> errors(Ndim,Ndim);
    ra<double> step(Ndim);

    if (trySIMPLEX) {
        step  = (*minum_);
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
    return;
}



void Lfit1::Exclude(double loExcl, double hiExcl) {
    if (loExcl > hiExcl) {
        LppUrk.LppIssue(305, "Lfit1::Exclude");
    }
    Lbins unFitBins = unFit->GetXbins();
    Lbins::bin loKill = unFitBins.FindBin(loExcl);
    if (loKill == Lbins::U) loKill = 1;
    Lbins::bin hiKill = unFitBins.FindBin(hiExcl);
    if (hiKill == Lbins::O) hiKill = unFitBins.Nbins();
    for (Lbins::bin i = loKill; i <= hiKill; ++i) {
        unFit->GetBinBin(i).Fill();
    }
    // Flag the existence of unfitted bins for maketemp(), called from show()
    unFit->GetBinBin(Lbins::U).Fill();
}
void Lfit1::ResetExclude() {
    *unFit -= *unFit;
}



double Lfit1::RMSresiduals() {
    double x,D,F;
    meansig residz;
    Lbins::bin Nbins = Data->GetXbins().Nbins();
    for (Lbins::bin i=1; i<=Nbins; ++i) {
        if (unFit->GetBinBin(i).GetValue() != 0) continue;
        D = Data->GetBinBin(i).GetValue();
        x = Data->GetXCenter(i);
        F = FitFunc(minum_,x);
        residz.push(D -F);
    }
    return residz.stdev();
}

int Lfit1::NDoF() {
    int usedBins = 0;
    for (Lbins::bin i=1; i<=Data->GetXbins().Nbins(); ++i) {
        if (unFit->GetBinBin(i).GetValue() == 0) ++usedBins;
    }
    return usedBins -fitr->Ndfree();
}



Lhist1 Lfit1::GetData() {
    return *Data;
}
Lhist1 Lfit1::GetMonte() {
    return *Monte;
}



// Only the data dots change size
void Lfit1::SetDotSize(int ns) {
    DotSize = xp(gold,ns);
}



void Lfit1::trimedges() {
    XlowAuto  = XhighAuto = false;
    XlowLim  = Data->GetXbins().LowestEdge();
    XhighLim = Data->GetXbins().HighestEdge();
    return;
}



void Lfit1::show() {
    plotbase::show();
    // I think this is needed for Lfit1 and bad for Lzcol and irrelevant elsewhere
    fprintf(LppPlot.ThePipe, "%s\n", "set y2tics font \"Times,14\"");
    fflush(LppPlot.ThePipe);

    // An Lfit1 has no key but it has a title, which is the info field of the data Lhist1
    fprintf(LppPlot.ThePipe, "%s\n", "set key off");
    string tmp = Data->info;
    fprintf(LppPlot.ThePipe, "%s%s%s\n", "set title \"", tmp.c_str(),"\"");
    // X axis label, likewise - not for main plot if there is a ratio plot
    if (Monte || minum_) {
        fprintf(LppPlot.ThePipe, "%s\n", "set xlabel \"\"");
    } else {
        tmp = Data->GetXbins().info;
        fprintf(LppPlot.ThePipe, "%s%s%s\n", "set xlabel \"", tmp.c_str(), "\" font \"Times,14\"");
    }
    // Data points like in a scatter plot
    fprintf(LppPlot.ThePipe, "%s%g\n", "set pointsize ",DotSize);
    fprintf(LppPlot.ThePipe, "%s\n", "set bars small");
    fprintf(LppPlot.ThePipe, "%s\n", "set lmargin at screen 0.10");
    fprintf(LppPlot.ThePipe, "%s\n", "set rmargin at screen 0.90");
    fprintf(LppPlot.ThePipe, "%s\n", "set xtics nomirror");
    fprintf(LppPlot.ThePipe, "%s\n", "set ytics nomirror");
    fprintf(LppPlot.ThePipe, "%s\n", "set y2tics nomirror");
    
    if        ( XlowAuto &&  XhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s\n", "set xrange[*:*] writeback");
    } else if ( XlowAuto && !XhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set xrange[*:",XhighLim,"] writeback");
    } else if (!XlowAuto &&  XhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set xrange[",XlowLim,":*] writeback");
    } else {
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set xrange[",XlowLim,":",XhighLim,"] writeback");
    }
    if        ( YlowAuto &&  YhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s\n", "set yrange[*:*]");
    } else if ( YlowAuto && !YhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set yrange[*:",YhighLim,"]");
    } else if (!YlowAuto &&  YhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set yrange[",YlowLim,":*]");
    } else {
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set yrange[",YlowLim,":",YhighLim,"]");
    }
    fprintf(LppPlot.ThePipe, "%s\n", "set y2range[*:*]");
    fflush(LppPlot.ThePipe);
    if (Monte || covar_) {
        fprintf(LppPlot.ThePipe, "%s\n", "set multiplot");
        fprintf(LppPlot.ThePipe, "%s\n", "set bmargin at screen 0.33");
    }

    maketempfile();

    // Put the stats on the screen too.
    char stats[40];
    sprintf(stats,"%s%g%s%g","U/O (data) ", Data->GetBinBin(Lbins::U).GetValue(),
            "/", Data->GetBinBin(Lbins::O).GetValue());
    addText(stats, 0.12, 0.88, 101);
    if (Monte) {
        sprintf(stats,"%s%g%s%g","U/O (M.C.) ", Monte->GetBinBin(Lbins::U).GetValue(),
                "/", Monte->GetBinBin(Lbins::O).GetValue());
        addText(stats, 0.12, 0.80, 102);
    }
    if (covar_) {
        if (fitr->forcedPositive) {
            sprintf(stats,"%s","Dubious uncertainties stifled");
        } else {
            double CL = pVal();
            sprintf(stats,"%s%g","Fit C.L. ",CL);
        }
        addText(stats, 0.12, 0.84, 103);
    }
    
    // Construct the plot command for the main window
    tmp = "plot";
    tmp += " \"/tmp/Lfit1_D.dat\" binary format=\"%3f\" with yerr ti \"";
    tmp += Data->info + "\" pt 13 lc -1";
    if (unFit->GetBinBin(Lbins::U).GetValue() != 0) {
        // There are some unfitted bins also
        tmp += ",\"/tmp/Lfit1_X.dat\" binary format=\"%3f\" with yerr pt 12 lc -1";
    }
    if (Monte) {
        tmp += ",\"/tmp/Lfit1_M.dat\" binary format=\"%2f\" with histep lc 7";
    }
    if (minum_) {
        // Plot the function, which might not have been fit yet
        tmp += ",\"/tmp/Lfit1_F.dat\" binary format=\"%2f\" with line lc 6 lw 2 ";
    }
    if (covar_ && !fitr->forcedPositive) {
        // Plot the +/- traces on the fit
        tmp += ",\"/tmp/Lfit1_uF.dat\" binary format=\"%2f\" with line lc 6 lt 0 ";
        tmp += ",\"/tmp/Lfit1_lF.dat\" binary format=\"%2f\" with line lc 6 lt 0 ";
    }
    fprintf(LppPlot.ThePipe, "%s\n", tmp.c_str());
    fflush(LppPlot.ThePipe);
    
    // Now do the ratio window
    if (Monte || covar_) {
        fprintf(LppPlot.ThePipe, "%s\n", "set tmargin at screen 0.27");
        fprintf(LppPlot.ThePipe, "%s\n", "set bmargin -1");
        fprintf(LppPlot.ThePipe, "%s\n", "set title \"\"");
        fprintf(LppPlot.ThePipe, "%s\n", "set xtics font \"Times,14\"");
        fprintf(LppPlot.ThePipe, "%s\n", "unset ytics");
        fprintf(LppPlot.ThePipe, "%s\n", "set y2tics font \"Times,14\"");
        tmp = Data->GetXbins().info;
        fprintf(LppPlot.ThePipe, "%s%s%s\n", "set xlabel \"", tmp.c_str(), "\" font \"Times,14\"");
        fprintf(LppPlot.ThePipe, "%s%g\n", "set pointsize ",DotSize);
        fprintf(LppPlot.ThePipe, "%s\n", "set bars small");
        fprintf(LppPlot.ThePipe, "%s\n", "set xrange restore");
        if        ( ZlowAuto &&  ZhighAuto ) {
            fprintf(LppPlot.ThePipe, "%s\n", "set y2range[*:*]");
        } else if ( ZlowAuto && !ZhighAuto ) {
            fprintf(LppPlot.ThePipe, "%s%g%s\n", "set y2range[*:",ZhighLim,"]");
        } else if (!ZlowAuto &&  ZhighAuto ) {
            fprintf(LppPlot.ThePipe, "%s%g%s\n", "set y2range[",ZlowLim,":*]");
        } else {
            fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set y2range[",ZlowLim,":",ZhighLim,"]");
        }

        fprintf(LppPlot.ThePipe, "%s\n", "unset logscale y2");
        if (logZ) {
            fprintf(LppPlot.ThePipe, "%s\n", "set logscale y2");
        }

        if (Monte) {
            tmp = "plot \"/tmp/Lfit1_dM.dat\" binary format=\"%3f\" with yerr pt 7 lc -1";
        }
        if (covar_) {
            tmp = "plot \"/tmp/Lfit1_dF.dat\" binary format=\"%3f\" with yerr pt 13 lc 6";
        }
        fprintf(LppPlot.ThePipe, "%s\n", tmp.c_str());
        fprintf(LppPlot.ThePipe, "%s\n", "unset multiplot");
    }
    if (mouseClose) fprintf(LppPlot.ThePipe, "%s\n", "pause mouse close");
    fflush(LppPlot.ThePipe);
    sleep(1);
}


void Lfit1::maketempfile() {
    FILE* fout=nullptr;
    typedef float REAL32;   // GNUplot wants 32 bit reals in binary files
    REAL32 lineout[4];
    
    // First Lhist1 or function, which will be there, is Data
    // Delete any existing file via new shell that flushes right now
    system("rm /tmp/Lfit1_D.dat");
    char buf1[32] = "/tmp/Lfit1_D.dat";
    if ( !(fout = fopen(buf1, "wb")) ) {
        LppUrk.LppIssue(14,"Lfit1::maketempfile() [1]");
    }
    double X,Y,dY;
    Lbins::bin Nbins = Data->GetXbins().Nbins();
    for (Lbins::bin i=1; i<=Nbins; ++i) {
        // These are the bins in the fit
        if (unFit->GetBinBin(i).GetValue() != 0) continue;
         X = Data->GetXCenter(i);
         Y = Data->GetBinBin(i).GetValue();
        dY = Data->GetBinBin(i).GetError();
        lineout[0] =  X;    lineout[1] =  Y;    lineout[2] = dY;
        if ( fwrite( lineout, sizeof(REAL32), 3, fout) == 0) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [2]");
        }
    }
    if (EOF == fclose(fout)) {
        LppUrk.LppIssue(14,"Lfit1::maketempfile() [3]");
    }
    
    if (unFit->GetBinBin(Lbins::U).GetValue() != 0) {
        // There are some unfitted bins also
        system("rm /tmp/Lfit1_X.dat");
        char buf2[32] = "/tmp/Lfit1_X.dat";
        if ( !(fout = fopen(buf2, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [4]");
        }
        for (Lbins::bin i=1; i<=Nbins; ++i) {
            if (unFit->GetBinBin(i).GetValue() == 0) continue;
            X = Data->GetXCenter(i);
            Y = Data->GetBinBin(i).GetValue();
            dY = Data->GetBinBin(i).GetError();
            lineout[0] =  X;    lineout[1] =  Y;    lineout[2] = dY;
            if ( fwrite( lineout, sizeof(REAL32), 3, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [5]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [6]");
        }
    }
    
    if (Monte) {
        system("rm /tmp/Lfit1_M.dat");
        char buf3[32] = "/tmp/Lfit1_M.dat";
        if ( !(fout = fopen(buf3, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [7]");
        }
        for (int i=1; i<=Nbins; ++i) {
             X = Monte->GetXCenter(i);
             Y = Monte->GetBinBin(i).GetValue();
            lineout[0] =  X;    lineout[1] =  Y;
            if ( fwrite( lineout, sizeof(REAL32), 2, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [8]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [9]");
        }
        system("rm /tmp/Lfit1_dM.dat");
        char buf4[32] = "/tmp/Lfit1_dM.dat";
        if ( !(fout = fopen(buf4, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [10]");
        }
        for (Lbins::bin i=1; i<=Nbins; ++i) {
            X = Data->GetXCenter(i);
            double N = Data->GetBinBin(i).GetValue();
            double M = Monte->GetBinBin(i).GetValue();
            if (M!=0) {
                Y =  N/M;
                if (BinomialErrs) {
                    dY = sqrt(N*(M-N)/M);
                } else {
                    dY  = Qadd(Data->GetBinBin(i).GetError()/N,
                              Monte->GetBinBin(i).GetError()/M);
                    dY *= Y;
                }
            } else {
                Y = 0;      dY = 0;
            }
            lineout[0] =  X;    lineout[1] =  Y;    lineout[2] =  dY;
            if ( fwrite( lineout, sizeof(REAL32), 3, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [11]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [12]");
        }
    }
    
    if (minum_) {
        //Plot the function, which may or may not have been fitted
        system("rm /tmp/Lfit1_F.dat");
        char buf5[32] = "/tmp/Lfit1_F.dat";
        if ( !(fout = fopen(buf5, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [13]");
        }
        for (Lbins::bin i=1; i<=Nbins; ++i) {
            X        = Data->GetXCenter(i);
            Y        = FitFunc(minum_,X);
            lineout[0] =  X;    lineout[1] =  Y;
            if ( fwrite( lineout, sizeof(REAL32), 2, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [14]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [15]");
        }
    }

    if (covar_) {
        // Plot all the paraphenalia of a fitted function
        // Calculate the uncertainties on the fitted function
        int Ndim = fitr->Ndfree();        Lbins::bin Nbin = Data->GetXbins().Nbins();
        // dFdx is numeric derivative w.r.t. parameter d, at bin b
        ra<double> dFdx(Ndim,Nbin);
        for (int d=1; d<=Ndim; ++d) {
            // Use a variation in amount of stdev to get d(FitFunc)/d(param)
            double save = (*minum_)(d);
            double step = covar(d,d)>=0 ? 0.01*sqrt(covar(d,d)) : 0.0;
            (*minum_)(d) += step;
            if (fitr->inBounds(d,(*minum_)(d)) != 0) {
                double low, high;
                fitr->getBounds(d, low,high);
                (*minum_)(d) = high;
                LppUrk.LppIssue(219,"Lfit1::maketempfile() [1]");
            }
            for (Lbins::bin b=1; b<=Nbin; ++b) {
                double x = Data->GetXCenter(b);
                dFdx(d,b)  = FitFunc(minum_, x);
            }
            (*minum_)(d) -= 2*step;
            if (fitr->inBounds(d,(*minum_)(d)) != 0) {
                double low, high;
                fitr->getBounds(d, low,high);
                (*minum_)(d) = low;
                LppUrk.LppIssue(219,"Lfit1::maketempfile() [2]");
            }
            for (Lbins::bin b=1; b<=Nbin; ++b) {
                double x = Data->GetXCenter(b);
                dFdx(d,b) -= FitFunc(minum_, x);
                dFdx(d,b) /= 2*step;
            }
            (*minum_)(d) = save;
        }
        // Then get total error at each bin
        ra<double> dF(Nbin);
        for (Lbins::bin b=1; b<=Nbin; ++b) {
            dF(b) = 0;
            for (int d=1; d<=Ndim; ++d) {
                for (int D=1; D<=Ndim; ++D) {
                    dF(b) += dFdx(d,b) *dFdx(D,b) *covar(d,D);
                }
            }
            dF(b) = sqrt(dF(b));
        }

        system("rm /tmp/Lfit1_uF.dat");
        char buf5[32] = "/tmp/Lfit1_uF.dat";
        if ( !(fout = fopen(buf5, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [16]");
        }
        for (Lbins::bin i=1; i<=Nbins; ++i) {
             X = Data->GetXCenter(i);
             Y = FitFunc(minum_,X) +dF(i);
            lineout[0] =  X;    lineout[1] =  Y;
            if ( fwrite( lineout, sizeof(REAL32), 2, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [17]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [18]");
        }
        system("rm /tmp/Lfit1_lF.dat");
        char buf6[32] = "/tmp/Lfit1_lF.dat";
        if ( !(fout = fopen(buf6, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [19]");
        }
        for (Lbins::bin i=1; i<=Nbins; ++i) {
            X = Data->GetXCenter(i);
            Y = FitFunc(minum_,X) -dF(i);
            lineout[0] =  X;    lineout[1] =  Y;
            if ( fwrite( lineout, sizeof(REAL32), 2, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [20]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [21]");
        }

        system("rm /tmp/Lfit1_dF.dat");
        char buf7[32] = "/tmp/Lfit1_dF.dat";
        if ( !(fout = fopen(buf7, "wb")) ) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [22]");
        }
        for (Lbins::bin i=1; i<=Nbins; ++i) {
            if (unFit->GetBinBin(i).GetValue() != 0) continue;
             X = Data->GetXCenter(i);
             Y = Data->GetBinBin(i).GetValue() - FitFunc(minum_,X);
            dY = Data->GetBinBin(i).GetError();
            lineout[0] =  X;    lineout[1] =  Y;    lineout[2] =  dY;
            if ( fwrite( lineout, sizeof(REAL32), 3, fout) == 0) {
                LppUrk.LppIssue(14,"Lfit1::maketempfile() [23]");
            }
        }
        if (EOF == fclose(fout)) {
            LppUrk.LppIssue(14,"Lfit1::maketempfile() [24]");
        }
    }
}
