/*  L++ Bessel functions of the 1st kind
 *  Created by Leo Bellantoni on 25 Oct 2023.
 *  Copyright 2023 FRA.  Well, those that exist I guess.
 *
 */
 #include "Bessel.h"
 




double BessJ(int n, double x) {
    if (n<0 || x<0.0) LppUrk.LppIssue(0, "BessJ(int,double)");

    double const ScaleUP = 1.0e+10;

    // Order 0 or 1, end of recursion
    if (n==0) return BessJ0(x);
    if (n==1) return BessJ1(x);
    // Simple case here.
    if (x==0.0) return 0.0;
    double twOverX = 2.0/x;
    double BJ0,BJ1;

    if (x>n) {
        // Upward recurrence from J0 or J1
        BJ0 = BessJ0(x);
        BJ1 = BessJ1(x);
        for (int j=1; j<n; ++j) {
            double tmp = j*twOverX*BJ1 -BJ0;
            BJ0 = BJ1;
            BJ1 = tmp;
        }
        return BJ1;
    } else {
        // Downward recurrence from "an even M".
        int M = n + nint(sqrt(40*n)); // 40 here from PTV&F
        // make M even here
        M = 2*fint(M/2);
        double uNorm, Sum, tmp;
        int jSum = 0;
        uNorm = Sum = tmp = 0.0;
        BJ1 = 1.0;

        for (int j=M; j>=1; --j) {
            BJ0 = j*twOverX*BJ1 -tmp;
            tmp = BJ1;
            BJ1 = BJ0;
            if (fabs(BJ1)>ScaleUP) {
                // Renormalize to prevent overflows
                BJ1   /= ScaleUP;
                tmp   /= ScaleUP;
                uNorm /= ScaleUP;
                Sum   /= ScaleUP;
            }
            if (jSum!=0) Sum  = Sum +BJ1;
            jSum = 1 -jSum;
            if (j==n) uNorm = tmp;
        }
        // Normalization happens here
        Sum = 2*Sum -BJ1;
        return uNorm/Sum;
    }
}



double BessY(int n, double x) {
    if (n<0 || x<0.0) LppUrk.LppIssue(0, "BessY(int,double)");

    // Order 0 or 1, end of recursion
    if (n==0) return BessY0(x);
    if (n==1) return BessY1(x);
    double twOverX = 2.0/x;
    double BY0 = BessY0(x);
    double BY1 = BessY1(x);

    for (int j=1; j<n; ++j) {
        double tmp = j*twOverX*BY1 -BY0;
        BY0 = BY1;
        BY1 = tmp;
    }
    return BY1;
}



double BessI(int n, double x) {
    if (n<0 || x<0.0) LppUrk.LppIssue(0, "BessI(int,double)");

    double const ScaleUP = 1.0e+10;

    // Order 0 or 1, end of recursion
    if (n==0) return BessI0(x);
    if (n==1) return BessI1(x);
    // Simple case here.
    if (x==0.0) return 0.0;
    double twOverX = 2.0/x;
    double uNorm, tmp;
    uNorm = tmp = 0.0;
    
    double BI0, BI1;
    BI1 = 1.0;

    int M = 2*(n + int(sqrt(40*n))); // 40 here from PTV&F
    for (int j=M; j>=1; --j) {
        BI0 = j*twOverX*BI1 +tmp;
        tmp =BI1;
        BI1 =BI0;
        if (fabs(BI1)>ScaleUP) {
            // Renormalize to prevent overflows
            uNorm /= ScaleUP;
            BI1   /= ScaleUP;
            tmp   /= ScaleUP;
        }
        if (j==n) uNorm = tmp;
    }
    // Normalization with BessI0 & BI1 (which has ScaleUP^^n)
    return uNorm * BessI0(x) / BI1;
}



double BessK(int n, double x) {
    if (n<0 || x<=0.0) LppUrk.LppIssue(0, "BessK(int,double)");

    // Order 0 or 1, end of recursion
    if (n==0) return BessK0(x);
    if (n==1) return BessK1(x);
    double twOverX = 2.0/x;
    double BK0 = BessK0(x);
    double BK1 = BessK1(x);

    for (int j=1; j<n; ++j) {
        double tmp = j*twOverX*BK1 +BK0;
        BK0 = BK1;
        BK1 = tmp;
    }
    return BK1;
}





//==============================================================================
// BessJ, BessY, BessI, BessK recurse to the following.  You don't need to call
// them directly although you can if you want.
double BessJ0(double x) {
    // Polynomial approximation to Bessel function J_0(x), for x>=0;
    if (x<0.0) LppUrk.LppIssue(0, "BessJ0(double)");
    double retval, y, ans1,ans2;
     if (x < 8.0) {
        y = SQR(x);
        ans1 =57568490574.0 +y*(-13362590354.0 +y*(651619640.7 +y*(-11214424.18 +y*(77392.33017 +y*(-184.9052456)))));
        ans2 =57568490411.0 +y*(1029532985.0 +y*(9494680.718 +y*(59272.64853 +y*(267.8532712 +y*1.0))));
        retval = ans1/ans2;
    } else {
        double z  = 8.0/x;
        y  = SQR(z);
        double xx = x-0.785398164;
        ans1=1.0 +y*(-0.1098628627e-2 +y*(0.2734510407e-4 +y*(-0.2073370639e-5 +y*0.2093887211e-6)));
        ans2 = -0.1562499995e-1 +y*(0.1430488765e-3 +y*(-0.6911147651e-5+ y*(0.7621095161e-6 -y*0.934945152e-7)));
        retval = sqrt(0.636619772/x) * (cos(xx)*ans1 -z*sin(xx)*ans2);
    }
    return retval;
}

double BessJ1(double x) {
    // Polynomial approximation to Bessel function J_1(x), for x>=0;
    if (x<0.0) LppUrk.LppIssue(0, "BessJ1(double)");
    double retval, y,z,xx, ans1,ans2;
    if (x < 8.0) {
        y = SQR(x);
        ans1 = x *
            (72362614232.0 +y*(-7895059235.0 +y*(242396853.1 +y*(-2972611.439 +y*(15704.48260 +y*(-30.16036606))))));
        ans2 = 144725228442.0 +y*(2300535178.0 +y*(18583304.74 +y*(99447.43394 +y*(376.9991397 +y*1.0))));
        retval = ans1 / ans2;
    } else {
        z  = 8.0/x;
        y  = SQR(z);
        xx = x -2.356194491;
        ans1 = 1.0 +y*(0.183105e-2 +y*(-0.3516396496e-4 +y*(0.2457520174e-5 +y*(-0.240337019e-6))));
        ans2 = 0.04687499995 +y*(-0.2002690873e-3 +y*(0.8449199096e-5 +y*(-0.88228987e-6 +y*0.105787412e-6)));
        retval = sqrt(0.636619772/x) * (cos(xx)*ans1 -z*sin(xx)*ans2);
    }
    return retval;
}
      
double BessY0(double x) {
    double retval, y,z,xx, ans1,ans2;
    if (x < 8.0) {
        y = SQR(x);
        ans1 = -2957821389.0 +y*(7062834065.0 +y*(-512359803.6 +y*(10879881.29+ y*(-86327.92757+y*228.4622733))));
        ans2 = 40076544269.0 +y*(745249964.8 +y*(7189466.438 +y*(47447.26470 +y*(226.1030244+y*1.0))));
        retval = (ans1/ans2) +0.636619772*BessJ0(x)*log(x);
    } else {
        z  = 8.0/x;
        y  = SQR(z);
        xx = x -0.785398164;
        ans1 = 1.0 +y*(-0.1098628627e-2 +y*(0.2734510407e-4 +y*(-0.2073370639e-5 +y*0.2093887211e-6)));
        ans2 = -0.1562499995e-1+ y*(0.1430488765e-3 +y*(-0.6911147651e-5 +y*(0.7621095161e-6 +y*(-0.934945152e-7))));
        retval = sqrt(0.636619772/x) * (sin(xx)*ans1+z*cos(xx)*ans2);
    }
    return retval;
}

double BessY1(double x){
    // Polynomial approximation to Bessel function Y_1(x), for x>=0; uses BessJ1(x).
    if (x<0.0) LppUrk.LppIssue(0, "BessY1(double)");
     
    double retval, y,z,xx,ans1,ans2;
    if (x < 8.0) {
        y = SQR(x);
        ans1 = x *
            (-0.4900604943e13 +y*(0.1275274390e13 +y*(-0.5153438139e11 +y*(0.7349264551e9 +y*(-0.4237922726e7 +y*0.8511937935e4)))));
        ans2 = 0.2499580570e14 +y*(0.4244419664e12 +y*(0.3733650367e10 +y*(0.2245904002e8 +y*(0.1020426050e6 +y*(0.3549632885e3+y)))));
        retval = (ans1/ans2) +0.636619772 * (BessJ1(x)*log(x) -1.0/x);
    } else {
        z  = 8.0/x;
        y  = SQR(z);
        xx = x -2.356194491;
        ans1 = 1.0+ y*(0.183105e-2+ y*(-0.3516396496e-4 +y*(0.2457520174e-5 +y*(-0.240337019e-6))));
        ans2 = 0.04687499995 +y*(-0.2002690873e-3 +y*(0.8449199096e-5 +y*(-0.88228987e-6 +y*0.105787412e-6)));
        retval = sqrt(0.636619772/x) * (sin(xx)*ans1 +z*cos(xx)*ans2);
    }
    return retval;
}

double BessI0(double x) {
    // Polynomial approximation to Bessel function I_0(x), for x>=0
    if (x<0.0) LppUrk.LppIssue(0, "BessI0(double)");

    double retval, y;
    if (x < 3.75) {
        y = SQR(x/3.75);
        retval =1.0 +y*(3.5156229 +y*(3.0899424 +y*(1.2067492 +y*(0.2659732 +y*(0.360768e-1 +y*0.45813e-2)))));
    } else {
        y = 3.75/x;
        retval = (exp(x)/sqrt(x)) *
            (0.39894228 +y*(0.1328592e-1 +y*(0.225319e-2+ y*(-0.157565e-2 +y*(0.916281e-2 +y*(-0.2057706e-1 +y*(0.2635537e-1 +y*(-0.1647633e-1 +y*0.392377e-2))))))));
    }
    return retval;
}

double BessI1(double x) {
    // Polynomial approximation to Bessel function I_1(x), for x>=0
    if (x<0.0) LppUrk.LppIssue(0, "BessI1(double)");
    double retval, y;
    if (x < 3.75) {
        y = SQR(x/3.75);
        retval = x *
            (0.5 +y*(0.87890594 +y*(0.51498869 +y*(0.15084934 +y*(0.2658733e-1 +y*(0.301532e-2 +y*0.32411e-3))))));
    } else {
        y = 3.75/x;
        retval = 0.2282967e-1 +y*(-0.2895312e-1+y*(0.1787654e-1 -y*0.420059e-2));
        retval = 0.39894228 +y*(-0.3988024e-1 +y*(-0.362018e-2 +y*(0.163801e-2 +y*(-0.1031555e-1+y*retval))));
        retval *= (exp(x)/sqrt(x));
    }
    return retval;
}

double BessK0(double x) {
    // Polynomial approximation to Bessel function K_0(x), for x>0  Calls BessI0
    if (x<=0.0) LppUrk.LppIssue(0, "BessK0(double)");
    double retval, y;
    if (x <= 2.0) {
        y = SQR(x) / 4.0;
        retval = (-log(x/2.0) * BessI0(x))
            + (-0.57721566 +y*(0.42278420 +y*(0.23069756 +y*(0.3488590e-1 +y*(0.262698e-2 +y*(0.10750e-3 +y*0.74e-5))))));
    } else {
        y = 2.0 / x;
        retval = (exp(-x) / sqrt(x)) *
            (1.25331414 +y*(-0.7832358e-1 +y*(0.2189568e-1 +y*(-0.1062446e-1 +y*(0.587872e-2 +y*(-0.251540e-2 +y*0.53208e-3))))));
    }
    return retval;
}

double BessK1(double x) {
    // Polynomial approximation to Bessel function K_1(x), for x>=0  Calls BessI1
    if (x<=0.0) LppUrk.LppIssue(0, "BessK1(double)");
    double retval, y;
    if (x <= 2.0) {
        y = SQR(x) / 4.0;
        retval = (log(x/2.0) * BessI1(x))
            +(1.0/x) * (1.0 +y*(0.15443144 +y*(-0.67278579 +y*(-0.18156897 +y*(-0.1919402e-1 +y*(-0.110404e-2 +y*(-0.4686e-4)))))));
    } else {
        y = 2.0 / x;
        retval = (exp(-x)/sqrt(x)) *
            (1.25331414 +y*(0.23498619 +y*(-0.3655620e-1 +y*(0.1504268e-1 +y*(-0.780353e-2 +y*(0.325614e-2 +y*(-0.68245e-3)))))));
    }
    return retval;
}
