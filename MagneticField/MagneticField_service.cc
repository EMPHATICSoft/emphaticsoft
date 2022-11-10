///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the Magnetic Field
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>

// EMPHATIC includes
#include "MagneticField/MagneticFieldService.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"

namespace emph
{
  //------------------------------------------------------------
  MagneticFieldService::MagneticFieldService(const fhicl::ParameterSet& pset,
				   art::ActivityRegistry & reg)
  {

    reconfigure(pset);
/*
    Jonathan decided to by-pass the fcl .. 
    Not sure this is the best option.. Paul Lebrun, Oct 20 2022. 
    cet::search_path sp("CETPKG_SOURCE");

    std::string fFileName;
    sp.find_file(fFieldFileName,fFileName);
    struct stat sb;
    if ( fFileName.empty() || stat(fFileName.c_str(), &sb)!=0 ) {
      // failed to resolve the file name
      throw cet::exception("NoMagFieldMap")
        << "Magnetic field map file " << fFileName << " not found!\n"
        << __FILE__ << ":" << __LINE__ << "\n";
    }
    
    fMagneticField = new emph::EMPHATICMagneticField(fFileName);
*/
    fMagneticField = new emph::EMPHATICMagneticField(fFieldFileName);
    // Temporary tweak and study: assume we have no ziptrack data for the outer core.. 
    // Does not seem to have a bad effect, except to slow down the tracking.. as expected.
//    fMagneticField->setUseOnlyTheCentralPart(true);
    
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
