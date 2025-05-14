/*  L++ integrands.h
 *  Created by Leo Bellantoni on 15 Sep 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  VEGAS algorithm requires functions at global scope.
 *  These are the integrands for doing a Helmholz decomposition
 *  of the EMPHATIC magnetic field
 *
 */
#pragma once

#include "../general/LppGlobals.h"
#include "../io/stringify.h"
#include "../ra/ra.h"
#include "../functions/quadd.h"
#include "../functions/xp.h"
#include "../functionals/VEGAS.h"
#include "../lies/stats.h"

#include "Funcs.h"
#include "FieldMap.h"
#include "RA3.h"





struct integrationParams {double rDisk; double zDiskU; double zDiskD;
                          ra<double>& rEval; FieldMap* map;
                          WhichMap surface; Lv3ax component;};

struct Arguments {ra<double> rEval; FieldMap* fieldMap;
                   double radius;double loZ;double hiZ;};





double vectorIntegrandFunc(double *X, int dim, void* params) {
    /* The integrand function.  Parameters are:
        rDisk:      radius of cylinder
        zDiskU:     upstream end of cylinder
        zDiskD:     downstream end of bestCylinder
        rEval:      the value of r in the integral, which is
                        the point we are doing the extrapolation at
        map:        the integrated measured field map
        surface:    upstream, downstream disk or central cylinder
        component:  x, y, or z
    */

    // Get all the relevant info first.
    integrationParams* paramSet = (integrationParams*)(params);
    double rDisk = paramSet->rDisk;             double zDiskU = paramSet->zDiskU;     double zDiskD = paramSet->zDiskD;
    FieldMap* map = paramSet->map;           // Indirection to avoid deep copy
    WhichMap surface = paramSet->surface;       // Upstream disk, downstream disk or barrel of cylinder
    Lv3ax component = paramSet->component;      // x, y or z component of B(rEval);
    ra<double> rPrime(3);
    RA_1<double> nHat;         nHat = 0.0;



    // If not on the surface, return zero
    double theta, Jacobian;
    switch (surface) {
    case upstream:
        // Integration is in (r, theta)
        theta = X[1];
        rPrime(x) = X[0]*cos(theta);      rPrime(y) = X[0]*sin(theta);      rPrime(z) = zDiskU;
        nHat(z)   = -1.0;
        Jacobian  = X[0];
        break;
    case central:
        // Integration is in (z, theta)
        theta = X[1];
        rPrime(x) = rDisk*cos(theta);     rPrime(y) = rDisk*sin(theta);     rPrime(z) = X[0];
        nHat(x)   = cos(theta);           nHat(y)   = sin(theta);
        Jacobian  = rDisk;
        break;
    case downstream:
        // Integration is in (r, theta)
        theta = X[1];
        rPrime(x) = X[0]*cos(theta);      rPrime(y) = X[0]*sin(theta);      rPrime(z) = zDiskD;
        nHat(z)   = +1.0;
        Jacobian  = X[0];
        break;
    }



    ra<double> BatRprimeR(3);
    if (!map->interpolate(rPrime,BatRprimeR)) {
        cout << "Urk! rPrime = " tabl rPrime(x)           tabl rPrime(y)           tabl rPrime(z) << endl;
        cout << "Urk! rEval  = " tabl paramSet->rEval(x)  tabl paramSet->rEval(y)  tabl paramSet->rEval(z) << endl;
        LppUrk.UsrIssue(-1, Fatal, "vectorIntegrandFunc", "Could not get B field at rPrime");
    }
    RA_1<double> BatRprime;
    BatRprime = BatRprimeR;

    RA_1<double> rMinusRprime;
    for (int i=x; i<=z; ++i) rMinusRprime(i) = paramSet->rEval(i) -rPrime(i);
    double dist = Qadd(rMinusRprime(x),rMinusRprime(y),rMinusRprime(z));
    double outFront = -1.0 / (4*pie*CUBE(dist));

    RA_1<double> integrand;     integrand = 0.0;
    
    integrand  = rMinusRprime;
    integrand *= nHat*BatRprime;
    RA_1<double> crossTerm;     crossTerm = 0.0;
    crossTerm = (nHat ^ BatRprime) ^ rMinusRprime;
    integrand += crossTerm;
    integrand *= Jacobian;
    integrand *= outFront;
	
    switch (component) {
    case (x):   return integrand(x);
    case (y):   return integrand(y);
    case (z):   return integrand(z);
	default:	return 0;			// Shut up, compiler.
    }
}





void doIntegration(VEGAS_function VEGASfunc, int nCalls, double xlo[2], double xhi[2], double& result, double& error){
    VEGAS_state* Illinois = VEGAS_alloc(VEGASfunc);
    Illinois->verbose     = VEGAS_chatty;
    Illinois->iterations  = VEGAS_nIterations;

    double pVal, relErr, absErr;        double const pValMin   = 0.01;
    double const relErrMax = 1.0e-3;    double const absErrMax = 1.0e-5;

    int nVEGAS = 0;
    do {
        // Stage is 0 on first entry, 2 on successive entries.
        VEGAS_integrate (&VEGASfunc, xlo, xhi, nCalls, Illinois, &result, &error);
        ++ nVEGAS;
        pVal = chiprbQ(Illinois->chisqIt*Illinois->it_num, Illinois->it_num -1);
        absErr = fabs(error);
        relErr = fabs(error/result);
        if (nVEGAS>8) {
            string messy = "Poor VEGAS convergence; error = " + stringify(error)
                        + "/" + stringify(result) + " = " + stringify(fabs(error/result))
                        + " and pVal = " + stringify(pVal) + " after " + stringify(nVEGAS)
                        + " calls to VEGAS";
            // cout << messy << endl;
            LppUrk.UsrIssue(-2, Warning, "doIntegration", messy.c_str());
        }
    } while (pVal < pValMin || (relErr>relErrMax && absErr>absErrMax) );        // 2 sigma, sorta
    
    VEGAS_free(Illinois);
    return;
}





ra<double> fullHelmholtz(Arguments argh) {
    // Quick copy for readabilty below
    ra<double> rEval(3);    rEval = argh.rEval;
    FieldMap* fieldMap = argh.fieldMap;
    double radius = argh.radius;
    double loZ = argh.loZ;      double hiZ = argh.hiZ;

    // Ranges of integration
    double xloDisk[2]   = {        0.0,     0.0 };
    double xhiDisk[2]   = { radius, 2.0*pie };
    double xloCylind[2] = {    loZ,     0.0 };
    double xhiCylind[2] = {    hiZ, 2.0*pie };



    // Set up the 9 VEGAS functions with their integration parameters
    integrationParams upStreamDiskX =
        {radius,loZ,hiZ, rEval, fieldMap, upstream, x};
    VEGAS_function VEGASfuncUpX = { &vectorIntegrandFunc, 2, (void*)&upStreamDiskX };
    integrationParams upStreamDiskY =
        {radius,loZ,hiZ, rEval, fieldMap, upstream, y};
    VEGAS_function VEGASfuncUpY = { &vectorIntegrandFunc, 2, (void*)&upStreamDiskY };
    integrationParams upStreamDiskZ =
        {radius,loZ,hiZ, rEval, fieldMap, upstream, z};
    VEGAS_function VEGASfuncUpZ = { &vectorIntegrandFunc, 2, (void*)&upStreamDiskZ };

    integrationParams cylinderX =
        {radius,loZ,hiZ, rEval, fieldMap, central, x};
    VEGAS_function VEGASfuncCnX = { &vectorIntegrandFunc, 2, (void*)&cylinderX };
    integrationParams cylinderY =
        {radius,loZ,hiZ, rEval, fieldMap, central, y};
    VEGAS_function VEGASfuncCnY = { &vectorIntegrandFunc, 2, (void*)&cylinderY };
    integrationParams cylinderZ =
        {radius,loZ,hiZ, rEval, fieldMap, central, z};
    VEGAS_function VEGASfuncCnZ = { &vectorIntegrandFunc, 2, (void*)&cylinderZ };

    integrationParams dnStreamDiskX =
        {radius,loZ,hiZ, rEval, fieldMap, downstream, x};
    VEGAS_function VEGASfuncDnX = { &vectorIntegrandFunc, 2, (void*)&dnStreamDiskX };
    integrationParams dnStreamDiskY =
        {radius,loZ,hiZ, rEval, fieldMap, downstream, y};
    VEGAS_function VEGASfuncDnY = { &vectorIntegrandFunc, 2, (void*)&dnStreamDiskY };
    integrationParams dnStreamDiskZ =
        {radius,loZ,hiZ, rEval, fieldMap, downstream, z};
    VEGAS_function VEGASfuncDnZ = { &vectorIntegrandFunc, 2, (void*)&dnStreamDiskZ };



    double result, error;
    ra<double> IntegralResult(3);
    IntegralResult = 0;

    doIntegration(VEGASfuncUpX, VEGAS_callsDisk,     xloDisk,xhiDisk, result,error);
    IntegralResult(x)  = result;
    doIntegration(VEGASfuncDnX, VEGAS_callsDisk,     xloDisk,xhiDisk, result,error);
    IntegralResult(x) += result;
    doIntegration(VEGASfuncCnX, VEGAS_callsCylinder, xloCylind,xhiCylind, result,error);
    IntegralResult(x) += result;

    doIntegration(VEGASfuncUpY, VEGAS_callsDisk,      xloDisk,xhiDisk, result,error);
    IntegralResult(y)  = result;
    doIntegration(VEGASfuncDnY, VEGAS_callsDisk,      xloDisk,xhiDisk, result,error);
    IntegralResult(y) += result;
    doIntegration(VEGASfuncCnY, VEGAS_callsCylinder,  xloCylind,xhiCylind, result,error);
    IntegralResult(y) += result;

    doIntegration(VEGASfuncUpZ, VEGAS_callsDisk,      xloDisk,xhiDisk, result,error);
    IntegralResult(z)  = result;
    doIntegration(VEGASfuncDnZ, VEGAS_callsDisk,      xloDisk,xhiDisk, result,error);
    IntegralResult(z) += result;
    doIntegration(VEGASfuncCnZ, VEGAS_callsCylinder,  xloCylind,xhiCylind, result,error);
    IntegralResult(z) += result;



    // Return ra is 3 evaluation point coordinates, 3 results from the Helmholtz decomposition
    // and 3 results with the correction applied.
    ra<double> FullResult(9);
    FullResult(1) =rEval(x);            FullResult(2) =rEval(y);            FullResult(3) = rEval(z);
    FullResult(4) =IntegralResult(x);   FullResult(5) =IntegralResult(y);   FullResult(6) =IntegralResult(z);
    double bMagCorr = Qadd(IntegralResult(x),IntegralResult(y),IntegralResult(z));
    FullResult(7) = IntegralResult(x) +dBxCorrect(rEval(x), rEval(y), bMagCorr);
    FullResult(8) = IntegralResult(y) +dByCorrect(rEval(x), rEval(y), bMagCorr);
    FullResult(9) = IntegralResult(z);
    
    return FullResult;
}
