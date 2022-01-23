#include "RawData/Unpacker.h"
#include <bitset>
#include <time.h>

namespace emph {
  namespace rawdata {
    
    std::vector<emph::rawdata::WaveForm> Unpack::GetWaveFormsFrom1720Fragment(emphaticdaq::CAENV1720Fragment& frag, size_t dataBeginBytes)
    {
      std::vector<emph::rawdata::WaveForm> wv(0);
      
      emphaticdaq::CAENV1720Fragment bb(frag);
      auto const* md = bb.Metadata();
      emphaticdaq::CAENV1720Event const* event_ptr = bb.Event();
      
      emphaticdaq::CAENV1720EventHeader header = event_ptr->Header;
      
      //      uint16_t fFragmentID = frag.fragmentID();
      
      //convert fragment access time to human-readable format
      //      struct timespec ts = frag.atime();
      //      char buff[100];
      //      strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
      
      //      uint64_t fTimestamp = frag.timestamp();
      
      std::cout 
	//	<< "\n\tFrom fragment header, typeString "  << frag.typeString()
	//	<< "\n\tFrom fragment header, timestamp  "  << (frag.timestamp()/1'000'000'000) <<" s, "<< std::setfill('0')<<std::setw(9)<<(frag.timestamp()%1'000'000'000)<<std::setw(0)<<" ns"
	//	<< "\n\tFrom fragment header, sequenceID "  << frag.sequenceID()
	//	<< "\n\tFrom fragment header, fragmentID "  << fFragmentID
	//	//	<< "\n\tFrom fragment header, size       "  << frag.size()
	//	<< "\n\tFrom fragment header, atime      "  << buff<<" "<<ts.tv_sec<<" ns" 
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
      //      fFragmentID -= fShift;
      //      TLOG(TLVL_INFO)
      //	<< "\n\tShifted fragment id is "  << fFragmentID;
      
      uint32_t t0(header.triggerTimeTag);
      int nChannels = md->nChannels;
      std::cout << "\tNumber of channels: " << nChannels << "\n";
      
      const int board = header.boardID;
      int boardNum = board;
      
      //--get the number of 32-bit words (quad_bytes) from the header
      uint32_t ev_size_quad_bytes = header.eventSize;
      uint32_t evt_header_size_quad_bytes = sizeof(emphaticdaq::CAENV1720EventHeader)/sizeof(uint32_t);
      uint32_t data_size_double_bytes = 2*(ev_size_quad_bytes - evt_header_size_quad_bytes);
      std::cout << "Data size = " << data_size_double_bytes << "\n";
      uint32_t wfm_length = data_size_double_bytes/nChannels;
      std::cout << "Channel waveform length = " << wfm_length << "\n";
      
      //--store the tick value for each acquisition 
      //      fTicksVec.resize(wfm_length);
      std::vector<uint16_t> adc;
      adc.resize(wfm_length);
      
      const uint16_t* data_begin = reinterpret_cast<const uint16_t*>(dataBeginBytes + sizeof(emphaticdaq::CAENV1720EventHeader));
      
      const uint16_t* value_ptr =  data_begin;
      uint16_t value = 0;
      size_t ch_offset = 0; 
      //--loop over channels
      std::cout<<"Looping over "<<nChannels<<" channels\n";
      for (int i_ch=0; i_ch<nChannels; ++i_ch){
	if (i_ch >= CAEN_V1720_MAX_CHANNELS) { 
	  TLOG(TLVL_ERROR)<<"found channel "<<i_ch<<" larger than " << CAEN_V1720_MAX_CHANNELS << "! How could it happen? Debug, debug!";
	  break;
	}
	//	fWvfmsVec[i_ch+nChannels*fFragmentID].resize(wfm_length);
	ch_offset = (size_t)(i_ch * wfm_length);

	//--loop over waveform samples
	for(size_t i_t=0; i_t<(size_t)wfm_length; ++i_t){ 
	  //	  fTicksVec[i_t] = t0*Ttt_DownSamp + i_t;   //timestamps, event level
	  value_ptr = data_begin + ch_offset + i_t; //pointer arithmetic
	  value = *(value_ptr);
	  adc[i_t] = value;
	  
	}  //--end loop samples
	emph::rawdata::WaveForm wvfrm(i_ch,boardNum,adc,t0);
	wv.push_back(wvfrm);
      } //--end loop channels 

      return wv;
    }
  } // end namespace rawdata
} // end namespace emph
