#ifndef DAQTOOLS_DAQUNPACK_H
#define DAQTOOLS_DAQUNPACK_H

namespace rawdata {

  class DAQUnpack {
  public:
    DAQUnpack();
    ~DAQUnpack();
  
  public:
    bool GetRawEvent(const std::vector<rawdata::FlatDAQData>& flatdaqdata,
                     daqdataformats::RawEvent& raw);
                     
    bool GetRawEventHeader(const std::vector<rawdata::FlatDAQData>& flatdaqdata,
                           daqdataformats::RawEventHeader& header);
  
  private:
    bool FlatDAQCheck(const std::vector<rawdata::FlatDAQData>& flatdaqdata);
  };
}

#endif
////////////////////////////////////////////////////////////////////////
