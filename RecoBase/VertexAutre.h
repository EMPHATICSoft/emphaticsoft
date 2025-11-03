////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Vertex, hopefully coming from the target. 
///          it needs an upstream track, Station 0 and 1 for Phase1b, and a set of Dwonstream track. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef VERTEXAUTRE_H
#define VERTEXAUTRE_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>

namespace rbex {
    
  class Vertex {
  public:
    Vertex(); // Default constructor
    virtual ~Vertex() {}; //Destructor
    
  private:

     int fId; 
     double fX, fY, fZ; // The XYZ point.  
     double fXErr, fYErr, fZErr; // The uncertainties, Minuit Minos (or Simplex error )    
     double fCovXYZ[3][3]; // covariance matrix to compute the uncertainties 
     double fChiSq; // 
     std::vector<int> fTrackUIDs; // the UIDs of the Dwn tracks track that define this vertex. 
    
  public:
   // Setters 
     inline void Reset()
     {// Set everyting to NONE or DBL_MAX, to be refilled again.  
       fX =  DBL_MAX; fY = DBL_MAX; fZ = DBL_MAX; fId = INT_MAX;
       fChiSq = -1.; 
       for (int i=0; i<3; ++i)
	 for (int j=0; j<3; ++j) fCovXYZ[i][j]=DBL_MAX;

       fTrackUIDs.clear();
     }

   inline void SetPosition(double x0, double y0, double z0) {
     fX = x0; fY = y0;  fZ = z0;
   }
   inline void SetPositionErr(double xE, double yE, double zE ) { 
     fXErr = xE; fYErr = yE;  fZErr = zE;
   }
   inline void SetID(int id ) { fId = id;}
   inline void SetChiSq(double c ) { fChiSq = c;}  
   inline void SetCovarianceMatrix(int i, int j, double v) { fCovXYZ[i][j]=v;}
   inline void AddTrackUID(int uid) { fTrackUIDs.push_back(uid); }

    // Getters
    inline int ID() const { return fId; } 
    inline double X() const { return fX; } 
    inline double Y() const { return fY; } 
    inline double Z() const { return fZ; } 
    inline double XErr() const { return fXErr; } 
    inline double YErr() const { return fYErr; } 
    inline double ZErr() const { return fZErr; } 
    inline double ChiSq() const { return fChiSq; } 
    inline size_t NumDwnstr() const {return fTrackUIDs.size(); }
    inline double CovMatrix(int i, int j) const {return fCovXYZ[i][j]; }
    inline int TrackUID(int i) const { return fTrackUIDs[i]; }

    friend std::ostream& operator << (std::ostream& o, const Vertex& h);
  };
  
}

#endif // VERTEXAUTRE_H
