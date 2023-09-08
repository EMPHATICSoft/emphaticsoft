/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 12 May 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Polynomial interpolation using LaGrange's formula and Neville's time-saver.
 *  Algorithm reworked from Press, Flannery, Teukolsky & Vetterling.
*/



#include "MagneticField/field_map_tools/polyMint.h"
using namespace std;



// Constructor for the masses
// Default, copy, assignment, destructor
polyMint::polyMint() : Xra_1(nullptr), Xra_2(nullptr),
    Xra_3(nullptr), Xra_4(nullptr), Fra(nullptr), nDim(0) {}

polyMint::polyMint(polyMint const& inPoly) {
    comcopy(inPoly,true);
    return;
}

polyMint& polyMint::operator=(polyMint const& rhs) {
    if (this != &rhs) comcopy(rhs,false);
    return *this;
}
void polyMint::comcopy(polyMint const& inPoly, bool constructing) {
    if (!constructing) {
        delete Xra_1;
        delete Xra_2;
        delete Xra_3;
        delete Xra_4;
        delete Fra;
    }
    Xra_1 = new ra<double>(*inPoly.Xra_1);
    Xra_2 = new ra<double>(*inPoly.Xra_2);
    if (inPoly.Xra_3==nullptr) {
        Xra_3 = nullptr;
    } else {
        Xra_3 = new ra<double>(*inPoly.Xra_3);
    }
    if (inPoly.Xra_4==nullptr) {
        Xra_4 = nullptr;
    } else {
        Xra_4 = new ra<double>(*inPoly.Xra_4);
    }
    Fra   = new ra<double>(*inPoly.Fra);
    nDim  = inPoly.nDim;
}

polyMint::~polyMint() {
    // Remember - it's always OK to delete nullptr
    delete Xra_1;   delete Xra_2;   delete Xra_3;   delete Xra_4;
    delete Fra;
}



void polyMint::setUpGrid(ra<double> Xin_1, ra<double> Xin_2) {
    if (Xin_1.ne2!=0 || Xin_2.ne2!=0) {
		cout << "Wrongly dimensioned argument in polyMint::setUpGrid(ra<double>,ra<double>)" << endl;
		abort();
    }
    if (Fra!=nullptr) {
		cout << "Define grid 1st, function values 2nd in polyMint::setUpGrid(ra<double>,ra<double>)" << endl;
		abort();
    }
    Xra_1 = new ra<double>(Xin_1.ne1);
    Xra_2 = new ra<double>(Xin_2.ne1);
    // Reset internal array indices to zero for simplicity later
    *Xra_1 = Xin_1;         *Xra_2 = Xin_2;
    Xra_1->setbase(0);      Xra_2->setbase(0);
    nDim = 2;
}
void polyMint::setUpGrid(ra<double> Xin_1, ra<double> Xin_2, ra<double> Xin_3) {
    if (Xin_1.ne2!=0 || Xin_2.ne2!=0 || Xin_3.ne2!=0) {
		cout << "Wrongly dimensioned argument in polyMint::setUpGrid(ra<double>,ra<double>,ra<double>)" << endl;
		abort();
    }
    if (Fra!=nullptr) {
		cout << "Define grid 1st, function values 2nd in polyMint::setUpGrid(ra<double>,ra<double>,ra<double>)" << endl;
		abort();
    }
    Xra_1 = new ra<double>(Xin_1.ne1);
    Xra_2 = new ra<double>(Xin_2.ne1);
    Xra_3 = new ra<double>(Xin_3.ne1);
    *Xra_1 = Xin_1;         *Xra_2 = Xin_2;         *Xra_3 = Xin_3;
    Xra_1->setbase(0);      Xra_2->setbase(0);      Xra_3->setbase(0);
    nDim = 3;
}
void polyMint::setUpGrid(ra<double> Xin_1, ra<double> Xin_2, ra<double> Xin_3, ra<double> Xin_4) {
    if (Xin_1.ne2!=0 || Xin_2.ne2!=0 || Xin_3.ne2!=0 || Xin_4.ne2!=0) {
		cout << "Wrongly dimensioned argument in polyMint::setUpGrid(ra<double>,ra<double>,ra<double>,ra<double>)" << endl;
		abort();
    }
    if (Fra!=nullptr) {
		cout << "Define grid 1st, function values 2nd in polyMint::setUpGrid(ra<double>,ra<double>,ra<double>,ra<double>)" << endl;
		abort();
    }
    Xra_1 = new ra<double>(Xin_1.ne1);
    Xra_2 = new ra<double>(Xin_2.ne1);
    Xra_3 = new ra<double>(Xin_3.ne1);
    Xra_4 = new ra<double>(Xin_4.ne1);
    *Xra_1 = Xin_1;         *Xra_2 = Xin_2;         *Xra_3 = Xin_3;         *Xra_4 = Xin_4;
    Xra_1->setbase(0);      Xra_2->setbase(0);      Xra_3->setbase(0);      Xra_4->setbase(0);
    nDim = 4;
}

void polyMint::fillValues(ra<double> Fin) {
    if (Xra_1==nullptr || Xra_2==nullptr) {
		cout << "Wrongly dimensioned argument in polyMint::fillValues(ra<double>)" << endl;
		abort();
    }
    if (Xra_1->ne1 != Fin.ne1 || Xra_2->ne1 != Fin.ne2) {
		cout << "Grid dimensions != function values dimension in polyMint::fillValues(ra<double>) [1]" << endl;
		abort();
    }
    if (nDim==3) {
        if (Xra_3->ne1 != Fin.ne3)  {
		cout << "Grid dimensions != function values dimension in polyMint::fillValues(ra<double>) [2]" << endl;
		abort();
    }
    }
    if (nDim==4) {
        if (Xra_4->ne1 != Fin.ne4)  {
		cout << "Grid dimensions != function values dimension in polyMint::fillValues(ra<double>) [3]" << endl;
		abort();
    }
    }

    Fra = new ra<double>(Fin);
    Fra->setbase(0, 0, 0, 0);   // Set 'em all to 0.
    return;
}



double polyMint::eval(ra<double> x) {
    if (x.ne1 != nDim) {
		cout << "Wrongly dimensioned argument in polyMint::eval(ra<double>)" << endl;
		abort();
    }



    int n4th = (nDim==4) ? Xra_4->ne1 : 0;
    int n3rd = (nDim==3) ? Xra_3->ne1 : 0;
    int n2nd =             Xra_2->ne1;
    int n1st =             Xra_1->ne1;
    int i1st, i2nd, i3rd, i4th;

    ra<double>* save1st;        ra<double>* save2nd;        ra<double>* save3rd;    double retval;
	save1st = save2nd = save3rd = nullptr;    // shut up, compiler
    switch (nDim) {
        case 4: {
            save1st = new ra<double>(Xra_2->ne1, Xra_3->ne1, Xra_4->ne1);   save1st->setbase(0,0,0);
            save2nd = new ra<double>(Xra_3->ne1, Xra_4->ne1);               save2nd->setbase(0,0);
            save3rd = new ra<double>(Xra_4->ne1);                           save3rd->setbase(0);
            *save1st = 0;      *save2nd = 0;        *save3rd = 0;       retval = 0;
            break;
        }
        case 3: {
            save1st = new ra<double>(Xra_2->ne1, Xra_3->ne1);               save1st->setbase(0,0);
            save2nd = new ra<double>(Xra_3->ne1);                           save2nd->setbase(0);
            save3rd = nullptr;  // Should never be accessed, actually
            *save1st = 0;      *save2nd = 0;        retval = 0;
            break;
        }
        case 2: {
            save1st = new ra<double>(Xra_2->ne1);                           save1st->setbase(0);
            save2nd = nullptr;
            save3rd = nullptr;
            *save1st = 0;      retval = 0;
        }
    }

    // contract the 1st index
    i4th = 0; do {
        i3rd = 0; do {
            i2nd = 0; do {
                i1st = 0; do {
                    double prod1st = 1.0;
                    for (int j1st=0; j1st<n1st; ++j1st) {
                        if (j1st!=i1st) prod1st *= (x(1) -(*Xra_1)(j1st)) / ((*Xra_1)(i1st) -(*Xra_1)(j1st));
                    }
                    if (nDim==4) (*save1st)(i2nd,i3rd,i4th) += (*Fra)(i1st,i2nd,i3rd,i4th) * prod1st;
                    if (nDim==3) (*save1st)(i2nd,i3rd)      += (*Fra)(i1st,i2nd,i3rd)      * prod1st;
                    if (nDim==2) (*save1st)(i2nd)           += (*Fra)(i1st,i2nd)           * prod1st;

                ++i1st;} while (i1st<n1st);
            ++i2nd;} while (i2nd<n2nd);
        ++i3rd;} while (i3rd<n3rd);
    ++i4th;} while (i4th<n4th);

    // Contract the 2nd index
    i4th = 0; do {
        i3rd = 0; do {
            i2nd = 0; do {
                double prod2nd = 1.0;
                for (int j2nd=0; j2nd<n2nd; ++j2nd) {
                    if (j2nd!=i2nd) prod2nd *= (x(2) -(*Xra_2)(j2nd)) / ((*Xra_2)(i2nd) -(*Xra_2)(j2nd));
                }
                if (nDim==4) (*save2nd)(i3rd,i4th) += (*save1st)(i2nd,i3rd,i4th) * prod2nd;
                if (nDim==3) (*save2nd)(i3rd)      += (*save1st)(i2nd,i3rd)      * prod2nd;
                if (nDim==2) retval                += (*save1st)(i2nd)           * prod2nd;
            ++i2nd;} while (i2nd<n2nd);
            if (nDim==2) return retval;
        ++i3rd;} while (i3rd<n3rd);
    ++i4th;} while (i4th<n4th);

    // Contract the 3rd index
    i4th = 0; do {
        i3rd = 0; do {
            double prod3rd = 1.0;
            for (int j3rd=0; j3rd<n3rd; ++j3rd) {
                if (j3rd!=i3rd) prod3rd *= (x(3) -(*Xra_3)(j3rd)) / ((*Xra_3)(i3rd) -(*Xra_3)(j3rd));
            }
            if (nDim==4) (*save3rd)(i4th) += (*save2nd)(i3rd,i4th) * prod3rd;
            if (nDim==3) retval           += (*save2nd)(i3rd)      * prod3rd;
        ++i3rd;} while (i3rd<n3rd);
        if (nDim==3) return retval;
    ++i4th;} while (i4th<n4th);

    // Contract the 4th index
    i4th = 0; do {
        double prod4th = 1.0;
        for (int j4th=0; j4th<n4th; ++j4th) {
            if (j4th!=i4th) prod4th *= (x(4) -(*Xra_4)(j4th)) / ((*Xra_4)(i4th) -(*Xra_4)(j4th));
        }
        retval += (*save3rd)(i4th) * prod4th;
    ++i4th;} while (i4th<n4th);
    return retval;
}
