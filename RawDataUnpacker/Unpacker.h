////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class with static functions to convert artdaq 
///          fragments to slightly more useful raw data products
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef RAWDATA_UNPACK
#define RAWDATA_UNPACK

#include <vector>
#include <iostream>
#include <utility>
#include <fstream>
#include "emphatic-artdaq/Overlays/CAENV1720Fragment.hh"
#include "emphatic-artdaq/Overlays/TRB3Fragment.hh"
#include "RawData/TRB3RawDigit.h"
#include "RawData/WaveForm.h"

namespace emph {

  namespace rawdata {

    class Unpack {
    public:

      static std::vector<emph::rawdata::WaveForm>
	GetWaveFormsFrom1720Fragment(emphaticdaq::CAENV1720Fragment& f, int id);

      static std::vector<emph::rawdata::TRB3RawDigit>
        GetTRB3RawDigitsFromFragment(emphaticdaq::TRB3Fragment& frag);
      
    };
    
  } // end namespace rawdata
} // end namespace emph

#endif // RAWDATA_UNPACK
