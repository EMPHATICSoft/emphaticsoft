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

#include "TGeoMatrix.h"

namespace emph {
  namespace dgmap {
  
    //----------------------------------------------------------------------
    
    DetGeoMap::DetGeoMap():
      fRun(0), fGeo(0), fAlign(0)
    {
      std::cout << "Created new DetGeoMap object!" << std::endl;
      fUseGeometry = true;      
    }
  
    //----------------------------------------------------------------------
    
    
    bool DetGeoMap::StationSensorPlaneToLineSegment (int station, int sensor, int plane, rb::LineSegment& ls, double dstrip) {
  
      int istrip = floor(dstrip);
      double delta_strip = dstrip-istrip;

      const emph::geo::SSDStation* st = fGeo->GetSSDStation(station);
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

      auto T = fAlign->SSDMatrix(station,plane,sensor);
      
      sp->LocalToMother(x0,tx0);
      sd->LocalToMother(tx0,tx1);
      st->LocalToMother(tx1,tx0);
      T->LocalToMaster(tx0,x0);

      sp->LocalToMother(x1,tx0);
      sd->LocalToMother(tx0,tx1);
      st->LocalToMother(tx1,tx0);
      T->LocalToMaster(tx0,x1);

      ls.SetX0(x0);
      ls.SetX1(x1);	  
      
      return true;
      
    }

    bool DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
    {
      int station = cl.Station();
      int sensor = cl.Sensor();
      int plane  = cl.Plane();
      double dstrip = cl.WgtAvgStrip();
      return StationSensorPlaneToLineSegment(station, sensor, plane, ls, dstrip);
    }  
    //----------------------------------------------------------------------

  } // end namespace dgmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
