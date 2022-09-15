////////////////////////////////////////////////////////////////////////
/// \brief   SSD hit class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/SSDHit.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  SSDHit::SSDHit() :   
    _angle(0.), _strip(0.), _pitch(60.)
  {

  }
  
  //------------------------------------------------------------

  SSDHit::SSDHit(const emph::rawdata::SSDRawDigit &ssd, const emph::geo::Detector &st)
  {
	  _angle = st.Rot();
	  _strip = ssd.Strip();
	  _pitch = 0.02;
  }
  
  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const SSDHit& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " SSD Strip = "     << std::setw(5) << std::right << h.Strip()
      << " Angle = "     << std::setw(5) << std::right << h.Angle()
      << " Pitch = "        << std::setw(5) << std::right << h.Pitch();     
    return o;
  }

  //------------------------------------------------------------

   void SSDHit::CalibrateXYZ(double *cal)
  {
	  _x=_x+cal[0];
  }
  
  //------------------------------------------------------------

 
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
