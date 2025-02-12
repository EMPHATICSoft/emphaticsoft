#ifndef TRB3RAWDIGIT_H
#define TRB3RAWDIGIT_H

#include <vector>
#include <iostream>
#include <inttypes.h>

//----------------------------------------------//
//   TDC = time-digital converter	 	//
//   FPGA = Field-programmable gate array       //
//   course time = leading edge time since T0   //
//   fine time = time between start and stop    //
//----------------------------------------------//

// For fine time measurements the Tapped Delay Line (TDL) method is used. 
// This method is based on a delay path with delay elements, which have similar propagation delays. 
// With the start signal the propagation along the delay line starts and with the stop signal the output of the each delay element is latched.
// The location of the propagating signal along the delay line defines the fine time between start and stop signals.

// https://indico.cern.ch/event/390748/contributions/2174357/attachments/1283600/1908034/RT2016_Minioral_trbtdc_2.pdf
// https://indico.gsi.de/event/5496/contributions/25455/attachments/18598/23317/FPGA_TDC.pdf


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
      uint32_t fdetChan;
      bool fIsNoise;    
      uint32_t fadc;
      uint32_t  fHitTime;
       

    public:

      TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement, uint64_t fragTS, bool IsNoise);
      TRB3RawDigit(uint32_t fpga, uint32_t header, uint32_t epoch, uint32_t measurement, uint64_t fragTS);
      TRB3RawDigit(uint32_t fpga, uint32_t detChan, uint32_t fHitTime, bool IsNoise);

      /// Is this the leading edge measurement?
      bool IsLeading() const {return ((tdc_measurement_word & 0x800) >> 11) == 1;}

      /// Is this the trailing edge measurement?
      bool IsTrailing() const {return ((tdc_measurement_word & 0x800) >> 11) == 0;}

      uint32_t GetFPGAHeaderWord() const {return fpga_header_word;}

      uint64_t GetFragmentTimestamp() const {return fragmentTimestamp;}
     
      uint32_t GetTime() const {return fHitTime;}
     
      /// Get board id
      uint32_t GetBoardId() const;
      /// Get the channel number
      uint32_t GetChannel() const {
        return ((tdc_measurement_word & 0xfc00000 ) >> 22 );
      }
      uint32_t GetDetChan() const {return fdetChan;};	


      void SetDetChannel(int ichan) {fdetChan = ichan; }


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
      double GetFinalTime() const;
 
      inline friend std::ostream& operator<<(std::ostream& os, const TRB3RawDigit& RawDig)
      {
	os << "Raw dig: (" << RawDig.GetBoardId() << ", " << RawDig.fdetChan  << ", " <<  RawDig.fHitTime << ", " <<  RawDig.fIsNoise << ")" << std::endl; 
	return os;
	}

 
    };

  } // end namespace rawdata
} // end namespace emph
#endif
