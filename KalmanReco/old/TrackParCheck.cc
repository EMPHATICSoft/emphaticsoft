#include "TrackTools.h"
#include <stdlib.h>

using namespace TrackTools;


TrackPar::TrackPar(TrackingType type, int charge, double z, double *par, double *cov){
	fType = type;
	fZ = z;
	fCharge = charge;
	
	//ParameterCheck(par);
	//CovarianceCheck(cov);

	for(int i = 0; i < 15; i++){
		if(i < 5){
			fPar[i] = par[i];
		}
		fCov[i] = cov[i];
	}	
}
//**************************************************************************************************
/*void TrackPar::ParameterCheck(double *par){
	if(par.size() > 5){
		cerr << "WARNING: " << __FUNCTION__ << ": parameter vector size is larger than 5! Only first 5 values will be used!" << endl;
	}
	else if(par.size() < 5){
		cerr << "ERROR: " << __FUNCTION__ << ": parameter vector size is larger than 5! Exiting..." << endl;
		exit(EXIT_FAILURE);
	}
}*/
//**************************************************************************************************
/*void TrackPar::CovarianceCheck(double *cov){
	if(cov.size() > 15){
		cerr << "WARNING: " << __FUNCTION__ << ": covariance matrix vector size is larger than 15! Only first 5 values will be used!" << endl;
	}
	else if(cov.size() < 15){
		cerr << "ERROR: " << __FUNCTION__ << ": covariance matrix vector size is smaller than 15! Exiting..." << endl;
		exit(EXIT_FAILURE);
	}
}*/
//**************************************************************************************************
void TrackPar::ChargeCheck(TrackingType type, double *par){
	
	switch(type){
		case eUnknown:
			cerr << "WARNING: " << __FUNCTION__ << ": TrackingType is unknown. Positive charge is assumed!" << endl;
			fCharge = 1;
			break;
		case eCartesian:
			cerr << "WARNING: " << __FUNCTION__ << ": Charge cannot be determined from eCartesian type. Positive charge is assumed!" << endl;
			fCharge = 1;
			break;
		case eNA61:
			if(par[0] > 0){
				fCharge = 1;
			}
			else fCharge = -1;
			break;		
		case eKisel:
			if(par[4] > 0){
				fCharge = 1;
			}
			else fCharge = -1;
			break;				
	}	
}
//**************************************************************************************************
/*void TrackPar::CheckIndex(int i, vector<double> vec){
	if(i < 0){
		cerr << "ERROR: " << __FUNCTION__ << ": vector index is negative! Exiting..." << endl;
		exit(EXIT_FAILURE);	
	}
	else if (i >= vec.size()){
		cerr << "ERROR: " << __FUNCTION__ << ": vector index is too large! Index = " << i << ", size = " << vec.size() << ". Exiting..." << endl;
		exit(EXIT_FAILURE);	
	}
}*/
