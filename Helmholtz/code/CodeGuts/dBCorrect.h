//
//  dBCorrect.h
//  L++
//
//  Created by Leo Bellantoni on 4 Jan 2024.
//  These are ad-hoc corrections to the reconstructed (interpolated) field,
//  which should be added to the results of the Helmholtz decomposition
//
#pragma once





double cornerCorrect(double x, double y, double magB) {
    double xHat = abs( (abs(x) -0.010)/ 0.005 );
    double yHat = abs( (abs(y) -0.010)/ 0.005 );
    double correct = fmax(0.0, 1.0 -fmax(xHat,yHat));
    correct *= -10.9e-3*magB;
    return correct;
}

double dByCorrect(double x, double y, double magB) {
    double retval = cornerCorrect(x, y, magB);
    if (magB < 1.25) {
        retval += 0;
    } else if (magB < 1.44) {
        retval += -( 92.8 -72.63*magB)/1000.0;
    } else if (magB < 1.56) {
        retval += -(-39.4 +19.17*magB)/1000.0;
    } else {
        retval += -(206.7 -138.57*magB)/1000.0;
    }
    return retval;
}

double dBxCorrect(double x, double y, double magB) {
    // Bounds of 4th order Chebyshev polynomial
    double lo = 0.0;        double hi = 1.7;
    double X = (magB -lo)/(hi -lo);
    double X2 = X*X;
    double X3 = X2*X;
    double X4 = X2*X2;
    
    // Results of the fit:
    double c0 = -0.103869;
    double c1 =  0.175718;
    double c2 = -0.115028;
    double c3 =  0.047777;
    double c4 = -0.010935;

    double retval = c0;
    retval += c1*( X );
    retval += c2*( 2.0*X2 -1.0);
    retval += c3*( 4.0*X3 -3.0*X);
    retval += c4*( 8.0*X4 -8.0*X2 +1.0);

    return -retval;
}
