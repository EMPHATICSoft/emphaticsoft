/*  L++ Lpdf
 *  Created by Leo Bellantoni on 4 Jan 2024.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  A lightweight but convenient wrapper to LHAPDF.
 *
 */
#pragma once



class wrappedPDF : public functional {
public:
    wrappedPDF(PDF* inPDF, int flavor, double q2) : pud(inPDF), pid(flavor), qT(q2) {};
    double operator()(ra<double>* X) { return pud->xfxQ2(pid, (*X)(1), qT); };
private:
    PDF* pud;
    int  pid;
    double qT;
};
