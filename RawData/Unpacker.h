#ifndef RAWDATA_UNPACK
#define RAWDATA_UNPACK

#include <vector>
#include "RawData/WaveForm.h"
#include "RawData/CAENV1720Fragment.h"

namespace emph {

  namespace rawdata {

    class Unpack {
    public:

      static std::vector<emph::rawdata::WaveForm>
	GetWaveFormsFrom1720Fragment(emphaticdaq::CAENV1720Fragment& f,
				     size_t dataBeginBytes);

      //      static std::vector<emph::rawdata::
    };
  } // end namespace rawdata
} // end namespace emph

#endif // RAWDATA_UNPACK
