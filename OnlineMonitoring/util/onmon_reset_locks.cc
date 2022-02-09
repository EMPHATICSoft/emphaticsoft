#include <iostream>
#include <string>
#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/IPCBlock.h"

int main(int argc, char** argv)
{
  std::string help1("-h");
  std::string help2("--help");
  if (argc!=2 || help1==argv[1] || help2==argv[1]) {
    std::cout << "Usage:\n"
	      << "%  onmon_reset_locks [handle]\n"
	      << "where [handle] is a 4-character shared memory block handle (EMPH, SSD1, ...)\n"
	      << std::endl;
    exit(0);
  }

  emph::onmon::IPC client(emph::onmon::kIPC_CLIENT,argv[1]);

  //
  // While we are resetting the locks, check the integrity of the
  // block markers. If any writes have over stepped their bounds,
  // these will be sullied
  //
  if (client.fIPC->fHeader!=BLOCK_HEADER) {
    std::cerr << "IPC header corrupted!" << std::endl;
  }
  if (client.fIPC->fFooter!=BLOCK_FOOTER) {
    std::cerr << "IPC footer corrupted!" << std::endl;
  }

  if (client.fIPC->fProcessResources.fHeader!=BLOCK_HEADER) {
    std::cerr << "ProcessResources header corrupted!" << std::endl;
  }
  if (client.fIPC->fProcessResources.fFooter!=BLOCK_FOOTER) {
    std::cerr << "ProcessResources footer corrupted!" << std::endl;
  }
  if (client.fIPC->fHistoList.fHeader!=BLOCK_HEADER) {
    std::cerr << "HistoList header corrupted!" << std::endl;
  }
  if (client.fIPC->fHistoList.fFooter!=BLOCK_FOOTER) {
    std::cerr << "HistoList footer corrupted!" << std::endl;
  }
  if (client.fIPC->fHistogram.fHeader!=BLOCK_HEADER) {
    std::cerr << "Histogram header corrupted!" << std::endl;
  }
  if (client.fIPC->fHistogram.fFooter!=BLOCK_FOOTER) {
    std::cerr << "Histogram footer corrupted!" << std::endl;
  }

  if (client.fShmId!=0) client.ResetAllLocks();

  return 0;
}
////////////////////////////////////////////////////////////////////////
