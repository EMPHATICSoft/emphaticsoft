////////////////////////////////////////////////////////////////////////
/// \brief   DetGeoMap class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "DetGeoMap/DetGeoMap.h"
#include "Geometry/service/GeometryService.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

namespace emph {
  namespace dgmap {
  
    //----------------------------------------------------------------------
    
    DetGeoMap::DetGeoMap()
      //      fIsLoaded(false), fMapFileName("")
    {
      //      fEChanMap.clear();
      //      fDChanMap.clear();
    }
  
    //----------------------------------------------------------------------
    
    bool DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
    {
      
      int station = cl.Station();
      int sensor = cl.Sensor();
      rb::planeView view = cl.View();
      double strip = cl.WgtAvgStrip();
      double pitch = 0.06;

      try {
	art::ServiceHandle<emph::geo::GeometryService> geo;
	auto geom = geo->Geo();
	
	const emph::geo::SSDStation &st = geom->GetSSDStation(station);
	const emph::geo::Detector &sd = st.GetSSD(sensor);
	
	double x0[3];
	double x1[3];
	x0[2] = x1[2] = sd.Pos()[2] + st.Pos()[2];
	double strippos = strip*pitch - sd.Height()/2;
	double cosrot = cos(sd.Rot());
	double sinrot = sin(sd.Rot());
	// note, an SSD that measures "y" (vertical position) has a rotation of 0 degrees.
	if (view == rb::X_VIEW) { // rotation angle is assumed to be ~90deg.
	  x0[0] = sd.Pos()[0] - strippos*sinrot;
	  x1[0] = sd.Pos()[0] + strippos*sinrot;
	  x0[1] = sd.Pos()[1] - (sd.Width()/2)*(1-cos(sd.Rot()));
	  x1[1] = sd.Pos()[1] + (sd.Width()/2)*(1-cos(sd.Rot()));
	  
	}
	else {
	  x0[1] = sd.Pos()[1] - strippos*cosrot;
	  x1[1] = sd.Pos()[1] + strippos*cosrot;
	  x0[0] = sd.Pos()[0] - (sd.Width()/2)*(1-sin(sd.Rot()));
	  x1[0] = sd.Pos()[0] + (sd.Width()/2)*(1-sin(sd.Rot()));
	}
	ls.SetX0(x0);
	ls.SetX1(x1);
      }
      catch(...) {
	return false;
      }

      return true;

    }

  } // end namespace dgmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
