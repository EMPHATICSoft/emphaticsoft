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
#ifndef SSDSTATIONPTALGO1_H
#define SSDSTATIONPTALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>
#include "RecoBase/SSDCluster.h"

namespace rb {

    typedef enum tSSDStationPtType { STNONE = 0, // unspecified. 
                                STXYONLY = 1,
                                STXUONLY = 2,
				STYUONLY = 3,
				STXYU = 4,
                                STXWONLY = 5,
				STYWONLY = 6,
				STXYW = 7,
			} SSDStationPtType;

    class SSDStationPtAlgo1 {
    
      public:
       
        SSDStationPtAlgo1();
	~SSDStationPtAlgo1() { ; }
        
      private:
      
        SSDStationPtType fType;
	int fStationNum;
	mutable int fUserFlag; 
	double fX, fY, fXErr, fYErr; // Assumes all measurement are taken at the same Z. 
	double fChiSq; // omly valid for 3 View Points. 
	double fUorVPred, fUorVObsRaw, fUorVObsCorr; 
	std::vector<int> fClIds;
	std::vector<emph::geo::sensorView> fClViews;
	std::vector<double> fClAvs, fClSigmas; // corrected 
        
      public: 
      
        inline void Reset() { // type will be non, vectors cleared.
	  fType = rb::STNONE;
	  fX = DBL_MAX; fY = DBL_MAX; fXErr = DBL_MAX; fYErr = DBL_MAX; fChiSq = DBL_MAX;
	  fUorVPred = DBL_MAX;  fUorVObsRaw = DBL_MAX; fUorVObsCorr = DBL_MAX;
	  fClIds.clear(); fClAvs.clear(); fClSigmas.clear(); fClViews.clear();
	} 
	inline void SetStationNum(int kSt) { fStationNum = kSt; } 
        void Add(std::vector<rb::SSDCluster>::const_iterator itCl, double aClCorrectedMean, double aClErr);
	inline void SetX(double xVal, double xErr) { fX = xVal; fXErr = xErr; }
	inline void SetY(double yVal, double yErr) { fY = yVal; fYErr = yErr; }
	inline void SetUorVInfo(double u1, double u2, double u3) { 
	  fUorVPred = u1; fUorVObsRaw = u2;  fUorVObsCorr = u3;
	}
	inline void SetStation(int aSt) { fStationNum = aSt; }
	inline void SetType(SSDStationPtType aType) { fType = aType; }
	inline void SetChiSq(double v) {fChiSq = v; }
	inline void SetUserFlag(int v) const  {fUserFlag = v;} 
	// 
	// Getter
	//
	inline rb::SSDStationPtType Type() const { return fType; }
	inline int Station() const { return fStationNum; } 
	inline int UserFlag() const { return fUserFlag; }
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
	inline int ClusterView(size_t k) const { 
	   if (k >= fClIds.size()) { return static_cast<size_t>(INT_MAX); } 
	   return fClViews[k];
        } 
	inline double ClusterCorrMeasurement(size_t k) const { 
	   if (k >= fClAvs.size()) { return DBL_MAX; } return fClAvs[k]; } 
	inline double ClusterMeasurementError(size_t k) const { 
	   if (k >= fClSigmas.size()) { return DBL_MAX; } return fClSigmas[k]; } 
	     
        // IO
	
       friend std::ostream& operator << (std::ostream& o, const SSDStationPtAlgo1& h);
    
	
   };
} // namespace rb
#endif // SSDSTATIONPTALGO1_H

    
        
