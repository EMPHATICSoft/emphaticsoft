#ifndef _FilesMerger_h_
#define _FilesMerger_h_

#include "SSD_FERMerger/Run.h"
#include "SSD_FERMerger/SSDConsts.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

namespace ssd {

  class PxEvent;
  
  namespace merger {
    class FilesMerger{
    public:
      FilesMerger();
      ~FilesMerger();
      void setStationNames   (const std::vector<std::string>& names){stationNames_ = names;}
      int  makeFilesList     (std::string  runNumber, std::string subrunNumber, std::string dir);
      int  makeFilesList     (unsigned int runNumber, unsigned int subrunNumber, std::string dir);
      int  makeStationBuffers(void);
      int  readStationBuffers(void);
      int  readFile          (void);
      int  merge             (void);
      int  readMergedFiles   (std::string fileName);
      int  writeMergedFiles  (std::string filesDirectory,std::string fileName = "");
      Run& getRun            (void){return theRun;}
      const std::multimap<std::string,std::string>& getFilesList     (void){return filesList_;}
      std::map<std::string,std::string>&            getStationBuffers(void){return stationBuffers_;}
      std::string BitString ( const std::vector<uint32_t>& cData, uint32_t pOffset, uint32_t pWidth );
      uint64_t mortonEncode(uint32_t pEvenWord, uint32_t pOddWord);
      void printWord(uint32_t w);
      void printWord(unsigned long long w);
      uint32_t reverseWord(uint32_t w);
    private:
      std::string                            runNumber_, subrunNumber_;
      std::multimap<std::string,std::string> filesList_;
      std::map<std::string,std::string>      stationBuffers_;
      std::vector<std::string>               stationNames_;
      
      Run                                    theRun;
      std::string                            rd53FileName_;
      //std::map<uint64_t,PxEvent*> memory_;
      
    };
  } // end namespace merger
} // end namespace ssd
#endif
