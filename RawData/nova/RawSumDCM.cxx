////////////////////////////////////////////////////////////////////////
/// \brief   RawTrigger class
/// \author  j
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/RawSumDCM.h"


namespace rawdata{
  
  //----------------------------------------------------------------------
  
  RawSumDCMPoint::RawSumDCMPoint() : fSumTrig_DCM_ID(0)
			 ,fSumTrig_DCM_dib(0)
			 ,fSumTrig_DCM_dcm(0)
			 ,fSumTrig_DCM_det(0)
			 ,fSumTrig_DCM_Type(RawSumDCMType_INVALID)
			 ,fSumTrig_DCM_TimeStampLow(0)
			 ,fSumTrig_DCM_TimeStampHigh(0)
  {
  }
  
      
} // end namespace rawdata
////////////////////////////////////////////////////////////////////////
