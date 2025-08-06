////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Beam track, 3D, reconstructed based on 2D on plane XZ, YZ, independentely, 
///          and cross-check downstream of the target with U projection (V currently too confusing.. ) 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef BEAMTRACK_H
#define BEAMTRACK_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"

namespace rb {

    typedef enum tBeamTrType { NONE = 0, // unspecified. 
                                XYONLY = 10,
                                XONLY = 2,
				YONLY = 3,
				XYUCONF1 = 11,
				XYUCONF2 = 12,
				XYUCONF3 = 13,
				XYUCONF4 = 14,
				XYVCONF1 = 21,
				XYVCONF2 = 22,
				XYVCONF3 = 23,
				XYUVCONF1 = 121, // prelim... incomplete, too confusing.
				XYUVCONF2 = 122,
				XYUVCONF3 = 123,
				XYUW5ST = 1000
			} BeamTrType;
				
  
  class BeamTrack {
  public:
    BeamTrack(); // Default constructor
    virtual ~BeamTrack() {}; //Destructor
    
  private:

     BeamTrType fTrType;
     mutable int fUserFlag; 
     double fTrXOffset, fTrYOffset, fTrXSlope, fTrYSlope; // Assume straight track, these are the parameters at Station 0.   
     double fTrXOffsetErr, fTrYOffsetErr, fTrXSlopeErr, fTrYSlopeErr; // uncertainties for quantities above .
     double fTrXCovOffSl, fTrYCovOffSl; // covariance matrxi to compute the uncertainties 
     double fChiSqX, fChiSqY;
     double fMomentum, fMomentumErr; 
//     std::vector<int> fNHitsXView, fNHitsYView;  not sure if relevant, limited use.. 
// Back pointer to SSD Cluster..
//
     std::vector<rb::SSDCluster>::const_iterator fItClX0, fItClX1, fItClY0, fItClY1; // careful, not initialized.. in the constructor.. 
	  
  public:
   // Setters 
   void Reset(); // Set everyting to NONE or DBL_MAX, to be refilled again.  
   void Reset(std::vector<rb::SSDCluster>::const_iterator endCls); 
     // Set everyting to NONE or DBL_MAX, to be refilled again, and iterator to upstream station to cend of clusters..  
     // In case we want to be safe,,  
   inline void SetType(BeamTrType t)  { fTrType = t; } 
   inline void SetTrParams(double x0, double y0, double xSl, double ySl) { 
     fTrXOffset = x0; fTrYOffset = y0; fTrXSlope = xSl; fTrYSlope = ySl; 
   }
   inline void SetTrItCls(std::vector<rb::SSDCluster>::const_iterator itClx0, 
                          std::vector<rb::SSDCluster>::const_iterator itClx1,
			  std::vector<rb::SSDCluster>::const_iterator itCly0,
			  std::vector<rb::SSDCluster>::const_iterator itCly1) { 
     fItClX0 = itClx0; fItClX1 = itClx1; fItClY0 = itCly0; fItClY1 = itCly1; 
   }
   inline void SetXTrParams(double x0, double xSl) { 
     fTrXOffset = x0; fTrXSlope = xSl; 
   }
   inline void SetYTrParams(double y0, double ySl) { 
     fTrYOffset = y0; fTrYSlope = ySl; 
   }
   inline void SetTrParamsErrs(double x0, double y0, double xSl, double ySl, double covX, double covY) { 
     fTrXOffsetErr = x0; fTrYOffsetErr = y0; fTrXSlopeErr = xSl; fTrYSlopeErr = ySl; 
     fTrXCovOffSl = covX; fTrYCovOffSl = covY;
   }
   inline void SetXTrParamsErrs(double x0, double xSl, double covX) { 
     fTrXOffsetErr = x0; fTrXSlopeErr = xSl; fTrXCovOffSl = covX;
   }
   inline void SetYTrParamsErrs(double y0, double ySl, double covY) { 
     fTrYOffsetErr = y0; fTrYSlopeErr = ySl; fTrYCovOffSl = covY;
   }
   inline void SetMomentum(double p, double pErr) { fMomentum=p; fMomentumErr = pErr; }
   inline void SetXChiSq(double c ) { fChiSqX = c;}
   inline void SetYChiSq(double c ) { fChiSqY = c;}
   inline void SetUserFlag(int v) const {fUserFlag = v;} 
   
    // Getters
    inline rb::BeamTrType Type() const { return fTrType; }
    inline double XOffset() const { return fTrXOffset; } 
    inline double XSlope() const { return fTrXSlope; } 
    inline double XOffsetErr() const { return fTrXOffsetErr; } 
    inline double XSlopeErr() const { return fTrXSlopeErr; } 
    inline double XChiSq() const { return fChiSqX; } 
    inline double XCovOffSl() const {return fTrXCovOffSl;}
    inline std::vector<rb::SSDCluster>::const_iterator ItClX0() const { return fItClX0; }
    inline std::vector<rb::SSDCluster>::const_iterator ItClX1() const { return fItClX1; }
    inline std::vector<rb::SSDCluster>::const_iterator ItClY0() const { return fItClY0; }
    inline std::vector<rb::SSDCluster>::const_iterator ItClY1() const { return fItClY1; }

    inline double YOffset() const { return fTrYOffset; } 
    inline double YSlope() const { return fTrYSlope; } 
    inline double YOffsetErr() const { return fTrYOffsetErr; } 
    inline double YSlopeErr() const { return fTrYSlopeErr; } 
    inline double YChiSq() const { return fChiSqY; } 
    inline double YCovOffSl() const {return fTrYCovOffSl;}
    inline int UserFlag() const { return fUserFlag; }
    inline double Momentum() const { return fMomentum; } 
    inline double MomentumErr() const { return fMomentumErr; } 
    
    
    friend std::ostream& operator << (std::ostream& o, const BeamTrack& h);
  };
  
}

#endif // BEAMTRACK_H
