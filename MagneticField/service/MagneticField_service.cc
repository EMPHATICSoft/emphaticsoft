///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Magnetic Field
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>

// EMPHATIC includes
#include "MagneticField/service/MagneticFieldService.h"
//#include "MagneticField/TestEmphMagneticField.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"

namespace emph
{
  //------------------------------------------------------------
  MagneticFieldService::MagneticFieldService(const fhicl::ParameterSet& pset,
				   art::ActivityRegistry & reg)
  {

    fMagneticField = new emph::MagneticField();

    reconfigure(pset);

    reg.sPreBeginRun.watch(this, &MagneticFieldService::preBeginRun);
    
  }
  
  //----------------------------------------------------------
  
  MagneticFieldService::~MagneticFieldService()
  {
  }
  
  //-----------------------------------------------------------
  void MagneticFieldService::reconfigure(const fhicl::ParameterSet& pset)
  {    
    fMagneticField->SetFieldFileName(pset.get< std::string >("FieldFileName"));
    fMagneticField->SetUseStlVector(pset.get< bool >("StoreMapAsStlVector"));
    fMagneticField->SetVerbosity(pset.get<int>("Verbosity"));

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
