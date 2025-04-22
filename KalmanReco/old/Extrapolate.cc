#include "TrackExtrapolation.h"
#include <iomanip>
#include <cmath>

void TrackExtrapolation::Extrapolate(double zStop){
	// Calculate number of extrapolation sts
	//fStopPar = fStartPar;
	int nsteps = (int) fabs(zStop - fStopPar.GetZ())/fStep;
	//cout << nsteps << endl;
	// Check if the extrapolation is backward or forward
	double st = fStep;
	if(zStop < fStopPar.GetZ())
		st *= -1;
	
	
	//cout << "radi" << endl;
	double J[5][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};


	double first = fStopPar.GetZ();
	// Do extrapolation in small steps	
	for(int i = 0; i < nsteps+1; i++){
	//do{
	
		if(i == nsteps){
			st = fabs(zStop - fStopPar.GetZ());
			if(zStop < first)
				st *= -1;
		}	
		

		// Calculate needed parameters
		double x = fStopPar.GetPar(0);
		double y = fStopPar.GetPar(1);	
		double tx = fStopPar.GetPar(2);
		double ty = fStopPar.GetPar(3);		
		double tx2 = tx*tx;
		double ty2 = ty*ty;
		double txy = tx*ty;
		double qdp = fStopPar.GetPar(4);
		double h = fKappa*qdp*sqrt(1+tx2+ty2);

		double dC = fKappa*qdp*fKappa*qdp;


		double magPoint[3] = {x + tx*st/2., y + ty*st/2., fStopPar.GetZ() + st/2.};
		
		double Bx = 0;
		double By = 0;
		double Bz = 0;
		fMagField->GetField(magPoint, Bx, By, Bz);
		//cout << magPoint[0] << " " << magPoint[1] << " " << magPoint[2] << " " << Bx << " " << By << " " << Bz << endl;
		
		fStopPar.SetZ(fStopPar.GetZ() + st);
		
				
		double dtx1 = txy*Bx - (1+tx2)*By + ty*Bz;
		double dtx2 = tx*(3*ty2+1)*Bx*Bx -2*ty*(3*tx2+1)*Bx*By + (3*ty2-tx2+1)*Bx*Bz + 3*tx*(tx2+1)*By*By - 4*txy*By*Bz - tx*Bz*Bz;
		double dtx3 = 3*txy*(5*ty2+3)*Bx*Bx*Bx - 3*(3*tx2+3*ty2+15*tx2*ty2+1)*Bx*Bx*By + ty*(-10*tx2+15*ty2+9)*Bx*Bx*Bz +  9*tx*ty*(5*tx2+3)*Bx*By*By
		 			+ tx*(10*tx2-40*ty2-2)*Bx*By*Bz - 11*tx*ty*Bx*Bz*Bz - 3*(tx2+1)*(5*tx2+1)*By*By*By + ty*(25*tx2+7)*By*By*Bz
		 			+ (7*tx2-4*ty2+1)*By*Bz*Bz - ty*Bz*Bz*Bz;
		
		
		double dty1 = (1+ty2)*Bx - txy*By - tx*Bz;
		double dty2 = 3*ty*(ty2+1)*Bx*Bx - 2*tx*(3*ty2+1)*Bx*By - 4*txy*Bx*Bz + ty*(3*tx2+1)*By*By + (3*tx2-ty2+1)*By*Bz - ty*Bz*Bz;
		double dty3 = 3*(ty2+1)*(5*ty2+1)*Bx*Bx*Bx -9*txy*(5*ty2+3)*Bx*Bx*By - tx*(25*ty2+7)*Bx*Bx*Bz + 3*(3*tx*tx+3*ty*ty+15*tx2*ty2+1)*Bx*By*By
		 			- ty*(-40*tx2+10*ty2-2)*Bx*By*Bz + (4*tx2-7*ty2-1)*Bx*Bz*Bz - 3*txy*(5*tx2+3)*By*By*By
		 			- tx*(15*tx2-10*ty2+9)*By*By*Bz + 11*txy*By*Bz*Bz + tx*Bz*Bz*Bz;
		
		x = x + tx*st + h*dtx1*st*st/2. + h*h*dtx2*st*st*st/6. + h*h*h*dtx3*st*st*st*st/24.;
		y = y + ty*st + h*dty1*st*st/2. + h*h*dty2*st*st*st/6. + h*h*h*dty3*st*st*st*st/24.;

		// fKappa = 2.99792e-4
		fStopPar.AddLength(sqrt(pow(tx*st + h*dtx1*st*st/2. + h*h*dtx2*st*st*st/6. + h*h*h*dtx3*st*st*st*st/24.,2) + 
			pow(ty*st + h*dty1*st*st/2. + h*h*dty2*st*st*st/6. + h*h*h*dty3*st*st*st*st/24.,2) + st*st));
		fStopPar.SetPar(0, x);
		fStopPar.SetPar(1, y);
		fStopPar.SetPar(2, tx + h*dtx1*st + h*h*dtx2*st*st/2. + h*h*h*dtx3*st*st*st/6.);
		fStopPar.SetPar(3, ty + h*dty1*st + h*h*dty2*st*st/2. + h*h*h*dty3*st*st*st/6.); 
		
		
		//REMOVE THIS
		/*if(fStopPar.GetZ()<-567.51){
			if(qdp < 0){
				fStopPar.SetPar(4, -1/(-st*4.5/1000. - 1/qdp));
			}
			else{
				fStopPar.SetPar(4, 1/(-st*4.5/1000. + 1/qdp));
			}
		}*/
		//**************************************
		if(fErrorEstimation){
		
			double dtx1dtx = ty*Bx - 2*tx*By;
			double dtx1dty = tx*Bx + Bz;
			
			double dtx2dtx = (3*ty2+1)*Bx*Bx - 12*txy*Bx*By - 2*tx*Bx*Bz + 3*(3*tx2+1)*By*By - 4*ty*By*Bz - Bz*Bz;
			double dtx2dty = 6*txy*Bx*Bx - 2*(3*tx2+1)*Bx*By + 6*ty*Bx*Bz - 4*tx*By*Bz;
			
			double dtx3dtx = 3*ty*(5*ty2+3)*Bx*Bx*Bx - 18*tx*(5*ty2+1)*Bx*Bx*By - 20*txy*Bx*Bx*Bz + 27*ty*(5*tx2+1)*Bx*By*By + 2*(15*tx2-20*ty2-1)*Bx*By*Bz 
			 			- 11*ty*Bx*Bz*Bz - 12*tx*(5*tx2+3)*By*By*By + 40*txy*By*By*Bz + 14*tx*By*Bz*Bz;
			double dtx3dty = 9*tx*(5*ty2+1)*Bx*Bx*Bx - 18*ty*(5*tx2+1)*Bx*Bx*By + (-10*tx2+45*ty2+9)*Bx*Bx*Bz + 9*tx*(5*tx2+3)*Bx*By*By -80*txy*Bx*By*Bz
			 			- 11*tx*Bx*Bz*Bz + (25*tx2+7)*By*By*Bz - 8*ty*By*Bz*Bz - Bz*Bz*Bz;
			
			
			double dty1dtx = -ty*By - Bz;
			double dty1dty = 2*ty*Bx + tx*By;
			
			double dty2dtx = -2*(3*ty2+1)*Bx*By - 4*ty*Bx*Bz + 6*txy*By*By + 6*tx*By*Bz;
			double dty2dty = 3*(3*ty2+1)*Bx*Bx - 12*txy*Bx*By - 4*tx*Bx*Bz + (3*tx2+1)*By*By - 2*ty*By*Bz - Bz*Bz;	
			
			double dty3dtx = -9*ty*(5*ty2+3)*Bx*Bx*By - (25*ty2+7)*Bx*Bx*Bz + 18*tx*(5*ty2+1)*Bx*By*By + 80*txy*Bx*By*Bz + 8*tx*Bx*Bz*Bz
			 			- 9*ty*(5*tx2+1)*By*By*By - (45*tx2-10*ty2+9)*By*By*Bz + 11*ty*By*Bz*Bz + Bz*Bz*Bz;
			double dty3dty = -12*ty*(5*ty2+3)*Bx*Bx*Bx - 27*tx*(5*ty2+1)*Bx*Bx*By - 40*txy*Bx*Bx*Bz + 18*ty*(5*tx2+1)*Bx*By*By + 2*(20*tx2-15*ty2+1)*Bx*By*Bz
			 			- 14*ty*Bx*Bz*Bz - 3*tx*(5*tx2+3)*By*By*By + 20*txy*By*By*Bz + 11*By*Bz*Bz;
			
			
			J[0][0] = 1;
			J[0][2] = st + h*dtx1dtx*st*st/2. + dC*tx*dtx1*st*st/2./h + h*h*dtx2dtx*st*st*st/6. + dC*tx*dtx2*st*st*st/3.
			 + h*h*h*dtx3dtx*st*st*st*st/24. + dC*tx*h*dtx3*st*st*st*st/12.;
			J[0][3] = h*dtx1dty*st*st/2. + dC*ty*dtx1*st*st/2./h + h*h*dtx2dty*st*st*st/6. + dC*ty*dtx2*st*st*st/3.
			 + h*h*h*dtx3dty*st*st*st*st/24. + dC*ty*h*dtx3*st*st*st*st/12.;
			J[0][4] = h*dtx1*st*st/(2.*qdp) + h*h*dtx2*st*st*st/(3.*qdp) + h*h*h*dtx3*st*st*st*st/(8.*qdp);
			
			J[1][1] = 1;
			J[1][2] = h*dty1dtx*st*st/2. + dC*tx*dty1*st*st/2./h + h*h*dty2dtx*st*st*st/6. + dC*tx*dty2*st*st*st/3.
			 + h*h*h*dty3dtx*st*st*st*st/24. + dC*tx*h*dty3*st*st*st*st/12.;
			J[1][3] = st + h*dty1dty*st*st/2. + dC*ty*dty1*st*st/2./h + h*h*dty2dty*st*st*st/6. + dC*ty*dty2*st*st*st/3.
			 + h*h*h*dty3dty*st*st*st*st/24. + dC*ty*h*dty3*st*st*st*st/12.;
			J[1][4] = h*dty1*st*st/(2.*qdp) + h*h*dty2*st*st*st/(3.*qdp) + h*h*h*dty3*st*st*st*st/(8.*qdp);
			
			J[2][2] = 1 + h*dtx1dtx*st + dC*tx*dtx1*st/h + h*h*dtx2dtx*st*st/2. + dC*tx*dtx2*st*st
			 + h*h*h*dtx3dtx*st*st*st/6. + dC*tx*h*dtx3*st*st*st/2.;
			J[2][3] = h*dtx1dty*st + dC*ty*dtx1*st/h + h*h*dtx2dty*st*st/2. + dC*ty*dtx2*st*st
			 + h*h*h*dtx3dty*st*st*st/6. + dC*ty*h*dtx3*st*st*st/2.;
			J[2][4] = h*dtx1*st/qdp + h*h*dtx2*st*st/qdp + h*h*h*dtx3*st*st*st/(2.*qdp);	
			
			J[3][2] = h*dty1dtx*st + dC*tx*dty1*st/h + h*h*dty2dtx*st*st/2. + dC*tx*dty2*st*st
			 + h*h*h*dty3dtx*st*st*st/6. + dC*tx*h*dty3*st*st*st/2.;
			J[3][3] = 1 + h*dty1dty*st + dC*ty*dty1*st/h + h*h*dty2dty*st*st/2. + dC*ty*dty2*st*st
			 + h*h*h*dty3dty*st*st*st/6. + dC*ty*h*dty3*st*st*st/2.;
			J[3][4] = h*dty1*st/qdp + h*h*dty2*st*st/qdp + h*h*h*dty3*st*st*st/(2.*qdp);		
			
			J[4][4] = 1;
			
			//cout << J[0][0] << " " << J[0][1] << " " << J[0][2] << " " << J[0][3] << " " <<  J[0][4]  << endl;
			/*cout << "************************************************************************" <<endl;
			cout << fixed << setprecision(9);
			for(int k = 0; k < 5; k++){
			
				for(int l = 0; l < 5; l++){
					cout  << J[k][l] << "	";
				}
				cout << endl;			
			}*/
			
			TransformCov(J, fStopPar.GetCov());
			//fStopPar.Print();
			AddNoise(fStopPar, fabs(st));
		}
		
	} 
	//SetCovarianceMatrix(cov, fStopPar);
	
	/*if(fabs(fStopPar.GetZ()-zStop) > 0){
		fStopPar.SetPar(0, fStopPar.GetPar(0)+fStopPar.GetPar(2)*(fStopPar.GetZ()-zStop));
		fStopPar.SetPar(1, fStopPar.GetPar(1)+fStopPar.GetPar(3)*(fStopPar.GetZ()-zStop));
	} */
}








void TrackExtrapolation::Extrapolate(double zStop, double st){
	fStep = st;
	Extrapolate(zStop);
}

void TrackExtrapolation::Extrapolate(double zStop, double st, double radLength){
	fStep = st;
	fRadLength = radLength;
	Extrapolate(zStop);
}



void TrackExtrapolation::AddNoise(TrackPar &trackPar, double X){

    //return;
	if(fRadLength == 0) return;
	
	double len = X/fRadLength;
	double SigTheta = 0.0136*fabs(trackPar.GetPar(4) * sqrt(len) * (1.+0.038*log(len)));
	
  	double p3 = trackPar.GetPar(2);
  	double p4 = trackPar.GetPar(3);	
  	
  	double p3p3 = SigTheta*SigTheta * (1 + p3*p3) * (1 + p3*p3 + p4*p4);
  	double p4p4 = SigTheta*SigTheta * (1 + p4*p4) * (1 + p3*p3 + p4*p4);
  	double p3p4 = SigTheta*SigTheta * p3*p4       * (1 + p3*p3 + p4*p4);
  	
  	trackPar.SetCov(0, trackPar.GetCov(0)+fStep*fStep*p3p3);
  	trackPar.SetCov(1, trackPar.GetCov(1)+fStep*fStep*p3p4);
  	trackPar.SetCov(2, trackPar.GetCov(2)-fStep*p3p3);
  	trackPar.SetCov(3, trackPar.GetCov(3)-fStep*p3p4);
  	
  	trackPar.SetCov(5, trackPar.GetCov(5)+fStep*fStep*p4p4);
  	trackPar.SetCov(6, trackPar.GetCov(6)-fStep*p3p4);
  	trackPar.SetCov(7, trackPar.GetCov(7)-fStep*p4p4);

	trackPar.SetCov(9, trackPar.GetCov(9)+p3p3);
  	trackPar.SetCov(10, trackPar.GetCov(10)+p3p4);
  	trackPar.SetCov(12, trackPar.GetCov(12)+p4p4);
  		
}
