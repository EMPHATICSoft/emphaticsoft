/// \file    Geometry.cxx
/// \brief
/// \version $Id: Geometry.cxx
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "Geometry.h"

bool geo::Geometry::LoadFromGDML(std::string gdmlFile)
{
  _geom = TGeoManager::Import(gdmlFile.c_str());

  if (_geom)
    return true;

  return false;

}

////////////////////////////////////////////////////////////////////////
