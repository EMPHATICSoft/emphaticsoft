/*  L++ Lbins
 *  Created by Leo Bellantoni on 12/03/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  The bins (axis) of a histogram or profile.
 *
 *  The low edge of a bin is in that bin; the high edge is in the next bin.
 *
 *  For specialization with date, convert date::DayNo() into a double
 *  and use constructor for bins of unequal sizes.
 *
 */
#pragma once



#include "ra.h"



class Lbins{
public:
    // Define overflow, underflow bins and bin type.  Must have O = 0 hahahaha
    // BTW, the underflow bin is the reason why bin numbers are int not addr
    // NaN is overflow.
    static int const U = -1;       static int const O = 0;
    typedef int bin;


     // Default, copy, assignment constructors & destructor
    Lbins();
    Lbins(Lbins const& inBins);
    Lbins& operator=(Lbins const& rhs);
    ~Lbins();

    // Deep equality for Lhisto and Lprof classes
    bool operator!=(Lbins const& rhs) const;
    bool operator==(Lbins const& rhs) const;
    
    // Constructors for the masses
    Lbins(bin Nbins, double lowest, double highest, std::string inInfo="");
    Lbins(ra<double> binarray, std::string inInfo="");     // Be sure binarray has Nbins+1 values

    // Member functions for the masses
    bin    FindBin(double x) const;
    bin    Nbins() const;
    double LowEdge(bin b) const;
    double Width  (bin b) const;
    double Center (bin b) const;
    double LowestEdge() const;
    double HighestEdge() const;
    std::string info;    // Axis title, whatever
    
private:
    bin  Nbins_;
    // "Left" and "right" edges of the axis
    double lowest_;         double highest_;
    // Array to define bin edges if bins are not uniformly sized.
    ra<double> *binarray_;
};
