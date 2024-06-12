/*  L++ Lprof1
 *  Created by Leo Bellantoni on 12/14/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 1D profile.
 *
 */
#include "Lprof1.h"
using namespace std;


// Basics
Lprof1::Lprof1() : ProfArray(nullptr),Xbins(Lbins()), info("") {}
Lprof1::Lprof1(Lprof1 const& inProf)
    : ProfArray(nullptr),Xbins(Lbins()), info("") {
    *this = inProf;
}
Lprof1& Lprof1::operator=(Lprof1 const& rhs) {
    if (this != &rhs) {
        Xbins = rhs.Xbins;
        delete ProfArray;
        if (rhs.ProfArray) ProfArray  = new ra<Lprofbin> (*rhs.ProfArray);
        info = rhs.info;
    }
    return *this;
}
Lprof1::~Lprof1() {
    delete ProfArray;
}


// Constructor for the masses
Lprof1::Lprof1(Lbins inXbins, string inInfo) :
    info(inInfo) {
    Xbins = inXbins;
    ProfArray  = new ra<Lprofbin> (GetXNbins() +2);
    ProfArray -> setbase(Lbins::U);
}


// Mathematical operators built on corresponding operations
// in Lprofbin
Lprof1  Lprof1::operator& (Lprof1 const& rhs) const {
	Lprof1 retval(*this);	return (retval &= rhs);
}
Lprof1& Lprof1::operator&=(Lprof1 const& rhs) {
    if (Xbins!=rhs.Xbins) {
        LppUrk.LppIssue(107,"Lprof1::operator& or &=(Lprof1)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        (*ProfArray)(i) &= (*rhs.ProfArray)(i);
    }
    return *this;
}

void Lprof1::Scale(double const& scale) {
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        ((*ProfArray)(i)).Scale(scale);
    }
    return;
}


// Filling
void Lprof1::Fill(double x, double value, double weight) {
    Lbins::bin X = Xbins.FindBin(x);
    ((*ProfArray)(X)).Fill(value,weight);
}


// Accessing bins
Lprofbin& Lprof1::GetBinPos(double x) const {
    Lbins::bin X = Xbins.FindBin(x);
    return GetBinBin(X);
}
Lprofbin& Lprof1::GetBinBin(Lbins::bin Bx) const {
    return (*ProfArray)(Bx);
}
Lprofbin Lprof1::GetXunder() const {
    return GetBinBin(Lbins::U);
}
Lprofbin Lprof1::GetXover() const {
    return GetBinBin(Lbins::O);
}
int Lprof1::GetXNbins() const {
    return Xbins.Nbins();
}
double Lprof1::GetXCenter(Lbins::bin b) const {
    return Xbins.Center(b);
}
double Lprof1::GetXWidth(Lbins::bin b) const {
    return Xbins.Width(b);
}
Lbins Lprof1::GetXbins() const {
    return Xbins;
}


// Integral
Lprofbin Lprof1::SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi) const {
    Lprofbin retval;
    if (Bxlo < Lbins::U || Bxhi > GetXNbins() || Bxlo > Bxhi) {
        LppUrk.LppIssue(12,"Lprof1::SumBins(bin,bin)");
    }
    for (Lbins::bin B=Bxlo; B<=Bxhi; ++B) {
        retval &= GetBinBin(B);
    }
    return retval;
}


// Get upper/lower values
double Lprof1::MinVal() const {
    double retval = +Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        Lprofbin thisbin =(*ProfArray)(Bx);
        retval = min(retval, thisbin.GetMean());
    }
    return retval;
}
double Lprof1::MaxVal() const {
    double retval = -Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        Lprofbin thisbin =(*ProfArray)(Bx);
        retval = max(retval, thisbin.GetMean());
    }
    return retval;
}
Lbins::bin Lprof1::MinBin() const {
    double minval = +Dbig;
    Lbins::bin retval = Lbins::U;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        double thisbin =(*ProfArray)(Bx).GetMean();
        if (thisbin < minval) {
            retval = Bx;
            minval = thisbin;
        }
    }
    return retval;
}
Lbins::bin Lprof1::MaxBin() const {
    double maxval = -Dbig;
    Lbins::bin retval = Lbins::U;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        double thisbin =(*ProfArray)(Bx).GetMean();
        if (thisbin > maxval) {
            retval = Bx;
            maxval = thisbin;
        }
    }
    return retval;
}


// CSV output not yet tested
void Lprof1::CSVout(string filename, bool RMS, bool overwrite) {
    string fullname = string(getenv("HOME")) +string("/Desktop");
    if (filename == "") {
        if (RMS) {
            fullname += string("/Lprof1rms");
        } else {
            fullname += string("/Lprof1err");
        }
    } else {
        fullname += "/" + filename;
    }
    fullname += string(".csv");

    if (!overwrite && !filehamna(fullname)) {
        cout << fullname << endl;
        LppUrk.LppIssue(13,"Lprof1::CSVout(string,bool,bool)");
    }

    fstream file(fullname.c_str(),ios_base::out);
    if (file.fail()) {
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lprof1::CSVout(string,bool,bool)");
    }

    if (RMS) {
        file << info << endl << "X bin center, X bin width, Mean, RMS" << endl;
    } else {
        file << info << endl << "X bin center, X bin width, Mean, Error(mean)" << endl;
    }
    if (file.bad()) {   // Wish they'd created exceptions for << operator by default
        file.close();   // Assume if the file's OK on 1st write it's always OK
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lprof1::CSVout(string,bool,bool)");
    }
    
    double err = RMS ? GetBinBin(Lbins::U).GetSDev() : GetBinBin(Lbins::U).GetErrMean();
    file << "U,-'" << GetBinBin(Lbins::U).GetMean() << "," << err << endl;
    for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
        err = RMS ? GetBinBin(X).GetSDev() : GetBinBin(X).GetErrMean();
        file << Xbins.Center(X) << "," << Xbins.Width(X) << "," << 
            GetBinBin(X).GetMean() << "," << Xbins.Width(X)  << "," << err <<
            endl;
    }
    err = RMS ? GetBinBin(Lbins::O).GetSDev() : GetBinBin(Lbins::O).GetErrMean();
    file << "O,-," << GetBinBin(Lbins::O).GetMean() << "," << err << endl;

    file.close();
    return;
}
