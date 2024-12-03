/*  L++ polynomial interpolation
 *  Created by Leo Bellantoni on 12 May 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Polynomial interpolation using LaGrange's formula and Neville's time-saver.
 *  Algorithm reworked from Press, Flannery, Teukolsky & Vetterling.
*/



#include "polyMint.h"
using namespace std;



// Constructor for the masses
// Default, copy, assignment, destructor
polyMint::polyMint() : Xra_1(nullptr), Xra_2(nullptr),
    Xra_3(nullptr), Xra_4(nullptr), Fra(nullptr), nDim(0) {}

polyMint::polyMint(polyMint const& inPoly)
    : Xra_1(nullptr),Xra_2(nullptr),Xra_3(nullptr),Xra_4(nullptr), Fra(nullptr) {
    *this = inPoly;
    return;
}

polyMint& polyMint::operator=(polyMint const& rhs) {
    if (this != &rhs) {
        delete Xra_1;
        delete Xra_2;
        delete Xra_3;
        delete Xra_4;
        delete Fra;

        if (rhs.Xra_1) Xra_1 = new ra<double>(*rhs.Xra_1);
        if (rhs.Xra_2) Xra_2 = new ra<double>(*rhs.Xra_2);
        if (rhs.Xra_3) Xra_3 = new ra<double>(*rhs.Xra_3);
        if (rhs.Xra_4) Xra_4 = new ra<double>(*rhs.Xra_4);
        if (rhs.Fra)   Fra   = new ra<double>(*rhs.Fra);
        nDim  = rhs.nDim;
    }
    return *this;
}

polyMint::~polyMint() {
    delete Xra_1;
    delete Xra_2;
    delete Xra_3;
    delete Xra_4;
    delete Fra;
    return;
}



void polyMint::setUpGrid(ra<double> Xin_1, ra<double> Xin_2) {
    if (Xin_1.ne2!=0 || Xin_2.ne2!=0)
        LppUrk.LppIssue(120, "polyMint::setUpGrid(ra<double>,ra<double>)");
    if (Fra!=nullptr)
        LppUrk.LppIssue(121, "polyMint::setUpGrid(ra<double>,ra<double>)");
    Xra_1 = new ra<double>(Xin_1.ne1);
    Xra_2 = new ra<double>(Xin_2.ne1);
    // Reset internal array indices to zero for simplicity later
    *Xra_1 = Xin_1;         *Xra_2 = Xin_2;
    Xra_1->setbase(0);      Xra_2->setbase(0);
    nDim = 2;
    return;
}
void polyMint::setUpGrid(ra<double> Xin_1, ra<double> Xin_2, ra<double> Xin_3) {
    if (Xin_1.ne2!=0 || Xin_2.ne2!=0 || Xin_3.ne2!=0)
        LppUrk.LppIssue(120, "polyMint::setUpGrid(ra<double>,ra<double>,ra<double>)");
    if (Fra!=nullptr)
        LppUrk.LppIssue(121, "polyMint::setUpGrid(ra<double>,ra<double>,ra<double>)");
    Xra_1 = new ra<double>(Xin_1.ne1);
    Xra_2 = new ra<double>(Xin_2.ne1);
    Xra_3 = new ra<double>(Xin_3.ne1);
    *Xra_1 = Xin_1;         *Xra_2 = Xin_2;         *Xra_3 = Xin_3;
    Xra_1->setbase(0);      Xra_2->setbase(0);      Xra_3->setbase(0);
    nDim = 3;
    return;
}
void polyMint::setUpGrid(ra<double> Xin_1, ra<double> Xin_2, ra<double> Xin_3, ra<double> Xin_4) {
    if (Xin_1.ne2!=0 || Xin_2.ne2!=0 || Xin_3.ne2!=0 || Xin_4.ne2!=0)
        LppUrk.LppIssue(120, "polyMint::setUpGrid(ra<double>,ra<double>,ra<double>,ra<double>)");
    if (Fra!=nullptr)
        LppUrk.LppIssue(121, "polyMint::setUpGrid(ra<double>,ra<double>,ra<double>,ra<double>)");
    Xra_1 = new ra<double>(Xin_1.ne1);
    Xra_2 = new ra<double>(Xin_2.ne1);
    Xra_3 = new ra<double>(Xin_3.ne1);
    Xra_4 = new ra<double>(Xin_4.ne1);
    *Xra_1 = Xin_1;         *Xra_2 = Xin_2;         *Xra_3 = Xin_3;         *Xra_4 = Xin_4;
    Xra_1->setbase(0);      Xra_2->setbase(0);      Xra_3->setbase(0);      Xra_4->setbase(0);
    nDim = 4;
}

void polyMint::fillValues(ra<double> Fin) {
    if (Xra_1==nullptr || Xra_2==nullptr)
        LppUrk.LppIssue(121, "polyMint::fillValues(ra<double>)");
    if (Xra_1->ne1 != Fin.ne1 || Xra_2->ne1 != Fin.ne2)
        LppUrk.LppIssue(122, "polyMint::fillValues(ra<double>) [1]");
    if (nDim==3) {
        if (Xra_3->ne1 != Fin.ne3) LppUrk.LppIssue(122, "polyMint::fillValues(ra<double>) [2]");
    }
    if (nDim==4) {
        if (Xra_4->ne1 != Fin.ne4) LppUrk.LppIssue(122, "polyMint::fillValues(ra<double>) [3]");
    }

    Fra = new ra<double>(Fin);
    Fra->setbase(0, 0, 0, 0);   // Set 'em all to 0.
    return;
}



double polyMint::eval(ra<double> x) {
    if (x.ne1 != nDim)
        LppUrk.LppIssue(120, "polyMint::eval(ra<double>)");



    int n4th = (nDim==4) ? Xra_4->ne1 : 0;
    int n3rd = (nDim==3) ? Xra_3->ne1 : 0;
    int n2nd =             Xra_2->ne1;
    int n1st =             Xra_1->ne1;
    int i1st, i2nd, i3rd, i4th;

    ra<double>* save1st;        ra<double>* save2nd;        ra<double>* save3rd;    double retval;
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
            if (nDim==2) {
                Ldelete(save1st);
                return retval;
            }
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
        if (nDim==3) {
            Ldelete(save1st);
            Ldelete(save2nd);
            return retval;
        }
    ++i4th;} while (i4th<n4th);

    // Contract the 4th index
    i4th = 0; do {
        double prod4th = 1.0;
        for (int j4th=0; j4th<n4th; ++j4th) {
            if (j4th!=i4th) prod4th *= (x(4) -(*Xra_4)(j4th)) / ((*Xra_4)(i4th) -(*Xra_4)(j4th));
        }
        retval += (*save3rd)(i4th) * prod4th;
    ++i4th;} while (i4th<n4th);
    Ldelete(save1st);
    Ldelete(save2nd);
    Ldelete(save3rd);
    return retval;
}
