////////////////////////////////////////////////////////////////////////
// $Id: ScanOptions.cxx,v 1.3 2011-01-20 16:43:29 p-nusoftart Exp $
//
// Display parameters for the hand scan view
//
// \author brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
// Framework includes

#include "nuevdb/EventDisplayBase/ScanOptions.h"

#include <iostream>

namespace evdb {

  //......................................................................
  ScanOptions::ScanOptions(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg) 
  {
    fIncludeMCInfo     = pset.get< bool                      >("IncludeMCInfo");
    fScanFileBase      = pset.get< std::string               >("FileNameBase");
    fCategories        = pset.get< std::vector<std::string>  >("Categories");
    fFieldLabels       = pset.get< std::vector<std::string>  >("FieldLabels");
    fFieldTypes        = pset.get< std::vector<std::string>  >("FieldTypes");
    fFieldsPerCategory = pset.get< std::vector<unsigned int> >("FieldsPerCategory");
  }
  
  //......................................................................
  ScanOptions::~ScanOptions() 
  {
  }
  
}


////////////////////////////////////////////////////////////////////////
