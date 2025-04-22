#include <iostream> 
#include <math.h> 
#include <algorithm> 

#include "TrackExtrapolation.h"
#include "TrackTools.h"
#include "TMath.h"

//#define DEBUG 0
/*************************************************************************************************************************

	
**************************************************************************************************************************/




void TrackExtrapolation::DoKalmanStep(Cluster &cluster, Plane &plane){

	#ifdef DEBUG
		cout << "********************************************************" << endl;
		cout << "CLUSTER" << endl;
		cout << "(x, y, z) = " << cluster.GetPosition(eX) << ", " << cluster.GetPosition(eY) << ", " << cluster.GetPosition(eZ) << ") [cm]" << endl;
		cout << "(dx, dy) = " << cluster.GetError(errX) << ", " << cluster.GetError(errY) << ") [cm]" << endl;
		cout << "********************************************************" << endl;
		cout << "Track parameters (BEFORE EXTRAPOLATION)" << endl;
		fStopPar.Print();
	#endif

	double xStart[3] = {fStopPar.GetPar(0),fStopPar.GetPar(1),fStopPar.GetZ()};
	//Extrapolate(cluster.GetPosition(eZ));


	double alpha, beta, gamma;
	double xP, yP, zP;
	
	plane.GetRotation(alpha, beta, gamma);
	plane.GetPosition(xP, yP, zP);
	
	alpha = alpha*TMath::Pi()/180.;
	beta = beta*TMath::Pi()/180.;
	gamma = gamma*TMath::Pi()/180.;   
		
	double H[4] = {cos(beta)*cos(gamma), cos(beta)*sin(gamma), -cos(alpha)*sin(gamma)+sin(alpha)*sin(beta)*cos(gamma), cos(alpha)*cos(gamma)+sin(alpha)*sin(beta)*sin(gamma)};
		
	double clxpos = cluster.GetPosition(eX);	
	double clypos = cluster.GetPosition(eX);
	double resid[2] = { clxpos-(H[0]*fStopPar.GetPar(0) + H[1]*fStopPar.GetPar(1)-(fStopPar.GetZ()-zP)*sin(beta)-xP), 
	                    clypos-(H[2]*fStopPar.GetPar(0) + H[3]*fStopPar.GetPar(1)+(fStopPar.GetZ()-zP)*sin(alpha)*cos(beta)-yP)};
	#ifdef DEBUG
		cout << "********************************************************" << endl;
		cout << "Track parameters (AFTER EXTRAPOLATION)" << endl;
		fStopPar.Print();
		cout << "********************************************************" << endl;
		cout << "RESIDUALS" << endl;
		cout << "(resX, resY) = " << resid[0] << ", " << resid[1] << ") [cm]" << endl;	
	#endif
	double *cov = fStopPar.GetCov();
	

	double S[3] = { cov[0]*H[0]*H[0] + 2*cov[1]*H[0]*H[1]+ cov[5]*H[1]*H[1] + cluster.GetError(errX)*cluster.GetError(errX), 
	                cov[0]*H[0]*H[2] + cov[1]*(H[0]*H[3]+H[1]*H[2]) + cov[5]*H[1]*H[3], 
	                cov[0]*H[2]*H[2]+2*cov[1]*H[2]*H[3]+cov[5]*H[3]*H[3] + cluster.GetError(errY)*cluster.GetError(errY)};
	
	//double det = (cov[0]+cluster.GetError(errX)*cluster.GetError(errX))*(cov[5]+cluster.GetError(errY)*cluster.GetError(errY)) - cov[1]*cov[1];
	double det = S[0]*S[2] - S[1]*S[1];
	//cout << resid[0] << " " << resid[1] << " " << det << endl;
	//double Sinv[3] = {(cov[5]+cluster.GetError(errY)*cluster.GetError(errY))/det, -cov[1]/det, (cov[0]+cluster.GetError(errX)*cluster.GetError(errX))/det};
	double Sinv[3] = {S[2]/det, -S[1]/det, S[0]/det};
	
	/*double K[5][2] = {	{cov[0]*Sinv[0] + cov[1]*Sinv[1], cov[0]*Sinv[1] + cov[1]*Sinv[2]}, 
						{cov[1]*Sinv[0] + cov[5]*Sinv[1], cov[1]*Sinv[1] + cov[5]*Sinv[2]},
						{cov[2]*Sinv[0] + cov[6]*Sinv[1], cov[2]*Sinv[1] + cov[6]*Sinv[2]},
						{cov[3]*Sinv[0] + cov[7]*Sinv[1], cov[3]*Sinv[1] + cov[7]*Sinv[2]},
						{cov[4]*Sinv[0] + cov[8]*Sinv[1], cov[4]*Sinv[1] + cov[8]*Sinv[2]}};*/
	double K[5][2] = {  {Sinv[0]*(cov[0]*H[0]+cov[1]*H[1]) + Sinv[1]*(cov[0]*H[2]+cov[1]*H[3]), Sinv[1]*(cov[0]*H[0]+cov[1]*H[1]) + Sinv[2]*(cov[0]*H[2]+cov[1]*H[3])},
	                    {Sinv[0]*(cov[1]*H[0]+cov[5]*H[1]) + Sinv[1]*(cov[1]*H[2]+cov[5]*H[3]), Sinv[1]*(cov[1]*H[0]+cov[5]*H[1]) + Sinv[2]*(cov[1]*H[2]+cov[5]*H[3])},
	                    {Sinv[0]*(cov[2]*H[0]+cov[6]*H[1]) + Sinv[1]*(cov[2]*H[2]+cov[6]*H[3]), Sinv[1]*(cov[2]*H[0]+cov[6]*H[1]) + Sinv[2]*(cov[2]*H[2]+cov[6]*H[3])},
	                    {Sinv[0]*(cov[3]*H[0]+cov[7]*H[1]) + Sinv[1]*(cov[3]*H[2]+cov[7]*H[3]), Sinv[1]*(cov[3]*H[0]+cov[7]*H[1]) + Sinv[2]*(cov[3]*H[2]+cov[7]*H[3])},
	                    {Sinv[0]*(cov[4]*H[0]+cov[8]*H[1]) + Sinv[1]*(cov[4]*H[2]+cov[8]*H[3]), Sinv[1]*(cov[4]*H[0]+cov[8]*H[1]) + Sinv[2]*(cov[4]*H[2]+cov[8]*H[3])}};
	#ifdef DEBUG
		/*cout << "********************************************************" << endl;
		cout << "KALMAN GAIN" << endl;
		for(int i = 0; i < 5; i++){
			cout << K[i][0] << "	" << K[i][1] << endl;
		}*/	
	#endif	

	fStopPar.AddLength(sqrt(pow(fStopPar.GetPar(0)+K[0][0]*resid[0]+K[0][1]*resid[1]-xStart[0], 2)+pow(fStopPar.GetPar(1)+K[1][0]*resid[0]+K[1][1]*resid[1]-xStart[1], 2)+pow(fStopPar.GetZ()-xStart[2], 2))-
	sqrt(pow(fStopPar.GetPar(0)-xStart[0], 2)+pow(fStopPar.GetPar(1)-xStart[1], 2)+pow(fStopPar.GetZ()-xStart[2], 2)));
	for(int i = 0; i < 5; i++){
		//cout << fStopPar.GetPar(i) << " " << K[i][0]*resid[0]+K[i][1]*resid[1] << endl;
		fStopPar.SetPar(i, fStopPar.GetPar(i) + K[i][0]*resid[0]+K[i][1]*resid[1]);
	}	
	
						
	double KHC[15] = {  (K[0][0]*H[0]+K[0][1]*H[2])*cov[0]+(K[0][0]*H[1]+K[0][1]*H[3])*cov[1], 
	                    (K[0][0]*H[0]+K[0][1]*H[2])*cov[1]+(K[0][0]*H[1]+K[0][1]*H[3])*cov[5], 
	                    (K[0][0]*H[0]+K[0][1]*H[2])*cov[2]+(K[0][0]*H[1]+K[0][1]*H[3])*cov[6], 
	                    (K[0][0]*H[0]+K[0][1]*H[2])*cov[3]+(K[0][0]*H[1]+K[0][1]*H[3])*cov[7], 
	                    (K[0][0]*H[0]+K[0][1]*H[2])*cov[4]+(K[0][0]*H[1]+K[0][1]*H[3])*cov[8], 
	                    
	                    (K[1][0]*H[0]+K[1][1]*H[2])*cov[1]+(K[1][0]*H[1]+K[1][1]*H[3])*cov[5], 
	                    (K[1][0]*H[0]+K[1][1]*H[2])*cov[2]+(K[1][0]*H[1]+K[1][1]*H[3])*cov[6], 
	                    (K[1][0]*H[0]+K[1][1]*H[2])*cov[3]+(K[1][0]*H[1]+K[1][1]*H[3])*cov[7], 
	                    (K[1][0]*H[0]+K[1][1]*H[2])*cov[4]+(K[1][0]*H[1]+K[1][1]*H[3])*cov[8], 
	                    
	                    (K[2][0]*H[0]+K[2][1]*H[2])*cov[2]+(K[2][0]*H[1]+K[2][1]*H[3])*cov[6], 
	                    (K[2][0]*H[0]+K[2][1]*H[2])*cov[3]+(K[2][0]*H[1]+K[2][1]*H[3])*cov[7], 
	                    (K[2][0]*H[0]+K[2][1]*H[2])*cov[4]+(K[2][0]*H[1]+K[2][1]*H[3])*cov[8], 
	                    
	                    (K[3][0]*H[0]+K[3][1]*H[2])*cov[3]+(K[3][0]*H[1]+K[3][1]*H[3])*cov[7],
	                    (K[3][0]*H[0]+K[3][1]*H[2])*cov[4]+(K[3][0]*H[1]+K[3][1]*H[3])*cov[8], 
	                    
	                    (K[4][0]*H[0]+K[4][1]*H[2])*cov[4]+(K[4][0]*H[1]+K[4][1]*H[3])*cov[8]};
	                    

	for(int i = 0; i < 15; i++){
		cov[i] = cov[i] - KHC[i];
	}					
	#ifdef DEBUG
		cout << "********************************************************" << endl;
		cout << "Track parameters (AFTER KALMAN STEP)" << endl;
		fStopPar.Print();
		cout << "********************************************************" << endl;
		cout << "********************************************************" << endl;

	#endif	
	
}


