///////////////////////////////////////////////////////////////////////////
/// \brief   Simple class to map detector channels to physical (x,y,z)
///          positions.
/// \author  jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "DetGeoMap/service/DetGeoMapService.h"

// Framework includes
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"

DEFINE_ART_SERVICE(emph::dgmap::DetGeoMapService)
