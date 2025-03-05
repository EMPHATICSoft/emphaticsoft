////////////////////////////////////////////////////////////////////////
/// \brief   LAPPDRawDigit class
/// \author  jpaley@fnal.gov, details of data format from Joe Pastika
////////////////////////////////////////////////////////////////////////
#include "RawData/LAPPDRawDigit.h"

#include <iostream>
namespace emph {

  namespace rawdata{

    //----------------------------------------------------------------------

    LAPPDRawDigit::LAPPDRawDigit():
      fACCHeader(0), fACDCHeader(0), fTime320(0), fTimeWR(0), 
      fAlignHeader(0), fIsNoise(false)    
    {
      memset(fData,0,1440*16);
    }
   
    //----------------------------------------------------------------------
    LAPPDRawDigit::LAPPDRawDigit(uint64_t* block) : 
      fIsNoise(false)
    {
      fACCHeader = block[0];
      fACDCHeader = block[1];
      fTime320 = block[2];
      fTimeWR = block[3]; 
      fAlignHeader = block[4];
      memcpy(fData,&block[5],1440*16);
    }

    //----------------------------------------------------------------------
    LAPPDRawDigit::LAPPDRawDigit(uint64_t accH, uint64_t acdcH, uint64_t t320, uint64_t twr, uint64_t aH, uint64_t* data) :
      fACCHeader(accH), fACDCHeader(acdcH), fTime320(t320), 
      fTimeWR(twr), fAlignHeader(aH), fIsNoise(false)          
    {
      memcpy(fData,data,1440*sizeof(uint64_t));
    }

  } // end namespace rawdata
} // end namespace emph
