/*  L++ Lprof2
 *  Created by Leo Bellantoni on 12/14/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 2D profile.
 *
 */
#include "Lprof2.h"
using namespace std;


// Basics
Lprof2::Lprof2() : ProfArray(nullptr),Xbins(Lbins()),Ybins(Lbins()),info("") {};
Lprof2::Lprof2(Lprof2 const& inProf)
    : ProfArray(nullptr),Xbins(Lbins()),Ybins(Lbins()),info("") {
    *this = inProf;
}
Lprof2& Lprof2::operator=(Lprof2 const& rhs) {
    if (this != &rhs) {
        Xbins = rhs.Xbins;
        Ybins = rhs.Ybins;
        delete ProfArray;
        if (rhs.ProfArray) ProfArray  = new ra<Lprofbin> (*rhs.ProfArray);
        info = rhs.info;
    }
    return *this;
}
Lprof2::~Lprof2() {
    delete ProfArray;
}





// Constructor for the masses
Lprof2::Lprof2(Lbins inXbins, Lbins inYbins, string inInfo) :
    info(inInfo) {
    Xbins = inXbins;    Ybins = inYbins;
    ProfArray  = new ra<Lprofbin> (GetXNbins() +2, GetYNbins() +2);
    ProfArray -> setbase(Lbins::U,Lbins::U);
}


// Mathematical operators built on corresponding operations
// in Lprofbin
Lprof2  Lprof2::operator& (Lprof2 const& rhs) const {
	Lprof2 retval(*this);	return (retval &= rhs);
}
Lprof2& Lprof2::operator&=(Lprof2 const& rhs) {
    if (Xbins!=rhs.Xbins || Ybins!=rhs.Ybins) {
        LppUrk.LppIssue(107,"Lprof2::operator& or &=(Lprof2)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            (*ProfArray)(i,j) &= (*rhs.ProfArray)(i,j);
        }
    }
    return *this;
}

void Lprof2::Scale(double const& scale) {
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            ((*ProfArray)(i,j)).Scale(scale);
        }
    }
    return;
}

Lprof2 Lprof2::GetErrorHisto(bool RMS) const {
    Lprof2 retval(*this);         Lprofbin nullbin;
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            (*retval.ProfArray)(i,j) = nullbin;
            if (RMS) {
                (*retval.ProfArray)(i,j).Fill( (*ProfArray)(i,j).GetSDev() );
            } else {
                (*retval.ProfArray)(i,j).Fill( (*ProfArray)(i,j).GetErrMean() );
            }
        }
    }
    return retval;
}



// Filling
void Lprof2::Fill(double x, double y, double value, double weight) {
    if (value!=value) return;       // Nan
    if (weight!=weight) return;
    Lbins::bin X = Xbins.FindBin(x);
    Lbins::bin Y = Ybins.FindBin(y);
    ((*ProfArray)(X,Y)).Fill(value,weight);
}


// Accessing bins
Lprofbin& Lprof2::GetBinPos(double x, double y) const {
    Lbins::bin X = Xbins.FindBin(x);
    Lbins::bin Y = Ybins.FindBin(y);
    return GetBinBin(X,Y);
}
Lprofbin& Lprof2::GetBinBin(Lbins::bin Bx, Lbins::bin By) const {
    return (*ProfArray)(Bx,By);
}
Lprofbin Lprof2::GetXunder() const {
    Lprofbin retval;
    for (Lbins::bin Y=1; Y<=GetYNbins(); ++Y) {
        retval &= GetBinBin(Lbins::U,Y);
    }
    return retval;
}
Lprofbin Lprof2::GetXover() const {
    Lprofbin retval;
    for (Lbins::bin Y=1; Y<=GetYNbins(); ++Y) {
        retval &= GetBinBin(Lbins::O,Y);
    }
    return retval;
}
Lprofbin Lprof2::GetYunder() const {
    Lprofbin retval;
    for (Lbins::bin X=1; X<=GetXNbins(); ++X) {
        retval &= GetBinBin(X,Lbins::U);
    }
    return retval;
}
Lprofbin Lprof2::GetYover() const {
    Lprofbin retval;
    for (Lbins::bin X=1; X<=GetXNbins(); ++X) {
        retval &= GetBinBin(X,Lbins::O);
    }
    return retval;
}
double Lprof2::GetXCenter(Lbins::bin b) const {
    return Xbins.Center(b);
}
double Lprof2::GetXWidth(Lbins::bin b) const {
    return Xbins.Width(b);
}
Lbins::bin Lprof2::GetXNbins() const {
    return Xbins.Nbins();
}
Lbins::bin Lprof2::GetYNbins() const {
    return Ybins.Nbins();
}
double Lprof2::GetYCenter(Lbins::bin b) const {
    return Ybins.Center(b);
}
double Lprof2::GetYWidth(Lbins::bin b) const {
    return Ybins.Width(b);
}
Lbins Lprof2::GetXbins() const {
    return Xbins;
}
Lbins Lprof2::GetYbins() const {
    return Ybins;
}


// Simple stats, integrals
Lprofbin Lprof2::SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi,
                         Lbins::bin Bylo, Lbins::bin Byhi) const {
    if (Bxlo < 1 || Bxhi > GetXNbins() || Bxlo > Bxhi) {
        LppUrk.LppIssue(12,"Lprof1::SumBins(bin,bin,bin,bin)");
    }
    if (Bylo < 1 || Byhi > GetYNbins() || Bylo > Byhi) {
        LppUrk.LppIssue(12,"Lprof1::SumBins(bin,bin,bin,bin)");
    }
    Lprofbin retval;
    for (Lbins::bin Bx=Bxlo; Bx<=Bxhi; ++Bx) {
        for (Lbins::bin By=Bylo; By<=Byhi; ++By) {
            retval &= GetBinBin(Bx,By);
        }
    }
    return retval;
}


// Get upper/lower values
double Lprof2::MinVal() const {
    ra<Lbins::bin> minbins(2);
    minbins = MinBin();
    return (*ProfArray)(minbins(1),minbins(2)).GetMean();
}
double Lprof2::MaxVal() const {
    ra<Lbins::bin> maxbins(2);
    maxbins = MaxBin();
    return (*ProfArray)(maxbins(1),maxbins(2)).GetMean();
}
ra<Lbins::bin> Lprof2::MinBin() const {
    ra<Lbins::bin> retval(2);
    double minval = +Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            double thisbin =(*ProfArray)(Bx,By).GetMean();
            if (thisbin < minval) {
                retval(1) = Bx;     retval(2) = By;
                minval = thisbin;
            }
        }
    }
    return retval;
}
ra<Lbins::bin> Lprof2::MaxBin() const {
    ra<Lbins::bin> retval(2);
    double minval = -Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            double thisbin =(*ProfArray)(Bx,By).GetMean();
            if (thisbin > minval) {
                retval(1) = Bx;     retval(2) = By;
                minval = thisbin;
            }
        }
    }
    return retval;
}



// CSV output not yet tested
void Lprof2::CSVout(string filename, bool RMS, bool overwrite) {
    string fullname;
    if (filename == "") {
        if (RMS) {
            fullname = string("Lprof2rms");
        } else {
            fullname = string("Lprof2err");
        }
    } else {
        fullname = filename;
    }
    fullname += string(".csv");

    if (!overwrite && !filehamna(fullname)) {
        cout << fullname << endl;
        LppUrk.LppIssue(13,"Lprof2::CSVout(string,bool,bool)");
    }

    fstream file(fullname.c_str(),ios_base::out);
    if (file.fail()) {
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lprof2::CSVout(string,bool,bool)");
    }

    if (RMS) {
        file << info << endl <<
            "X bin center, X bin width, Y bin center, Y bin width, Mean, RMS" <<
            endl;
    } else {
        file << info << endl << 
            "X bin center, X bin width, Y bin center, Y bin width, Mean, Error(mean)"
            << endl;
    }
    if (file.bad()) {   // Wish they'd created exceptions for << operator by default
        file.close();   // Assume if the file's OK on 1st write it's always OK
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lprof2::CSVout(string,bool,bool)");
    }
    
    double err = RMS ? GetBinBin(Lbins::U,Lbins::U).GetSDev()
                     : GetBinBin(Lbins::U,Lbins::U).GetErrMean();
    file << "U,-,U,-," << GetBinBin(Lbins::U,Lbins::U).GetMean() << "," << err << endl;
    for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
        err = RMS ? GetBinBin(X,Lbins::U).GetSDev() : GetBinBin(X,Lbins::U).GetErrMean();
        file << Xbins.Center(X) << "," << Xbins.Width(X) << ",U,-," << 
            GetBinBin(X,Lbins::U).GetMean() << "," << err << endl;
    }
    err = RMS ? GetBinBin(Lbins::O,Lbins::U).GetSDev()
              : GetBinBin(Lbins::O,Lbins::U).GetErrMean();
    file << "O,-,U,-," << GetBinBin(Lbins::O,Lbins::U).GetMean() << "," << err << endl;

    for (Lbins::bin Y = 1; Y <= GetYNbins(); ++Y) {
        err = RMS ? GetBinBin(Lbins::U,Y).GetSDev()
                  : GetBinBin(Lbins::U,Y).GetErrMean();
        file << "U,-," << Ybins.Center(Y) << "," << Ybins.Width(Y) << "," << GetBinBin(Lbins::U,Y).GetMean() << "," << err << endl;
        for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
            err = RMS ? GetBinBin(X,Y).GetSDev() : GetBinBin(X,Y).GetErrMean();
            file << Xbins.Center(X) << "," << Xbins.Width(X) << "," << 
                Ybins.Center(X) << "," << Ybins.Width(X) << "<" << 
                GetBinBin(X,Y).GetMean() << "," << err << endl;
        }
        err = RMS ? GetBinBin(Lbins::O,Y).GetSDev()
                  : GetBinBin(Lbins::O,Y).GetErrMean();
        file << "O,-," << Ybins.Center(Y) << "," << Ybins.Width(Y) << "," << GetBinBin(Lbins::O,Y).GetMean() << "," << err << endl;
    }

    err = RMS ? GetBinBin(Lbins::U,Lbins::O).GetSDev()
              : GetBinBin(Lbins::U,Lbins::O).GetErrMean();
    file << "U,-,O,-," << GetBinBin(Lbins::U,Lbins::O).GetMean() << "," << err << endl;
    for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
        err = RMS ? GetBinBin(X,Lbins::O).GetSDev() : GetBinBin(X,Lbins::O).GetErrMean();
        file << Xbins.Center(X) << "," << Xbins.Width(X) << ",O,-," << 
            GetBinBin(X,Lbins::O).GetMean() << "," << err << endl;
    }
    err = RMS ? GetBinBin(Lbins::O,Lbins::O).GetSDev()
              : GetBinBin(Lbins::O,Lbins::O).GetErrMean();
    file << "O,-,O,-," << GetBinBin(Lbins::O,Lbins::O).GetMean() << "," << err << endl;


    file.close();
    return;
}
