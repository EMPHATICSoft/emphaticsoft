////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class with static functions to convert SSD data 
///          slightly more useful raw data products
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef SSDRAWDATA_UNPACK
#define SSDRAWDATA_UNPACK

#include <vector>
#include <iostream>
#include <utility>
#include <fstream>
#include "RawData/SSDRawDigit.h"

namespace emph {

  namespace rawdata {

    class SSDUnpack {
    public:

      static std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit>> readSSDHitsFromFileStream(std::ifstream&, bool isFirst);
      
    };
    
  } // end namespace rawdata
} // end namespace emph

#endif // SSDRAWDATA_UNPACK
