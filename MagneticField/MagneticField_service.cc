///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Magnetic Field
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "MagneticField/MagneticFieldService.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace emph
{
  //------------------------------------------------------------
  MagneticFieldService::MagneticFieldService(const fhicl::ParameterSet& pset,
				   art::ActivityRegistry & reg)
  {

    reconfigure(pset);
    
    fMagneticField = new emph::EMPHATICMagneticField(fFieldFileName);
    
    reg.sPreBeginRun.watch(this, &MagneticFieldService::preBeginRun);
    
  }
  
  //----------------------------------------------------------
  
  MagneticFieldService::~MagneticFieldService()
  {
  }
  
  //-----------------------------------------------------------
  void MagneticFieldService::reconfigure(const fhicl::ParameterSet& pset)
  {
    
    fFieldFileName = pset.get< std::string >("FieldFileName");
  }
  
  //----------------------------------------------------------
  // If we have run-dependent field, do something here to reload
  // the field if necessary
  //----------------------------------------------------------
  void MagneticFieldService::preBeginRun(const art::Run& )
  {
    
  }
  
}

DEFINE_ART_SERVICE(emph::MagneticFieldService)
