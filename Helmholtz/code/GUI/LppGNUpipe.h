/*  L++ LppGNUpipe
 *  Created by Leo Bellantoni on 1/11/14.
 *  Copyright 2014 URA. All rights reserved.
 *
 *  An class to open and operate a pipe to GNUplot.  In most cases only a single instance
 *  is created but there is no great need to code up the classical singleton pattern.
 *  However, don't build copy or assignment constructors.
 *
 */
#pragma once



#include <stdio.h>

// Can't just include LppGlobals, as it has to declare an LppGNUpipe!
#include "problems/LppError.h"
extern LppError   LppUrk;



class LppGNUpipe{
public:
	// Synthesized destructor is fine; just specify the default constructor.
    LppGNUpipe();
    
    // Issue a new canvas/plot number
    int NewCanvasNo();

    // Close the pipe
    ~LppGNUpipe();

    // Pipe is public so plotting classes can use it
    FILE* ThePipe;

private:
    int Ncanvases;
    
    // Forbid copy and assignment constructors
    LppGNUpipe& operator=(LppGNUpipe const& rhs);
    LppGNUpipe(LppGNUpipe const& inPipe);

};
