/*  L++ plotbase
 *  Created by Leo Bellantoni on 1/11/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  Base class for plots made with GNUplot
 *
 */
#include "plotbase.h"
using namespace std;


// Constructors
plotbase::plotbase() :
    logX(false),logY(false),logZ(false), mouseClose(false),
    // Use GNUplot's autorange by default
    XlowAuto(true),XhighAuto(true), YlowAuto(true),YhighAuto(true), ZlowAuto(true),ZhighAuto(true),
    XlowLim(0.0),  XhighLim(1.0),   YlowLim(0.0),  YhighLim(1.0),   ZlowLim(0.0),  ZhighLim(1.0),
    Xwide(600),Yhigh(450) {
    CanvasNo = LppPlot.NewCanvasNo();
    SetTermSize(Xwide,Yhigh);
}

plotbase::plotbase(plotbase const& inBase) {
    *this = inBase;
}
plotbase& plotbase::operator=(plotbase const& rhs) {
    if (this != &rhs) {
        logX = rhs.logX;     logY = rhs.logY;     logZ = rhs.logZ;
        XlowAuto = rhs.XlowAuto;     XhighAuto = rhs.XhighAuto;
        YlowAuto = rhs.YlowAuto;     YhighAuto = rhs.YhighAuto;
        ZlowAuto = rhs.ZlowAuto;     ZhighAuto = rhs.ZhighAuto;
        XlowLim  = rhs.XlowLim;      XhighLim  = rhs.XhighLim;
        YlowLim  = rhs.YlowLim;      YhighLim  = rhs.YhighLim;
        ZlowLim  = rhs.ZlowLim;      ZhighLim  = rhs.ZhighLim;
        Xwide    = rhs.Xwide;        Yhigh     = rhs.Yhigh;
        
        CanvasNo = LppPlot.NewCanvasNo();   // Want a new canvas

        fflush(LppPlot.ThePipe);
        SetTermSize(Xwide,Yhigh);
    }
    return *this;
}



void plotbase::SetTermSize(int X, int Y) {
    fprintf(LppPlot.ThePipe, "%s%d%s%d, %d\n", "set term x11 title \"L++ plot\" ",
            CanvasNo, " size ", X, Y);
    fprintf(LppPlot.ThePipe, "%s\n", "set term x11 font \"Times,12\" ");
}

void plotbase::addText(string scribz, double x, double y, int tag, int size) {
    fprintf(LppPlot.ThePipe, "%s%d%s%s%s%f, %f %s%d\n", "set label ",tag," \"",
            scribz.c_str(),"\" at screen ",x,y," front font \"Times,",size);
}
void plotbase::clearText(int tag) {
    fprintf(LppPlot.ThePipe, "%s%d\n", "unset label ",tag);
}



void plotbase::show() {
	sleep(plotWaitTime);	// For emphaticgpvms

    fprintf(LppPlot.ThePipe, "%s\n", "set  xtics font \"Times,13\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set  ytics font \"Times,13\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set cbtics font \"Times,13\"");
    if (logX) {
        fprintf(LppPlot.ThePipe, "%s\n", "unset nonlinear x");      // Just guessing here
        fprintf(LppPlot.ThePipe, "%s\n", "unset link x2");          // Just guessing here
        fprintf(LppPlot.ThePipe, "%s\n", "set logscale x");
    } else {
        fprintf(LppPlot.ThePipe, "%s\n", "unset logscale x");
    }
    fflush(LppPlot.ThePipe);
    if (logY) {
        fprintf(LppPlot.ThePipe, "%s\n", "unset nonlinear y");      // Just guessing here
        fprintf(LppPlot.ThePipe, "%s\n", "unset link y2");          // Just guessing here
        fprintf(LppPlot.ThePipe, "%s\n", "set logscale y");
    } else {
        fprintf(LppPlot.ThePipe, "%s\n", "unset logscale y");
    }
    fflush(LppPlot.ThePipe);
    if (logZ) {
        fprintf(LppPlot.ThePipe, "%s\n", "unset nonlinear cb");     // Just guessing here
        fprintf(LppPlot.ThePipe, "%s\n", "unset nonlinear y2");     //
        //fprintf(LppPlot.ThePipe, "%s\n", "unset link cb");        // wild-ass ones at that
        fprintf(LppPlot.ThePipe, "%s\n", "unset link y2");          // Just guessing here
        fprintf(LppPlot.ThePipe, "%s\n", "set logscale cb");
        fprintf(LppPlot.ThePipe, "%s\n", "set logscale y2");
    } else {
        fprintf(LppPlot.ThePipe, "%s\n", "unset logscale cb");
        fprintf(LppPlot.ThePipe, "%s\n", "unset logscale y2");
    }
    fflush(LppPlot.ThePipe);
    
    fprintf(LppPlot.ThePipe, "%s\n", "set title font \"Times,14\"");
    fprintf(LppPlot.ThePipe, "%s\n", "set tmargin -1");
    fprintf(LppPlot.ThePipe, "%s\n", "set bmargin -1");
    fprintf(LppPlot.ThePipe, "%s\n", "set lmargin -1");
    fprintf(LppPlot.ThePipe, "%s\n", "set rmargin -1");
    clearText(101);   clearText(102);   clearText(103);
    fflush(LppPlot.ThePipe);
    return;
}



void plotbase::close() {
    fprintf(LppPlot.ThePipe, "%s%d\n", "set term x11 close ", CanvasNo);
    fflush(LppPlot.ThePipe);
}

