/*  L++ Lhist1
 *  Created by Leo Bellantoni on 12/16/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 1D histogram.
 *
 */
#include "Lhist1.h"
using namespace std;


// Basic constructors
Lhist1::Lhist1() :
    Xbins(Lbins()), HistArray(nullptr), info("") {
}
Lhist1::Lhist1(Lhist1 const& inHist)
    :HistArray(nullptr), Xbins(Lbins()) {
    *this = inHist;
}
Lhist1& Lhist1::operator=(Lhist1 const& rhs) {
    if (this != &rhs) {
        Xbins = rhs.Xbins;
        delete HistArray;
        if (rhs.HistArray) HistArray  = new ra<Lhistbin> (*rhs.HistArray);
        info = rhs.info;
    }
    return *this;
}

// Destructor because we allocated memory for an ra
Lhist1::~Lhist1() {
    delete HistArray;
}


// Constructor for the masses
Lhist1::Lhist1(Lbins inXbins, string inInfo) :
    info(inInfo) {
    Xbins = inXbins;
    HistArray  = new ra<Lhistbin> (GetXNbins() +2);
    HistArray -> setbase(Lbins::U);
}
Lhist1::Lhist1(Lprof1 inProf, bool PlotRMS) :
    info(inProf.info) {
    Xbins = inProf.GetXbins();
    HistArray  = new ra<Lhistbin> (GetXNbins() +2);
    HistArray -> setbase(Lbins::U);
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        double val = inProf.GetBinBin(i).GetMean();
        (*HistArray)(i).SetValue(val);
        double bar;
        if (PlotRMS) {
            bar = inProf.GetBinBin(i).GetSDev();
        } else {
            bar = inProf.GetBinBin(i).GetErrMean();
        }
        (*HistArray)(i).SetError(bar);
    }
    return;
}

// Mathematical operators built on corresponding operations
// in Lhistbin.  Defined constructors should make it impossible
// to call an operation on an uninitialized Lhist.
Lhist1  Lhist1::operator+ (Lhist1 const& rhs) const {
	Lhist1 retval(*this);	return (retval += rhs);
}
Lhist1& Lhist1::operator+=(Lhist1 const& rhs) {
    if (Xbins!=rhs.Xbins) {
        LppUrk.LppIssue(107,"Lhist1::operator+ or +=(Lhist1)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        (*HistArray)(i) += (*rhs.HistArray)(i);
    }
    return *this;
}
Lhist1  Lhist1::operator- (Lhist1 const& rhs) const {
	Lhist1 retval(*this);	return (retval -= rhs);
}
Lhist1& Lhist1::operator-=(Lhist1 const& rhs) {
    if (Xbins!=rhs.Xbins) {
        LppUrk.LppIssue(107,"Lhist1::operator- or -=(Lhist1)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        (*HistArray)(i) -= (*rhs.HistArray)(i);
    }
    return *this;
}

Lhist1  Lhist1::operator* (Lhist1 const& rhs) const {
	Lhist1 retval(*this);	return (retval *= rhs);
}
Lhist1& Lhist1::operator*=(Lhist1 const& rhs) {
    if (Xbins!=rhs.Xbins) {
        LppUrk.LppIssue(107,"Lhist1::operator* or *=(Lhist1)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        (*HistArray)(i) *= (*rhs.HistArray)(i);
    }
    return *this;
}

void Lhist1::Scale(double const& scale) {
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        ((*HistArray)(i)).Scale(scale);
    }
    return;
}


Lhist1  Lhist1::operator/ (Lhist1 const& rhs) const {
	Lhist1 retval(*this);	return (retval /= rhs);
}
Lhist1& Lhist1::operator/=(Lhist1 const& rhs) {
    if (Xbins!=rhs.Xbins) {
        LppUrk.LppIssue(107,"Lhist1::operator/ or /=(Lhist1)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        if ((*rhs.HistArray)(i).GetValue() == 0.0) {
            // Don't flag over/underflow bins of zero
            if (i!=Lbins::U && i!=Lbins::O) {
                LppUrk.LppIssue(108,"Lhist1::operator/ or /=(Lhist1)");
            }
            (*HistArray)(i).SetValue(0);
            (*HistArray)(i).SetError(0);
        } else {
            (*HistArray)(i) /= (*rhs.HistArray)(i);
        }
    }
    return *this;
}
Lhist1  Lhist1::operator% (Lhist1 const& rhs) const {
	Lhist1 retval(*this);	return (retval %= rhs);
}
Lhist1& Lhist1::operator%=(Lhist1 const& rhs) {
    if (Xbins!=rhs.Xbins) {
        LppUrk.LppIssue(107,"Lhist1::operator% or %=(Lhist1)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        if ((*rhs.HistArray)(i).GetValue() == 0.0) {
            if (i!=Lbins::U && i!=Lbins::O) {
                LppUrk.LppIssue(108,"Lhist1::operator% or %=(Lhist1)");
            }
            (*HistArray)(i).SetValue(0);
            (*HistArray)(i).SetError(0);
        } else {
            (*HistArray)(i) %= (*rhs.HistArray)(i);
        }
    }
    return *this;
}


// Filling
void Lhist1::Fill(double x, double weight) {
    Lbins::bin X = Xbins.FindBin(x);
    ((*HistArray)(X)).Fill(weight);
}


// Accessing bins
Lhistbin& Lhist1::GetBinPos(double x) const {
    Lbins::bin X = Xbins.FindBin(x);
    return GetBinBin(X);
}
Lhistbin& Lhist1::GetBinBin(Lbins::bin Bx) const {
    return (*HistArray)(Bx);
}
Lhistbin Lhist1::GetXunder() const {
    return GetBinBin(Lbins::U);
}
Lhistbin Lhist1::GetXover() const {
    return GetBinBin(Lbins::O);
}
Lbins::bin Lhist1::GetXNbins() const {
    return Xbins.Nbins();
}
double Lhist1::GetXCenter(Lbins::bin b) const {
    return Xbins.Center(b);
}
double Lhist1::GetXWidth(Lbins::bin b) const {
    return Xbins.Width(b);
}
Lbins Lhist1::GetXbins() const {
    return Xbins;
}


// Simple stats, integrals
double Lhist1::GetXsum() {
    Lhistbin sumbin = SumBins(1,GetXNbins());
    return sumbin.GetValue();
}
double Lhist1::GetXave() {
    double retval = 0;
    for (Lbins::bin B=1; B<=GetXNbins(); ++B) {
        retval += GetBinBin(B).GetValue() * Xbins.Center(B);
    }
    retval /= GetXsum();
    return retval;
}
double Lhist1::GetXrms() {
    double retval = 0;
    double mean = GetXave();
    for (Lbins::bin B=1; B<=GetXNbins(); ++B) {
        retval += GetBinBin(B).GetValue() * xp( Xbins.Center(B) -mean, 2 );
    }
    retval /= GetXsum();
    return sqrt(retval);
}
Lhistbin Lhist1::SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi) const {
    if (Bxlo < Lbins::U || Bxhi > GetXNbins() || Bxlo > Bxhi) {
        LppUrk.LppIssue(12,"Lhist1::SumBins(bin,bin)");
    }
    Lhistbin retval;
    for (Lbins::bin B=Bxlo; B<=Bxhi; ++B) {
        retval += GetBinBin(B);
    }
    return retval;
}


// Get upper/lower values
double Lhist1::MinVal() const {
    double retval = +Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        Lhistbin thisbin =(*HistArray)(Bx);
        retval = min(retval, thisbin.GetValue());
    }
    return retval;
}
double Lhist1::MaxVal() const {
    double retval = -Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        Lhistbin thisbin =(*HistArray)(Bx);
        retval = max(retval, thisbin.GetValue());
    }
    return retval;
}
Lbins::bin Lhist1::MinBin() const {
    double minval = +Dbig;
    Lbins::bin retval = Lbins::U;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        double thisbin =(*HistArray)(Bx).GetValue();
        if (thisbin < minval) {
            retval = Bx;
            minval = thisbin;
        }
    }
    return retval;
}
Lbins::bin Lhist1::MaxBin() const {
    double maxval = -Dbig;
    Lbins::bin retval = Lbins::U;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        double thisbin =(*HistArray)(Bx).GetValue();
        if (thisbin > maxval) {
            retval = Bx;
            maxval = thisbin;
        }
    }
    return retval;
}


// CSV output: bin center, value, bin width, error
void Lhist1::CSVout(string filename, bool overwrite) {
    string fullname = ".";
    if (filename == "") {
        fullname += string("/Lhist1");
    } else {
        fullname += "/" + filename;
    }
    fullname += string(".csv");

    if (!overwrite && !filehamna(fullname)) {
        cout << fullname << endl;
        LppUrk.LppIssue(13,"Lhist1::CSVout(string)");
    }
    
    fstream file(fullname.c_str(),ios_base::out);
    if (file.fail()) {
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lhist1::CSVout(string)");
    }

    file << info << endl << "X bin center, X bin width, Value, Error" << endl;
    if (file.bad()) {       // Wish they'd created exceptions for << operator by default
        file.close();       // Assume if the file's OK on 1st write it's always OK
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lhist1::CSVout(string)");
    }

    file <<
        "U,-," << GetBinBin(Lbins::U).GetValue() << "," << GetBinBin(Lbins::U).GetError()
        << endl;
    for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
        file <<
            Xbins.Center(X) << "," << Xbins.Width(X) << "," <<
            GetBinBin(X).GetValue()<< "," << GetBinBin(X).GetError() << endl;

    }
    file <<
        "O,-," << GetBinBin(Lbins::O).GetValue() << "," << GetBinBin(Lbins::O).GetError()
        << endl;
    file.close();
    return;
}
