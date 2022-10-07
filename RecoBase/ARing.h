////////////////////////////////////////////////////////////////////////
/// \brief   Definition of ARICH ring
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef ARING_H
#define ARING_H

#include <vector>
#include <stdint.h>
#include <iostream>

//#include "RawData/RawDigit.h"

namespace rb {
  
  class ARing {
  public:
    ARing(); // Default constructor
    virtual ~ARing() {}; //Destructor
    
  private:

    float _center[3]; // rotation angle about the vertical y-axis
    float _radius;
    int _nhits;
    std::vector<unsigned short int> _pmtAnodesIndices;
    
  public:
    // Getters
    float  XCenter() const { return _center[0]; }
    float  YCenter() const { return _center[1]; }
    float  ZCenter() const { return _center[2]; }
    const float* Center() const { return _center; }
    float  Radius() const { return _radius; }
    int    NHits() const { return _nhits; }
    size_t NIndicesI() const {return _pmtAnodesIndices.size();}
    std::vector<unsigned short int>::const_iterator cbeginI() const { return _pmtAnodesIndices.cbegin(); } 
    std::vector<unsigned short int>::const_iterator cendI() const { return _pmtAnodesIndices.cend(); } 

    // Setters
    void SetCenter(float x[3]) { for (int i=0; i<3; ++i) _center[i] = x[i]; }
    void SetRadius(float r) { _radius = r; }
    void SetNHits(int nh) { _nhits = nh; }
    void SetPmtAnodesIndices(const std::vector<unsigned short int> &vh) { _pmtAnodesIndices = vh; } // deep copy of ~ 20 to 50 hits per events. 
    
    //    friend std::ostream& operator << (std::ostream& o, const ARing& h);
  };
  
}

#endif // ARING_H
