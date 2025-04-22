/*****************************************************************************
*
*
*	\author M. Pavin\n
*	Email: mpavin@cern.ch \n
*	\date 20. 07. 2018.
*/
//*****************************************************************************

#ifndef RECONSTRUCTION_H
#define RECONSTRUCTION_H

#include <string>
#include <vector>
#include <iostream>

#include "TMinuit.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TrackExtrapolation.h"
#include "Plane.h"

//#define DEBUG 0
class Reconstruction {

	public:
	
                Reconstruction (std::string mapFile, int instance=0);
		Reconstruction(){};

		~Reconstruction (){}


  		//****************************************************************
   		//*********************MC RECONSTRUCTION************************** 		

   		void DoMC(std::vector<double> hitX, std::vector<double> hitY, std::vector<double> hitZ, std::vector<double> hitP, std::vector<double> hitTheta, std::vector<double> hitPhi);
   		void AddPlane(Plane& plane){fPlanes.push_back(plane);}
		TH1D* GetHistodp(){return hdp;}
		TH1D* GetHistodtx(){return hdtx;}
		TH1D* GetHistodty(){return hdty;}
		TH1D* GetHistodx(){return hdx;}
		TH1D* GetHistody(){return hdy;}
		
		TH1D* GetHistodpRel(){return hdpRel;}
		TH1D* GetHistodtxRel(){return hdtxRel;}
		TH1D* GetHistodtyRel(){return hdtyRel;}
		TH1D* GetHistodxRel(){return hdxRel;}
		TH1D* GetHistodyRel(){return hdyRel;}
		
		TrackExtrapolation* GetExtrapolation(){return fExtrap;}
  		//****************************************************************				

		// Getters and setters
		const vector<Plane>& get_planes() const { return fPlanes; }

		void set_planes( const vector<Plane>& planes ) { fPlanes = planes; }

		const vector<Cluster>& get_clusters() const { return clusters; }

 	private:

		//void CalculateClusterPosition(int id, double x1, double y1, double &x, double &y, double &xErr, double &yErr);
		void CalculateClusterPosition(Cluster &c);

		TrackExtrapolation *fExtrap;
		TRandom3 *fRandom;
		TH1D *fCharge;
		TH1D *hdp;
		TH1D *hdtx;
		TH1D *hdty;
		TH1D *hdx;
		TH1D *hdy;
		
		TH1D *hdpRel;
		TH1D *hdtxRel;
		TH1D *hdtyRel;
		TH1D *hdxRel;
		TH1D *hdyRel;
		
		vector<Plane> fPlanes;
		vector<Cluster> clusters;
};

#endif
