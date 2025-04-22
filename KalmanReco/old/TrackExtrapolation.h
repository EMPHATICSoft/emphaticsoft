#ifndef TRACKEXTRAPOLATION_H
#define TRACKEXTRAPOLATION_H

#include <iostream>
#include "TrackTools.h"
#include "MagneticField.h"
#include <string>
#include "Plane.h"
//#define DEBUG 1
using namespace std;

using namespace TrackTools;

class TrackExtrapolation{

	public:
		TrackExtrapolation(bool cov, double step, string field, double radLength=0);
		TrackExtrapolation(bool cov, TrackPar trPar,  double step, string field, double radLength=0);
		~TrackExtrapolation();
		//**********************************Get functions********************************
		double GetStep(){return fStep;}
		TrackPar& GetStartTrackParam(){return fStartPar;}
		TrackPar& GetStopTrackParam(){return fStopPar;}

		static double GetKappa(){return fKappa;}
		bool GetErrorEstimation(){return fErrorEstimation;}
		//**********************************Set functions********************************
		void SetTrackPar(TrackPar &par){fStartPar = par; fStopPar = par;}
		//void SetClusters(const evt::rec::Track& track, const evt::RecEvent& recEvent);
		void SetRadLength(double val){fRadLength = val;}
		void SetStep(double step){fStep = step;}
		void SetErrorEstimation(bool cov){fErrorEstimation = cov;};
		//**********************************Other functions******************************
	
		void Extrapolate(double zStop, double step, double radLength);
		void Extrapolate(double zStop, double radLength);
		void Extrapolate(double zStop);
		void ExtrapolateToPlane(const double &a, const double&b, const double& c, const double& d);
		//void ExtrapolateToPlane(double radLength, Plane& plane);
		//void ExtrapolateToPlane(Plane& plane);
		void DoKalmanStep(Cluster &cluster, Plane &plane);

		
	private:


		void AddNoise(TrackPar &trackPar, double X);

		
		TrackPar fStartPar;
		TrackPar fStopPar;
		
		bool fErrorEstimation;
		
		double fStep;
		double fLength;

		MagneticField *fMagField;
		
		static double fKappa;
		double fRadLength;

		//help variables;
		
};

#endif
