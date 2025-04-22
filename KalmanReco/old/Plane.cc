#include "Plane.h"
#include "TMath.h"

Plane::Plane(){
    for(int i = 0; i < 3; i++){
        fPosition[i] = 0;
        fRotation[i] = 0;
        fSize[i] = -1;
    }
    
    fEfficiency = 1;
    fChargeSharing = 0;
    fPitch = 0.006;
    fNADCChannels = 8;
    fActiveLayer = 0;
    
    SVector3 fNormVector;
}

void Plane::SetPosition(double x, double y, double z){
    fPosition[0] = x;
    fPosition[1] = y;
    fPosition[2] = z;
}

void Plane::SetRotation(double alpha, double beta, double gamma){
    fRotation[0] = alpha;
    fRotation[1] = beta;
    fRotation[2] = gamma;
    
   // UpdateLayerEquations();
    InitializeMatrices();
}

void Plane::SetSize(double sx, double sy, double sz, double radLength){
    fSize[0] = sx;
    fSize[1] = sy;
    fSize[2] = sz;
    
    fLayers.push_back(sz);
    fRadiationLengths.push_back(radLength);
}

void Plane::SetSize(double sx, double sy){
    fSize[0] = sx;
    fSize[1] = sy;
}

void Plane::AddLayers(std::vector<double> layers, std::vector<double> radLengths){
    fLayers = layers;
    fRadiationLengths = radLengths;
    
    double sz = 0;
    for(unsigned int i = 0; i < layers.size(); i++){
        sz += layers.at(i);
    }
    fSize[2] = sz;
    
    UpdateLayerEquations();
}

void Plane::AddLayer(double sz, double radLength){
    fLayers.push_back(sz);
    fRadiationLengths.push_back(radLength);  
    
    fSize[2] += sz;  
    
    UpdateLayerEquations();
}

void Plane::GetPosition(double& x, double& y, double &z){
    x = fPosition[0];
    y = fPosition[1];
    z = fPosition[2];
}

void Plane::GetRotation(double& alpha, double& beta, double& gamma){
    alpha = fRotation[0];
    beta = fRotation[1];
    gamma = fRotation[2];
}

void Plane::GetSize(double &sx, double& sy, double &sz){
    sx = fSize[0];
    sy = fSize[1];
    sz = fSize[2];
}

void Plane::InitializeMatrices(){
	double alpha = fRotation[0]*TMath::Pi()/180.;
	double beta = fRotation[1]*TMath::Pi()/180.;
	double gamma = fRotation[2]*TMath::Pi()/180.;   
    

	fRotMat(0,0) = cos(beta )*cos(gamma);
	fRotMat(0,1) =-cos(alpha)*sin(gamma)+sin(alpha)*sin(beta )*cos(gamma);
	fRotMat(0,2) = sin(alpha)*sin(gamma)+cos(alpha)*sin(beta )*cos(gamma);
	fRotMat(1,0) = cos(beta)*sin(gamma);
	fRotMat(1,1) = cos(alpha)*cos(gamma)+sin(alpha)*sin(beta)*sin(gamma);
	fRotMat(1,2) =-sin(alpha)*cos(gamma)+cos(alpha)*sin(beta)*sin(gamma);
	fRotMat(2,0) =-sin(beta);
	fRotMat(2,1) = sin(alpha)*cos(beta);
	fRotMat(2,2) = cos(alpha)*cos(beta);
	
	int flag;
	fRotMatInv = fRotMat.Inverse(flag);
}

void Plane::translateXY(SVector3 &V, bool back){
	if(!back){
	    V(0) = V(0) + fPosition[0];
	    V(1) = V(1) + fPosition[1];
	}
	else{//we can shift back the coordinate
	    V(0) = V(0) - fPosition[0];
	    V(1) = V(1) - fPosition[1];
	}
}

void Plane::FromLocalToGlobal(Cluster &c){

    SVector3 V;
    V(0) = c.GetPosition(eX);
    V(1) = c.GetPosition(eY);
    V(2) = c.GetPosition(eZ);
    
    
    V = fRotMat*V;
    translateXY(V,false);
    V(2) = V(2) + fPosition[2];
    
    c.SetPosition(V(0), V(1), V(2));

}

void Plane::FromGlobalToLocal(Cluster &c){
    SVector3 V;
    V(0) = c.GetPosition(eX);
    V(1) = c.GetPosition(eY);
    V(2) = c.GetPosition(eZ);
    
    translateXY (V,true);
    V(2) = V(2) - fPosition[2];
    V = fRotMatInv*V;
    V(2) = 0;
    
    c.SetPosition(V(0), V(1), V(2));
    
}

void Plane::UpdateLayerEquations(){
        
    fDPar.clear();
    
	fNormVector(0) = 0;
	fNormVector(1) = 0;
	fNormVector(2) = 1;
		
	fNormVector = fRotMat*fNormVector;
    
    double d = -fNormVector(0)*fPosition[0] - fNormVector(1)*fPosition[1] - fNormVector(2)*fPosition[2];
    
    
    double halfThickness = 0;
    for(unsigned int i = 0; i < fLayers.size(); i++){
        if(i < fActiveLayer) halfThickness += fLayers.at(i);
        else if ( i == fActiveLayer) halfThickness += fLayers.at(i)/2.;
    } 


    //double x1 = 0;
    //double y1 = 0;
    double z1 = -d/fNormVector(2);
    
    double start = -halfThickness; 
    double norm = sqrt(fNormVector(0)*fNormVector(0) + fNormVector(1)*fNormVector(1) + fNormVector(2)*fNormVector(2));
    
    double dTemp = -fNormVector(2)*z1+start*norm;
    fDPar.push_back(dTemp);
    cout<< "Add plane: " << fNormVector(0) << " " << fNormVector(1) << " " << fNormVector(2) << " " << dTemp << endl;
    for(unsigned int i = 0; i < fLayers.size(); i++){
        if(i == fActiveLayer) {
            fDPar.push_back(d);
            cout<< "Add plane: " << fNormVector(0) << " " << fNormVector(1) << " " << fNormVector(2) << " " << d << endl;
        }
        
        start += fLayers.at(i);
        dTemp = -fNormVector(2)*z1+start*norm;
        fDPar.push_back(dTemp);
        
        cout<< "Add plane: " << fNormVector(0) << " " << fNormVector(1) << " " << fNormVector(2) << " " << dTemp << endl;
    }
}
