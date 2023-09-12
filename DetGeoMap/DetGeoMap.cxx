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

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoNavigator.h"

namespace emph {
  namespace dgmap {
  
    //----------------------------------------------------------------------
    
    DetGeoMap::DetGeoMap()
    {
      //      art::ServiceHandle<emph::geo::GeometryService> geo;
    }
  
    //----------------------------------------------------------------------
    
    bool DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
    {
      
      int station = cl.Station();
      int sensor = cl.Sensor();
      int plane  = cl.Plane();
      //      rb::planeView view = cl.View();
      double strip = cl.WgtAvgStrip();
      double pitch = 0.06;

      try {
	art::ServiceHandle<emph::geo::GeometryService> geo;
	auto geom = geo->Geo();
	
	const emph::geo::SSDStation &st = geom->GetSSDStation(station);
	const emph::geo::Plane& pln = st.GetPlane(plane);
	const emph::geo::Detector &sd = pln.SSD(sensor);
	
	double x0[3];
	double x1[3];
	x0[2] = x1[2] = sd.Pos()[2] + st.Pos()[2];
	double strippos = strip*pitch - sd.Height()/2;
	double cosrot = cos(sd.Rot());
	double sinrot = sin(sd.Rot());
	double tx0[2], tx1[2];

	tx0[0] = -sd.Width()/2;
	tx1[0] = sd.Width()/2;
	tx0[1] = strippos;
	tx1[1] = strippos;

	x0[0] = -tx0[0]*cosrot + tx0[1]*sinrot + sd.Pos()[0];
	x0[1] = tx0[0]*sinrot + tx0[1]*cosrot + sd.Pos()[1];

	x1[0] = -tx1[0]*cosrot + tx1[1]*sinrot + sd.Pos()[0];
	x1[1] = tx1[0]*sinrot + tx1[1]*cosrot + sd.Pos()[1];

	/*
	tx0[0] = -sd.Width()/2 + sd.Pos()[0];
	tx1[0] = sd.Width()/2 + sd.Pos()[0];
	tx0[1] = strippos + sd.Pos()[1];
	tx1[1] = strippos + sd.Pos()[1];

	x0[0] = tx0[0]*cosrot - tx0[1]*sinrot; 
	x0[1] = tx0[0]*sinrot + tx0[1]*cosrot; 

	x1[0] = tx1[0]*cosrot - tx1[1]*sinrot; 
	x1[1] = tx1[0]*sinrot + tx1[1]*cosrot; 
	*/
 
	ls.SetX0(x0);
	ls.SetX1(x1);	  
      }
      catch(...) {
	return false;
      }
      
      return true;

    }

    //----------------------------------------------------------------------
    void DetGeoMap::Reset()
    {

    }

    //----------------------------------------------------------------------

  } // end namespace dgmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
