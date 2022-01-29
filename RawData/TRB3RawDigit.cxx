////////////////////////////////////////////////////////////////////////
/// \brief   TRB3RawDigit class
/// \author  lackey32@fnal.gov, copying code from Thomas Lindner
////////////////////////////////////////////////////////////////////////
#include "RawData/TRB3RawDigit.h"

#include <iostream>
namespace emph {

  namespace rawdata{

    //----------------------------------------------------------------------

    TRB3RawDigit::TRB3RawDigit():
      fgpa_header_word(0),
      tdc_header_word(0),
      tdc_epoch_word(0),
      tdc_measurement_word(0),
      event_index(0), fragmentTimestamp(0)
    {
    }

    //----------------------------------------------------------------------

    TRB3RawDigit::TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement, uint64_t fragTS):
      fgpa_header_word(fpga),
      tdc_header_word(header),
      tdc_epoch_word(epoch),
      tdc_measurement_word(measurement),
      event_index(0), fragmentTimestamp(fragTS)
    {
    }

  } // end namespace rawdata
} // end namespace emph
