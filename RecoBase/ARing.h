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
    std::vector<double> LogLikelihood;  //1 for each Hyp (pion,kaon,prot)
    
  public:
    // Getters
    float  XCenter() const { return _center[0]; }
    float  YCenter() const { return _center[1]; }
    float  ZCenter() const { return _center[2]; }
    const float* Center() const { return _center; }
    float  Radius() const { return _radius; }
    int    NHits() const { return _nhits; }
    double LL_pion() const{return LogLikelihood[0];}
    double LL_kaon() const{return LogLikelihood[1];}
    double LL_prot() const{return LogLikelihood[2];}

    // Setters
    void SetCenter(float x[3]) { for (int i=0; i<3; ++i) _center[i] = x[i]; }
    void SetRadius(float r) { _radius = r; }
    void SetNHits(int nh) { _nhits = nh; }
    void SetLogLikelihoood(std::vector<double> LL){ for (int i=0; i<3; ++i) LogLikelihood[i] = LL[i];}    
    
    inline friend std::ostream& operator<<(std::ostream& os, const ARing& ring)
      {
	os << "RING: LL (" << ring.LogLikelihood[0] << ", " << ring.LogLikelihood[1]  << ", " <<  ring.LogLikelihood[2] << ")" << std::endl; 
	return os;
	}
  };
  
}

#endif // ARING_H
