/*  L++ Lbins
 *  Created by Leo Bellantoni on 12/03/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  The bins (axis) of a histogram or profile.
 *
 */
#include "Lbins.h"
using namespace std;

// Basic constructors
Lbins::Lbins() :
    Nbins_(0), lowest_(0.0),highest_(0.0), info(""),
    binarray_(nullptr) {
}
Lbins::Lbins(Lbins const& inBins)
    : binarray_(nullptr){
    *this = inBins;
}
Lbins& Lbins::operator=(Lbins const& rhs) {
    if (this != &rhs) {
        delete binarray_;
        if (rhs.binarray_) binarray_ = new ra<double>(*rhs.binarray_);

        Nbins_   = rhs.Nbins_;
        lowest_  = rhs.lowest_;
        highest_ = rhs.highest_;
        info     = rhs.info;
    }
    return *this;
}

// Destructor because we might have allocated memory for the ra binarray_
Lbins::~Lbins() {
    delete binarray_;
}

// Deep equality operator for Lprof and Lhist classes
bool Lbins::operator!=(const Lbins &rhs) const {
    return (!(*this==rhs));
}
bool Lbins::operator==(const Lbins &rhs) const {
    if (Nbins_   != rhs.Nbins_)    return false;
    if (lowest_  != rhs.lowest_)   return false;
    if (highest_ != rhs.highest_)  return false;
    if (binarray_==nullptr && rhs.binarray_!=nullptr) return false;
    if (binarray_!=nullptr && rhs.binarray_==nullptr) return false;
    if (binarray_ != nullptr) {
        for (bin i=1; i<=binarray_->ne1; ++i) {
            if ((*binarray_)(i) != (*rhs.binarray_)(i)) return false;
        }
    }
    return true;
}


// Constructors for the masses
Lbins::Lbins(bin Nbins, double lowest, double highest, string inInfo) :
    Nbins_(Nbins), lowest_(lowest), highest_(highest),
    info(inInfo), binarray_(nullptr) {
    if (lowest >= highest) LppUrk.LppIssue(0,"Lbins::Lbins(bin,double,double,string)");
}
Lbins::Lbins(ra<double> binarray, string inInfo) {
    if (binarray.ne2> 0) {
        // Better be a 1-dimensional ra!
        LppUrk.LppIssue(7, "Lbins::Lbins(ra<double>)");
    }
    // Force first bin to be numbered 1
    binarray.setbase(1);
    bin in_ne1 = binarray.ne1;
    binarray_ = new ra<double>(in_ne1);
    for (bin i=1; i<=in_ne1; ++i) {
        if (i!=1) {
            // Check sequencing is right
            if (binarray(i) <= binarray(i-1)) {
                LppUrk.LppIssue(11, "Lbins::Lbins(ra<double>)");
            }
        }
        (*binarray_)(i) = binarray(i);
    }
    // Keep info re binarray here, use binarray==nullptr to test for
    // uniform vs non-uniform binning
    Nbins_   = in_ne1 -1;
    lowest_  = binarray(1);
    highest_ = binarray(in_ne1);
    info     = inInfo;
    return;
}


// Member functions for the masses
Lbins::bin Lbins::FindBin(double x) const {
    bin retval;
    if (x <  lowest_)  return U;
    if (x >= highest_) return O;
    if (binarray_ == nullptr) {
        // All bins the same width
        retval  = bin( (x - lowest_) / Width(1) );
        ++retval;
    } else {
        // Variable bins.  Seems possible to use std::upper_bound on the
        // valarray in the ra, even though valarray has no iterators.
        double* thebin =
            upper_bound(&(binarray_->val[0]),&(binarray_->val[Nbins_]), x);
        --thebin;
        // Now thebin is a pointer to some element of binarray_ (which points
        // into val); but I need to convert that back to an integer.  C/C++
        // standard for pointer arithmetic says don't divide by sizeof(double)
        // Rvalue here is difference of 2 double* which is a long.
        retval = thebin - &(binarray_->val[0]);
        ++retval;
    }
    return retval;
}
Lbins::bin Lbins::Nbins() const { return Nbins_; }
double Lbins::LowEdge(bin b) const {
    if ( b < 1 ) {
        LppUrk.LppIssue(12,"Lbins::LowEdge(bin) underflow");
    }
    if ( b > Nbins_ ) {
        // Strictly speaking, one COULD return highest_
        LppUrk.LppIssue(12,"Lbins::LowEdge(bin) overflow");
    }
    if (binarray_ == nullptr) {
        // All bins the same width
        return lowest_ + (b-1)*Width(1);
    } else {
        // Variable bins
        return (*binarray_)(b);
    }
}
double Lbins::Width(bin b) const {
    if (binarray_ == nullptr) {
        // All bins the same width, need not check b
        return (highest_ - lowest_) / Nbins_;
    }
    // Variable bins
    if ( b < 1 ) {
        LppUrk.LppIssue(12,"Lbins::Width(bin) underflow");
    }
    if ( b > Nbins_ ) {
        LppUrk.LppIssue(12,"Lbins::Width(bin) overflow");
    }
    if (b < Nbins_) {
        return (*binarray_)(b+1) - (*binarray_)(b);
    } else {
        return   highest_        - (*binarray_)(b);
    }
}
double Lbins::Center(bin b) const {
    if ( b < 1 ) {
        LppUrk.LppIssue(12,"Lbins::Center(bin) underflow");
    }
    if ( b > Nbins_ ) {
        LppUrk.LppIssue(12,"Lbins::Center(bin) underflow");
    }
    return LowEdge(b) + Width(b)/2.0;
}
double Lbins::LowestEdge() const {
    return lowest_;
}
double Lbins::HighestEdge() const {
    return highest_;
}
