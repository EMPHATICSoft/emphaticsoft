/*  L++ Lfield
 *  Created by Leo Bellantoni on 3 APr 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Class for 2D vector plots made with GNUplot, using Ldata for input
 */
#pragma once



#include "LppGlobals.h"
#include "LppExcept.h"
#include "ra.h"
#include "plotbase.h"
#include "Ldata.h"



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


    void trimedges();
    void show();
    enum FieldCorn {sXlo=1,sYlo=2,sXhi=3,sYhi=4};



private:
    Ldata* Vector_;

    double arrowScale;
    ra<double> maketempfile();
};
