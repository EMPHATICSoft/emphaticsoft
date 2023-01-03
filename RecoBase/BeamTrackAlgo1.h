////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Beam track, 3D, reconstructed based on 2D on plane XZ, YZ, independentely, 
///          and cross-check downstream of the target with U projection (V currently too confusing.. ) 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef BEAMTRACKALGO1_H
#define BEAMTRACKALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"

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
			} BeamTrType;
				
  
  class BeamTrackAlgo1 {
  public:
    BeamTrackAlgo1(); // Default constructor
    virtual ~BeamTrackAlgo1() {}; //Destructor
    
  private:

     BeamTrType fTrType;
     double fTrXOffset, fTrYOffset, fTrXSlope, fTrYSlope; // Assume straight track, these are the parameters at Station 0.   
     double fTrXOffsetErr, fTrYOffsetErr, fTrXSlopeErr, fTrYSlopeErr; // uncertainties for quantities above .
     double fTrXCovOffSl, fTrYCovOffSl; // covariance matrxi to compute the uncertainties 
     double fChiSqX, fChiSqY; 
//     std::vector<int> fNHitsXView, fNHitsYView;  not sure if relevant, limited use.. 
	  
  public:
   // Setters 
   void Reset(); // Set everyting to NONE or DBL_MAX, to be refilled again.  
   inline void SetType(BeamTrType t)  { fTrType = t; } 
   inline void SetTrParams(double x0, double y0, double xSl, double ySl) { 
     fTrXOffset = x0; fTrYOffset = y0; fTrXSlope = xSl; fTrYSlope = ySl; 
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
   inline void SetXChiSq(double c ) { fChiSqX = c;}
   inline void SetYChiSq(double c ) { fChiSqY = c;}
   
    // Getters
    inline rb::BeamTrType Type() const { return fTrType; }
    inline double XOffset () const { return fTrXOffset; } 
    inline double XSlope () const { return fTrXSlope; } 
    inline double XOffsetErr () const { return fTrXOffsetErr; } 
    inline double XSlopeErr () const { return fTrXSlopeErr; } 
    inline double XChiSq () const { return fChiSqX; } 
    inline double XCovOffSl() const {return fTrXCovOffSl;}
    

    inline double YOffset () const { return fTrYOffset; } 
    inline double YSlope () const { return fTrYSlope; } 
    inline double YOffsetErr () const { return fTrYOffsetErr; } 
    inline double YSlopeErr () const { return fTrYSlopeErr; } 
    inline double YChiSq () const { return fChiSqY; } 
    inline double YCovOffSl() const {return fTrYCovOffSl;}
    
    friend std::ostream& operator << (std::ostream& o, const BeamTrackAlgo1& h);
  };
  
}

#endif // BEAMTRACKALGO1_H
