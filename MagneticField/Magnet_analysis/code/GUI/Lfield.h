/*  L++ Lfield
 *  Created by Leo Bellantoni on 3 APr 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Class for 2D vector plots made with GNUplot, using Ldata for input.
 *
 *  The (x,y) postion of the vector is the (x,y) fields in the Ldata::Point
 *  entries and the length & direction of the vector is given by the (dx,dy)
 *  fields.
 */
#pragma once



#include "../general/LppGlobals.h"
#include "../problems/LppExcept.h"
#include "../ra/ra.h"
#include "../GUI/plotbase.h"
#include "../histograms/Ldata.h"



class Lfield : public plotbase {
public:
    // Default, copy, assignment constructors, destructor
    Lfield();
    //
    Lfield(Lfield const& inPlot);
    Lfield& operator=(Lfield const& rhs);
    ~Lfield();



    // Constructor for the masses
    explicit Lfield(Ldata const& inData);

    // Default is 1.0
    void SetArrowLength(double maxLen);
    
    double MinVal();
    double MaxVal();

    void trimedges();
    // Option is to show scale factor rather than length of
    // longest arrow.
    void show(bool showScale=false);
    enum FieldCorn {sXlo=1,sYlo=2,sXhi=3,sYhi=4};



private:
    Ldata* Vector_;

    double arrowScale;
    ra<double> maketempfile();
};
