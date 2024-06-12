/* L++ unRea
 * Created by Leo Bellantoni on 16 Sep 2020
 * Copyright FRA 2020.  All rights reserved.
 *
 * A template for float, double or long double implementation of real
 * numbers with uncertainties.  Limited in that one can't mix-n-match
 * types eg multiply an unReal<float> by an unReal<double>.  But what
 * would you want to do that for, anyway?  At least it checks that the
 * template argument is a float, double or long double.
 *
 * Needs still sums and differences in quadrature
 *
 */
#pragma once



#include <type_traits>
#include "quadd.h"



template <typename T> class unReal {
public:
    T val;
    T err;



    // Default destructor should be OK.  Need initializing, copy and
    // assignment constructors.  Dynamic testing for correct type, not static
    unReal() {
       if ( std::is_same<T,float>::value ||
            std::is_same<T,double>::value ||
            std::is_same<T,long double>::value ) {
           val = 0;   err = 0;
       } else {
           LppUrk.LppIssue(118,"unReal()");
       }
       return;
   }
    unReal(T val_in, T err_in) {
        if ( err_in < T(0.0) ) LppUrk.LppIssue(118,"unReal(T,T) [1]");
        if ( std::is_same<T,float>::value ||
             std::is_same<T,double>::value ||
             std::is_same<T,long double>::value ) {
            val = val_in;   err = err_in;
        } else {
            LppUrk.LppIssue(118,"unReal(T,T) [2]");
        }
        return;
    }
    unReal(unReal<T> const& unReal_in) {
        *this = unReal_in;
        return;
    }
    unReal<T>& operator=(unReal<T> const& rhs) {
        if (this != &rhs) {
            val = rhs.val;
            err = rhs.err;
        }
        return *this;
    }



    // Simple operators which assume zero correlations
    unReal<T> operator-() const {
        return unReal<T>(-val,err);
    }
    unReal<T> operator+(unReal const& rhs) {
        return sumWithCorrel( *this, rhs, 0.0);
    }
    unReal<T> operator-(unReal const& rhs) {
        return sumWithCorrel( *this, -(rhs), 0.0);
    }
    unReal<T> operator*(unReal const& rhs) {
        return productWithCorrel( *this, rhs, 0.0);
    }
    unReal<T> operator/(unReal const& rhs) {
        return ratioWithCorrel( *this, rhs, 0.0);
    }



    // Complicated operations with non-zero correlations
    friend unReal<T> sumWithCorrel(unReal<T> a, unReal<T> b, T rho) {
        if (abs(rho) > 1) LppUrk.LppIssue(119,"unReal::sumWithCorrel");
        T combVal = a.val + b.val;
        // Employ the better roundoff of Qadd algorithms
        T combErr = Qadd( a.err, b.err);
        // Although if rho != 0, there is some error in this sqrt.
        T correl = sqrt( T(2.0) * abs(rho) * (a.err*b.err) );
        if (rho>0) combErr = Qadd(combErr,correl);
        if (rho<0) combErr = Qsub(combErr,correl);
        return unReal<T>( combVal, combErr );
    }
    friend unReal<T> productWithCorrel(unReal<T> a, unReal<T> b, T rho) {
        if (abs(rho) > 1) LppUrk.LppIssue(119,"unReal::sumWithCorrel");
        T combVal = a.val * b.val;
        // Employ the better roundoff of Qadd algorithms
        T combErr = Qadd( a.err/a.val, b.err/b.val);
        // Although if rho != 0, there is some error in this sqrt.
        T correl = sqrt(T(2.0) * abs(rho*(a.err/a.val)*(b.err/b.val)) );
        combErr = Qadd(combErr,correl);
        combErr *= abs(combVal);
        return unReal<T>( combVal, combErr );
    }
    friend unReal<T> ratioWithCorrel(unReal<T> a, unReal<T> b, T rho) {
        if (abs(rho) > 1) LppUrk.LppIssue(119,"unReal::ratioWithCorrel");
        T combVal = a.val / b.val;
        // Employ the better roundoff of Qadd algorithms
        T combErr = Qadd( a.err/a.val, b.err/b.val);
        // Although if rho != 0, there is some error in this sqrt.
        T correl = sqrt(T(2.0) * abs(rho*(a.err/a.val)*(b.err/b.val)) );
        combErr = Qsub(combErr,correl);
        combErr *= abs(combVal);
        return unReal<T>( combVal, combErr );
    }



    // A few special functions
    friend unReal<T> log(unReal<T> a) {
        T retVal = log(a.val);
        T retErr = a.err/a.val;
        return unReal<T>( retVal, retErr );
    }
    friend unReal<T> exp(unReal<T> a) {
        T retVal = exp(a.val);
        T retErr = a.err * abs(a.val);
        return unReal<T>( retVal, retErr );
    }
    friend unReal<T> sin(unReal<T> a) {
        T retVal = sin(a.val);
        T retErr = abs(cos(a.val) * a.err);
        return unReal<T>( retVal, retErr );
    }
    friend unReal<T> cos(unReal<T> a) {
        T retVal = cos(a.val);
        T retErr = abs(sin(a.val) * a.err);
        return unReal<T>( retVal, retErr );
    }
};
