////////////////////////////////////////////////////////////////////////
/// \brief   T0SegmentHit
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <iomanip>

#include "RecoBase/T0SegmentHit.h"

  namespace rb {
  
    T0SegmentHit::T0SegmentHit():
    _isSingle(true),  // Give the benefit of the doubt 
    _segNum(INT_MAX), // the segment number, along X 
    _pitch(3.), // T0 device used in Phase1b
    _x(DBL_MAX),
    _y(DBL_MAX),
    _z(DBL_MAX),
    _tUp(DBL_MAX),
    _tDown(DBL_MAX),
    _tCal(DBL_MAX) {
        ;
     }
     
     T0SegmentHit::T0SegmentHit(int SegmentNumber, double tUp, double tDown, bool isSingleParticle, const emph::geo::Detector &st):
    _isSingle(isSingleParticle),
    _segNum(SegmentNumber), // rotation angle about the vertical y-axis
    _pitch(3.), // T0 device used in Phase1b To be picked up from the Geometry 
    _x(st.Pos().X() + (5 + _segNum  - 1)*_pitch) ,
    _y(st.Pos().Y()),
    _z(st.Pos().Z()),
    _tUp(tUp),
    _tDown(tDown),
    _tCal(DBL_MAX) {
        ;
     }
   
    std::ostream& operator<< (std::ostream& o, const  T0SegmentHit& r)
       {
        auto prevFlags = o.flags(); auto prevPrecision = o.precision();
        o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        o << " Dump from emph::rb::T0SegmentHit ";
        if (r.SingleParticle()) o <<  " Single Particle, segment  ";
	else o <<  " Multiple peaks for , segment  ";
	o << r.SegNumber() << " arrival time   " << r.Time() << std::endl;
	o << " Time Up SiPM " << r.TimeUp() << " Down " << r.TimeDown();
	if (r.TimeCal() != DBL_MAX) o << " Calibrated time " << r.TimeCal();
	o << std::endl; 
	o << " ------------------------------------------------------------" << std::endl << std::endl;
        o.setf(prevFlags); o.precision(prevPrecision);
        return o;
       }
    
  }  
