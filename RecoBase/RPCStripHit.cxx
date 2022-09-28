////////////////////////////////////////////////////////////////////////
/// \brief   RPCStripHit
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <iomanip>

#include "RecoBase/RPCStripHit.h"

  namespace rb {
    RPCStripHit::RPCStripHit():
    _stripNum(INT_MAX), // the segment number, along X 
    _pitch(31.5), //  Guess, assumed 8 strips over a height of 252 mm 
    _x(DBL_MAX),
    _y(DBL_MAX),
    _z(DBL_MAX),
    _tLeft(DBL_MAX),
    _tRight(DBL_MAX),
    _tCal(DBL_MAX) {
        ;
     }
     RPCStripHit::RPCStripHit(int StripNumber, double tLeft, double tRight, const emph::geo::Detector &st):
    _stripNum(StripNumber), // rotation angle about the vertical y-axis
    _pitch(31.5), // T0 device used in Phase1b To be picked up from the Geometry 
    _x(st.Pos().X()),   
    _y(st.Pos().Y() + _pitch/2. + (4 + _stripNum -1)*_pitch), // To be checked.  
    _z(st.Pos().Z()),
    _tLeft(tLeft),
    _tRight(tRight),
    _tCal(DBL_MAX) {
        ;
     }
   
    std::ostream& operator<< (std::ostream& o, const  RPCStripHit& r)
       {
        auto prevFlags = o.flags(); auto prevPrecision = o.precision();
        o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        o << " Dump from emph::rb::RPCStripHit ";
        o <<  " Strip number  ";
	o << r.StripNumber() << " arrival time   " << r.Time() << std::endl;
	o << " Time Left " << r.TimeLeft() << " Right " << r.TimeRight();
	if (r.TimeCal() != DBL_MAX) o << " Calibrated time " << r.TimeCal();
	o << std::endl; 
	o << " ------------------------------------------------------------" << std::endl << std::endl;
        o.setf(prevFlags); o.precision(prevPrecision);
        return o;
       }
    
  }  
