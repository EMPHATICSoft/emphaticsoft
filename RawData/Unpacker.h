////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class with static functions to convert fragments to
///          slightly more useful raw data products
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef RAWDATA_UNPACK
#define RAWDATA_UNPACK

#include <vector>
#include "RawData/WaveForm.h"
#include "RawData/CAENV1720Fragment.h"
#include "RawData/TRB3Fragment.h"
#include "RawData/TRB3RawDigit.h"

namespace emph {

  namespace rawdata {

    class Unpack {
    public:

      static std::vector<emph::rawdata::WaveForm>
	GetWaveFormsFrom1720Fragment(emphaticdaq::CAENV1720Fragment& f,
				     size_t dataBeginBytes);

      static std::vector<emph::rawdata::TRB3RawDigit>
        GetTRB3RawDigitsFromFragment(emphaticdaq::TRB3Fragment& frag);
      //      static std::vector<emph::rawdata::
    };
  } // end namespace rawdata
} // end namespace emph

#endif // RAWDATA_UNPACK
