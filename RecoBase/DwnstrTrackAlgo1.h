////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Downstream track, 3D, reconstructed based on 2D on plane XZ, YZ, independentely, 
///          and cross-check downstream of the target with U projection (V currently too confusing.. ) 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef DWNSTRTRACKALGO1_H
#define DWNSTRTRACKALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>

namespace rb {

    typedef enum tDwnstrTrType { TRDWNNONE = 0, // unspecified. 
                                 FOURSTATION = 1, // Best case
                                 FIVESTATION = 11, //Phase1c, downstream of target
				 STATION234 = 2,
				 STATION235 = 3, 
                                 SIXSTATION = 10 // Best case, when we have no magnet, target in fron ot Station 0
			} DwnstrTrType;
				
  
  class DwnstrTrackAlgo1 {
  public:
    DwnstrTrackAlgo1(); // Default constructor
    virtual ~DwnstrTrackAlgo1() {}; //Destructor
    
  private:

     DwnstrTrType fTrType;
     int fId; 
     mutable int fUserFlag; 
     double fTrXOffset, fTrYOffset, fTrXSlope, fTrYSlope; // curved track track, these are the parameters at Station 2. (phase1b) 
     double fTrMom; // momentum                                                   //   
     double fTrXOffsetErr, fTrYOffsetErr, fTrXSlopeErr, fTrYSlopeErr; // uncertainties for quantities above .
     double fTrMomErr; // momentum                                                   //   
     std::vector<double> fCovXY; // covariance matrix to compute the uncertainties 
     double fChiSq; // Note: if  atrack is abritrated away, it's chi-Square will be set to DBL_MAX, at some point. 
//     std::vector<int> fNHitsXView, fNHitsYView;  not sure if relevant, limited use.. 
     double fChiSqSts, fChiSqKlmX, fChiSqKlmY; // Addition info to quantify fits vs Kalman fits. 
	  
  public:
   // Setters 
   inline void Reset() { // Set everyting to NONE or DBL_MAX, to be refilled again.  
     fTrXOffset =  DBL_MAX; fTrYOffset = DBL_MAX; fId = INT_MAX;
     fTrXSlope = DBL_MAX; fTrYSlope = DBL_MAX; fTrMom = 120.0;
     fChiSq = -1.; for (size_t k=0; k!= fCovXY.size(); k++) fCovXY[k] = DBL_MAX;
   }
   inline void Reset(bool NoMagnet) { // Set everyting to NONE or DBL_MAX, to be refilled again.  
     fTrXOffset =  DBL_MAX; fTrYOffset = DBL_MAX; fId = INT_MAX;
     fTrXSlope = DBL_MAX; fTrYSlope = DBL_MAX; fTrMom = 120.0;
     if (NoMagnet) fCovXY.resize(16);
     fChiSq = -1.; for (size_t k=0; k!= fCovXY.size(); k++) fCovXY[k] = DBL_MAX;
   }
   inline void SetType(DwnstrTrType t)  { fTrType = t; } 
   inline void SetTrParams(double x0, double xSl, double y0, double ySl, double p) { 
     fTrXOffset = x0; fTrYOffset = y0; fTrXSlope = xSl; fTrYSlope = ySl; fTrMom = p;
   }
   inline void SetTrParamsErrs(double x0, double xSl, double y0, double ySl, double p) { 
     fTrXOffsetErr = x0; fTrYOffsetErr = y0; fTrXSlopeErr = xSl; fTrYSlopeErr = ySl;
     fTrMomErr = p;
   }
   inline void SetID(int id ) { fId = id;}
   inline void SetChiSq(double c ) { fChiSq = c;}  
   inline void SetChiSqKlmInfo(double cSt, double cx, double cy ) { fChiSqSts = cSt; fChiSqKlmX = cx; fChiSqKlmY = cy;}  
   inline void SetCovarianceMatrix(size_t k, double v) { if (k < fCovXY.size()) fCovXY[k] = v;}
   inline void SetUserFlag(int v) const {fUserFlag = v;} 

    // Getters
    inline rb::DwnstrTrType Type() const { return fTrType; }
    inline int ID() const { return fId; } 
    inline double XOffset() const { return fTrXOffset; } 
    inline double XSlope() const { return fTrXSlope; } 
    inline double XOffsetErr() const { return fTrXOffsetErr; } 
    inline double XSlopeErr() const { return fTrXSlopeErr; } 
    inline double ChiSq() const { return fChiSq; } 
    inline double ChiSqSts() const { return fChiSqSts; } 
    inline double ChiSqKlmX() const { return fChiSqKlmX; } 
    inline double ChiSqKlmY() const { return fChiSqKlmY; } 
    
    

    inline double YOffset() const { return fTrYOffset; } 
    inline double YSlope() const { return fTrYSlope; } 
    inline double YOffsetErr() const { return fTrYOffsetErr; } 
    inline double YSlopeErr() const { return fTrYSlopeErr; } 
    inline size_t NumParams() const { return  ( (fCovXY.size() == 25) ? 5 : 4); } 
    inline double Momentum() const { return fTrMom; } 
    inline double MomentumErr() const { return fTrMomErr; } 
    inline int UserFlag() const { return fUserFlag; }
    
    inline std::vector<double> CovMatrix() const {return fCovXY;}
    inline double CovMatrix(size_t i, size_t j) const {return fCovXY[5*i +j];} // No checks.. 
    
    friend std::ostream& operator << (std::ostream& o, const DwnstrTrackAlgo1& h);
  };
  
}

#endif // DWNSTRTRACKALGO1_H
