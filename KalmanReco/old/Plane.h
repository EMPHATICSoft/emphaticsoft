#ifndef PLANE_H
#define PLANE_H

#include <string>
#include <vector>
#include <iostream>

#include "TrackTools.h"

#include <Math/SMatrix.h>
#include <Math/SVector.h>

typedef ROOT::Math::SMatrix<double,3>                                       SMatrix33;
typedef ROOT::Math::SVector<double,3>                                       SVector3;

using namespace TrackTools;
class Plane {

	public:
	
		Plane ();
		
		void SetPosition(double x, double y, double z);
		void SetRotation(double alpha, double beta, double gamma);
		void SetSize(double sx, double sy, double sz, double radLength);
		void SetSize(double sx, double sy);
		void AddLayers(std::vector<double> layers, std::vector<double> radLengths);
		void AddLayer(double sz, double radLength);
		void SetActiveLayer(unsigned int l){fActiveLayer = l;}
		void SetEfficiency(double eff){fEfficiency = eff;}
		void SetPitch(double pitch){fPitch = pitch;}
		void SetChargeSharing(double q){fChargeSharing = q;}
		void SetNumberOfADCChannels(int n){fNADCChannels = n;}
		void SetAlignFit( bool infit, bool fitrot, bool fitz ){
		  in_align_fit = infit; align_rot = fitrot; align_z = fitz; }

		
		void GetPosition(double& x, double& y, double &z);
		void GetRotation(double& alpha, double& beta, double& gamma);
		void GetSize(double &sx, double& sy, double& sz);
		double GetSizeX(){return fSize[0];}
		double GetSizeY(){return fSize[1];}
		double GetSizeZ(){return fSize[2];}
		double GetZPosition(){return fPosition[2];}
		std::vector<double>& GetLayers(){return fLayers;}
		std::vector<double>& GetRadiationLengths(){return fRadiationLengths;}
		std::vector<double>& GetDPars(){return fDPar;}
		unsigned int GetActiveLayer(){return fActiveLayer;}
		double GetEfficiency(){return fEfficiency;}
		double GetPitch(){return fPitch;}
		double GetChargeSharing(){return fChargeSharing;}
		
		const SVector3 GetPlaneNormVector(){return fNormVector;}
		const SMatrix33& GetRotationMatrix(){return fRotMat;}
		const SMatrix33& GetInverseRotationMatrix(){return fRotMatInv;}
		int GetNumberOfADCChannels(){return fNADCChannels;}

		bool AlignFit() const { return in_align_fit; }
		bool AlignRotFit() const { return align_rot; }
		bool AlignZFit() const { return align_z; } 

		//***************************************************************************************
        void FromLocalToGlobal(Cluster &c);
        void FromGlobalToLocal(Cluster &c);
        
		
	private:
	    void InitializeMatrices();
	    void translateXY(SVector3 &V, bool back);
	    
	    void UpdateLayerEquations();
	    double fPosition[3];
	    double fRotation[3];
	    double fSize[3];
	    std::vector<double> fLayers;
	    std::vector<double> fRadiationLengths;
	    std::vector<double> fDPar;
	    
	    unsigned int fActiveLayer;
	    
	    double fEfficiency;
	    double fPitch;
	    double fChargeSharing;
	    int fNADCChannels;

	    bool in_align_fit;
	    bool align_rot;
	    bool align_z;

        SMatrix33 fRotMat;
        SMatrix33 fRotMatInv;
        SVector3 fNormVector;
};

#endif    
		
		
