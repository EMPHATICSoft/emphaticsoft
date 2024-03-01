/*  L++ BLUE
 *  Created by Leo Bellantoni on 2/5/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A class to implement the Best Linear Unbiased Estimator for a number of
 *  measurements.
 *
 */
#include "BLUE.h"





BLUE::BLUE()
    : values(nullptr),covariances(nullptr),
    blue(0),u_blue(0),chi2(0),Nexp(0) {};
BLUE::BLUE(BLUE const& inBLUE)
    : values(nullptr),covariances(nullptr) {
    *this = inBLUE;
}
BLUE& BLUE::operator=(BLUE const& rhs) {
    if (this != &rhs) {
        delete values;
        delete covariances;
        if (rhs.values)      values = new ra<double>(*rhs.values);
        if (rhs.covariances) covariances = new ra<double>(*rhs.covariances);
    }
    return *this;
};
BLUE::~BLUE() {
    delete values;
    delete covariances;
}






BLUE::BLUE(ra<double> inValues, ra<double> inCovariances) {
    // Check for valid inputs
    if ( (inValues.ne1 != inCovariances.ne1)      ||
         (inValues.ne2 != 0)                    ||
         (inCovariances.ne1 != inCovariances.ne2) ||
         (inCovariances.ne3 != 0) ) {
        LppUrk.LppIssue(6,"BLUE::BLUE(ra<double>,ra<double>) [1]");
    }

    Nexp = inValues.ne1;
    if (Nexp==1) LppUrk.LppIssue(212,"BLUE::BLUE(ra<double>,ra<double>)");

    // Examine the input covariance matrix too
    for (int i=1; i<=Nexp; ++i) {
        for (int j=i+1; j<=Nexp; ++j) {
            double upper = inCovariances(i,j);
            double lower = inCovariances(j,i);
            if ( upper==0 || lower==0 ) {
                // If one side is zero, symmeterize it
                if (upper == 0) {
                    inCovariances(i,j) = lower;
                } else {
                    inCovariances(j,i) = upper;
                }
            } else {
                // Verify that the covariance is symmetric
                if ( upper != lower ) {
                    LppUrk.LppIssue(217,"BLUE::BLUE(ra<double>,ra<double>) [2]");
                }
            }
        }
    }
    
    // Save inputs for later plotting of pulls, if possible.
    values      = new ra<double>(Nexp);
    covariances = new ra<double>(Nexp,Nexp);
    for (int i=1; i<=Nexp; ++i) {
        (*values)(i) = inValues(i);
        for (int j=1; j<=Nexp; ++j) {
            (*covariances)(i,j) = inCovariances(i,j);
        }
    }

    // Put covariance into an Lmat and invert it
    Lmat tmp(inCovariances);
    Lmat CovInv = tmp.inverse();    // If not invertible, will get fatal LppExcept

    // Calculate the alpha vector of the original paper
    Lvec alpha(0.0, Nexp);
    double norm = 0.0;
    for (int i=1; i<=Nexp; ++i) {
        for (int j=1; j<=Nexp; ++j) {
            alpha(i) += CovInv(i,j);
        }
        norm += (alpha(i)).real();  // imaginary part is zero anyway
    }
    for (int i=1; i<=Nexp; ++i) {
        alpha(i) /= norm;
        if (alpha(i).real() <= 0 ) {
            LppUrk.LppIssue(56,"BLUE::BLUE(ra<double>,ra<double>)");
            std::cout << "For measurement number " << i << std::endl;
        }
    }
    
    // Compute the BLUE
    blue = 0.0;
    for (int i=1; i<=Nexp; ++i) {
        blue += ( (alpha(i)).real() )*inValues(i);
    }
    // Uncertainty therein
    u_blue = 0.0;
    for (int i=1; i<=Nexp; ++i) {
        for (int j=1; j<=Nexp; ++j) {
            u_blue += ( (alpha(i)).real() )*( (alpha(j)).real() )*inCovariances(i,j);
        }
    }
    u_blue = sqrt(u_blue);
    // Chi-squared
    chi2 = 0.0;
    for (int i=1; i<=Nexp; ++i) {
        for (int j=1; j<=Nexp; ++j) {
            chi2 += (inValues(i)-blue) * (CovInv(i,j).real()) * (inValues(j)-blue);
        }
    }
}


// Getters for the results of the computation
double BLUE::getEstimate() {return blue;}
double BLUE::getUncertainty() {return u_blue;}
double BLUE::getChi2() {return chi2;}
double BLUE::getCL() {return chiprbQ(chi2,Nexp-1);}


// Make a plot, but only if...
void BLUE::showPulls(bool printPulls) {
    for (int i=1; i<=Nexp; ++i) {
        for (int j=i+1; j<=Nexp; ++j) {
            if ( (*covariances)(i,j) != 0) {
                LppUrk.LppIssue(53, "BLUE::showPulls()");
                break;
            }
        }
    }
    Lbins  bIndigo(Nexp, 0.5, Nexp+0.5, "Measurement No.");
    Lhist1 hIndigo(bIndigo,"Pull");
    for (int i=1; i<=Nexp; ++i) {
        double  y = (*values)(i) - getEstimate();
        double dy = sqrt( (*covariances)(i,i) );
        hIndigo.GetBinBin(i).SetValue(y);      hIndigo.GetBinBin(i).SetError(dy);
            if (printPulls) {
            std::cout << "Measurement " << i << " adds " << xp(fabs(y)/dy,2) <<
                 " to the chi-squared." << std::endl;
        }

    }
    Lfit1  pIndigo(hIndigo);
    pIndigo.XhighAuto = pIndigo.XlowAuto = false;
    pIndigo.XlowLim = 0.5;      pIndigo.XhighLim = Nexp +0.5;
    pIndigo.show();
}

// Get the scale factor for discrepant measurements according to PDG algorithm
double BLUE::PDGscale() {
    for (int i=1; i<=Nexp; ++i) {
        for (int j=i+1; j<=Nexp; ++j) {
            if ( (*covariances)(i,j) != 0) {
                LppUrk.LppIssue(54, "BLUE::PDGscale()");
                break;
            }
        }
    }
    // Are any measurement disallowed for having too large an uncertainty?
    int Nuse = 0;
    ra<bool> useme(Nexp);
    double cutoff = 3 *sqrt(Nexp) *u_blue;
    for (int i=1; i<=Nexp; ++i) {
        double thiserr = sqrt( (*covariances)(i,i) );
        if ( (useme(i) = (thiserr <= cutoff)) ) ++Nuse;
    }
    if (Nuse <= 1) {
        LppUrk.LppIssue(55, "BLUE::PDGscale()");
        return 1.0;
    }
    
    // Recursively, recalculate the uncertainty without these values
    ra<double> useVals(Nuse);     ra<double> useCovs(Nuse,Nuse);        useCovs = 0;
    int Iuse = 1;
    for (int i=1; i<=Nexp; ++i) {
        if (useme(i)) {
            useVals(Iuse)      = (*values)(i);
            useCovs(Iuse,Iuse) = (*covariances)(i,i);
            ++Iuse;
        }
    }
    BLUE used_only(useVals,useCovs);
    return sqrt( used_only.getChi2() / (Nuse-1) );
}

