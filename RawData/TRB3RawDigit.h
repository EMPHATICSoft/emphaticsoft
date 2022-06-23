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

    //private:
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

      uint32_t GetBoardId() const {
	uint32_t id = 0;
	if      ((fpga_header_word & 0xffff)==0x100) id=100;
	else if ((fpga_header_word & 0xffff)==0x101) id=101;
	else if ((fpga_header_word & 0xffff)==0x102) id=102;
	else if ((fpga_header_word & 0xffff)==0x103) id=103;
	else if ((fpga_header_word & 0xffff)==0x200) id=200;
	else if ((fpga_header_word & 0xffff)==0x201) id=201;
	else if ((fpga_header_word & 0xffff)==0x202) id=202;
	else if ((fpga_header_word & 0xffff)==0x203) id=203;
	else if ((fpga_header_word & 0xffff)==0x300) id=300;
	else if ((fpga_header_word & 0xffff)==0x301) id=301;
	else if ((fpga_header_word & 0xffff)==0x302) id=302;
	else if ((fpga_header_word & 0xffff)==0x303) id=303;
	else if ((fpga_header_word & 0xffff)==0x400) id=400;
	else if ((fpga_header_word & 0xffff)==0x401) id=401;
	else if ((fpga_header_word & 0xffff)==0x402) id=402;
	else if ((fpga_header_word & 0xffff)==0x403) id=403;
	else if ((fpga_header_word & 0xffff)==0x500) id=500;
	else if ((fpga_header_word & 0xffff)==0x501) id=501;
	else if ((fpga_header_word & 0xffff)==0x502) id=502;
	else if ((fpga_header_word & 0xffff)==0x503) id=503;
	return id;
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

      /// Get the channel number
      uint32_t GetChannel() const {
        return ((tdc_measurement_word & 0xfc00000 ) >> 22 );
      }

      // semi calibrated time in picoseconds
      // linear calibration
      // low  value of fine TDC hits = 17
      // high value of fine TDC hits = 473
      double GetFinalTime() const {
        return ((double)GetEpochCounter())*10240026.0
             + ((double) GetCoarseTime()) * 5000.0
             - ((((double)GetFineTime())-17.0)/(473.0 - 17.0)) * 5000.0;
      }

    };

  } // end namespace rawdata
} // end namespace emph
#endif
