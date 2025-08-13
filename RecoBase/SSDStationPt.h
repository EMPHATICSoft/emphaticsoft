////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a reconstructed space point at a given station, based on either two or three SSDClusters
///          For Phase1b, Used primarly downstream of the target.  
///          The Z position is assumed to be in between the Z position of the sensors, the track slope need not 
///          to be known.  
///          The ID,  average and uncertainties of the SSDClusters measured position are stored in this class, 
///          This data is corrected for misalignment, using the volatileAlignmentParameters. 
///          So, this data is ready to be used in the reconstruction of the downstream (of the target) tracks. 
///
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDSTATIONPT_H
#define SSDSTATIONPT_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>
#include "RecoBase/SSDCluster.h"
#include "RecoBase/LineSegment.h"

namespace rbex {

	typedef enum tSSDStationPtType
	{ 
		STNONE = 0, // unspecified. 
		STXYONLY = 1,
		STXUONLY = 2,
		STYUONLY = 3,
		STXYU = 4,
		STXWONLY = 5,
		STYWONLY = 6,
		STXYW = 7,
	} SSDStationPtType;

	typedef std::vector<rb::SSDCluster>::const_iterator myItCl;

	class SSDStationPt
	{
	
	  public:
	   
		SSDStationPt();
		~SSDStationPt() { ; }
        
      private:
      
        SSDStationPtType fType;
		int fId;
		int fStationNum;
		mutable int fUserFlag; 
		double fX, fY; // Assumes all measurement are taken at the same Z. 
		mutable double fXErr, fYErr; // A bit Sleazy:  we will change the uncertainty, based on a change of the momentm. 
		// See below  
		double fChiSq; // only valid for 3 View Points. 
		double fUorVPred, fUorVObsRaw, fUorVObsCorr; 
		bool fHasXOverlap, fHasYOverlap, fHasUorVOverlap; // only applicable to Phase1c
		std::vector<int> fClIds;
		std::vector<emph::geo::sensorView> fClViews;
		std::vector<int> fClSensorIds;
		std::vector<double> fClAvs, fClSigmas; // corrected 
		rb::LineSegment fLineStripUorV; // For debugging the representation of the space point graphically. 
		std::vector<myItCl> fItClusters; // duplicate info above, but convienent.. 3 pointers not a big memory footprint.
	
	  public: 
	  
		inline void Reset() 
		{ // type will be non, vectors cleared.
			fType = rbex::STNONE;
			fX = DBL_MAX; fY = DBL_MAX; fXErr = DBL_MAX; fYErr = DBL_MAX; fChiSq = DBL_MAX;
			fUorVPred = DBL_MAX;  fUorVObsRaw = DBL_MAX; fUorVObsCorr = DBL_MAX;
			fHasXOverlap = false; fHasYOverlap = false; fHasUorVOverlap = false;
			fClIds.clear(); fClAvs.clear(); fClSigmas.clear(); fClViews.clear(); fClSensorIds.clear();
			fItClusters.clear();
		} 

		inline void SetStationNum(int kSt) { fStationNum = kSt; } 
		inline void SetID(int i) { fId = i; } 
		void Add(std::vector<rb::SSDCluster>::const_iterator itCl, double aClCorrectedMean, double aClErr);
		inline void SetX(double xVal, double xErr) { fX = xVal; fXErr = xErr; }
		inline void SetY(double yVal, double yErr) { fY = yVal; fYErr = yErr; }
		inline void SetUorVInfo(double u1, double u2, double u3) 
		{ 
			fUorVPred = u1; fUorVObsRaw = u2;  fUorVObsCorr = u3;
		}
		inline void SetStation(int aSt) { fStationNum = aSt; }
		inline void SetClSensorIds(int kSeX, int kSeY, int kSeUorV) { 
		fClSensorIds.clear(); fClSensorIds.push_back(kSeX);  fClSensorIds.push_back(kSeY);
		fClSensorIds.push_back(kSeUorV); 
		}
		inline void SetType(SSDStationPtType aType) { fType = aType; }
		inline void SetChiSq(double v) {fChiSq = v; }
		inline void SetUserFlag(int v) const  {fUserFlag = v;} 
		inline void SetLineUorV(const rb::LineSegment &lsUV) { fLineStripUorV = lsUV; }
		inline void SetXOverlap(bool t=true) { fHasXOverlap = t;}
		inline void SetYOverlap(bool t=true) { fHasYOverlap = t;}
		inline void SetUorVOverlap(bool t=true) { fHasUorVOverlap = t;}
		// 
		// Getter
		//
		inline SSDStationPtType Type() const { return fType; }
		inline int Station() const { return fStationNum; } 
		inline int UserFlag() const { return fUserFlag; }
		inline int ID() const { return fId; }  
		inline double X() const { return fX; }  
		inline double Y() const { return fY; }  
		inline double XErr() const { return fXErr; }  
		inline double YErr() const { return fYErr; } 
		inline double ChiSq() const { return fChiSq; }
		inline double UorWPred() const { return fUorVPred; } 
		inline double UorWObsRaw()  const{ return fUorVObsRaw; } 
		inline double UorWObsCorr() const { return fUorVObsCorr; } 
		inline size_t NumClusters() const { return fClIds.size(); }
		inline int ClusterID(size_t k) const { 
		if (k >= fClIds.size()) { return INT_MAX; } 
		return fClIds[k]; 
		} 
		inline emph::geo::sensorView ClusterView(size_t k) const 
		{ 
		if (k >= fClViews.size()) { return emph::geo::INIT; } 
		return fClViews[k];
		} 
		inline int ClusterSensorId(size_t k) const 
		{ 
		if (k >= fClSensorIds.size()) { return static_cast<size_t>(INT_MAX); } 
		return fClSensorIds[k];
		} 
		inline double ClusterCorrMeasurement(size_t k) const { 
		if (k >= fClAvs.size()) { return DBL_MAX; } return fClAvs[k]; } 
		inline double ClusterMeasurementError(size_t k) const { 
		if (k >= fClSigmas.size()) { return DBL_MAX; } return fClSigmas[k]; }
			
		void ReScaleMultUncert(double multScatt120, double pOld, double pNew) const;
	
		inline rb::LineSegment LineStripUorV() const { return fLineStripUorV; } 
		inline void fillItClusters(std::vector<myItCl> &itCls) const {
		itCls.clear(); for (size_t k=0; k != fItClusters.size(); k++) { itCls.push_back(fItClusters[k]); }
		}
		inline void addItClusters(std::vector<myItCl> &itCls) const {
		for (size_t k=0; k != fItClusters.size(); k++) { itCls.push_back(fItClusters[k]); }
		}
		inline void addOverlapClusters(const myItCl &itClAdded, double addedMeas, double addedMeasErr) {
		fItClusters.push_back(itClAdded); fClIds.push_back(itClAdded->ID()); 
		fClViews.push_back(itClAdded->View()); fClSensorIds.push_back(itClAdded->Sensor());
		fClAvs.push_back(addedMeas); fClSigmas.push_back(addedMeasErr);
		}
	
	   friend std::ostream& operator << (std::ostream& o, const SSDStationPt& h);
	
	
   };
} // namespace rbex
#endif // SSDSTATIONPT_H
