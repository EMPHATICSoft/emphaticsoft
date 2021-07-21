///////////////////////////////////////////////////////////////////////////////
/// \file     DAQUnpack.cxx
/// \brief    Producer module to extract DAQ header variables relevant to
///           trigger monitoring
/// \author   Justin Vasel <jvasel@indiana.edu>
///////////////////////////////////////////////////////////////////////////////

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"

// NOvA Online
#include "DAQDataFormats/RawEvent.h"
#include "DAQDataFormats/RawTrigger.h"
#include "DAQDataFormats/RawTriggerMask.h"
#include "DAQDataFormats/RawDataBlock.h"
#include "DAQDataFormats/RawMicroBlock.h"
#include "DAQDataFormats/RawMicroSlice.h"
#include "DAQDataFormats/RawNanoSlice.h"
#include "DAQChannelMap/DAQChannelMap.h"

// NOvA Offline
#include "RawData/FlatDAQData.h"
#include "RawData/RawSumDropMB.h"
#include "RawData/DAQUnpack.h"


namespace rawdata {
  //...........................................................................
  DAQUnpack::DAQUnpack() { }


  //...........................................................................
  DAQUnpack::~DAQUnpack() { }


  //...........................................................................
  /// Sanity check for FlatDAQData. If there is not exactly one FlatDAQData object, bail.
  /// \param flatdaqdata : FlatDAQData
  bool DAQUnpack::FlatDAQCheck(const std::vector<rawdata::FlatDAQData>& flatdaqdata)
  {
    if (flatdaqdata.size() != 1) {
      mf::LogError("InvalidFlatDAQData")
      << " Must have one and only one flat daq object. Found "
      << flatdaqdata.size();
      
      return false;
    }
    
    return true;
  }


  //...........................................................................
  /// Unpack the raw event
  /// \param flatdaqdata : FlatDAQData
  /// \param raw : Raw event
  bool DAQUnpack::GetRawEvent(const std::vector<rawdata::FlatDAQData>& flatdaqdata,
                                    daqdataformats::RawEvent& raw)
  {    
    if ( this->FlatDAQCheck(flatdaqdata) ){
      raw.readData(flatdaqdata[0].getRawBufferPointer());
      return true;
    }
    
    mf::LogWarning("RawEventNotFound")
    << "Could not find a raw event in the FlatDAQData.";
    
    return false;
  }
  
  
  //...........................................................................
  /// Unpack the raw event header
  /// \param flatdaqdata : FlatDAQData
  /// \param header : Raw event header
  bool DAQUnpack::GetRawEventHeader(const std::vector<rawdata::FlatDAQData>& flatdaqdata,
                                          daqdataformats::RawEventHeader& header)
  {
    daqdataformats::RawEvent raw;
    if ( (this->GetRawEvent(flatdaqdata, raw)) ) {
      raw.getHeader();
      header = *(raw.getHeader());
      return true;
    }
    
    mf::LogWarning("RawEventHeaderNotFound")
    << "Could not find a header in the raw event.";
    
    return false;
  }


}
////////////////////////////////////////////////////////////////////////////////
