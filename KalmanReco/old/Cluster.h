/*****************************************************************
* Author:		M. Pavin 
* Email:		mpavin@triumf.ca
* Date created: 02/14/2018
* Collaboration:EMPHATIC (T1396)
*
* Streamable ROOT class for storing Si clusters
******************************************************************/

#ifndef ROOT_Cluster
#define ROOT_Cluster

#ifndef ROOT_TObject
#include "TObject.h"
#endif

enum ClusterCoordinate{
	eX = 0,
	eY,
	eZ,
	eXY
};
class Cluster : public TObject{
	public:
		Cluster ();
		Cluster (double x, double y, double z);
		Cluster (double x, double y, double z, double xerr, double yerr, double zerr, int PlaqueId);
		Cluster (const Cluster &cluster);
		~Cluster();
		
		void Copy(TObject &cluster) const;
		
		double GetCoordinate (ClusterCoordinate c) const {return fCoordinates[c];}
		double GetCoordinateError (ClusterCoordinate c) const {return fErrors[c];}
		int GetPlaqueId() {return fPlaqueId;}
		bool IsLocal() {return fLocal;}
		
		void SetCoordinate (ClusterCoordinate c, double val) {fCoordinates[c] = val;}
		void SetError (ClusterCoordinate c, double val) {fErrors[c] = val;}
		void SetPlaqueId (int id) {fPlaqueId = id;}
		void SetLocalSystem(bool isLocal){fLocal = isLocal;}
	private:
		
		double fCoordinates[3];
		double fErrors[4];
		int fPlaqueId;
		bool fLocal;
	ClassDef(Cluster, 1)
};

#endif
