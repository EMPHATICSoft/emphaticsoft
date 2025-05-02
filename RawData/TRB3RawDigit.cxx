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
      fpga_header_word(0),
      tdc_header_word(0),
      tdc_epoch_word(0),
      tdc_measurement_word(0),
      event_index(0), fragmentTimestamp(0), fdetChan(-1),  fIsNoise(false),
      fadc(0), fHitTime(0)
    {
    }
   
    //----------------------------------------------------------------------

    TRB3RawDigit::TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement, uint64_t fragTS, bool IsNoise):
      fpga_header_word(fpga),
      tdc_header_word(header),
      tdc_epoch_word(epoch),
      tdc_measurement_word(measurement),
      event_index(0), fragmentTimestamp(fragTS), fdetChan(-1), 
      fIsNoise(IsNoise), fadc(0)
    {
	fHitTime = this->GetFinalTime(); 	
    }
 
    TRB3RawDigit::TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement, uint64_t fragTS):
      fpga_header_word(fpga),
      tdc_header_word(header),
      tdc_epoch_word(epoch),
      tdc_measurement_word(measurement),
      event_index(0), fragmentTimestamp(fragTS), fdetChan(-1), 
      fIsNoise(false), fadc(0)
    {
	fIsNoise = false;
        fHitTime = this->GetFinalTime();
    }
  

    //----------------------------------------------------------------------

    uint32_t TRB3RawDigit::GetBoardId() const {
      uint32_t id = 99;
      if      ((fpga_header_word & 0xffff)==0x100) id=0;
      else if ((fpga_header_word & 0xffff)==0x101) id=1;
      else if ((fpga_header_word & 0xffff)==0x102) id=2;
      else if ((fpga_header_word & 0xffff)==0x103) id=3;
      else if ((fpga_header_word & 0xffff)==0x200) id=4;      // ARICH
      else if ((fpga_header_word & 0xffff)==0x201) id=5;      // ARICH
      else if ((fpga_header_word & 0xffff)==0x202) id=6;      // ARICH
      else if ((fpga_header_word & 0xffff)==0x203) id=7;      // ARICH
      else if ((fpga_header_word & 0xffff)==0x300) id=8;      // ARICH
      else if ((fpga_header_word & 0xffff)==0x301) id=9;      // ARICH
      else if ((fpga_header_word & 0xffff)==0x302) id=10;     // ARICH
      else if ((fpga_header_word & 0xffff)==0x303) id=11;     // ARICH
      else if ((fpga_header_word & 0xffff)==0x400) id=12;     // ARICH
      else if ((fpga_header_word & 0xffff)==0x401) id=13;     // ARICH
      else if ((fpga_header_word & 0xffff)==0x402) id=14;     // ARICH
      else if ((fpga_header_word & 0xffff)==0x403) id=15;     // ARICH
      else if ((fpga_header_word & 0xffff)==0x500) id=16;
      else if ((fpga_header_word & 0xffff)==0x501) id=17;
      else if ((fpga_header_word & 0xffff)==0x502) id=18;
      else if ((fpga_header_word & 0xffff)==0x503) id=19;
      else std::cout << "fpga " << std::hex << fpga_header_word << " (hex) "
                       << std::dec << fpga_header_word << " (dec) "
                       << "is not supported" << std::endl;
      return id;
    }

    //----------------------------------------------------------------------

    double TRB3RawDigit::GetFinalTime() const {

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


  } // end namespace rawdata
} // end namespace emph
