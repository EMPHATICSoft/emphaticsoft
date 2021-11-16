//
//  CAENConfiguration.cc
//
//  Class to read and contain SBND CAEN configuration 
//   parameters
//
//
#define TRACE_NAME "CAENConfiguration"
#include "artdaq/DAQdata/Globals.hh"

#include "CAENConfiguration.hh"

// Constructor
sbndaq::CAENConfiguration::CAENConfiguration(fhicl::ParameterSet const & ps):
  link(0),
  firstBoardId(0),
  nBoards(0),
  enableReadout(0),
  boardId(0),
  recordLength(0),
  postPercent(0),
  irqWaitTime(0),
  allowTriggerOverlap(true),
  usePedestals(0),
  dacValue(0),
  dynamicRange(0),
  ioLevel(0),
  nChannels(0),
  triggerPolarity(0),
  extTrgMode(0),
  swTrgMode(0),
  acqMode(0),
  debugLevel(0),
  runSyncMode(0),
  outputSignalMode(0),
  eventCounterWarning(0),
  memoryAlmostFull(0),
  analogMode(0),
  testPattern(0)
{
  link                 = ps.get<int>("link");
  enableReadout        = ps.get<int>("enableReadout");
  boardId              = ps.get<int>("boardId");
  recordLength         = ps.get<int>("recordLength");
  runSyncMode          = ps.get<int>("runSyncMode");
  outputSignalMode     = ps.get<int>("outputSignalMode");
  allowTriggerOverlap  = ps.get<bool>("allowTriggerOverlap");
  usePedestals         = ps.get<bool>("usePedestals");
  dacValue             = ps.get<int>("dacValue");
  dynamicRange         = ps.get<int>("dynamicRange");
  ioLevel              = ps.get<int>("ioLevel");
  nChannels            = ps.get<int>("nChannels");
  extTrgMode           = ps.get<int>("extTrgMode");
  swTrgMode            = ps.get<int>("swTrgMode");
  acqMode              = ps.get<int>("acqMode");
  triggerPolarity      = ps.get<int>("triggerPolarity");
  triggerPulseWidth    = ps.get<uint8_t>("triggerPulseWidth");
  debugLevel           = ps.get<int>("debugLevel");
  postPercent          = ps.get<int>("postPercent");
  irqWaitTime          = ps.get<int>("irqWaitTime");
  eventCounterWarning  = ps.get<int>("eventCounterWarning");
  memoryAlmostFull     = ps.get<int>("memoryAlmostFull");
  readoutMode          = ps.get<int>("readoutMode");
  analogMode           = ps.get<int>("analogMode");
  testPattern          = ps.get<int>("testPattern");

  char tag[1024];
  channelEnableMask = 0;

  for ( int j=0; j<MAX_CHANNELS; j++){
    sprintf(tag,"triggerThreshold%d", j);
    triggerThresholds[j] = ps.get<uint16_t>(tag);
  }

  if ( enableReadout ){
    for ( int j=0; j<MAX_CHANNELS; j++){
      sprintf(tag,"channelEnable%d", j);
      channelEnable[j] = ps.get<bool>(tag);
      if ( channelEnable[j] )
      {
	channelEnableMask |= ( 1 << j );
	sprintf(tag,"channelPedestal%d", j);
	pedestal[j] = ps.get<int>(tag);
      }

      /*
      sprintf(tag,"channelSelfTrgMode%d", j);
      channelSelfTrgMode[j] = ps.get<bool>(tag);
      if ( channelSelfTrgMode[j] )
      {
	channelEnableMask |= ( 1 << j );
	sprintf(tag,"channelPedestal%d", j);
	pedestal[j] = ps.get<int>(tag);
      }
      */
    }
  }
}


void sbndaq::CAENConfiguration::print(std::ostream& os) 
{
  os << *this;
}

std::ostream& operator<<(std::ostream& os, const sbndaq::CAENConfiguration& e)
{
  os << "CAEN Configuration" << std::endl;
  os << "  Link                  " << e.link << std::endl;
  os << "  nBoards               " << e.nBoards << std::endl;
  os << "  EnableReadout         " << e.enableReadout << std::endl;
  os << "  RecordLength          " << e.recordLength << std::endl;
  os << "  AllowTriggerOverlap   " << e.allowTriggerOverlap << std::endl;
  os << "  UsePedestals          " << e.usePedestals << std::endl;
  os << "  DacValue              " << e.dacValue << std::endl;
  os << "  DynamicRange          " << e.dynamicRange << std::endl;
  os << "  nChannels             " << e.nChannels << std::endl;
  os << "  PostPercent           " << e.postPercent << "%" << std::endl;
  os << "  IrqWaitTime           " << e.irqWaitTime << std::endl;
  os << "  IOLevel (NIM or TTL)  " << e.ioLevel << " " 
     << sbndaq::CAENDecoder::IOLevel((CAEN_DGTZ_IOLevel_t)e.ioLevel) << std::endl;
  os << "  TriggerPolarity       " << e.triggerPolarity << " " 
     << sbndaq::CAENDecoder::TriggerPolarity((CAEN_DGTZ_TriggerPolarity_t)e.triggerPolarity) << std::endl;
  os << "  ExtTrgMode            " << e.extTrgMode << " " 
     << sbndaq::CAENDecoder::TriggerMode((CAEN_DGTZ_TriggerMode_t)e.extTrgMode) << std::endl;
  os << "  SWTrgMode             " << e.swTrgMode << " " 
     << sbndaq::CAENDecoder::TriggerMode((CAEN_DGTZ_TriggerMode_t)e.swTrgMode) << std::endl;
  for ( int j=0; j<sbndaq::CAENConfiguration::MAX_CHANNELS; j++)
      os << "    Channel " << j << " Threshold " << e.triggerThresholds[j] << std::endl;
  os << "  AcqMode               " << e.acqMode << " " 
     << sbndaq::CAENDecoder::AcquisitionMode((CAEN_DGTZ_AcqMode_t)e.acqMode) << std::endl;
  os << "  DebugLevel            " << e.debugLevel << std::endl;
  os << "  EventCounterWarning   " << e.eventCounterWarning << std::endl;
  os << "  MemoryAlmostFull      " << e.memoryAlmostFull << std::endl;
  os << "  ReadoutMode           " << e.readoutMode << " " 
     << sbndaq::CAENDecoder::EnaDisMode((CAEN_DGTZ_EnaDis_t)e.readoutMode) << std::endl;
  os << "  AnalogMode            " << e.analogMode << std::endl;
  os << "  TestPattern           " << e.testPattern << std::endl;
  os << "  BoardId               " << e.boardId << 
      "  EnableReadout " << e.enableReadout << std::endl;
  if ( e.enableReadout )
  {
    os << "    ChannelEnableMask   0x" << std::hex << e.channelEnableMask << std::dec << std::endl;
    for ( int j=0; j<sbndaq::CAENConfiguration::MAX_CHANNELS; j++)
    {
      if ( e.channelEnable[j] )
      {
	os << "    Channel " << j << " Pedestal " << e.pedestal[j] << std::endl;
      }
    }
  }
  return(os);
}
