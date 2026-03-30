///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide a map between detector Geometry and 
///         detector channels.
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef DETGEOMAPSERVICE_H
#define DETGEOMAPSERVICE_H

#include "DetGeoMap/DetGeoMap.h"

//Framework includes
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Principal/Run.h"

namespace art {
  class ActivityRegistry;
  class Run;
}

namespace fhicl {
  class ParameterSet;
}

namespace emph
{
  namespace dgmap
  {
    
    class DetGeoMapService 
    {
    public:
      // Get a DetGeoMapService instance here
      DetGeoMapService(const fhicl::ParameterSet& pset,
			art::ActivityRegistry& reg);
      virtual ~DetGeoMapService();
      
      void reconfigure(const fhicl::ParameterSet& pset);
    
      void preBeginRun(const art::Run& run);

      DetGeoMap* Map() const {
//         std::cerr << " DetGeoMapService, pointer to detGeoMap " << fDetGeoMap << std::endl;
	 if ((fDetGeoMap == nullptr) || (fDetGeoMap == 0)) {
	    std::cerr << " ... Problem, no valid DetGeoMap pointer,  quit now " << std::endl; exit(2);
	 }
         return fDetGeoMap; 
      }
      DetGeoMap* MapRef() const {
	if ((fDetGeoMapRef == nullptr) || (fDetGeoMapRef == 0)) {
	    std::cerr << " ... Problem, no valid DetGeoMapRef pointer,  quit now " << std::endl; exit(2);
	}
       return fDetGeoMapRef; 
     }
      
    private:
      DetGeoMap* fDetGeoMap;
      DetGeoMap* fDetGeoMapRef;
      bool fUseGeometry;
      bool fUseGeometryRef;
      bool fUseAlign;
      
    };
    
  }
}

DECLARE_ART_SERVICE(emph::dgmap::DetGeoMapService, SHARED)

#endif
