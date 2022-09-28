////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD hit
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef T0SEGMENTHIT_H
#define T0SEGMENTHIT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "Geometry/Geometry.h"

namespace rb {
  
  class T0SegmentHit {
  public:
    T0SegmentHit(); // Default constructor
    T0SegmentHit(int SegmentNumber, double tUp, double tDown, bool isSingleParticle, const emph::geo::Detector &st); // Default constructor
    virtual ~T0SegmentHit() {}; //Destructor
    
  private:

    bool _isSingle;  // clean, no additional peak, Unipolar. 
    int _segNum; // rotation angle about the vertical y-axis
    double _pitch;

    double _x, _y, _z, _tUp, _tDown;
    double _tCal;

  public:
    // Getters
    inline bool SingleParticle() const { return _isSingle; }
    inline double SegNumber() const { return _segNum; }
    inline double Time() const { return 0.5 * (_tUp + _tDown); }
    inline double TimeUp() const { return _tUp; }
    inline double TimeDown() const { return _tDown; }
    inline double TimeCal() const { return _tCal; }

    // Setters
    inline void SetSingleParticle(bool t) { _isSingle = t;}
    inline void SetSegmentNumber(int n) { _segNum = n; _x = (5 + _segNum  - 1)*_pitch; } // un calibrated.
    inline void SetPitch(double pitch) { _pitch = pitch; }
    inline void SetTimeUpDown(double tDown, double tUp) { _tUp = tUp, _tDown = tDown; }


    void CalibrateTime(double *calUp, double *calDown); // to be implemented, 
    void CalibratePosition(double *xShit, double *yShift, double *zShift); // to be implemented, 
    
    friend std::ostream& operator << (std::ostream& o, const T0SegmentHit& h);
  };
  
}

#endif // T0SEGMENTHIT_H
