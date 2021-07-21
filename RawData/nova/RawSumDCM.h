/////////////////////////////
/// \author  atsaris@fnal.gov
/// \date
////////////////////////////

#ifndef RAWSUMDCM_H
#define RAWSUMDCM_H

#include <vector>
#include <stdint.h>

namespace rawdata {
  enum RawSumDCMType {
    RawSumDCMType_INVALID = 0xFF,
    RawSumDCMType_DCMMissing = 0,
    RawSumDCMType_MuSlMissing = 1,
    RawSumDCMType_MuSlCorrupt = 2
  };
  class RawSumDCMPoint;

  class RawSumDCM {
  public:
  RawSumDCM() : _points(){}
    ~RawSumDCM(){;}

  public:
    std::vector<RawSumDCMPoint> _points;
  };

  class RawSumDCMPoint
  {
  public:
    RawSumDCMPoint(); // Default constructor
    ~RawSumDCMPoint(){}
        
  public:
    uint32_t           fSumTrig_DCM_ID;
    uint8_t            fSumTrig_DCM_dib;
    uint8_t            fSumTrig_DCM_dcm;
    uint8_t            fSumTrig_DCM_det;
    RawSumDCMType      fSumTrig_DCM_Type;
    uint32_t           fSumTrig_DCM_TimeStampLow;
    uint32_t           fSumTrig_DCM_TimeStampHigh;
    
  private:
    
  };
}

#endif // RAWSUMDROPMB_H
///////////////////////////////////////////////////////////////////////////////////////////
