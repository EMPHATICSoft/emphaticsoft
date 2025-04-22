#include "TrackTools.h"
#include <stdlib.h>
#include <math.h>


using namespace TrackTools;
using namespace std;

/*void TrackTools::GetCovarianceMatrix(double (*cov)[5], TrackPar &parIn){
	int globId = 0;
	for(int i = 0; i < 5; i++){
		for(int j = i; j < 5; j++){
			cov[i][j] = parIn.GetCov(globId);
			if(i != j){
				cov[j][i] = parIn.GetCov(globId);
			}
			globId++;
		}
	}
}

void TrackTools::SetCovarianceMatrix(double (*cov)[5], TrackPar &parIn){
	int globId = 0;
	for(int i = 0; i < 5; i++){
		for(int j = i; j < 5; j++){
			parIn.SetCov(globId,cov[i][j]);
			globId++;
		}
	}
}*/

void TrackTools::TransformCov(double (*J)[5], double *c){
	//double Jt[5][5];
	/*double cT[5][5] = {	{c[0][0] ,c[0][1] , c[0][2], c[0][3], c[0][4]},
						{c[1][0] ,c[1][1] , c[1][2], c[1][3], c[1][4]},
						{c[2][0] ,c[2][1] , c[2][2], c[2][3], c[2][4]},
						{c[3][0] ,c[3][1] , c[3][2], c[3][3], c[3][4]},
						{c[4][0] ,c[4][1] , c[4][2], c[4][3], c[4][4]}};*/
						
	double cT[15] = {c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], c[9], c[10], c[11], c[12], c[13], c[14]};
	/*for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){
			Jt[i][j] = J[j][i];
			cTemp[i][j] = 0;
		}		
	}*/
	
	/*for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){
			for(int k = 0; k < 5; k++){
				cTemp[i][j] += cov[i][k]*J[j][k];
			}
		}		
	}
	
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){
			cov[i][j] = 0;
			for(int k = 0; k < 5; k++){
				cov[i][j] += J[i][k]*cTemp[k][j];
			}
		}		
	}*/
	
	c[0] = J[0][0]*(cT[0]*J[0][0] + cT[1]*J[0][1] + cT[2]*J[0][2] + cT[3]*J[0][3] + cT[4]*J[0][4]) 
			+ J[0][1]*(cT[1]*J[0][0] + cT[5]*J[0][1] + cT[6]*J[0][2] + cT[7]*J[0][3] + cT[8]*J[0][4]) 
			+ J[0][2]*(cT[2]*J[0][0] + cT[6]*J[0][1] + cT[9]*J[0][2] + cT[10]*J[0][3] + cT[11]*J[0][4]) 
			+ J[0][3]*(cT[3]*J[0][0] + cT[7]*J[0][1] + cT[10]*J[0][2] + cT[12]*J[0][3] + cT[13]*J[0][4]) 
			+ J[0][4]*(cT[4]*J[0][0] + cT[8]*J[0][1] + cT[11]*J[0][2] + cT[13]*J[0][3] + cT[14]*J[0][4]);
	
	
	//cout << c[0] << endl;	
	c[1] = J[0][0]*(cT[0]*J[1][0] + cT[1]*J[1][1] + cT[2]*J[1][2] + cT[3]*J[1][3] + cT[4]*J[1][4]) 
			+ J[0][1]*(cT[1]*J[1][0] + cT[5]*J[1][1] + cT[6]*J[1][2] + cT[7]*J[1][3] + cT[8]*J[1][4]) 
			+ J[0][2]*(cT[2]*J[1][0] + cT[6]*J[1][1] + cT[9]*J[1][2] + cT[10]*J[1][3] + cT[11]*J[1][4]) 
			+ J[0][3]*(cT[3]*J[1][0] + cT[7]*J[1][1] + cT[10]*J[1][2] + cT[12]*J[1][3] + cT[13]*J[1][4]) 
			+ J[0][4]*(cT[4]*J[1][0] + cT[8]*J[1][1] + cT[11]*J[1][2] + cT[13]*J[1][3] + cT[14]*J[1][4]);
	//cout << J[1][0] << " " << J[1][1] << " " << J[1][2] << " " << J[1][3] << " " << J[1][4] << endl;
	//cout << c[3] << " " << c[7] << " " << c[10] << " " << c[12] << " " << c[13] << endl;
	c[2] = J[0][0]*(cT[0]*J[2][0] + cT[1]*J[2][1] + cT[2]*J[2][2] + cT[3]*J[2][3] + cT[4]*J[2][4]) 
			+ J[0][1]*(cT[1]*J[2][0] + cT[5]*J[2][1] + cT[6]*J[2][2] + cT[7]*J[2][3] + cT[8]*J[2][4]) 
			+ J[0][2]*(cT[2]*J[2][0] + cT[6]*J[2][1] + cT[9]*J[2][2] + cT[10]*J[2][3] + cT[11]*J[2][4]) 
			+ J[0][3]*(cT[3]*J[2][0] + cT[7]*J[2][1] + cT[10]*J[2][2] + cT[12]*J[2][3] + cT[13]*J[2][4]) 
			+ J[0][4]*(cT[4]*J[2][0] + cT[8]*J[2][1] + cT[11]*J[2][2] + cT[13]*J[2][3] + cT[14]*J[2][4]);

		
	c[3] = J[0][0]*(cT[0]*J[3][0] + cT[1]*J[3][1] + cT[2]*J[3][2] + cT[3]*J[3][3] + cT[4]*J[3][4]) 
			+ J[0][1]*(cT[1]*J[3][0] + cT[5]*J[3][1] + cT[6]*J[3][2] + cT[7]*J[3][3] + cT[8]*J[3][4]) 
			+ J[0][2]*(cT[2]*J[3][0] + cT[6]*J[3][1] + cT[9]*J[3][2] + cT[10]*J[3][3] + cT[11]*J[3][4]) 
			+ J[0][3]*(cT[3]*J[3][0] + cT[7]*J[3][1] + cT[10]*J[3][2] + cT[12]*J[3][3] + cT[13]*J[3][4]) 
			+ J[0][4]*(cT[4]*J[3][0] + cT[8]*J[3][1] + cT[11]*J[3][2] + cT[13]*J[3][3] + cT[14]*J[3][4]);

	
	c[4] = J[0][0]*(cT[0]*J[4][0] + cT[1]*J[4][1] + cT[2]*J[4][2] + cT[3]*J[4][3] + cT[4]*J[4][4]) 
			+ J[0][1]*(cT[1]*J[4][0] + cT[5]*J[4][1] + cT[6]*J[4][2] + cT[7]*J[4][3] + cT[8]*J[4][4]) 
			+ J[0][2]*(cT[2]*J[4][0] + cT[6]*J[4][1] + cT[9]*J[4][2] + cT[10]*J[4][3] + cT[11]*J[4][4]) 
			+ J[0][3]*(cT[3]*J[4][0] + cT[7]*J[4][1] + cT[10]*J[4][2] + cT[12]*J[4][3] + cT[13]*J[4][4]) 
			+ J[0][4]*(cT[4]*J[4][0] + cT[8]*J[4][1] + cT[11]*J[4][2] + cT[13]*J[4][3] + cT[14]*J[4][4]);

	
	c[5] = J[1][0]*(cT[0]*J[1][0] + cT[1]*J[1][1] + cT[2]*J[1][2] + cT[3]*J[1][3] + cT[4]*J[1][4]) 
			+ J[1][1]*(cT[1]*J[1][0] + cT[5]*J[1][1] + cT[6]*J[1][2] + cT[7]*J[1][3] + cT[8]*J[1][4]) 
			+ J[1][2]*(cT[2]*J[1][0] + cT[6]*J[1][1] + cT[9]*J[1][2] + cT[10]*J[1][3] + cT[11]*J[1][4]) 
			+ J[1][3]*(cT[3]*J[1][0] + cT[7]*J[1][1] + cT[10]*J[1][2] + cT[12]*J[1][3] + cT[13]*J[1][4]) 
			+ J[1][4]*(cT[4]*J[1][0] + cT[8]*J[1][1] + cT[11]*J[1][2] + cT[13]*J[1][3] + cT[14]*J[1][4]);
			
	c[6] = J[1][0]*(cT[0]*J[2][0] + cT[1]*J[2][1] + cT[2]*J[2][2] + cT[3]*J[2][3] + cT[4]*J[2][4]) 
			+ J[1][1]*(cT[1]*J[2][0] + cT[5]*J[2][1] + cT[6]*J[2][2] + cT[7]*J[2][3] + cT[8]*J[2][4]) 
			+ J[1][2]*(cT[2]*J[2][0] + cT[6]*J[2][1] + cT[9]*J[2][2] + cT[10]*J[2][3] + cT[11]*J[2][4]) 
			+ J[1][3]*(cT[3]*J[2][0] + cT[7]*J[2][1] + cT[10]*J[2][2] + cT[12]*J[2][3] + cT[13]*J[2][4]) 
			+ J[1][4]*(cT[4]*J[2][0] + cT[8]*J[2][1] + cT[11]*J[2][2] + cT[13]*J[2][3] + cT[14]*J[2][4]);
	
		
	c[7] = J[1][0]*(cT[0]*J[3][0] + cT[1]*J[3][1] + cT[2]*J[3][2] + cT[3]*J[3][3] + cT[4]*J[3][4]) 
			+ J[1][1]*(cT[1]*J[3][0] + cT[5]*J[3][1] + cT[6]*J[3][2] + cT[7]*J[3][3] + cT[8]*J[3][4]) 
			+ J[1][2]*(cT[2]*J[3][0] + cT[6]*J[3][1] + cT[9]*J[3][2] + cT[10]*J[3][3] + cT[11]*J[3][4]) 
			+ J[1][3]*(cT[3]*J[3][0] + cT[7]*J[3][1] + cT[10]*J[3][2] + cT[12]*J[3][3] + cT[13]*J[3][4]) 
			+ J[1][4]*(cT[4]*J[3][0] + cT[8]*J[3][1] + cT[11]*J[3][2] + cT[13]*J[3][3] + cT[14]*J[3][4]);

	
	c[8] = J[1][0]*(cT[0]*J[4][0] + cT[1]*J[4][1] + cT[2]*J[4][2] + cT[3]*J[4][3] + cT[4]*J[4][4]) 
			+ J[1][1]*(cT[1]*J[4][0] + cT[5]*J[4][1] + cT[6]*J[4][2] + cT[7]*J[4][3] + cT[8]*J[4][4]) 
			+ J[1][2]*(cT[2]*J[4][0] + cT[6]*J[4][1] + cT[9]*J[4][2] + cT[10]*J[4][3] + cT[11]*J[4][4]) 
			+ J[1][3]*(cT[3]*J[4][0] + cT[7]*J[4][1] + cT[10]*J[4][2] + cT[12]*J[4][3] + cT[13]*J[4][4]) 
			+ J[1][4]*(cT[4]*J[4][0] + cT[8]*J[4][1] + cT[11]*J[4][2] + cT[13]*J[4][3] + cT[14]*J[4][4]);


	c[9] = J[2][0]*(cT[0]*J[2][0] + cT[1]*J[2][1] + cT[2]*J[2][2] + cT[3]*J[2][3] + cT[4]*J[2][4]) 
			+ J[2][1]*(cT[1]*J[2][0] + cT[5]*J[2][1] + cT[6]*J[2][2] + cT[7]*J[2][3] + cT[8]*J[2][4]) 
			+ J[2][2]*(cT[2]*J[2][0] + cT[6]*J[2][1] + cT[9]*J[2][2] + cT[10]*J[2][3] + cT[11]*J[2][4]) 
			+ J[2][3]*(cT[3]*J[2][0] + cT[7]*J[2][1] + cT[10]*J[2][2] + cT[12]*J[2][3] + cT[13]*J[2][4]) 
			+ J[2][4]*(cT[4]*J[2][0] + cT[8]*J[2][1] + cT[11]*J[2][2] + cT[13]*J[2][3] + cT[14]*J[2][4]);
			
	c[10] = J[2][0]*(cT[0]*J[3][0] + cT[1]*J[3][1] + cT[2]*J[3][2] + cT[3]*J[3][3] + cT[4]*J[3][4]) 
			+ J[2][1]*(cT[1]*J[3][0] + cT[5]*J[3][1] + cT[6]*J[3][2] + cT[7]*J[3][3] + cT[8]*J[3][4]) 
			+ J[2][2]*(cT[2]*J[3][0] + cT[6]*J[3][1] + cT[9]*J[3][2] + cT[10]*J[3][3] + cT[11]*J[3][4]) 
			+ J[2][3]*(cT[3]*J[3][0] + cT[7]*J[3][1] + cT[10]*J[3][2] + cT[12]*J[3][3] + cT[13]*J[3][4]) 
			+ J[2][4]*(cT[4]*J[3][0] + cT[8]*J[3][1] + cT[11]*J[3][2] + cT[13]*J[3][3] + cT[14]*J[3][4]);

	
	c[11] = J[2][0]*(cT[0]*J[4][0] + cT[1]*J[4][1] + cT[2]*J[4][2] + cT[3]*J[4][3] + cT[4]*J[4][4]) 
			+ J[2][1]*(cT[1]*J[4][0] + cT[5]*J[4][1] + cT[6]*J[4][2] + cT[7]*J[4][3] + cT[8]*J[4][4]) 
			+ J[2][2]*(cT[2]*J[4][0] + cT[6]*J[4][1] + cT[9]*J[4][2] + cT[10]*J[4][3] + cT[11]*J[4][4]) 
			+ J[2][3]*(cT[3]*J[4][0] + cT[7]*J[4][1] + cT[10]*J[4][2] + cT[12]*J[4][3] + cT[13]*J[4][4]) 
			+ J[2][4]*(cT[4]*J[4][0] + cT[8]*J[4][1] + cT[11]*J[4][2] + cT[13]*J[4][3] + cT[14]*J[4][4]);

		
	c[12] = J[3][0]*(cT[0]*J[3][0] + cT[1]*J[3][1] + cT[2]*J[3][2] + cT[3]*J[3][3] + cT[4]*J[3][4]) 
			+ J[3][1]*(cT[1]*J[3][0] + cT[5]*J[3][1] + cT[6]*J[3][2] + cT[7]*J[3][3] + cT[8]*J[3][4]) 
			+ J[3][2]*(cT[2]*J[3][0] + cT[6]*J[3][1] + cT[9]*J[3][2] + cT[10]*J[3][3] + cT[11]*J[3][4]) 
			+ J[3][3]*(cT[3]*J[3][0] + cT[7]*J[3][1] + cT[10]*J[3][2] + cT[12]*J[3][3] + cT[13]*J[3][4]) 
			+ J[3][4]*(cT[4]*J[3][0] + cT[8]*J[3][1] + cT[11]*J[3][2] + cT[13]*J[3][3] + cT[14]*J[3][4]);
		
	c[13] = J[3][0]*(cT[0]*J[4][0] + cT[1]*J[4][1] + cT[2]*J[4][2] + cT[3]*J[4][3] + cT[4]*J[4][4]) 
			+ J[3][1]*(cT[1]*J[4][0] + cT[5]*J[4][1] + cT[6]*J[4][2] + cT[7]*J[4][3] + cT[8]*J[4][4]) 
			+ J[3][2]*(cT[2]*J[4][0] + cT[6]*J[4][1] + cT[9]*J[4][2] + cT[10]*J[4][3] + cT[11]*J[4][4]) 
			+ J[3][3]*(cT[3]*J[4][0] + cT[7]*J[4][1] + cT[10]*J[4][2] + cT[12]*J[4][3] + cT[13]*J[4][4]) 
			+ J[3][4]*(cT[4]*J[4][0] + cT[8]*J[4][1] + cT[11]*J[4][2] + cT[13]*J[4][3] + cT[14]*J[4][4]);
			
	c[14] = J[4][0]*(cT[0]*J[4][0] + cT[1]*J[4][1] + cT[2]*J[4][2] + cT[3]*J[4][3] + cT[4]*J[4][4]) 
			+ J[4][1]*(cT[1]*J[4][0] + cT[5]*J[4][1] + cT[6]*J[4][2] + cT[7]*J[4][3] + cT[8]*J[4][4]) 
			+ J[4][2]*(cT[2]*J[4][0] + cT[6]*J[4][1] + cT[9]*J[4][2] + cT[10]*J[4][3] + cT[11]*J[4][4]) 
			+ J[4][3]*(cT[3]*J[4][0] + cT[7]*J[4][1] + cT[10]*J[4][2] + cT[12]*J[4][3] + cT[13]*J[4][4]) 
			+ J[4][4]*(cT[4]*J[4][0] + cT[8]*J[4][1] + cT[11]*J[4][2] + cT[13]*J[4][3] + cT[14]*J[4][4]);
}

void TrackTools::ConvertTrackPar(TrackPar &parIn, TrackingType typeOut){
	TrackingType typeIn = parIn.GetType();

	if(typeIn == typeOut){
		cerr << "WARNING: " << __FUNCTION__ << ": input and output tracking types are equal. Skiping conversion!" << endl;
		return;
	}
	else if(typeIn == eUnknown){
		cerr << "WARNING: " << __FUNCTION__ << ": unknown input tracking type. Skiping conversion!" << endl;
		return;
	}
	else if(typeOut == eUnknown){
		cerr << "WARNING: " << __FUNCTION__ << ": unknown output tracking type. Skiping conversion!" << endl;
		return;
	}

					
	double J[5][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
	//double cov[5][5];
	

	/*for(int i = 0; i < 5; i++){
		//J[i] = new double[5];
		//cov[i] = new double[5];
		for(int j = 0; j < 5; j++){
			J[i][j] = 0;
			cov[i][j] = 0;
		}
	}	*/
	//GetCovarianceMatrix(cov, parIn);
					
	switch(typeIn){

		case eCartesian:
		{
			switch(typeOut){
				case eNA61:
				{
					double pxz = sqrt(parIn.GetPar(2)*parIn.GetPar(2) + parIn.GetPar(4)*parIn.GetPar(4));
					double p1 = parIn.GetCharge()/pxz;
					double p2 = parIn.GetPar(3)/pxz;
					double p3 = atan(parIn.GetPar(4)/parIn.GetPar(2));
					double p4 = parIn.GetPar(0);
					double p5 = parIn.GetPar(1);

					/*J[0][0] = 1;
					J[1][1] = 1;
					J[2][2] = -parIn.GetCharge()*parIn.GetPar(2)/(pxz*pxz*pxz);
					J[2][4] = -parIn.GetCharge()*parIn.GetPar(4)/(pxz*pxz*pxz);	
					J[3][2] = -parIn.GetPar(2)*parIn.GetPar(3)/(pxz*pxz*pxz);
					J[3][3] = 1/pxz;
					J[3][4] = -parIn.GetPar(3)*parIn.GetPar(4)/(pxz*pxz*pxz);	
					J[4][2] = -2*parIn.GetPar(4)*parIn.GetPar(4)/(pxz*(pxz*pxz +(pxz-parIn.GetPar(2))*(pxz-parIn.GetPar(2))));
					J[4][4] = 2*parIn.GetPar(2)*parIn.GetPar(4)/(pxz*(pxz*pxz +(pxz-parIn.GetPar(2))*(pxz-parIn.GetPar(2))));*/
					cout << parIn.GetPar(0) << " " << parIn.GetPar(1) << endl;
					J[0][2] = -parIn.GetCharge()*parIn.GetPar(2)/(pxz*pxz*pxz);
					J[0][4] = -parIn.GetCharge()*parIn.GetPar(4)/(pxz*pxz*pxz);
					J[1][2] = -parIn.GetPar(2)*parIn.GetPar(3)/(pxz*pxz*pxz);
					J[1][3] = 1/pxz;
					J[1][4] = -parIn.GetPar(3)*parIn.GetPar(4)/(pxz*pxz*pxz);
					J[2][2] = -parIn.GetPar(4)/(pxz*pxz);
					J[2][4] = parIn.GetPar(2)/(pxz*pxz);
					J[3][0] = 1;
					J[4][1] = 1;
					
					parIn.SetPar(0, p1);
					parIn.SetPar(1, p2);					
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);		
					parIn.SetType(typeOut);		
					TransformCov(J, parIn.GetCov());												
					break;
				}
				case eKisel:
				{			
					double p = sqrt(parIn.GetPar(2)*parIn.GetPar(2) + parIn.GetPar(3)*parIn.GetPar(3) + parIn.GetPar(4)*parIn.GetPar(4));
					double p3 = parIn.GetPar(2)/parIn.GetPar(4);
					double p4 = parIn.GetPar(3)/parIn.GetPar(4);
					double p5 = parIn.GetCharge()/p;

					J[0][0] = 1;
					J[1][1] = 1;
					J[2][2] = 1/parIn.GetPar(4);
					J[2][4] = -parIn.GetPar(2)/parIn.GetPar(4)/parIn.GetPar(4);	
					J[3][3] = 1/parIn.GetPar(4);
					J[3][4] = -parIn.GetPar(3)/parIn.GetPar(4)/parIn.GetPar(4);
					J[4][2] = -parIn.GetCharge()*parIn.GetPar(2)/p/p/p;	
					J[4][3] = -parIn.GetCharge()*parIn.GetPar(3)/p/p/p;
					J[4][4] = -parIn.GetCharge()*parIn.GetPar(4)/p/p/p;
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);		
					parIn.SetType(typeOut);		
					TransformCov(J, parIn.GetCov());	
					break;
				}
						
			}
			break;
		}
		case eNA61:
		{
			switch(typeOut){
				case eCartesian:
				{
					//double ca = cos(parIn.GetPar(4)/2.);
					//double ta = tan(parIn.GetPar(4)/2.);
					double p1 = parIn.GetPar(3);
					double p2 = parIn.GetPar(4);
					double p3 = parIn.GetCharge()*cos(parIn.GetPar(2))/parIn.GetPar(0);
					double p4 = parIn.GetCharge()*parIn.GetPar(1)/parIn.GetPar(0);
					double p5 = parIn.GetCharge()*sin(parIn.GetPar(2))/parIn.GetPar(0);
					
					/*J[0][0] = 1;
					J[1][1] = 1;
					J[2][2] = -p3/parIn.GetPar(2);
					J[2][4] = -parIn.GetCharge()/(2*parIn.GetPar(2)*ca*ca);
					J[3][2] = -p4/parIn.GetPar(2);
					J[3][3] = parIn.GetCharge()/parIn.GetPar(2);
					J[4][2] = -p5/parIn.GetPar(2);
					J[4][4] = (1-ta)/(2*ca*ca*parIn.GetPar(2)*sqrt(ta*(2-ta)));*/
					
					J[0][3] = 1;
					J[1][4] = 1;
					J[2][0] = -cos(parIn.GetPar(2))/(parIn.GetPar(0)*parIn.GetPar(0));
					J[2][2] = -sin(parIn.GetPar(2))/parIn.GetPar(0);
					J[3][0] = -parIn.GetCharge()*parIn.GetPar(1)/(parIn.GetPar(0)*parIn.GetPar(0));
					J[3][1] = parIn.GetCharge()/parIn.GetPar(0);
					J[4][0] = -sin(parIn.GetPar(2))/(parIn.GetPar(0)*parIn.GetPar(0));
					J[4][2] = cos(parIn.GetPar(2))/parIn.GetPar(0);

					parIn.SetPar(0, p1);
					parIn.SetPar(1, p2);					
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);		
					parIn.SetType(typeOut);		
					TransformCov(J, parIn.GetCov());								
					break;
				}
				case eKisel:
				{

					
					double b=sqrt(1+parIn.GetPar(1)*parIn.GetPar(1));
					double p1 = parIn.GetPar(3);
					double p2 = parIn.GetPar(4);
					double p3 = 1/tan(parIn.GetPar(2));
					double p4 = parIn.GetPar(1)/fabs(sin(parIn.GetPar(2)));
					double p5 = parIn.GetPar(0)/b;
					
					J[0][3] = 1;
					J[1][4] = 1;
					J[2][2] = -1/(sin(parIn.GetPar(2))*sin(parIn.GetPar(2)));
					J[3][1] = 1/sin(parIn.GetPar(2));
					J[3][2] = -parIn.GetPar(1)*cos(parIn.GetPar(2))/(sin(parIn.GetPar(2))*sin(parIn.GetPar(2)));
					J[4][0] = 1/b;
					J[4][1] = -parIn.GetPar(0)*parIn.GetPar(1)/(b*b*b);

					//cout << J[4][0] << endl;
					parIn.SetPar(0, p1);
					parIn.SetPar(1, p2);					
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);	
					/*double ta = tan(parIn.GetPar(4)/2.);
					double d = sqrt(ta*(2-ta));
					double d1 = sqrt(1+parIn.GetPar(3)*parIn.GetPar(3));
					double ca = cos(parIn.GetPar(4)/2.)*cos(parIn.GetPar(4)/2.);
					//cout << ta << " " << parIn.GetPar(3) << " " << d << endl;
					double p3 = (1-ta)/d;
					double p4 = parIn.GetPar(3)/d;
					double p5 = parIn.GetPar(2)/d1;
					
					J[0][0] = 1;
					J[1][1] = 1;
					J[2][4] = -1/(2*ca*d*d*d);
					J[3][3] = 1/d;
					J[3][4] = -parIn.GetPar(3)*(1-ta)/(2*ca*d*d*d);
					J[4][2] = 1/d1;
					J[4][3] = -parIn.GetPar(2)*parIn.GetPar(3)/(d1*d1*d1);
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);	*/	
					parIn.SetType(typeOut);		
					TransformCov(J, parIn.GetCov());	
					break;
				}
						
			}		
			break;
		}
		case eKisel:
		{
			switch(typeOut){
				case eCartesian:
				{
					double a=sqrt(1+parIn.GetPar(2)*parIn.GetPar(2)+parIn.GetPar(3)*parIn.GetPar(3));
					double p3 = parIn.GetCharge()*parIn.GetPar(2)/parIn.GetPar(4)/a;
					double p4 = parIn.GetCharge()*parIn.GetPar(3)/parIn.GetPar(4)/a;
					double p5 = parIn.GetCharge()/parIn.GetPar(4)/a;
					
					J[0][0] = 1;
					J[1][1] = 1;
					J[2][2] = parIn.GetCharge()*(1+parIn.GetPar(3)*parIn.GetPar(3))/(a*a*a*parIn.GetPar(4));
					J[2][3] = -parIn.GetCharge()*parIn.GetPar(2)*parIn.GetPar(3)/(a*a*a*parIn.GetPar(4));
					J[2][4] = -parIn.GetCharge()*parIn.GetPar(2)/(a*parIn.GetPar(4)*parIn.GetPar(4));
					J[3][2] = -parIn.GetCharge()*parIn.GetPar(2)*parIn.GetPar(3)/(a*a*a*parIn.GetPar(4));
					J[3][3] = parIn.GetCharge()*(1+parIn.GetPar(2)*parIn.GetPar(2))/(a*a*a*parIn.GetPar(4));
					J[3][4] = -parIn.GetCharge()*parIn.GetPar(3)/(a*parIn.GetPar(4)*parIn.GetPar(4));
					J[4][2] = -parIn.GetCharge()*parIn.GetPar(2)/(a*a*a*parIn.GetPar(4));
					J[4][3] = -parIn.GetCharge()*parIn.GetPar(3)/(a*a*a*parIn.GetPar(4));
					J[4][4] = -parIn.GetCharge()/(a*parIn.GetPar(4)*parIn.GetPar(4));
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);		
					parIn.SetType(typeOut);		
					TransformCov(J, parIn.GetCov());		
					break;
				}
				case eNA61:
				{
					double a=sqrt(1+parIn.GetPar(2)*parIn.GetPar(2)+parIn.GetPar(3)*parIn.GetPar(3));
					double b=sqrt(1+parIn.GetPar(2)*parIn.GetPar(2));
					double p1 = parIn.GetPar(4)*a/b;
					double p2 = parIn.GetPar(3)/b;
					double p3 = atan(1/parIn.GetPar(2));
					double p4 = parIn.GetPar(0);
					double p5 = parIn.GetPar(1);
					
					J[0][2] = -parIn.GetPar(2)*parIn.GetPar(3)*parIn.GetPar(3)*parIn.GetPar(4)/(a*b*b*b);
					J[0][3] = parIn.GetPar(3)*parIn.GetPar(4)/(a*b);
					J[0][4] = a/b;
					J[1][2] = -parIn.GetPar(2)*parIn.GetPar(3)/(b*b*b);;
					J[1][3] = 1/b;
					J[2][2] = -1/(b*b);
					J[3][0] = 1;
					J[4][1] = 1;

					parIn.SetPar(0, p1);
					parIn.SetPar(1, p2);					
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);				
					/*J[0][0] = 1;
					J[1][1] = 1;
					J[2][2] = -parIn.GetPar(2)*parIn.GetPar(3)*parIn.GetPar(3)*parIn.GetPar(4)/(a*b*b*b);
					J[2][3] = parIn.GetPar(3)*parIn.GetPar(4)/(a*b);
					J[2][4] = a/b;
					J[3][2] = -parIn.GetPar(2)*parIn.GetPar(3)/(b*b*b);
					J[3][3] = 1/b;
					J[4][2] = -2/(b*b+(b-parIn.GetPar(2))*(b-parIn.GetPar(2)))/b;
					parIn.SetPar(2, p3);
					parIn.SetPar(3, p4);
					parIn.SetPar(4, p5);	*/	
					parIn.SetType(typeOut);		
					TransformCov(J, parIn.GetCov());		
					break;
				}
						
			}			
			break;
		}
		
	}
	
	

	//SetCovarianceMatrix(cov, parIn);
}
