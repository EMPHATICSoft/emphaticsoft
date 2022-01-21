#ifndef TRB3RAWDIGIT_H
#define TRB3RAWDIGIT_H

#include <vector>
#include <iostream>
#include <inttypes.h>

namespace rawdata {
  class TRB3RawDigit {
  public:
    TRB3RawDigit(); // Default constructor
    virtual ~TRB3RawDigit() {}; // Destructor

  private:
    /// Found fields to hold the header, measurement error words.
    uint32_t fgpa_header_word;
    uint32_t tdc_header_word;
    uint32_t tdc_epoch_word;
    uint32_t tdc_measurement_word;
    int event_index;

  public:
    TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement);

    /// Is this the leading edge measurement?
    bool IsLeading() const {return 0;}
    /// Is this the trailing edge measurement?
    bool IsTrailing() const {return 0;}

    uint32_t GetBoardId() const {return fgpa_header_word & 0xf;}

    /// Get the TDC measurement
    uint32_t GetMeasurement() const {
      return (tdc_measurement_word & 0x7ffff);
    }

    uint32_t GetFineTime() const {
      return (tdc_measurement_word & 0x1ff000) >> 12;
    };

    uint32_t GetCoarseTime() const {return tdc_measurement_word & 0x7ff;};

    // This epoch counter rolls every 10us
    uint32_t GetEpochCounter() const {return tdc_epoch_word & 0xfffffff;};

    /// Get the channel number
    uint32_t GetChannel() const {
      return ((tdc_measurement_word & 0xfc00000 ) >> 22 );
    }

  };
}

#endif
