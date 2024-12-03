//  Created by Leo Bellantoni on 4/1/23.

#include "LppGlobals.h"
#include "FieldMap.h"
#include "stats.h"





// You probably want this to be false.
#define PLOTAVENOTDIFF false

// Reasonably accurate.  I think.
int const nIntegrateIter = 5;


class integrandOverX {
public:
    integrandOverX(FieldMap* fgMapIn, FieldMap* cnMapIn,
        double yIn, double zIn, meansig* countIn) :
        fgMapLocal(fgMapIn), cnMapLocal(cnMapIn),
        yEval(yIn),zEval(zIn), counter(countIn)  {};



    double operator()(double xEval) {

        ra<double> Bfringe(3);          ra<double> Bcenter(3);
        ra<double> Xi(3);
        Xi(x)=xEval;    Xi(y) = yEval;      Xi(z) = zEval;

        bool canInterpFringe = fgMapLocal->interpolate(Xi, Bfringe);
        if (!canInterpFringe) {
            LppUrk.UsrIssue(-4, Inform, "evalFieldDiffs::operator()", "Could not interpolate fringe field");
            return 0.0;
        }
        bool canInterpCenter = cnMapLocal->interpolate(Xi, Bcenter);
        if (!canInterpCenter) {
            LppUrk.UsrIssue(-4, Inform, "evalFieldDiffs::operator()", "Could not interpolate center field");
            return 0.0;
        }

        // Return absolute difference of fields
        double chiX,chiY,chiZ, retval;
        chiX = (Bfringe(x) -Bcenter(x));
        chiY = (Bfringe(y) -Bcenter(y));
        chiZ = (Bfringe(z) -Bcenter(z));

        if (PLOTAVENOTDIFF) {
            chiX = (Bfringe(x) +Bcenter(x)) / 2.0;
            chiY = (Bfringe(y) +Bcenter(y)) / 2.0;
            chiZ = (Bfringe(z) +Bcenter(z)) / 2.0;
        }

        retval = Qadd(chiX, chiY, chiZ);
        counter->push(retval);
         // DEBUG  cout << "At (x,y,z):" << Xi(x) tabl Xi(y) tabl Xi(z) tabl " diffB= " tabl retval << endl;
        return retval;
    }
private:
    FieldMap* fgMapLocal;
    FieldMap* cnMapLocal;
    double yEval, zEval;
    meansig*  counter;
};




    
class integrandOverY {
public:
    integrandOverY(FieldMap* fgMapIn, FieldMap* cnMapIn,
        double xLowIn, double xHighIn, double zIn, meansig* countIn) :
        fgMap(fgMapIn), cnMap(cnMapIn),
        xLow(xLowIn), xHigh(xHighIn), zEval(zIn),
        counter(countIn) {
    };



    double operator()(double yEval) {

        integrandOverX funcOfX(fgMap,cnMap,
            yEval, zEval, counter);

        ra<double> integral1D(2);
        integral1D = integrateObj(funcOfX, xLow, xHigh, nIntegrateIter);
        return integral1D(1);
    }
private:
    FieldMap* fgMap;
    FieldMap* cnMap;
    double xLow, xHigh;
    double zEval;
    meansig* counter;
};




    
class integralOverXY {
public:
    integralOverXY(FieldMap* fgMapIn, FieldMap* cnMapIn,
        double xLowIn, double xHighIn, double yLowIn, double yHighIn) :
        fgMapLocal(fgMapIn), cnMapLocal(cnMapIn),
        xLow(xLowIn), xHigh(xHighIn), yLow(yLowIn), yHigh(yHighIn) {
    };



    meansig countAtZ;



    double operator()(double zEval) {

        integrandOverY funcOfY(fgMapLocal,cnMapLocal,
            xLow,xHigh, zEval, &countAtZ);

        ra<double> integral2D(2);
        integral2D = integrateObj(funcOfY, yLow, yHigh, nIntegrateIter);
        return integral2D(1);
    }
private:
    FieldMap* fgMapLocal;
    FieldMap* cnMapLocal;
    double xLow, xHigh, yLow, yHigh;

};
