//  Created by Leo Bellantoni on 4/1/23.

#include "FieldMap.h"
#include "stats.h"



namespace findOffs {
    
    // Vector of iterators to points in the central field map where
    // we will try to match the fringe field.
    vector<FieldMap::AtPoint> testItrs;
    int fitVerbHere;



    //=========================================================================
    //=========================================================================
    class FringeChi2 : public functional {
    public:
        FieldMap* fgMapLocal;
        FieldMap* cnMapLocal;
        int nCanInterp;

        // Constructors, destructor
        double probeErr;
        ra<double>* positionErr;
        FringeChi2() : positionErr(nullptr),fgMapLocal(nullptr),cnMapLocal(nullptr) {};
        FringeChi2(double probeErr_in, ra<double> positionErr_in, FieldMap* fgMap_in, FieldMap* cnMap_in) {
            if (positionErr_in.ne1!=3 || positionErr_in.ne2!=0)
                LppUrk.UsrIssue(-3, Fatal, "FringeChi2::FringeChi2(errors)", "Error in errors.");
            probeErr = probeErr_in;
            positionErr = new ra<double>(3);
            *positionErr = positionErr_in;
            fgMapLocal = fgMap_in;
            cnMapLocal = cnMap_in;
        }
        ~FringeChi2() {delete positionErr;}; // Don't deallocate the maps!  You will need them!


        double operator()(ra<double>* X) {
            nCanInterp = 0;            double chi2Sum = 0.0;
            fgMapLocal->setOffset(*X);
            ra<double> cnOffset = cnMapLocal->getOffset();
            ra<double> Bfringe(3);          ra<double> thisXi(3);
            ra<double> dBx(3);              ra<double> dBy(3);          ra<double> dBz(3);
            meansig typicalX, typicalY, typicalZ;
            for (addr iTest=0; iTest<testItrs.size(); ++iTest) {
                // thisXi is in the lab coordinate system
                thisXi(x) = testItrs[iTest].x -cnOffset(x);
                thisXi(y) = testItrs[iTest].y -cnOffset(y);
                thisXi(z) = testItrs[iTest].z -cnOffset(z);
                bool canInterp = fgMapLocal->interpolate(thisXi, Bfringe);
                if  (canInterp) {
                    FieldMap::FieldMapInd cnInd;
                    bool canGrad = cnMapLocal->findInd(&testItrs[iTest],cnInd);
                    if (!canGrad) continue;
                    dBx = cnMapLocal->findGrad(cnInd, x);
                    dBy = cnMapLocal->findGrad(cnInd, y);
                    dBz = cnMapLocal->findGrad(cnInd, z);
                    
                    // Is this really the right chi-squared?
                    double errX = Qadd(dBx(x)*(*positionErr)(x), dBx(y)*(*positionErr)(y), dBx(z)*(*positionErr)(z));
                    double errY = Qadd(dBy(x)*(*positionErr)(x), dBy(y)*(*positionErr)(y), dBy(z)*(*positionErr)(z));
                    double errZ = Qadd(dBz(x)*(*positionErr)(x), dBz(y)*(*positionErr)(y), dBz(z)*(*positionErr)(z));
                    errX = Qadd(errX, probeErr);    errY = Qadd(errY, probeErr);    errZ = Qadd(errZ, probeErr);
                    
                    double chiX = (Bfringe(x) -testItrs[iTest].Bx) / errX;
                    double chiY = (Bfringe(y) -testItrs[iTest].By) / errY;
                    double chiZ = (Bfringe(z) -testItrs[iTest].Bz) / errZ;
                    // Check for NaNs
                    if (chiX==chiX) {typicalX.push(chiX);   chi2Sum += SQR(chiX);   ++nCanInterp;};
                    if (chiY==chiY) {typicalY.push(chiY);   chi2Sum += SQR(chiY);   ++nCanInterp;};
                    if (chiZ==chiZ) {typicalZ.push(chiZ);   chi2Sum += SQR(chiZ);   ++nCanInterp;};
                }
            }

            if (fitVerbHere>=2) {
                cout << "chi2 at offset (" << (*X)(x) << ", " << (*X)(y) << ", " <<
                    (*X)(z) << ")\tis " << chi2Sum << "; interpolated at " <<
                    nCanInterp << " central map grid directions & points, out of " << 3*testItrs.size();
                if (fitVerbHere==3) {
                    cout << " with typical deviations of ("
                    << typicalX.stdev() <<", "<< typicalY.stdev() <<", "<< typicalZ.stdev()
                    <<")"<<  endl;
                } else {
                    cout << endl;
                }
            }
            return chi2Sum;
        }
    };



    //=========================================================================
    //=========================================================================
    ra<double> findOffsets(FieldMap* cnMap, FieldMap* fgMap) {
    
        ra<double>  startOff(3);
        ra<double>  answer(3);          ra<double> errors(3,3);
        ra<double>  step(3);            step  = 0.000001;
        FieldMap::FieldMapItr itr;      FieldMap::FieldMapInd unused;
        ra<double>  cnOff(3);           cnOff = cnMap->getOffset();
        ra<double>  Xi(3);



        itr = cnMap->pBegin;
        for (; itr<cnMap->pEnd; ++itr) {
            // Do not consider points not in the central map
            if (itr->valid) {
                Xi(x)  = itr->x;        Xi(y)  = itr->y;        Xi(z)  = itr->z;
                // Subtract offset to get from cnMap itr to lab coordinates
                Xi(x) -= cnOff(x);      Xi(y) -= cnOff(y);      Xi(z) -= cnOff(z);
                if (fgMap->insideMap(Xi,unused)) {
                    testItrs.push_back(*itr);
                }
            }
        }

        startOff = fgMap->getOffset();
        ra<double> positionErr(3);
        positionErr     = (whichMagnet==CMSM) ? CMSMmechanicalErr : SABRmechanicalErr;
        double probeErr = (whichMagnet==CMSM) ? CMSMprobeErr      : SABRprobeErr;
        FringeChi2 chi2fg(probeErr,positionErr,fgMap,cnMap);
        fitter* fgFitr = new fitter(chi2fg, &startOff);
        fgFitr->makeGirl(fitVerbosity);        fitVerbHere = fitVerbosity;
        
        if (trySIMPLEX) {
            fgFitr->finalTolerance = 0.0001;
            fgFitr->SIMPLEX(step);
            answer = fgFitr->getMinimum();
        } else {
            fgFitr->gradient();
            answer = fgFitr->getMinimum();
            try {
                fgFitr->hesse();
                errors = fgFitr->getCovariance();
            } catch (LppExcept& e){
                errors = 0;
            }
        }
        cout<< "For " << chi2fg.nCanInterp -3 << " degrees of freedom." <<endl;



        return answer;
    }   // end findOffsets
}
