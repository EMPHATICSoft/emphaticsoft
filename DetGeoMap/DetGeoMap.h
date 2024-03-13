////////////////////////////////////////////////////////////////////////
/// \brief   Simple class to map detector channels to physical (x,y,z)
///          positions.
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef DETGEOMAP_H
#define DETGEOMAP_H

#include <vector>
#include <map>
#include <stdint.h>
#include <iostream>

#include "ChannelMap/ChannelDefs.h"
#include "Geometry/DetectorDefs.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "Simulation/SSDHit.h"
#include "RawData/SSDRawDigit.h"
#include "Geometry/service/GeometryService.h"
#include "Align/service/AlignService.h"

namespace emph {

  namespace dgmap {

    class DetGeoMap {
    public:
      DetGeoMap(); // Default constructor
      virtual ~DetGeoMap() {}; //Destructor

      void SetRun(int run) { fRun = run;} 
      void SetUseGeometry(bool ug) { fUseGeometry = ug; }
      void SetAlign(emph::Align* align) { fAlign = align; }
      void SetGeometry(const emph::geo::Geometry* geo) { fGeo = geo; }
      //      bool LoadMap(std::string fname="");
      //      void SetAbortIfFileNotFound(bool f) { fAbortIfFileNotFound = f;}
      //      void SetMapFileName(std::string fname) { if (fname != fMapFileName) {
      //	  fMapFileName = fname; fIsLoaded=false;} }

      bool SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls);
      
    private:
      bool fUseGeometry;
      int fRun;
      const emph::geo::Geometry* fGeo;
      emph::Align* fAlign;

      //      bool fIsLoaded;
      //      bool fAbortIfFileNotFound;
      //      std::string fMapFileName;
      //      std::map<emph::cmap::EChannel,emph::cmap::DChannel> fEChanMap;
      //      std::map<emph::cmap::DChannel,emph::cmap::EChannel> fDChanMap;

    };

  }
}

#endif // DETGEOMAP_H

