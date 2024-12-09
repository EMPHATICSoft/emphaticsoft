/*  L++ plotbase
 *  Created by Leo Bellantoni on 1/11/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  Base class for plots made with GNUplot
 */
#pragma once



#include <string>
#include <unistd.h>     // for sleep

#include "LppGlobals.h"



class plotbase {
public:
    // Default constructor must correctly fetch plot number
    // Assignment operator below
    plotbase();
    plotbase(plotbase const& inBase);
    
    
    void SetTermSize(int X,int Y);  // Default is 600 x 450

    // Virtual destructor, like other virtual methods, need to be defined
    // Like other destructors it better not throw anything!  In this case, all
    // the data members of this class are primitives and self-destruct.
    virtual ~plotbase() {}
    
    // Many setters and getters?  Nah!  Just make data members public!
    bool logX,logY,logZ;
    bool mouseClose;

    bool  XlowAuto,XhighAuto, YlowAuto,YhighAuto, ZlowAuto,ZhighAuto;
    double XlowLim,XhighLim,  YlowLim,YhighLim,   ZlowLim,ZhighLim;

    // (x,y) coordinate system goes from (0,0) at lower left of entire screen -
    // not just the region containing the plot to (1,1) at upper right.  Tag is
    // an integer between 1 and 100 to number the text being added.
    void addText(std::string scribz, double x, double y, int tag, int size=14);
    void clearText(int tag);

    // Set ranges from histobins
    virtual void trimedges(){};
    virtual void show();
    // Is this really useful?
    void close();

protected:
    int Xwide,Yhigh;
    int CanvasNo;

    int  const   plotWaitTime    = 30;		// OK let's try 30 seconds instead.

    // Assignment operator not for mortal man
    plotbase& operator=(plotbase const& rhs);
};
