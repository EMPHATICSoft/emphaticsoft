/*  L++ ra
 *  Created by Leo Bellantoni on 12/7/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  A fast-access, column-major (FORTRAN-style, not odometer-style) array, 
 *  with up to 4 dimensions.  (Many widely available matrix packages with
 *  C/C++ interfaces will accept either row- or column- major forms.)
 *
 *  Indexing is with integers, and the default base is 1.  The setbase methods
 *  allow you to change this.  Copy from standard C array with fillFromC methods.
 *
 *  Although ra< > is a wrapper around std::valarray, and you may use valarray
 *  operations on the val data member of the ra, which is public.  So, since
 *  valarray supports application of most most mathematical functions from
 *  <cmath> directly to the array (a nice feature not shared by e.g.
 *  vector<double>, you may
 *
 *          ra<double> array(5);
 *          array = 2.0;
 *          array.val = log(array.val);
 *
 *  Because the indexing routines return values by reference, it can be used in
 *  an assignment operator: array(3) = 2.2;
 *  If e.g. addition is provided for type T, the += operator will work:
 *  array(2) += 2.1;
 *
 *  If raBOUND is defined, bounds checking is done, except for slicing operations.
 *
 *  To slice, use one of the sliceRA methods to create a gslice into the valarray
 *  directly.  One or more of the arguments will be array.ALL.  An
 *  ra::val[gslice]  can be used as an lvalue.
 *
 */
#ifndef Lpp_ra_H
#define Lpp_ra_H

#define   raBOUND

#include <iostream>
#include <valarray>
using std::valarray;        using std::gslice;



template <class T> class ra {
public:
    // Data members, no setters/getters.  Equivalent valarray will be resized
    int ne1, ne2, ne3, ne4;            // int rather than size_t to compare vs off1 or i1
    valarray<T> val;                // Public for Lbins::FindBin, Lmat, others.
    static inline struct oneval {} ALL;

    // Destructor is not needed; valarray, like the container classes, can
    // destroy itself in the same manner as e.g. int or double members.
    ~ra() {}
    
    // Set/get-base methods:
    void setbase(int  o1) {
        this->off1 = o1;
    }
    void getbase(int& o1) {
        o1 = this->off1;
    }
    void setbase(int  o1, int  o2) {
        this->off1 = o1;  this->off2 = o2;
    }
    void getbase(int& o1, int& o2) {
        o1 = this->off1;  o2 = this->off2;
    }
    void setbase(int  o1, int  o2, int  o3) {
        this->off1 = o1;  this->off2 = o2;  this->off3 = o3;
    }
    void getbase(int& o1, int& o2, int& o3) {
        o1 = this->off1;  o2 = this->off2;  o3 = this->off3;
    }
    void setbase(int  o1, int  o2, int  o3, int  o4) {
        this->off1 = o1;  this->off2 = o2;  this->off3 = o3;  this->off4 = o4;
    }
    void getbase(int& o1, int& o2, int& o3, int& o4) {
        o1 = this->off1;  o2 = this->off2;  o3 = this->off3;  o4 = this->off4;
    }
 
    // A few simple math operators.  Do not allow such math
    // with an ra<T> arguments for fear of dimensional perils.
    ra<T>& operator+= (T rhs) {
        T* p = ptr +ntot;
        while (p > ptr) *--p += rhs;
        return *this;
    }
    ra<T>& operator-= (T rhs) {
        T* p = ptr +ntot;
        while (p > ptr) *--p -= rhs;
        return *this;
    }
    ra<T>& operator*= (T rhs) {
        T* p = ptr +ntot;
        while (p > ptr) *--p *= rhs;
        return *this;
    }
    ra<T>& operator/= (T rhs) {
        T* p = ptr +ntot;
        while (p > ptr) *--p /= rhs;
        return *this;
    }

    // Ick.  Sometimes a valarray operation can put the valarray in a
    // new memory location... valarray::operator= did this to me once.
    void fixptr() {ptr = &val[0];}

private:
    long int ntot;
    int ntotBits = 4*sizeof(ntot);
    T* ptr;
    int off1,off2,off3,off4;






// =============================================================================
// Constructors defined here ===================================================
// =============================================================================
public:
    // Simple constructors: ra(4) creates 4 element array, indexed 1,2,3,4.
    explicit ra(int n1) {
        if (n1 < 1) {
            std::cout << "Bad argument in ra::ra(int)" << std::endl;
            abort();
        }
        ne1 = n1;
        ne2  = 0;    ne3  = 0;    ne4  = 0;    ntot = n1;
        off1 = 1;    off2 = 1;    off3 = 1;    off4 = 1;
        val.resize(ntot);
        fixptr();
        return;
    }
    ra(int n1, int n2) {
        if (n1 < 1 || n2 < 1) {
            std::cout << "Bad argument in ra::ra(int,int)" << std::endl;
            abort();
        }
        #ifdef raBOUND
        // Check for integer multiply overflow
        if ( log2(n1)+log2(n2) > 8*sizeof(ntot) ) {
            std::cout << "Overflow of ra size in ra::ra(int,int)" << std::endl;
            abort();
        }
        #endif
        ne1 =  n1;    ne2 = n2;   ne3  = 0;    ne4  = 0;    ntot = n1*n2;
        off1 = 1;    off2 = 1;    off3 = 1;    off4 = 1;
        val.resize(ntot);
        fixptr();
        return;
    }
    ra(int n1, int n2, int n3) {
        if (n1 < 1 || n2 < 1 || n3 < 1) {
            std::cout << "Bad argument in ra::ra(int.int,int)" << std::endl;
            abort();
        }
        #ifdef raBOUND
        // Check for integer multiply overflow
        if ( log2(n1)+log2(n2)+log2(n3) > 8*sizeof(ntot) ) {
            std::cout << "Overflow of ra size in ra::ra(int,int,int)" << std::endl;
            abort();
        }
        #endif
        ne1 = n1;    ne2 = n2;   ne3 = n3;    ne4 = 0;    ntot = n1*n2*n3;
        off1 = 1;    off2 = 1;    off3 = 1;    off4 = 1;
        val.resize(ntot);
        fixptr();
        return;
    }
    ra(int n1, int n2, int n3, int n4) {
        if (n1 < 1 || n2 < 1 || n3 < 1 || n4 < 1) {
            std::cout << "Bad argument in ra::ra(int,int,int,int)" << std::endl;
            abort();
        }
        #ifdef raBOUND
        // Check for integer multiply overflow
        if ( log2(n1)+log2(n2)+log2(n3)+log2(n4) > 8*sizeof(ntot) ) {
            std::cout << "Overflow of ra size in ra::ra(int,int,int,int)" << std::endl;
            abort();
        }
        #endif
        ne1 = n1;    ne2 = n2;   ne3 = n3;    ne4 = n4;    ntot = n1*n2*n3*n4;
        off1 = 1;    off2 = 1;    off3 = 1;    off4 = 1;
        val.resize(ntot);
        fixptr();
        return;
    }

    // Copy constructor (from ra objects)
    ra(ra<T> const& rhs) {
        comcopy(rhs);
    }

    // Assignment constructor, scalar assignment constructor both deep copy
    // Neither constructor checks for dimensions of result
    ra<T>& operator= (ra<T> const& rhs) {
        if (this != &rhs) comcopy(rhs);
        return *this;
    }
    ra<T>& operator= (T rhs) {
        T* p = ptr +ntot;                    // Start one size_tess past array end
        while (p > ptr) *--p = rhs;         // Don't mess with off1 or ne2 etc.
        return *this;
    }

    
private:
    // Copy routine common to copy and assignment constructors
    void comcopy(ra<T> const& rhs) {
        ne1  = rhs.ne1;        ne2  = rhs.ne2;        ne3  = rhs.ne3;        ne4  = rhs.ne4;
        off1 = rhs.off1;    off2 = rhs.off2;    off3 = rhs.off3;    off4 = rhs.off4;
        ntot = rhs.ntot;
        val.resize(ntot);
        val = rhs.val;                        // Deep copy via valarray functions
        fixptr();
    }



// =============================================================================
// Indexing operators defined here =============================================
// =============================================================================
public:
    // Fast indexing with FORTRAN-style () notation; can't use [] because C++
    // requires only 1 argument to the overloaded [] operator.  The keyword
    // "inline" just a suggestion to the compiler.
    inline T& operator() (int i1) const {
        #ifdef raBOUND
        if (ne2 > 0) {
            std::cout << "ra incorrectly indexed at ra::operator(int)" << std::endl;
            abort();
        }
        if (i1 > (ne1+off1-1) || i1 < off1) {
            std::cout << "ra index out of bounds at ra::operator(int)" << std::endl;
            abort();
        }
        #endif
        return ptr[i1-off1];
    }
    inline T& operator() (int i1, int i2) const {
        #ifdef raBOUND
        if (ne2 == 0 || ne3 > 0) {
            std::cout << "ra incorrectly indexed at ra::operator(int,int)" << std::endl;
            abort();
        }
        if (i1 > (ne1+off1-1) || i2 > (ne2+off2-1) || i1 < off1 || i2 < off2) {
            std::cout << "ra index out of bounds at ra::operator(int,int)" << std::endl;
            abort();
        }
        #endif
        return ptr[(i1-off1) +ne1*(i2-off2)];
    }
    inline T& operator() (int i1, int i2, int i3) const {
        #ifdef raBOUND
        if (ne3 == 0 || ne4 > 0) {
            std::cout << "ra incorrectly indexed at ra::operator(int,int,int)" << std::endl;
            abort();
        }
        if (i1 > (ne1+off1-1) || i2 > (ne2+off2-1) || i3 > (ne3+off3-1)) {
            std::cout << "ra index out of bounds at ra::operator(int,int,int)" << std::endl;
            abort();
        }
        if (i1 < off1 || i2 < off2 || i3 < off3)  {
            std::cout << "ra index out of bounds at ra::operator(int,int,int)" << std::endl;
            abort();
        }
        #endif
        return ptr[(i1-off1) +ne1*((i2-off2) +ne2*(i3-off3))];
    }
    inline T& operator() (int i1, int i2, int i3, int i4) const {
        #ifdef raBOUND
        if (ne4 == 0) {
            std::cout << "ra incorrectly indexed at ra::operator(int,int,int,int)" << std::endl;
            abort();
        }
        if (i1 > (ne1+off1-1) || i2 > (ne2+off2-1) ||
            i3 > (ne3+off3-1) || i4 > (ne4+off4-1) ) {
            std::cout << "ra index out of bounds at ra::operator(int,int,int,int)" << std::endl;
            abort();
        }
        if (i1 < off1 || i2 < off2 || i3 < off3 || i4 < off4) {
            std::cout << "ra index out of bounds at ra::operator(int,int,int,int)" << std::endl;
            abort();
        }
        #endif
        return ptr[(i1-off1) +ne1*((i2-off2) +ne2*((i3-off3) +ne3*(i4-off4)))];
    }

    
    


// =============================================================================
// Methods to copy from Kernighan & Ritchie arrays and from valarrays ==========
// =============================================================================
   void fillFromC(T KandR[], size_t nCopy) {
        // Fill an ra from a 1-d K & R style array
        #ifdef raBOUND
        if (nCopy != static_cast<size_t>(ntot)) {
            std::cout << "Overflow of ra size in ra::fillFromC(T*)" << std::endl;
            abort();
        }
        #endif

        for (int i=0; i<ntot; ++i) {
            ptr[i] = KandR[i];
        }
    }

   void fillFromValarray(valarray<T>& inVal) {
        // Fill an ra from a std::valarray
        #ifdef raBOUND
        if (inVal.size() != ntot) {
            std::cout << "Overflow of ra size in ra::fillFromC(valarray<T>&)" << std::endl;
            abort();
        }
        #endif

        for (size_t i=0; i<ntot; ++i) {
            ptr[i] = inVal[i];
        }
    }





// =============================================================================
// Slicing operators defined here ==============================================
// These slicing operators construct gslices and do not change the offsets or ==
// dimensions of the ra<>.  They should be pretty fast. ========================
// BOUNDS ARE NEVER CHECKED FOR SLICING! =======================================
//
    int const ne0 = 1;      // makes the patterns in the following code clearer
//
//  Here are the patterns:
//      1) The value of start is the indexing formula from operator() but with
//          the oneval terms removed.
//      2) The number of entries in the length & start arguments, ie the number
//          of layers in the gslice, is the number of oneval arguments.
//      3) The length is filled with ne? values, where ? are the oneval indices
//          in right to left, ie decreasing numeric order by gslice layer.
//      4) The stride is filled with products of ne? values from the ne? value
//          one less than that in the corresponding length, down to ne0.
// =============================================================================

    gslice sliceRA(oneval c1, int i2) const {
        size_t start    =  static_cast<size_t>(ne1*(i2-off2));
        size_t length[] = {static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
    gslice sliceRA(int i1, oneval c2) const{
        size_t start    =  static_cast<size_t>(i1-off1);
        size_t length[] = {static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
//  ---------------------------------------------------------------------------
    gslice sliceRA(oneval c1, oneval c2, int i3) const {
        size_t start    =  static_cast<size_t>(ne1*ne2*(i3-off3));
        size_t length[] = {static_cast<size_t>(ne2),static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(oneval c1, int i2, oneval c3) const {
        size_t start    =  static_cast<size_t>(ne1*(i2-off2));
        size_t length[] = {static_cast<size_t>(ne3),    static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(oneval c1, int i2, int i3) const {
        // New code
        size_t start    =  static_cast<size_t>(ne1*(i2-off2) +ne1*ne2*(i3-off3)) ;
        size_t length[] = {static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
    gslice sliceRA(int i1, oneval c2, oneval c3) const {
        size_t start    =  static_cast<size_t>(i1-off1);
        size_t length[] = {static_cast<size_t>(ne3),    static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2),static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(int i1, oneval c2, int i3) const {
        // New code
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*ne2*(i3-off3)) ;
        size_t length[] = {static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
    gslice sliceRA(int i1, int i2, oneval c3) const {
        // New code
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*(i2-off2)) ;
        size_t length[] = {static_cast<size_t>(ne3)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
//  ---------------------------------------------------------------------------
    gslice sliceRA(oneval c1, oneval c2, oneval c3, int i4) const {
        size_t start    =  static_cast<size_t>(ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne3),    static_cast<size_t>(ne2),static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2),static_cast<size_t>(ne1),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,3),valarray<size_t>(stride,3));
    }
    gslice sliceRA(oneval c1, oneval c2, int i3, oneval c4) const {
        size_t start    =  static_cast<size_t>(ne1*ne2*(i3-off3));
        size_t length[] = {static_cast<size_t>(ne4),        static_cast<size_t>(ne2),static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3),static_cast<size_t>(ne1),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,3),valarray<size_t>(stride,3));
    }
    gslice sliceRA(oneval c1, oneval c2, int i3, int i4) const {
        size_t start    =  static_cast<size_t>(ne1*ne2*(i3-off3) +ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne2),static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(oneval c1, int i2, oneval c3, oneval c4) const {
        size_t start    =  static_cast<size_t>(ne1*(i2-off2));
        size_t length[] = {static_cast<size_t>(ne4),        static_cast<size_t>(ne3),    static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3),static_cast<size_t>(ne1*ne2),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,3),valarray<size_t>(stride,3));
    }
    gslice sliceRA(oneval c1, int i2, oneval c3, int i4) const {
        size_t start    =  static_cast<size_t>(ne1*(i2-off2) +ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne3),    static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne2*ne1),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(oneval c1, int i2, int i3, oneval c4) const {
        size_t start    =  static_cast<size_t>(ne1*(i2-off2) +ne1*ne2*(i3-off3));
        size_t length[] = {static_cast<size_t>(ne4),        static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3),static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(oneval c1, int i2, int i3, int i4) const {
        size_t start    =  static_cast<size_t>(ne1*(i2-off2) +ne1*ne2*(i3-off3) +ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne1)};
        size_t stride[] = {static_cast<size_t>(ne0)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
    gslice sliceRA(int i1, oneval c2, oneval c3, oneval c4) const {
        size_t start    =  static_cast<size_t>(i1-off1);
        size_t length[] = {static_cast<size_t>(ne4),        static_cast<size_t>(ne3),    static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3),static_cast<size_t>(ne1*ne2),static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,3),valarray<size_t>(stride,3));
    }
    gslice sliceRA(int i1, oneval c2, oneval c3, int i4) const {
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne3),    static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2),static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(int i1, oneval c2, int i3, oneval c4) const {
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*ne2*(i3-off4));
        size_t length[] = {static_cast<size_t>(ne4),        static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3),static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(int i1, oneval c2, int i3, int i4) const {
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*ne2*(i3-off3) +ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne2)};
        size_t stride[] = {static_cast<size_t>(ne1)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
    gslice sliceRA(int i1, int i2, oneval c3, oneval c4) const {
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*(i2-off2));
        size_t length[] = {static_cast<size_t>(ne4),        static_cast<size_t>(ne3)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3),static_cast<size_t>(ne1*ne2)};
        return gslice(start,valarray<size_t>(length,2),valarray<size_t>(stride,2));
    }
    gslice sliceRA(int i1, int i2, oneval c3, int i4) const {
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*(i2-off2) +ne1*ne2*ne3*(i4-off4));
        size_t length[] = {static_cast<size_t>(ne3)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }
    gslice sliceRA(int i1, int i2, int i3, oneval c4) const {
        size_t start    =  static_cast<size_t>((i1-off1) +ne1*(i2-off2) +ne1*ne2*(i3-off3));
        size_t length[] = {static_cast<size_t>(ne4)};
        size_t stride[] = {static_cast<size_t>(ne1*ne2*ne3)};
        return gslice(start,valarray<size_t>(length,1),valarray<size_t>(stride,1));
    }

};


#endif
