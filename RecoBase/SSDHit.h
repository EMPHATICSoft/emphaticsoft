////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD hit
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDHIT_H
#define SSDHIT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "RawData/RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "Geometry/Geometry.h"

namespace rb {
  
  class SSDHit {
  public:
    SSDHit(); // Default constructor
    SSDHit(const emph::rawdata::SSDRawDigit &ssdraw, const emph::geo::Detector &st); // Default constructor
    virtual ~SSDHit() {}; //Destructor
    
  private:

    double _angle; // rotation angle about the vertical y-axis
    double _strip;  // avg. strip position
    double _pitch;

	 double _x, _y, _z;

  public:
    // Getters
    double Angle() const { return _angle; }
    double Strip() const { return _strip; }
    double Pitch() const { return _pitch; }

    // Setters
    void SetAngle(double ang) { _angle = ang; }
    void SetStrip(double strip) { _strip = strip; }
    void SetPitch(double pitch) { _pitch = pitch; }

    void CalibrateXYZ(double *cal); 
    
    friend std::ostream& operator << (std::ostream& o, const SSDHit& h);
  };
  
}

#endif // SSDHIT_H
