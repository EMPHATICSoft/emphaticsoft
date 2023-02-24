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

namespace emph {

  namespace dgmap {

    class DetGeoMap {
    public:
      DetGeoMap(); // Default constructor
      virtual ~DetGeoMap() {}; //Destructor

      //      bool LoadMap(std::string fname="");
      //      void SetAbortIfFileNotFound(bool f) { fAbortIfFileNotFound = f;}
      //      void SetMapFileName(std::string fname) { if (fname != fMapFileName) {
      //	  fMapFileName = fname; fIsLoaded=false;} }

      bool SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls);

    private:

      //      bool fIsLoaded;
      //      bool fAbortIfFileNotFound;
      //      std::string fMapFileName;
      //      std::map<emph::cmap::EChannel,emph::cmap::DChannel> fEChanMap;
      //      std::map<emph::cmap::DChannel,emph::cmap::EChannel> fDChanMap;

    };

  }
}

#endif // DETGEOMAP_H

