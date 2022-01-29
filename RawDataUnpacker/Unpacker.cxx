////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class with static functions to convert fragments to
///          slightly more useful raw data products
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "RawDataUnpacker/Unpacker.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include <bitset>
#include <time.h>

namespace emph {
  namespace rawdata {

    std::vector<emph::rawdata::WaveForm> Unpack::GetWaveFormsFrom1720Fragment(emphaticdaq::CAENV1720Fragment& frag)
    {
      bool isVerbose = false;
      char* verboseStr = getenv("EMPH_UNPACK_VERBOSE");
      if (verboseStr) {
	std::string verboseEnv(verboseStr);
	if (verboseEnv == "1" || verboseEnv == "Y" || verboseEnv == "y")
	  isVerbose = true;
      }
      
      std::vector<emph::rawdata::WaveForm> wv(0);

      uint64_t fragTS = frag.Timestamp();
      emphaticdaq::CAENV1720Fragment bb(frag);
      auto const* md = bb.Metadata();
      emphaticdaq::CAENV1720Event const* event_ptr = bb.Event();

      emphaticdaq::CAENV1720EventHeader header = event_ptr->Header;

      if (isVerbose) {
	std::cout
	  << "\n\tFrom header, eventSize           "  << header.eventSize
	  << "\n\tFrom header, marker              "  << header.marker
	  << "\n\tFrom header, channelMask         "  << header.channelMask
	  << "\n\tFrom header, pattern             "  << header.pattern
	  << "\n\tFrom header, eventFormat         "  << header.eventFormat
	  << "\n\tFrom header, reserved            "  << header.reserved
	  << "\n\tFrom header, boardFail           "  << header.boardFail
	  << "\n\tFrom header, boardID             "  << header.boardID
	  << "\n\tFrom header, event counter       "  << header.eventCounter
	  << "\n\tFrom header, triggerTimeTag      "  << header.triggerTimeTag
	  << "\n\tFrom header, triggerTimeRollover "  << header.triggerTimeRollOver()
	  << "\n\tFrom header, extendedTriggerTime "  << header.extendedTriggerTime()
	  << "\n\tFrom metadata, boardID           "  << md->boardID
	  << "\n\tFrom metadata, nChannels         "  << md->nChannels
	  << "\n\tFrom metadata, nSamples          "  << md->nSamples
	  << "\n\tFrom metadata, timeStampSec      "  << md->timeStampSec
	  << "\n\tFrom metadata, timeStampNSec     "  << md->timeStampNSec
	  << "\n\tFrom metadata, chTemps           "
	  << md->chTemps[0]<<", "
	  << md->chTemps[1]<<", "
	  << md->chTemps[2]<<", "
	  << md->chTemps[3]<<", "
	  << md->chTemps[4]<<", "
	  << md->chTemps[5]<<", "
	  << md->chTemps[6]<<", "
	  << md->chTemps[7];
      }

      uint32_t t0(header.triggerTimeTag);
      int nChannels = md->nChannels;
      if (isVerbose)
	std::cout << "\tNumber of channels: " << nChannels << "\n";
      
      const int board = header.boardID;
      int boardNum = board;

      if (isVerbose) {
	//--get the number of 32-bit words (quad_bytes) from the header
	uint32_t ev_size_quad_bytes = header.eventSize;
	uint32_t evt_header_size_quad_bytes = sizeof(emphaticdaq::CAENV1720EventHeader)/sizeof(uint32_t);
	uint32_t data_size_double_bytes = 2*(ev_size_quad_bytes - evt_header_size_quad_bytes);
	std::cout << "Data size = " << data_size_double_bytes << "\n";
      }
      
      std::vector<uint16_t> adc;
      adc.resize(md->nSamples);
      
      const uint16_t* data_begin = frag.Data();
      
      const uint16_t* value_ptr =  data_begin;
      uint16_t value = 0;
      size_t ch_offset = 0;
      //--loop over channels
      if (isVerbose)
	std::cout<<"Looping over "<<nChannels<<" channels\n";
      for (int i_ch=0; i_ch<nChannels; ++i_ch){
	if (i_ch >= CAEN_V1720_MAX_CHANNELS) {
	  mf::LogWarning("Unpack::GetWaveFormsFrom1720Fragment")
	    << "found channel "<<i_ch<<" larger than "
	    << CAEN_V1720_MAX_CHANNELS << "! How could it happen? Debug, debug!";
	  break;
	}
	ch_offset = (size_t)(i_ch * md->nSamples);

	//--loop over waveform samples
	if (isVerbose) std::cout << "Channel " << i_ch << std::endl;
	for(size_t i_t=0; i_t<(size_t)md->nSamples; ++i_t){
	  value_ptr = data_begin + ch_offset + i_t; //pointer arithmetic
	  value = *(value_ptr);
	  if (isVerbose) std::cout << value << " ";
	  adc[i_t] = value;
	}  //--end loop samples
	if (isVerbose) std::cout << std::endl;
	emph::rawdata::WaveForm wvfrm(i_ch,boardNum,adc,t0,fragTS);
	wv.push_back(wvfrm);
      } //--end loop channels
      
      return wv;
    }

    //---------------------------------------------------------------------
    // Unpack TRB3 data
    std::vector<emph::rawdata::TRB3RawDigit> Unpack::GetTRB3RawDigitsFromFragment(emphaticdaq::TRB3Fragment& frag)
    {
      bool isVerbose = false;
      char* verboseStr = getenv("EMPH_UNPACK_VERBOSE");
      if (verboseStr) {
	std::string verboseEnv(verboseStr);
	if (verboseEnv == "1" || verboseEnv == "Y" || verboseEnv == "y")
	  isVerbose = true;
      }
      
      uint64_t fragTS = frag.timestamp();

      std::vector<rawdata::TRB3RawDigit> trb3vec;
      emphaticdaq::TRB3Fragment::TRB3EventHeader const* header = frag.dataBegin();
      // skip past header
      const uint32_t* data_word = reinterpret_cast<uint32_t const*>(header+1);

      // loop over sub sub events (0500, 0501, 0502, 0503)
      for (unsigned int isse=0; isse<4; isse++) {
        emphaticdaq::TRB3Fragment::TRB3SubEventHeader const* sseheader = reinterpret_cast<emphaticdaq::TRB3Fragment::TRB3SubEventHeader const*>(data_word);
        data_word++;
        uint32_t tdc_header = *(data_word);
        uint32_t epoch_word = 0;

        // loop over words in sub sub event
        for (unsigned int iword=0; iword<sseheader->subevent_size; iword++){
          uint32_t word = *(data_word+iword);
          if ((word & 0xe0000000) == 0x60000000)
            epoch_word = word;
          if((word & 0xe0000000) == 0x80000000){
    	       uint32_t tdc_word = word;
             rawdata::TRB3RawDigit trb3dig;
             trb3dig.fgpa_header_word = sseheader->subevent_id;
             trb3dig.tdc_header_word  = tdc_header;
             trb3dig.tdc_epoch_word   = epoch_word;
             trb3dig.tdc_measurement_word = tdc_word;
	     trb3dig.fragmentTimestamp = fragTS;
	     if (isVerbose)
	       std::cout << "Making raw digit: " << sseheader->subevent_id << ", " << tdc_header << ", " << epoch_word << ", " << tdc_word << std::endl;
             trb3vec.push_back(trb3dig);
	  }
	} // end sub sub event
	data_word+=sseheader->subevent_size;
      } // end loop over sub sub events
      
      return trb3vec;
    }
    
    //------------------------------------------------------------------------------
    
    std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit>> Unpack::readSSDHitsFromFileStream(std::ifstream& file_handle) {
      // TODO sanity checks for input file
      
      const uint64_t kOnes = -1;
      const uint32_t kMaxHits = 500;
      const size_t kDataSize = sizeof(unsigned long long);
      uint64_t rawdata_buffer;
      uint64_t bco;
      
      std::vector<emph::rawdata::SSDRawDigit> ssd_hits;
      ssd_hits.reserve(kMaxHits);
      bool start_flag = false;
      
      while (!file_handle.eof()) {
        file_handle.read((char*)(&rawdata_buffer), kDataSize);
        if (rawdata_buffer == kOnes) {
	  if (start_flag) {
	    // event reading was previously started, now got end marker
	    // return as soon as this event is fully read
	    start_flag = false;
	    break;
	  }
	  else  {
	    // maybe got an extra event separator?
	    // TODO better logging
	    std::cout << "Warning: Got extra event marker. Check file?\n";
	    continue;
	  }
        }
        else {
	  // event data
	  if (!start_flag) {
	    start_flag = true;
	    // read the bco clock time at the start of each event
	    // we call an extra read here since the first data block of the event is unused
	    file_handle.read((char*)(&bco), kDataSize);
	    continue;
	  }
	  uint64_t rawdata_tmp = 0;
	  uint64_t rawdata_ordered = 0;
	  for (size_t byte = 0; byte < kDataSize; byte++) {
	    rawdata_tmp = ((rawdata_buffer >> ((kDataSize - byte - 1) * 8)) & 0xff) << (byte * 8);
	    rawdata_ordered += rawdata_tmp;
	  }
	  rawdata_buffer = rawdata_ordered;
	  uint32_t trig_type = (rawdata_buffer >> 32) & 0xfffff;
	  uint32_t module = (rawdata_buffer >> 27) & 0x07;
	  uint32_t chip = (rawdata_buffer >> 24) & 0x07;
	  uint32_t set = (rawdata_buffer >> 12) & 0x1f;
	  uint32_t strip = (rawdata_buffer >> 17) & 0x0f;
	  uint32_t t = (rawdata_buffer >> 4) & 0xff;
	  uint32_t adc = (rawdata_buffer >> 1) & 0x7;
	  uint32_t station = (rawdata_buffer >> 56) & 0xf;
	  
	  rawdata::SSDRawDigit hit(station, module, chip, set, strip, t, adc, trig_type);
	  ssd_hits.push_back(hit);
        }
      }
      return std::make_pair(bco, ssd_hits);
    }
    
  } // end namespace rawdata
} // end namespace emph
