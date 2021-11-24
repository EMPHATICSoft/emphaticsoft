#include "PoolBuffer.hh"
#define TRACE_NAME "PoolBuffer"
#include <future>
#include <iomanip>
#include <iostream>

#include <sstream>
#include <thread>


using emphaticdaq::PoolBuffer;

std::string PoolBuffer::debugInfo() const {
  assert(_poolBufferSize != 0);
  TLOG(DEBUGINFO) << "Object  PoolBuffer.";
  TLOG(DEBUGINFO) << "\n Red zone checks: " << (_enableRedzones != 0 ? "enabled" : "disabled");
  TLOG(DEBUGINFO) << "\n Allocated size: " << poolBufferSize();
  TLOG(DEBUGINFO) << "\n DataBlock size: " << blockSize();
  TLOG(DEBUGINFO) << "\n Padded DataBlock size: " << poolBufferSize() / blockCount();
  TLOG(DEBUGINFO) << "\n Active DataBlock count: " << activeBlockCount();
  TLOG(DEBUGINFO) << "\n Free DataBlock count: " << freeBlockCount();
  TLOG(DEBUGINFO) << "\n Total DataBlock count: " << blockCount();
  TLOG(DEBUGINFO) << "\n Total DataBlocks written: " << totalBlocksWritten();
  TLOG(DEBUGINFO) << "\n Total bytes written: " << totalBytesWritten();
  TLOG(DEBUGINFO) << "\n Total DataBlocks read: " << totalBlocksRead();
  TLOG(DEBUGINFO) << "\n Total bytes read: " << totalBytesRead();
  TLOG(DEBUGINFO) << "\n PoolBuffer fully drained count: " << fullyDrainedCount();
  TLOG(DEBUGINFO) << "\n PoolBuffer low water mark: " << lowWaterMark();
  TLOG(DEBUGINFO) << "\n Unretured DataBlock count: " << blockCount() - activeBlockCount() - freeBlockCount();
  return {};
}
