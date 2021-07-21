////////////////////////////////////////////////////////////////////////
/// \brief  FlatDAQData class contains data for each event
///         as it is presented in a raw DAQ data file.
///         Use DAQDataFormats::RawEvent to interpret it.
/// \author denis@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef FLATDAQDATA_H
#define FLATDAQDATA_H

#include <vector>

namespace rawdata {
  
  class FlatDAQData {
  public:
    FlatDAQData()  {} // Default constructor
    ~FlatDAQData() {}

    void*              getRawBufferPointer() const {return (void*) &(fRawBuffer[0]);}
  
    std::vector<char>  fRawBuffer;  ///< Flat event data

  };// end of Class FlatDAQData
}//end of namespace daq2raw

#endif // FLATDAQDATA_H

