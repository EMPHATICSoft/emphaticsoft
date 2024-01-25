/*  L++ Lhist2
 *  Created by Leo Bellantoni on 12/14/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A 2D histogram.
 *
 */
#include "Lhist2.h"
using namespace std;


// Basic constructors
Lhist2::Lhist2() :
    HistArray(nullptr), Xbins(Lbins()),Ybins(Lbins()), info("") {}
Lhist2::Lhist2(Lhist2 const& inHist)
    : HistArray(nullptr), Xbins(Lbins()),Ybins(Lbins()) {
    *this = inHist;
}
Lhist2& Lhist2::operator=(Lhist2 const& rhs) {
    // Prevent self assignment
    if (this != &rhs) {
        Xbins = rhs.Xbins;
        Ybins = rhs.Ybins;
        delete HistArray;
        HistArray  = new ra<Lhistbin> (*rhs.HistArray);
    }
    info = rhs.info;
    return *this;
}
Lhist2::~Lhist2() {
    delete HistArray;
}


// Constructors for the masses
Lhist2::Lhist2(Lbins inXbins, Lbins inYbins, string inInfo) :
    info(inInfo) {
    Xbins = inXbins;    Ybins = inYbins;
    HistArray  = new ra<Lhistbin> (GetXNbins() +2, GetYNbins() +2);
    HistArray -> setbase(Lbins::U,Lbins::U);
}
Lhist2::Lhist2(Lprof2 inProf, bool PlotRMS) :
    info(inProf.info) {
    Xbins = inProf.GetXbins();      Ybins = inProf.GetYbins();
    HistArray  = new ra<Lhistbin> (GetXNbins() +2, GetYNbins() +2);
    HistArray -> setbase(Lbins::U,Lbins::U);
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            double val = inProf.GetBinBin(i,j).GetMean();
            (*HistArray)(i,j).SetValue(val);
            double bar;
            if (PlotRMS) {
                bar = inProf.GetBinBin(i,j).GetSDev();
            } else {
                bar = inProf.GetBinBin(i,j).GetErrMean();
            }
        (*HistArray)(i,j).SetError(bar);
        }
    }
    
}


// Mathematical operators built on corresponding operations
// in Lhistbin.  Defined constructors should make it impossible
// to call an operation on an uninitialized Lhist.
Lhist2  Lhist2::operator+ (Lhist2 const& rhs) const {
	Lhist2 retval(*this);	return (retval += rhs);
}
Lhist2& Lhist2::operator+=(Lhist2 const& rhs) {
    if (Xbins!=rhs.Xbins || Ybins!=rhs.Ybins) {
        LppUrk.LppIssue(107,"Lhist2::operator+ or +=(Lhist2)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            (*HistArray)(i,j) += (*rhs.HistArray)(i,j);
        }
    }
    return *this;
}
Lhist2  Lhist2::operator- (Lhist2 const& rhs) const {
	Lhist2 retval(*this);	return (retval -= rhs);
}
Lhist2& Lhist2::operator-=(Lhist2 const& rhs) {
    if (Xbins!=rhs.Xbins || Ybins!=rhs.Ybins) {
        LppUrk.LppIssue(107,"Lhist2::operator- or -=(Lhist2)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            (*HistArray)(i,j) -= (*rhs.HistArray)(i,j);
        }
    }
    return *this;
}

Lhist2  Lhist2::operator* (Lhist2 const& rhs) const {
	Lhist2 retval(*this);	return (retval *= rhs);
}
Lhist2& Lhist2::operator*=(Lhist2 const& rhs) {
    if (Xbins!=rhs.Xbins || Ybins!=rhs.Ybins) {
        LppUrk.LppIssue(107,"Lhist2::operator* or *=(Lhist2)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            (*HistArray)(i,j) *= (*rhs.HistArray)(i,j);
        }
    }
    return *this;
}

void Lhist2::Scale(double const& scale) {
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            ((*HistArray)(i,j)).Scale(scale);
        }
    }
    return;
}


Lhist2  Lhist2::operator/ (Lhist2 const& rhs) const {
	Lhist2 retval(*this);	return (retval /= rhs);
}
Lhist2& Lhist2::operator/=(Lhist2 const& rhs) {
    if (Xbins!=rhs.Xbins || Ybins!=rhs.Ybins) {
        LppUrk.LppIssue(107,"Lhist2::operator/ or /=(Lhist2)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            if ((*rhs.HistArray)(i,j).GetValue() == 0.0) {
                // Don't flag over/underflow bins of zero
                if (i!=Lbins::U && i!=Lbins::O && j!=Lbins::U && i!=Lbins::O) {
                    LppUrk.LppIssue(108,"Lhist2::operator/ or /=(Lhist2)");
                }
                (*HistArray)(i,j).SetValue(0);
                (*HistArray)(i,j).SetError(0);
            } else {
                (*HistArray)(i,j) /= (*rhs.HistArray)(i,j);
            }
        }
    }
    return *this;
}
Lhist2  Lhist2::operator% (Lhist2 const& rhs) const {
	Lhist2 retval(*this);	return (retval %= rhs);
}
Lhist2& Lhist2::operator%=(Lhist2 const& rhs) {
    if (Xbins!=rhs.Xbins || Ybins!=rhs.Ybins) {
        LppUrk.LppIssue(107,"Lhist2::operator% or %=(Lhist2)");
    }
    for (Lbins::bin i=Lbins::U; i<=GetXNbins(); ++i) {
        for (Lbins::bin j=Lbins::U; j<=GetYNbins(); ++j) {
            if ((*rhs.HistArray)(i,j).GetValue() == 0.0) {
                if (i!=Lbins::U && i!=Lbins::O && j!=Lbins::U && i!=Lbins::O) {
                    LppUrk.LppIssue(108,"Lhist2::operator% or %=(Lhist2)");
                }
                (*HistArray)(i,j).SetValue(0);
                (*HistArray)(i,j).SetError(0);
            } else {
                (*HistArray)(i,j) %= (*rhs.HistArray)(i,j);
            }
        }
    }
    return *this;
}


// Filling
void Lhist2::Fill(double x, double y, double weight) {
    Lbins::bin X = Xbins.FindBin(x);
    Lbins::bin Y = Ybins.FindBin(y);
    ((*HistArray)(X,Y)).Fill(weight);
}


// Accessing bins
Lhistbin& Lhist2::GetBinPos(double x, double y) const {
    Lbins::bin X = Xbins.FindBin(x);
    Lbins::bin Y = Ybins.FindBin(y);
    return GetBinBin(X,Y);
}
Lhistbin& Lhist2::GetBinBin(Lbins::bin Bx, Lbins::bin By)  const{
    return (*HistArray)(Bx,By);
}
Lhistbin Lhist2::GetXunder() const {
    Lhistbin retval;
    for (Lbins::bin Y=1; Y<=GetYNbins(); ++Y) {
        retval += GetBinBin(Lbins::U,Y);
    }
    return retval;
}
Lhistbin Lhist2::GetXover() const {
    Lhistbin retval;
    for (Lbins::bin Y=1; Y<=GetYNbins(); ++Y) {
        retval += GetBinBin(Lbins::O,Y);
    }
    return retval;
}
Lhistbin Lhist2::GetYunder() const {
    Lhistbin retval;
    for (Lbins::bin X=1; X<=GetXNbins(); ++X) {
        retval += GetBinBin(X,Lbins::U);
    }
    return retval;
}
Lhistbin Lhist2::GetYover() const {
    Lhistbin retval;
    for (Lbins::bin X=1; X<=GetXNbins(); ++X) {
        retval += GetBinBin(X,Lbins::O);
    }
    return retval;
}
int Lhist2::GetXNbins() const {
    return Xbins.Nbins();
}
double Lhist2::GetXCenter(Lbins::bin b) const {
    return Xbins.Center(b);
}
double Lhist2::GetXWidth(Lbins::bin b) const {
    return Xbins.Width(b);
}
int Lhist2::GetYNbins() const {
    return Ybins.Nbins();
}
double Lhist2::GetYCenter(Lbins::bin b) const {
    return Ybins.Center(b);
}
double Lhist2::GetYWidth(Lbins::bin b) const {
    return Ybins.Width(b);
}
Lbins Lhist2::GetXbins() const {
    return Xbins;
}
Lbins Lhist2::GetYbins() const {
    return Ybins;
}


// Simple stats, integrals
double Lhist2::GetXYsum() {
    Lhistbin sumbin = SumBins(1,GetXNbins(),1,GetYNbins());
    return sumbin.GetValue();
}
double Lhist2::GetXave() {
    double retval = 0;
    for (Lbins::bin B=1; B<=GetXNbins(); ++B) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            retval += GetBinBin(B,By).GetValue() * Xbins.Center(B);
        }
    }
    retval /= GetXYsum();
    return retval;
}
double Lhist2::GetXrms() {
    double retval = 0;
    double mean = GetXave();
    for (Lbins::bin B=1; B<=GetXNbins(); ++B) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            retval += GetBinBin(B,By).GetValue() * xp( Xbins.Center(B) -mean, 2 );
        }
    }
    retval /= GetXYsum();
    return sqrt(retval);
}
double Lhist2::GetYave() {
    double retval = 0;
    for (Lbins::bin B=1; B<=GetYNbins(); ++B) {
        for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
            retval += GetBinBin(Bx,B).GetValue() * Ybins.Center(B);
        }
    }
    retval /= GetXYsum();
    return retval;
}
double Lhist2::GetYrms() {
    double retval = 0;
    double mean = GetYave();
    for (Lbins::bin B=1; B<=GetYNbins(); ++B) {
        for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
            retval += GetBinBin(Bx,B).GetValue() * xp( Ybins.Center(B) -mean, 2 );
        }
    }
    retval /= GetXYsum();
    return sqrt(retval);
}
Lhistbin Lhist2::SumBins(Lbins::bin Bxlo, Lbins::bin Bxhi,
                         Lbins::bin Bylo, Lbins::bin Byhi) const {
    if (Bxlo < Lbins::U || Bxhi > GetXNbins() || Bxlo > Bxhi) {
        LppUrk.LppIssue(12,"Lhist1::SumBins(bin,bin,bin,bin)");
    }
    if (Bylo < Lbins::U || Byhi > GetYNbins() || Bylo > Byhi) {
        LppUrk.LppIssue(12,"Lhist1::SumBins(bin,bin,bin,bin)");
    }
    Lhistbin retval;
    for (Lbins::bin Bx=Bxlo; Bx<=Bxhi; ++Bx) {
        for (Lbins::bin By=Bylo; By<=Byhi; ++By) {
            retval += GetBinBin(Bx,By);
        }
    }
    return retval;
}


// Get upper/lower values
double Lhist2::MinVal() const {
    double retval = +Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            Lhistbin thisbin =(*HistArray)(Bx,By);
            retval = min(retval, thisbin.GetValue());
        }
    }
    return retval;
}
double Lhist2::MaxVal() const {
    double retval = -Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            Lhistbin thisbin =(*HistArray)(Bx,By);
            retval = max(retval, thisbin.GetValue());
        }
    }
    return retval;
}
ra<Lbins::bin> Lhist2::MinBin() const {
    ra<int> retval(2);
    double minval = +Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            double thisbin =(*HistArray)(Bx,By).GetValue();
            if (thisbin < minval) {
                retval(1) = Bx;     retval(2) = By;
                minval = thisbin;
            }
        }
    }
    return retval;
}
ra<Lbins::bin> Lhist2::MaxBin() const {
    ra<int> retval(2);
    double minval = -Dbig;
    for (Lbins::bin Bx=1; Bx<=GetXNbins(); ++Bx) {
        for (Lbins::bin By=1; By<=GetYNbins(); ++By) {
            double thisbin =(*HistArray)(Bx,By).GetValue();
            if (thisbin > minval) {
                retval(1) = Bx;     retval(2) = By;
                minval = thisbin;
            }
        }
    }
    return retval;
}



// CSV output: bin centers, contents, error.
void Lhist2::CSVout(string filename, bool overwrite) {
    string fullname = ".";
    if (filename == "") {
        fullname += string("/Lhist1");
    } else {
        fullname += "/" + filename;
    }
    fullname += string(".csv");

    if (!overwrite && !filehamna(fullname)) {
        cout << fullname << endl;
        LppUrk.LppIssue(13,"Lhist2::CSVout(string)");
    }
    
    fstream file(fullname.c_str(),ios_base::out);
    if (file.fail()) {
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lhist2::CSVout(string)");
    }

    file << info << endl << "X bin center, X bin width, Y bin center, Y bin width, Value, Error" << endl;
    if (file.bad()) {   // Wish they'd created exceptions for << operator by default
        file.close();   // Assume if the file's OK on 1st write it's always OK
        cout << filename << endl;
        LppUrk.LppIssue(14,"Lhist2::CSVout(string)");
    }

    file << "U,-,U,-," << GetBinBin(Lbins::U,Lbins::U).GetValue() << "," <<
        GetBinBin(Lbins::U,Lbins::U).GetError() << endl;
    for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
        file << Xbins.Center(X) << "," << Xbins.Width(X) << ",U,-," << 
            GetBinBin(X,Lbins::U).GetValue() << "," << 
            GetBinBin(X,Lbins::U).GetError() << endl;
    }
    file << "O,-,U,-," << GetBinBin(Lbins::O,Lbins::U).GetValue() << "," <<
        GetBinBin(Lbins::O,Lbins::U).GetError() << endl;

    for (Lbins::bin Y = 1; Y <= GetYNbins(); ++Y) {
        file << "U,-," << Ybins.Center(Y) << "," << Ybins.Width(Y) << "," 
            << GetBinBin(Lbins::U,Y).GetValue() << "," << 
            GetBinBin(Lbins::U,Y).GetError() << endl;
        for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
            file << Xbins.Center(X) << "," << Xbins.Width(X) << "," << 
                Ybins.Center(Y) << "," << Ybins.Width(Y) << "," <<
                GetBinBin(X,Y).GetValue() << "," << GetBinBin(X,Y).GetError() 
                << endl;
        }
        file << "O,-," << Ybins.Center(Y) << "," << Ybins.Width(Y) << "," <<GetBinBin(Lbins::O,Y).GetValue() << "," << 
            GetBinBin(Lbins::O,Y).GetError() << endl;
    }

    file << "U,-,O,-," << GetBinBin(Lbins::U,Lbins::O).GetValue() << "," << 
        GetBinBin(Lbins::U,Lbins::O).GetError() << endl;
    for (Lbins::bin X = 1; X <= GetXNbins(); ++X) {
        file << Xbins.Center(X) << "," << Xbins.Width(X) << ",O,-," << 
            GetBinBin(X,Lbins::O).GetValue() << "," <<
            GetBinBin(X,Lbins::O).GetError() << endl;
    }
    file << "O,-,O,-," << GetBinBin(Lbins::O,Lbins::O).GetValue() << "," << 
        GetBinBin(Lbins::O,Lbins::O).GetError() << endl;

    file.close();
    return;
}

