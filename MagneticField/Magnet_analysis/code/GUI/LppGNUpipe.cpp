/*  L++ LppGNUplot
 *  Created by Leo Bellantoni on 1/11/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  An class to open and operate a pipe to GNUplot.  In most cases only a single instance
 *  is created but there is no great need to code up the classical singleton pattern.
 *
 */
#include "LppGNUpipe.h"
#include "LppGlobals.h"
using namespace std;


// Basic constructor only
LppGNUpipe::LppGNUpipe() :
    Ncanvases(0) {
    // Uses lower-level C constructs rather than C++ idea because development is speeded
    // up by taking ideas from N. Devillard's gnuplot_i.h.  See below re. LANG
    ThePipe = popen("export LANG='en_US.UTF-16'; /usr/bin/gnuplot --persist","w");
    if (!ThePipe) {
        LppUrk.LppIssue(304,"LppGNUplot::LppGNUlot()");
    }
    
    
    // Previously had set LANG to UTF-8; that gave wonky numbers when using
    // sci notation in scales.  But it got rid of the iconv->degree sign
    // error.  This statement and UTF-16 seems to fix both problems.
    fprintf(ThePipe, "%s\n",   "set locale \"en_US.UTF-8\" ");
    fprintf(ThePipe, "%s%e\n", "set zero ", 1000.0*Deps);
    fprintf(ThePipe, "%s\n",   "set angles degrees");
}


// A destructor needed to free space
LppGNUpipe::~LppGNUpipe() {
    pclose(ThePipe);
}

int LppGNUpipe::NewCanvasNo(){
    return ++Ncanvases;
}

