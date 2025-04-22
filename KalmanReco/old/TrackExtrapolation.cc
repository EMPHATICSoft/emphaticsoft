#include "TrackExtrapolation.h"


double TrackExtrapolation::fKappa = 0.000299792458L;
TrackExtrapolation::TrackExtrapolation(bool cov, double step, string field, double radLength){
	fStep = step;
	fRadLength = radLength;
	TrackPar temp(eKisel);
	fStartPar = temp;
	fStopPar = temp;
	fErrorEstimation = cov;
	
	fMagField = new MagneticField(field);
}
TrackExtrapolation::TrackExtrapolation(bool cov, TrackPar trPar,  double step, string field, double radLength){
	SetTrackPar(trPar);
	fStep = step;
	fRadLength = radLength;
	fErrorEstimation = cov;
	fStartPar = trPar;
	fStopPar = trPar;

	fMagField = new MagneticField(field);
}

TrackExtrapolation::~TrackExtrapolation(){
	delete fMagField;
	fMagField = NULL;
}

/*void TrackExtrapolation:ExtrapolateToPlane(double step, double radLength, Plane& plane){
    fRadLength = radLength;
    fStep = step;
    
    ExtrapolateToPlane(zStop, plane);
}

void TrackExtrapolation::ExtrapolateToPlane(double radLength, Plane& plane){
    fRadLength = radLength;
    
    ExtrapolateToPlane(plane);
}

void TrackExtrapolation:ExtrapolateToPlane(Plane& plane){

    	Cluster cPred;
		SVector3 vRot;
		vRot(0) = 0;
		vRot(1) = 0;
		vRot(2) = 1;
		
		vRot = plane.GetRotationMatrix()*vRot;

        

}*/


void TrackExtrapolation::ExtrapolateToPlane(const double &a, const double&b, const double& c, const double& d){
    #ifdef DEBUG
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "DEBUG OUTPUT" << endl;
    cout << __func__ << " in " << __FILE__ << endl;
    cout << "   Extrapolate from z = " << fStopPar.GetZ() << " cm" << endl;
    int iter = 0;
    #endif
    double z = - (d + a*fStopPar.GetPar(0) + b*fStopPar.GetPar(1))/(a*fStopPar.GetPar(2) + b*fStopPar.GetPar(3) + c);

    double nStep = abs(z-fStopPar.GetZ())/fStep;
    
    
    while(nStep > 1){
    
        #ifdef DEBUG
        cout << "   Iteration: " << iter << endl;
        cout << "   nSteps = " << nStep << ", z = " << z << " cm" << endl;
        cout << "   Extrapolate from z = " << fStopPar.GetZ() << " cm" << endl;
        cout << "   Extrapolate to z = " << z - 0.25*(z-fStopPar.GetZ()) << " cm " << endl;
        cout << a << " " << b << " " << c << " " << d << endl;
        iter++;
        #endif
        Extrapolate(z - 0.25*(z-fStopPar.GetZ()));

        z = - (d + a*fStopPar.GetPar(0) + b*fStopPar.GetPar(1))/(a*fStopPar.GetPar(2) + b*fStopPar.GetPar(3) + c);
        nStep = abs(z-fStopPar.GetZ())/fStep;
    }
    
    Extrapolate(z);
    
    #ifdef DEBUG
    cout << "   Final z = " << fStopPar.GetZ() << " cm" << endl;    
    #endif 
}
