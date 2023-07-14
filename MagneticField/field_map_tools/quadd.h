/*  L++ quadd
 *  Created by Leo Bellantoni on 12/21/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  Arithmetic in quadrature, done in a sound way.  N.b. The standard library
 *  <numeric> has a template function for inner_products.  Signatures with
 *  float and doubles are inlined here; the cpp file has the long double
 *  versions which are not suitable for inlining.  Also here, the 3-argument
 *  template.
 *
 */
#ifndef LppQuadd_H
#define LppQuadd_H


#include <iostream>
#include <cmath>


// Compiler should do type casts and optimization correctly anyway;
// explicit casts & inlines are only here for documentation/clarity.
inline float Qadd(float x, float y) {
    return float(sqrt( double(x)*double(x) + double(y)*double(y)));
}
inline double Qadd(double x, double y){
    return double(sqrt( ((long double)x)*((long double)x)
                      + ((long double)y)*((long double)y) ));
}
// For double, 3 argument form is oft required
inline double Qadd(double x, double y, double z){
    double absX,absY,absZ;
    absX = fabs(x);     absY = fabs(y);     absZ = fabs(z);
    if ( absX>absY && absX>absZ ) return Qadd(x,Qadd(y,z));
    if ( absY>absX && absY>absZ ) return Qadd(y,Qadd(x,z));
    return Qadd(z,Qadd(x,y));
}
// For long double, constructor-style casts do not seem to work.
long double Qadd(long double x, long double y);

inline float Qsub(float const x, float const y) {
    if (fabs(x) < fabs(y)) {
        std::cout << "Can not subtract float " << y << " from float "
            << x << " in quadrature" << std::endl;
        abort();
    }
    return float(sqrt( double(x)*double(x) - double(y)*double(y) ));
}
inline double Qsub(double const x, double const y) {
    if (fabs(x) < fabs(y)) {
        std::cout << "Can not subtract double " << y << " from double "
            << x << " in quadrature" << std::endl;
        abort();
    }
    return double(sqrt( ((long double)x)*((long double)x) 
                      - ((long double)y)*((long double)y) ));
}
long double Qsub(long double x, long double y);

template <typename T> T Qadd(T x, T y, T z) {
    if       ((x >= y)&&(x >= z)) {
        return Qadd(x,Qadd(y,z));
    } else if ((y >= x)&&(y >= z)) {
        return Qadd(y,Qadd(x,z));
    } else {
        return Qadd(z,Qadd(x,y));
    }
}



#endif
