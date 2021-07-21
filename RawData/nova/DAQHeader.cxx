////////////////////////////////////////////////////////////////////////
/// \brief  DAQHeader class
/// \author jpaley@indiana.edu/
/// \date  
////////////////////////////////////////////////////////////////////////
#include "RawData/DAQHeader.h"

#include <iostream>
#include <string>

#include "NovaDAQConventions/DAQConventions.h"

namespace rawdata
{
  
  //----------------------------------------------------------------------
  
  DAQHeader::DAQHeader() : 
    fStatus(0), fDetId(novadaq::cnv::kUNKNOWN_DET), fTotalMicroSlices(-1)
  {
    
  }
  
  //----------------------------------------------------------------------
  
  DAQHeader::DAQHeader(int detId, unsigned int status, int TotalMicroSlices) :
    fStatus(status), fDetId(detId), fTotalMicroSlices(TotalMicroSlices)
  { 
    
  }
  
  DAQHeader::DAQHeader(const DAQHeader& daqheader):
    fStatus(daqheader.fStatus),
    fDetId (daqheader.fDetId),
    fTotalMicroSlices (daqheader.fTotalMicroSlices)
  {

  }

  
  //----------------------------------------------------------------------
  
  DAQHeader::~DAQHeader()
  {
    
  }
  
} //end namespace rawdata
////////////////////////////////////////////////////////////////////////
