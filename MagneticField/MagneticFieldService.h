///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the magnetic field
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef MAGNETICFIELDSERVICE_H
#define MAGNETICFIELDSERVICE_H

#include "MagneticField/MagneticField.h"

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"

namespace emph
{
  class MagneticFieldService 
  {
  public:
    // Get a MagneticFieldService instance here
    MagneticFieldService(const fhicl::ParameterSet& pset,
			 art::ActivityRegistry& reg);
    virtual ~MagneticFieldService();
    
    void reconfigure(const fhicl::ParameterSet& pset);
    
    void preBeginRun(const art::Run& run);
    
    emph::EMPHATICMagneticField* Field() const { return fMagneticField; }
    
  private:
    emph::EMPHATICMagneticField* fMagneticField;
    std::string fFieldFileName;
    
  };
  
}

DECLARE_ART_SERVICE(emph::MagneticFieldService, SHARED)

#endif
