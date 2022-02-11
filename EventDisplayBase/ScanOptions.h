////////////////////////////////////////////////////////////////////////
// $Id: ScanOptions.h,v 1.4 2011-02-21 22:34:19 brebel Exp $
//
// Display parameters for scanning
//
// \author brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_SCANOPTIONS_H
#define EVDB_SCANOPTIONS_H

#include "TGScrollBar.h"
#include "TGCanvas.h"

#ifndef __CINT__ // root 5
#include <string>
#include <vector>

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace evdb {
  class ScanOptions 
  {
  public:
    ScanOptions(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    ~ScanOptions();
    
    bool        fIncludeMCInfo;           ///> true if MC information is to be included in scan output
    std::string fScanFileBase;            ///> base file name for scanning

    // below are vectors to describe the different categories that are 
    // important to the scan.  fCategories are the broad categories you want
    // to describe, ie, tracks, neutrinos, etc.  fFieldsPerCategory tells
    // the ScanWindow how many fields are in the categories.  fFieldTypes
    // and fFieldLabels tell the ScanWindow the type of each field and
    // what to call each field

    std::vector<std::string>  fCategories;        ///> names of the various categories for the scan
    std::vector<unsigned int> fFieldsPerCategory; ///> number of fields in each category
    std::vector<std::string>  fFieldTypes;        ///> types of the fields, ie TextEntry, Buttons, NumberEntry, etc
    std::vector<std::string>  fFieldLabels;       ///> labels for each of the fields, whatever you want to call them

  };
}//namespace
#endif // __CINT__
DECLARE_ART_SERVICE(evdb::ScanOptions, LEGACY)
#endif

