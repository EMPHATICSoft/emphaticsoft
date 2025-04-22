#ifndef TrackTools_H
#define TrackTools_H

#include <vector>
#include <iostream>

using namespace std;

namespace TrackTools{
	
	enum TrackingType{
					//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
		eUnknown,	// It's not known!										+
					// (p1,		p2,		p3,		p4,		p5)					+
		eCartesian,	// (x, 		y, 		px, 	py, 	pz)					+
		eNA61,		// (x, 		y, 		q/pxz, 	py/pxz, 2atan((pxz-px)/pxz)	+
		eKisel	 	// (x, 		y, 		px/pz, 	py/pz, 	q/p)				+
					//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
	};
	

	enum Coordinate{
		eX,
		eY,
		eZ
	};		
	
	enum CorError{
		errX,
		errY
	};
	class TrackPar{
		public:
			TrackPar();
			TrackPar(TrackingType type);
			TrackPar(TrackingType type, double z, double *par);
			TrackPar(TrackingType type, double z, double *par, double *cov);
			TrackPar(TrackingType type, int charge, double z, double *par, double *cov);
		
			TrackPar& operator= (TrackPar &parIn);
			TrackingType GetType(){return fType;}
			int GetCharge(){return fCharge;}
			double GetPar(int i);
			double GetCov(int i);
			double* GetCov(){return fCov;}
			double GetStd(int i);
			double GetZ(){return fZ;}
			double GetLength(){return fLength;}
			
			void SetType(TrackingType type){fType = type;}
			void SetCharge(int charge){fCharge = charge;}
			void SetPar(int i, double val);
			void SetPar(double *par);
			void SetCov(int i, double val);
			void SetCov(double *cov);
			void SetZ(double val){fZ = val;}
			void SetLength(double length){fLength = length;}
			void AddLength(double d){fLength += d;}
			void Print();
		private:
		
			//void ParameterCheck(vector<double> &par);
			//void CovarianceCheck(vector<double> &cov);
			void ChargeCheck(TrackingType type, double *par);
			//void CheckIndex(int i, vector<double> vec);
			
			TrackingType fType;
			int fCharge;
			
			double fZ;
			
			double fPar[5];
			double fCov[15];
			
			double fLength;
	
	};


	class Cluster{
		public:
			Cluster(double x, double y, double z, double dx, double dy, int type, int id): fType(type) {fPos[0] = x; fPos[1]= y; fPos[2] = z; fError[0] = dx; fError[1] = dy; fPlaneId = id;}
			
			void SetPosition(double x, double y, double z){fPos[0] = x; fPos[1] = y; fPos[2] = z;}
			void SetPositionError(double dx, double dy){fError[0] = dx; fError[1] = dy;}
			void SetType(int type){fType = type;}
			void SetPlaneId(int id){fPlaneId = id;}
			
			double GetPosition(Coordinate c) const;
			double GetError(CorError c) const;
			int GetType(){return fType;}
			int GetPlaneId(){return fPlaneId;}
		private:
			double fPos[3];
			double fError[2];
			int fType;
			int fPlaneId;
	
	};	
	//void GetCovarianceMatrix(double (*cov)[5], TrackPar &parIn);
	//void SetCovarianceMatrix(double (*cov)[5], TrackPar &parIn);
	void TransformCov(double (*J)[5], double *c);
	void ConvertTrackPar(TrackPar &parIn, TrackingType type);
	

}

#endif
