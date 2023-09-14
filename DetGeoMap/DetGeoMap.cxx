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
#include <cmath>

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
      double dstrip = cl.WgtAvgStrip();
      int istrip = floor(dstrip);
      double delta_strip = dstrip-istrip;
      art::ServiceHandle<emph::geo::GeometryService> geo;
      auto geom = geo->Geo();
      
      const emph::geo::SSDStation* st = geom->GetSSDStation(station);
      const emph::geo::Plane* pln = st->GetPlane(plane);
      const emph::geo::Detector* sd = pln->SSD(sensor);
      const emph::geo::Strip* sp = sd->GetStrip(istrip);
      double pitch = 0.06;

      double x0[3];
      double x1[3];
      double tx0[3];
      double tx1[3];

      x0[1] = x1[1] = delta_strip*pitch;
      x0[2] = x1[2] = 0.;
      x0[0] = -sd->Width()/2;
      x1[0] = sd->Width()/2;

      sp->GeoMatrix()->LocalToMaster(x0,tx0);
      sd->GeoMatrix()->LocalToMaster(tx0,tx1);
      st->GeoMatrix()->LocalToMaster(tx1,x0);

      sp->GeoMatrix()->LocalToMaster(x1,tx0);
      sd->GeoMatrix()->LocalToMaster(tx0,tx1);
      st->GeoMatrix()->LocalToMaster(tx1,x1);
 
      ls.SetX0(x0);
      ls.SetX1(x1);	  
      
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
