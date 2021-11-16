
#ifndef _CAENDecoder_h
#define _CAENDecoder_h

#include <atomic>
#include <type_traits>
//#include "messagefacility/MessageLogger/MessageLogger.h"
#include <map>
#include <string>
#include <initializer_list>
#include "CAENDigitizer.h"
#include "CAENDigitizerType.h"
extern "C"
{
#include "CAENComm.h"
#include "CAENVMElib.h"
}

namespace sbndaq
{    
class CAENDecoder
{
 public:

  const static char * zeit(char delimiter = ':');
  static void commError(CAENComm_ErrorCode retcod, 
			      const std::string label);
  static void vmeError(CVErrorCodes retcod, 
		       const std::string label);
  static void checkError(CAEN_DGTZ_ErrorCode err, 
			       const std::string label,
			       int boardId=-1);

  const static std::string CAENError(CAEN_DGTZ_ErrorCode err)
  {
    static const std::map<CAEN_DGTZ_ErrorCode, std::string> errors{{CAEN_DGTZ_Success, "CAEN_DGTZ_Success" },
            { CAEN_DGTZ_CommError,                "CAEN_DGTZ_CommError" },
            { CAEN_DGTZ_GenericError,             "CAEN_DGTZ_GenericError" },
            { CAEN_DGTZ_InvalidParam,             "CAEN_DGTZ_InvalidParam" },
            { CAEN_DGTZ_InvalidLinkType,          "CAEN_DGTZ_InvalidLinkType" },
	    { CAEN_DGTZ_InvalidHandle,            "CAEN_DGTZ_InvalidHandle" },
            { CAEN_DGTZ_MaxDevicesError,          "CAEN_DGTZ_MaxDevicesError" },
            { CAEN_DGTZ_BadBoardType,             "CAEN_DGTZ_BadBoardType" },
            { CAEN_DGTZ_BadInterruptLev,          "CAEN_DGTZ_BadInterruptLev" },
            { CAEN_DGTZ_BadEventNumber,           "CAEN_DGTZ_BadEventNumber" },
            { CAEN_DGTZ_ReadDeviceRegisterFail,   "CAEN_DGTZ_ReadDeviceRegisterFail" },
            { CAEN_DGTZ_WriteDeviceRegisterFail,  "CAEN_DGTZ_WriteDeviceRegisterFail" },
            { CAEN_DGTZ_InvalidChannelNumber,     "CAEN_DGTZ_InvalidChannelNumber" },
            { CAEN_DGTZ_ChannelBusy,              "CAEN_DGTZ_ChannelBusy" },
            { CAEN_DGTZ_FPIOModeInvalid,          "CAEN_DGTZ_FPIOModeInvalid" },
            { CAEN_DGTZ_WrongAcqMode,             "CAEN_DGTZ_WrongAcqMode" },
            { CAEN_DGTZ_FunctionNotAllowed,       "CAEN_DGTZ_FunctionNotAllowed" },
            { CAEN_DGTZ_Timeout,                  "CAEN_DGTZ_Timeout" },
            { CAEN_DGTZ_InvalidBuffer,            "CAEN_DGTZ_InvalidBuffer" },
            { CAEN_DGTZ_EventNotFound,            "CAEN_DGTZ_EventNotFound" },
            { CAEN_DGTZ_InvalidEvent,             "CAEN_DGTZ_InvalidEvent" },
            { CAEN_DGTZ_OutOfMemory,              "CAEN_DGTZ_OutOfMemory" },
            { CAEN_DGTZ_CalibrationError,         "CAEN_DGTZ_CalibrationError" },
            { CAEN_DGTZ_DigitizerNotFound,        "CAEN_DGTZ_DigitizerNotFound" },
            { CAEN_DGTZ_DigitizerAlreadyOpen,     "CAEN_DGTZ_DigitizerAlreadyOpen" },
            { CAEN_DGTZ_DigitizerNotReady,        "CAEN_DGTZ_DigitizerNotReady" },
            { CAEN_DGTZ_InterruptNotConfigured,   "CAEN_DGTZ_InterruptNotConfigured" },
            { CAEN_DGTZ_DigitizerMemoryCorrupted, "CAEN_DGTZ_DigitizerMemoryCorrupted" },
            { CAEN_DGTZ_DPPFirmwareNotSupported,  "CAEN_DGTZ_DPPFirmwareNotSupported" },
            { CAEN_DGTZ_NotYetImplemented,        "CAEN_DGTZ_NotYetImplemented" }};
    auto it = errors.find (err);
    if (it == errors.end ()) return "CAEN_Unknown_error";
    return it->second;
  }


  const static std::string AnalogMonOutput(CAEN_DGTZ_AnalogMonitorOutputMode_t mode)
  {
    static const std::map<CAEN_DGTZ_AnalogMonitorOutputMode_t, std::string> table{
       { CAEN_DGTZ_AM_TRIGGER_MAJORITY, "CAEN_DGTZ_AM_TRIGGER_MAJORITY" },
       { CAEN_DGTZ_AM_TEST, "CAEN_DGTZ_AM_TEST" },
       { CAEN_DGTZ_AM_ANALOG_INSPECTION, "CAEN_DGTZ_AM_ANALOG_INSPECTION" },
       { CAEN_DGTZ_AM_BUFFER_OCCUPANCY, "CAEN_DGTZ_AM_BUFFER_OCCUPANCY" },
       { CAEN_DGTZ_AM_VOLTAGE_LEVEL, "CAEN_DGTZ_AM_VOLTAGE_LEVEL" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string ZeroSuppressionMode(CAEN_DGTZ_ZS_Mode_t mode)
  {
    static const std::map<CAEN_DGTZ_ZS_Mode_t, std::string> table{{CAEN_DGTZ_ZS_NO, "CAEN_DGTZ_ZS_NO" },
       { CAEN_DGTZ_ZS_NO,  "CAEN_DGTZ_ZS_INT" },
       { CAEN_DGTZ_ZS_INT, "CAEN_DGTZ_ZS_ZLE" },
       { CAEN_DGTZ_ZS_AMP, "CAEN_DGTZ_ZS_AMP" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }


  const static std::string AcquisitionMode(CAEN_DGTZ_AcqMode_t mode)
  {
    static const std::map<CAEN_DGTZ_AcqMode_t, std::string> table{
       { CAEN_DGTZ_SW_CONTROLLED, "CAEN_DGTZ_SW_CONTROLLED" },
       { CAEN_DGTZ_S_IN_CONTROLLED, "CAEN_DGTZ_S_IN_CONTROLLED" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string OutputSignalMode(CAEN_DGTZ_OutputSignalMode_t mode)
  {
    static const std::map<CAEN_DGTZ_OutputSignalMode_t, std::string> table{
        { CAEN_DGTZ_TRIGGER, "CAEN_DGTZ_TRIGGER" },
	{ CAEN_DGTZ_FASTTRG_ALL, "CAEN_DGTZ_FASTTRG_ALL" },
	{ CAEN_DGTZ_FASTTRG_ACCEPTED, "CAEN_DGTZ_FASTTRG_ACCEPTED" },
        { CAEN_DGTZ_BUSY, "CAEN_DGTZ_BUSY" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string RunSynchronizationMode(CAEN_DGTZ_RunSyncMode_t mode)
  {
    static const std::map<CAEN_DGTZ_RunSyncMode_t, std::string> table{
        { CAEN_DGTZ_RUN_SYNC_Disabled, "CAEN_DGTZ_RUN_SYNC_Disabled" },
	{ CAEN_DGTZ_RUN_SYNC_TrgOutTrgInDaisyChain, "CAEN_DGTZ_RUN_SYNC_TrgOutTrgInDaisyChain" },
	{ CAEN_DGTZ_RUN_SYNC_TrgOutSinDaisyChain, "CAEN_DGTZ_RUN_SYNC_TrgOutSinDaisyChain" },
	{ CAEN_DGTZ_RUN_SYNC_SinFanout, "CAEN_DGTZ_RUN_SYNC_SinFanout" },
       { CAEN_DGTZ_RUN_SYNC_GpioGpioDaisyChain, "CAEN_DGTZ_RUN_SYNC_GpioGpioDaisyChain" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string TriggerPolarity(CAEN_DGTZ_TriggerPolarity_t mode)
  {
    static const std::map<CAEN_DGTZ_TriggerPolarity_t, std::string> table{
      { CAEN_DGTZ_TriggerOnRisingEdge,  "CAEN_DGTZ_TriggerOnRisingEdge" },
      { CAEN_DGTZ_TriggerOnFallingEdge, "CAEN_DGTZ_TriggerOnFallingEdge" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string TriggerMode(CAEN_DGTZ_TriggerMode_t mode)
  {
    static const std::map<CAEN_DGTZ_TriggerMode_t, std::string> table{
      { CAEN_DGTZ_TRGMODE_DISABLED, "CAEN_DGTZ_TRGMODE_DISABLED" },
      { CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, "CAEN_DGTZ_TRGMODE_EXTOUT_ONLY" },
      { CAEN_DGTZ_TRGMODE_ACQ_ONLY, "CAEN_DGTZ_TRGMODE_ACQ_ONLY" },
      { CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, "CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string EnaDisMode(CAEN_DGTZ_EnaDis_t mode)
  {
    static const std::map<CAEN_DGTZ_EnaDis_t, std::string> table{
      { CAEN_DGTZ_DISABLE, "CAEN_DGTZ_DISABLE" },
      { CAEN_DGTZ_ENABLE,  "CAEN_DGTZ_ENABLE" }};
    auto it = table.find(mode);
    if (it == table.end ()) return "CAEN_UNKNOWN";
    return it->second;
  }

  const static std::string IRQMode(CAEN_DGTZ_IRQMode_t mode)
  {
    static const std::map<CAEN_DGTZ_IRQMode_t, std::string> table{
       { CAEN_DGTZ_IRQ_MODE_ROAK, "CAEN_DGTZ_IRQ_MODE_ROAK" },
       { CAEN_DGTZ_IRQ_MODE_RORA, "CAEN_DGTZ_IRQ_MODE_RORA" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }

  const static std::string IOLevel(CAEN_DGTZ_IOLevel_t mode)
  {
    static const std::map<CAEN_DGTZ_IOLevel_t, std::string> table{
       { CAEN_DGTZ_IOLevel_NIM, "CAEN_DGTZ_IOLevel_NIM" },
       { CAEN_DGTZ_IOLevel_TTL, "CAEN_DGTZ_IOLevel_TTL" }};
    auto it = table.find (mode);
    if (it == table.end ()) return "CAEN_Unknown";
    return it->second;
  }
};
}

#endif
