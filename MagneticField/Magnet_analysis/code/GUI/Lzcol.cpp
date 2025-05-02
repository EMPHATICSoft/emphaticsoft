/*  L++ Lzcol
 *  Created by Leo Bellantoni on 1/11/14.
 *  Copyright 2014 Leo Bellantoni. All rights reserved.
 *
 *  Class for zcol plots made with GNUplot
 *
 */
#include "Lzcol.h"
using namespace std;



// Constructors
Lzcol::Lzcol() :
    isProfile(false), Histo(nullptr), Profile(nullptr) {}

Lzcol::Lzcol(Lzcol const& inPlot)
    : Histo(nullptr), Profile(nullptr),
    plotbase(inPlot) {
    *this = inPlot;
}
Lzcol& Lzcol::operator=(Lzcol const& rhs) {
    if (this != &rhs) {
        delete Histo;
        delete Profile;

        if (rhs.Histo)   Histo   = new Lhist2(*rhs.Histo);
        if (rhs.Profile) Profile = new Lprof2(*rhs.Profile);

        isProfile = rhs.isProfile;
        plotbase::operator=(rhs);
    }
    return *this;
}



// Constructors for the masses
Lzcol::Lzcol(Lhist2 const& inHist) {
    isProfile = false;
    Histo     = new Lhist2(inHist);
    Profile   = nullptr;
}
Lzcol::Lzcol(Lprof2 const& inProf) {
    isProfile = true;
    Histo     = nullptr;
    Profile   = new Lprof2(inProf);
}

// Had to allocate space for the Lhist or Lprof on the heap
Lzcol::~Lzcol() {
    delete Profile;
    delete Histo;
    return;
}



// The working part
Lhist2 Lzcol::GetHisto() {
    if (isProfile) {
        LppUrk.LppIssue(306, "Lzcol::getHisto");
    }
    return *Histo;
}
Lprof2 Lzcol::GetProfile() {
    if (!isProfile) {
        LppUrk.LppIssue(306, "Lzcol::getProfile");
    }
    return *Profile;
}




void Lzcol::trimedges() {
    XlowAuto  = YlowAuto  = XhighAuto = YhighAuto = false;
    if (isProfile) {
        XlowLim  = Profile->GetXbins().LowestEdge();
        XhighLim = Profile->GetXbins().HighestEdge();
        YlowLim  = Profile->GetYbins().LowestEdge();
        YhighLim = Profile->GetYbins().HighestEdge();
    } else {
        XlowLim  = Histo->GetXbins().LowestEdge();
        XhighLim = Histo->GetXbins().HighestEdge();
        YlowLim  = Histo->GetYbins().LowestEdge();
        YhighLim = Histo->GetYbins().HighestEdge();    
    }
    return;
}



void Lzcol::blankEmpties(double blankVal) {
    // To set unfilled bins to 1/2 way between the two so they are white.
    if (isProfile) LppUrk.LppIssue(312, "Lzcol::blankEmpties()");

    if (blankVal==-Dbig) blankVal = (Histo->MaxVal() +Histo->MinVal())/2.0;
    int nXbins = Histo->GetXNbins();            int nYbins = Histo->GetYNbins();
    for (int iXbin=1; iXbin<=nXbins; ++iXbin) {
        for (int iYbin=1; iYbin<=nYbins; ++iYbin) {
            if (Histo->GetBinBin(iXbin,iYbin).virgin) {
                Histo->GetBinBin(iXbin,iYbin).SetValue(blankVal);
                // Lest you want to re-blankEmpties later after some more filling.  (what pun?)
                Histo->GetBinBin(iXbin,iYbin).virgin = true;
            }
        }
    }
    return;
}




void Lzcol::show() {
    plotbase::show();
    fprintf(LppPlot.ThePipe, "%s\n", "set key noautotitle");
    
    // An Lzcol has a title, which is the info field of the Lhist2 or
    // Lprof2 that the Lzcol also has
    string tmp;
    if (isProfile) {
        tmp = Profile->info;
    } else {
        tmp = Histo->info;
    }
    fprintf(LppPlot.ThePipe, "%s%s%s\n", "set title \"", tmp.c_str(),"\"");

    if (isProfile) {
        tmp = Profile->GetXbins().info;
    } else {
        tmp = Histo->GetXbins().info;
    }
    fprintf(LppPlot.ThePipe, "%s%s%s\n", "set xlabel \"", tmp.c_str(), "\" font \"Times,14\"");
    if (isProfile) {
        tmp = Profile->GetYbins().info;
    } else {
        tmp = Histo->GetYbins().info;
    }
    fprintf(LppPlot.ThePipe, "%s%s%s\n", "set ylabel \"", tmp.c_str(), "\" font \"Times,14\"");

    
    if        ( XlowAuto &&  XhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s\n", "set xrange[*:*]");
    } else if ( XlowAuto && !XhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set xrange[*:",XhighLim,"]");
    } else if (!XlowAuto &&  XhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set xrange[",XlowLim,":*]");
    } else {
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set xrange[",XlowLim,":",XhighLim,"]");
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
    if        ( ZlowAuto &&  ZhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s\n", "set cbrange[*:*]");
    } else if ( ZlowAuto && !ZhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set cbrange[*:",ZhighLim,"]");
    } else if (!ZlowAuto &&  ZhighAuto ) {
        fprintf(LppPlot.ThePipe, "%s%g%s\n", "set cbrange[",ZlowLim,":*]");
    } else {
        fprintf(LppPlot.ThePipe, "%s%g%s%g%s\n", "set cbrange[",ZlowLim,":",ZhighLim,"]");
    }
    
    // Type "show colornames" to get all the colors you'd ever want
    fprintf(LppPlot.ThePipe, "%s", "set palette defined ");
    fprintf(LppPlot.ThePipe, "%s\n", "(0.0 0.3 0.3 1.0, 0.5 \"white\", 1.0 \"dark-red\")");
    
    fprintf(LppPlot.ThePipe, "%s\n", "set colorbox");
    // Following command needed in gnuplot 5 - can make senseless error message
    fprintf(LppPlot.ThePipe, "%s\n", "set link y2");
    fprintf(LppPlot.ThePipe, "%s\n", "set pm3d corners2color c1 map");

    fprintf(LppPlot.ThePipe, "%s\n", "unset y2tics");
    //fprintf(LppPlot.ThePipe, "%s\n", "set ytics nomirror");

    Lbins::bin nb;
    if (isProfile) {
        nb = Profile->GetXbins().Nbins();
    } else {
        nb = Histo->GetXbins().Nbins();
    }
    fprintf(LppPlot.ThePipe, "%s%d\n", "set sample ",nb);
    if (isProfile) {
        nb = Profile->GetYbins().Nbins();
    } else {
        nb = Histo->GetYbins().Nbins();
    }
    fprintf(LppPlot.ThePipe, "%s%d\n", "set isosample ",nb);
    
    // Must construct and pipe a plot command, which requires a temporary datafile.
    maketempfile();
    fprintf(LppPlot.ThePipe, "%s\n", "splot \"/tmp/Lzcol.dat\" binary");
    if (mouseClose) fprintf(LppPlot.ThePipe, "%s\n", "pause mouse close");
    fflush(LppPlot.ThePipe);
    sleep(1);
}


void Lzcol::maketempfile() {
    FILE* fout=nullptr;
    typedef float REAL32;   // GNUplot wants 32 bit reals in binary files
    
    // Also, we need to have a value for writing into the temp file
    // on the right and top boundaries that will not be plotted but
    // which also will not blow the cbrange
    double dummy = 0;
    if (isProfile) {
        dummy = ( Profile->MinVal() + Profile->MaxVal() )/2.0;
    } else {
        dummy = ( Histo->MinVal()   + Histo->MaxVal()   )/2.0;
    }

    // Delete any existing file via new shell that flushes right now
    system("rm /tmp/Lzcol.dat");
    char buf[32] = "/tmp/Lzcol.dat";
    if ( !(fout = fopen(buf, "wb")) ) {
        LppUrk.LppIssue(14,"Lzcol::maketempfile() [1]");
    }

    REAL32 length; int lengthint, Nlines;
    if (isProfile) {
        // Number of bins plus 1 for the size of each record plus 1
        // more for extra end point that isn't plotted but needs to
        // be in the data file because of how gnuplot does this
        length = lengthint = Profile->GetXbins().Nbins() +2;
        Nlines = Profile->GetYbins().Nbins();
    } else {
        length = lengthint = Histo->GetXbins().Nbins() +2;
        Nlines = Histo->GetYbins().Nbins();
    }
    // malloc vector long enough to span y
    REAL32* lineout = (REAL32*) malloc(sizeof(REAL32)*lengthint);
    
	// First line is number of bins in y, and those bin centers
    lineout[0] = length-1;
    for (int j=1; j<lengthint-1; ++j) {
        lineout[j] = isProfile ? Profile->GetXbins().LowEdge(j)
                               : Histo->GetXbins().LowEdge(j);
    }
    lineout[lengthint-1] = Profile ? Profile->GetXbins().HighestEdge()
                                   : Histo->GetXbins().HighestEdge();
    if ( fwrite( lineout, sizeof(REAL32), lengthint, fout) == 0) {
        LppUrk.LppIssue(14,"Lzcol::maketempfile() [2]");
    }
    for (int i=1; i<=Nlines; ++i) {
        lineout[0] = isProfile ? Profile->GetYbins().LowEdge(i)
                               : Histo->GetYbins().LowEdge(i);
        for (int j=1; j<lengthint-1; ++j) {
            lineout[j] = isProfile ? Profile->GetBinBin(j,i).GetMean()
                                   : Histo->GetBinBin(j,i).GetValue();
        }
        lineout[lengthint-1] = dummy;
        if ( fwrite( lineout, sizeof(REAL32), lengthint, fout) == 0) {
            LppUrk.LppIssue(14,"Lzcol::maketempfile() [3]");
        }
    }
    // Output a dummy row for the X strip on top of plot that isn't plotted
    lineout[0] = isProfile ? Profile->GetYbins().HighestEdge()
                           : Histo->GetYbins().HighestEdge();
    for (int j=1; j<lengthint; ++j) {
        lineout[j] = dummy;
    }
    if ( fwrite( lineout, sizeof(REAL32), lengthint, fout) == 0) {
        LppUrk.LppIssue(14,"Lzcol::maketempfile() [4]");
    }
    if (EOF == fclose(fout)) {
        LppUrk.LppIssue(14,"Lzcol::maketempfile() [5]");
    }
    Lfree(lineout);
}
