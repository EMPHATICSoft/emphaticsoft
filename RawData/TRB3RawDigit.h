#ifndef TRB3RAWDIGIT_H
#define TRB3RAWDIGIT_H

#include <vector>
#include <iostream>
#include <inttypes.h>

namespace emph {

  namespace rawdata {
    class TRB3RawDigit {
    public:
      TRB3RawDigit(); // Default constructor
      virtual ~TRB3RawDigit() {}; // Destructor

    private:
      /// Found fields to hold the header, measurement error words.
      uint32_t fpga_header_word;
      uint32_t tdc_header_word;
      uint32_t tdc_epoch_word;
      uint32_t tdc_measurement_word;
      int event_index;
      uint64_t fragmentTimestamp;
      
    public:
      TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement, uint64_t fragTS);

      /// Is this the leading edge measurement?
      bool IsLeading() const {return ((tdc_measurement_word & 0x800) >> 11) == 1;}
      /// Is this the trailing edge measurement?
      bool IsTrailing() const {return ((tdc_measurement_word & 0x800) >> 11) == 0;}

      uint32_t GetFPGAHeaderWord() const {return fpga_header_word;};

      uint64_t GetFragmentTimestamp() const {return fragmentTimestamp;};

      uint32_t GetBoardId() const {
	uint32_t id = 0;
	if      ((fpga_header_word & 0xffff)==0x100) id=0;
	else if ((fpga_header_word & 0xffff)==0x101) id=1;
	else if ((fpga_header_word & 0xffff)==0x102) id=2;
	else if ((fpga_header_word & 0xffff)==0x103) id=3;
	else if ((fpga_header_word & 0xffff)==0x200) id=4;
	else if ((fpga_header_word & 0xffff)==0x201) id=5;
	else if ((fpga_header_word & 0xffff)==0x202) id=6;
	else if ((fpga_header_word & 0xffff)==0x203) id=7;
	else if ((fpga_header_word & 0xffff)==0x300) id=8;
	else if ((fpga_header_word & 0xffff)==0x301) id=9;
	else if ((fpga_header_word & 0xffff)==0x302) id=10;
	else if ((fpga_header_word & 0xffff)==0x303) id=11;
	else if ((fpga_header_word & 0xffff)==0x400) id=12;
	else if ((fpga_header_word & 0xffff)==0x401) id=13;
	else if ((fpga_header_word & 0xffff)==0x402) id=14;
	else if ((fpga_header_word & 0xffff)==0x403) id=15;
	else if ((fpga_header_word & 0xffff)==0x500) id=16;
	else if ((fpga_header_word & 0xffff)==0x501) id=17;
	else if ((fpga_header_word & 0xffff)==0x502) id=18;
	else if ((fpga_header_word & 0xffff)==0x503) id=19;
	return id;
      }

      /// Get the channel number
      uint32_t GetChannel() const {
        return ((tdc_measurement_word & 0xfc00000 ) >> 22 );
      }

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

      // semi calibrated time in picoseconds
      double GetFinalTime() const {

        // default linear calibration constants
        double trb3LinearLowEnd = 17.0;
        double trb3LinearHighEnd = 473.0;

        // T0
        if (GetBoardId()==18) {
          trb3LinearLowEnd = 15.0;
          trb3LinearHighEnd = 494.0;
        }
        // RPC
        else if (GetBoardId()==19) {
          trb3LinearLowEnd = 15.0;
          trb3LinearHighEnd = 476.0;
        }

        return ((double)GetEpochCounter())*10240026.0
             + ((double) GetCoarseTime())*5000.0
             - ((((double)GetFineTime())-trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
      }

    };

  } // end namespace rawdata
} // end namespace emph
#endif
