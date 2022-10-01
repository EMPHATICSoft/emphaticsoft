#ifndef RPCSTRIPHIT_H
#define RPCSTRIPHIT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "Geometry/Geometry.h"

namespace rb {
  
  class RPCStripHit {
  public:
    RPCStripHit(); // Default constructor
    RPCStripHit(int StripNumber, double tLeft, double tRight, const emph::geo::Detector &st); // specific constructor.  Future version. 
    virtual ~RPCStripHit() {}; //Destructor
    
  private:

    int _stripNum; // rotation angle about the vertical y-axis
    double _pitch;

    double _x, _y, _z, _tLeft, _tRight, _totLeft, _totRight;
    double _tCal;

  public:
    // Getters
    inline double StripNumber() const { return _stripNum; }
    inline double Time() const { return 0.5 * (_tLeft + _tRight); } // To be reviewed when we have the SSD tracks.. 
    inline double TimeLeft() const { return _tLeft; }
    inline double TimeOverThresholdLeft() const { return _totLeft; }
    inline double TimeOverThresholdRight() const { return _totRight; }
    inline double TimeRight() const { return _tRight; }
    inline double TimeCal() const { return _tCal; }

    // Setters
    inline void SetStripNumber(int n) { _stripNum = n; }
    inline void SetPitch(double pitch) { _pitch = pitch; }
    inline void SetTimeLeftRight(double tL, double tR) { _tLeft = tL, _tRight = tR; }
    inline void SetTimeOverThresholdLeftRight(double tL, double tR) { _totLeft = tL, _totRight = tR; }

    void CalibrateTime(double *calLeft, double *calRight); 
    void CalibratePosition(double *xShit, double *yShift, double *zShift); // to be implemented, 
    
    friend std::ostream& operator << (std::ostream& o, const RPCStripHit& h);
  };
  
}

#endif // RPCSTRIPHIT_H
