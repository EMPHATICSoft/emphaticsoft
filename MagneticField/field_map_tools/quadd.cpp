/*  L++ quadd
 *  Created by Leo Bellantoni on 12/21/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  Arithmetic in quadrature, done in a sound way.  N.b. The standard library
 *  <numeric> has a template function for inner_products.  Signatures with
 *  float and doubles are inlined in the header file; here are the long double
 *  versions which are not suitable for inlining.  Also in the header file is
 *  the three-argument Qadd template.
 *
 */
#include "quadd.h"

// For long double, use a modification of the algorithm of Cleve Moler and 
// Donald Morrison, IBM J. Res. Develop, v27,n6, (1983) p 577-581.
long double Qadd(long double x, long double y) {
    long double p,r,s,t,u, r2;
    p = (std::abs(x)>std::abs(y) ? std::abs(x) : std::abs(y));
    if (p == 0.0) return p;
    r = (std::abs(x)<std::abs(y) ? std::abs(x) : std::abs(y));
    r = r/p;   r = r*r;
    while (true){
        t = 4.0 + r;
        if (t == 4.0) return p;
        s = r/t;
        u = 1.0 + 2.0*s;
        p = u*p;
        r2 = (s/u);
        r = r2*r2 *r;
    }
}

// For subtraction in quadrature, precision limit is when 1+2S is exactly the
// same as 1.0d0.  The convergence of this algorithm can be as much as 100,000 
// times numeric_limits<long double>::epsilon away from the correct result.  A
// one-pass Newton-Raphson iteration has been added to the Moler-Morrison
// algorithm to help with this.
long double Qsub(long double x, long double y) {
    long double p,q,r,s,t, r2,r3;
    p = std::abs(x) - std::abs(y);
    if (p < 0.0) {
        std::cout << "Can not subtract long double " << y << " from long double "
            << x << " in quadrature" << std::endl;
        abort();
    } else if (p == 0.0) {
        return 0.0;
    }
    p = std::abs(x);   q = std::abs(y);   r3 = -1.0;
    while (r3 != p) {
        r2 = (q/p);   r = -r2*r2;
        s = r/(4.0+r);
        r3 = p;
        p = (1.0 + 2.0*s)*p;
        q = s*q;
    }
    // Linear extrap here.  Evaluation order in final expression very non-trivial!
    t = Qadd(p,y);
    return p - t*((t-std::abs(x))/p);
}
