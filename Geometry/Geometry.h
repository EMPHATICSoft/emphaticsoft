/// \file    Geometry.h
/// \brief
/// \version $Id: Geometry.h
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#include <string>

#include "TGeoManager.h"

namespace geo {
  class Geometry {
    
  public:
    Geometry() {};
    Geometry(std::string fname) { LoadFromGDML(fname); }
    ~Geometry() {};
    
    bool LoadFromGDML(std::string gdmlFile);
    
    TGeoManager* RootGeoManager() const { return _geom; }
    
  private:
    
    TGeoManager* _geom;
    
  };

} // end namespace geo
