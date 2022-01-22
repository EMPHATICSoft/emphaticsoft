#ifndef artdaq_ots_Overlays_TRB3Fragment_hh
#define artdaq_ots_Overlays_TRB3Fragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib_except/exception.h"

#include <ostream>
#include <vector>

// Implementation of "TRB3Fragment", an artdaq::Fragment overlay class

namespace ots
{
class TRB3Fragment;

// Let the "<<" operator dump the TRB3Fragment's data to stdout
std::ostream& operator<<(std::ostream&, TRB3Fragment const&);
}  // namespace ots

class ots::TRB3Fragment
{
  public:
  
  struct TRB3EventHeader {
    uint32_t unknown_word_1;
    uint32_t unknown_word_2;
    uint32_t event_size;
    uint32_t endian_marker_1 : 1;
    uint32_t unknown_word_3 : 30;
    uint32_t endian_marker_2 : 1;
    uint32_t central_fpga_id : 16;
    uint32_t unknown_word_4 : 16;
    uint32_t trigger_code : 8;
    uint32_t trigger_number : 24;
  };
   
  struct TRB3SubEventHeader {
    uint32_t subevent_id : 16;
    uint32_t subevent_size :16;
  };

  // Deduced from https://github.com/EMPHATICSoft/emphatic_om/blob/master/TRB3DecoderV2.cxx
  struct TRB3TDCData {
    uint32_t epoch_word; // 011b in bits 29-31
    uint32_t tdc_word; // 100b in bits 29-31
  };

  struct TRB3SubEventTrailer {
    uint32_t seb_id : 16; // 0x5555
    uint32_t length : 16; // 0x0001
    uint32_t error_status;
  };

	// The constructor simply sets its const private member "artdaq_Fragment_"
	// to refer to the artdaq::Fragment object

	TRB3Fragment(artdaq::Fragment const& f) : artdaq_Fragment_(f) {}


  TRB3EventHeader const* dataBegin() const { return reinterpret_cast<TRB3EventHeader const*>(artdaq_Fragment_.dataBeginBytes()); }

  size_t subEventCount() const;
  TRB3SubEventHeader const* subEventIndex(size_t idx) const;

  bool validateTRB3Data() const;

  private:
	artdaq::Fragment const& artdaq_Fragment_;
};

#endif /* artdaq_ots_Overlays_TRB3Fragment_hh */
