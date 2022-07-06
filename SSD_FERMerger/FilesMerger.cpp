#include "SSD_FERMerger/FilesMerger.h"
#include "SSD_FERMerger/Hit.h"
#include "SSD_FERMerger/Event.h"
#include "SSD_FERMerger/DataDecoder.h"
#include "SSD_FERMerger/PxEvent.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string> // std::string, std::stoi
#include <ctime>
#include <errno.h>
#include <dirent.h>
#include <cstdlib>
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <arpa/inet.h>
#include <vector>
#include <boost/utility.hpp>
#include "stib.h"
#include <algorithm>
#include <stdint.h>
#include <bitset>

using namespace std;
namespace ssd {
  namespace merger {
    int stripminus = 0;
    int stripnochange = 0;
    int stripplus = 0;
    int triggerminus = 0;
    int triggernochange = 0;
    int triggerplus = 0;
    uint currentprimitiveBCO = 0;
    uint lastprimitiveBCO = 0;
      
    //-----------------------------------------------------------------------
    FilesMerger::FilesMerger()
    {
    }
  
    //-----------------------------------------------------------------------
    FilesMerger::~FilesMerger()
    {
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::makeFilesList(unsigned int runNumber, unsigned int subrunNumber, std::string dir)
    {
      //std::cout << "Run Number " << runNumber << std::endl;
      stringstream runN, subrunN;
      runN.str("");
      subrunN.str("");
      runN << runNumber;
      subrunN << subrunNumber;
      return makeFilesList(runN.str(), subrunN.str(), dir);
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::makeFilesList(string runNumber, string subrunNumber, std::string dir)
    {
      std::multimap<std::string, std::string> tmpFilesList;
      filesList_.clear();
      //std::cout << "Run Number " << runNumber << std::endl;
      runNumber_ = runNumber;
      subrunNumber_ = subrunNumber;
      if (stationNames_.size() == 0)
	{
	  cout << __PRETTY_FUNCTION__ << "You need to set the station names first!" << endl;
	  return -1;
	}
    
      DIR *pDir;
      struct dirent *pDirEntries;
      for (unsigned int station = 0; station < stationNames_.size(); station++)
	{
	  //cout << __PRETTY_FUNCTION__<< stationNames_[station] << endl;
	  string subDir;
	  if (stationNames_[station].find("FER") != string::npos)
	    {
	      subDir = dir + "OtsData/";
	    }
	  else if (stationNames_[station].find("nim") != string::npos)
	    {
	      subDir = dir + "NimPlus/";
	    }
	
	  cout << __PRETTY_FUNCTION__ << "\tChecking dir: " << subDir << endl;
	
	  pDir = opendir(subDir.c_str());
	  if (!pDir)
	    {
	      cout << __PRETTY_FUNCTION__ << "\tOpendir() failure; can't open dir: " << subDir << endl;
	      return -1;
	    }
	  errno = 0;
	  while ((pDirEntries = readdir(pDir)))
	    {
	      string fileName = pDirEntries->d_name;
	      //cout << __PRETTY_FUNCTION__ << "FileName: " << fileName << endl;
	      if ((fileName.find("Run" + runNumber + "_") != string::npos && (fileName.find("_" + subrunNumber_ + "_")) != string::npos && fileName.find(stationNames_[station]) != string::npos))
		{
		
		  //std::cout << "Run Number " << runNumber << std::endl;
		  cout << __PRETTY_FUNCTION__ << fileName << " station name: " << stationNames_[station] << endl;
		  std::cout << __PRETTY_FUNCTION__ << std::string("Run" + runNumber_ + "_" + subrunNumber_) << std::endl;
		  tmpFilesList.insert(pair<string, string>(stationNames_[station], subDir + "/" + fileName));
		}
	    }
	
	  if (errno)
	    {
	      cout << __PRETTY_FUNCTION__ << "\tReaddir() failure; terminating" << endl;
	      return -1;
	    }
	  closedir(pDir);
	}
      vector<string> files;
      for (unsigned int station = 0; station < stationNames_.size(); station++)
	{
	  pair<multimap<string, string>::iterator, multimap<string, string>::iterator> stationRange;
	  stationRange = tmpFilesList.equal_range(stationNames_[station]);
	  files.clear();
	  for (multimap<string, string>::iterator fileIt = stationRange.first; fileIt != stationRange.second; fileIt++)
	    {
	      files.push_back(fileIt->second);
	    }
	  sort(files.begin(), files.end());
	  for (vector<string>::iterator fileIt = files.begin(); fileIt != files.end(); fileIt++)
	    {
	      filesList_.insert(pair<string, string>(stationNames_[station], *fileIt));
	      cout << __PRETTY_FUNCTION__ << "Using file: " << *fileIt << endl;
	    }
	}
      if (filesList_.size() == 0)
	{
	  cout << __PRETTY_FUNCTION__ << "There are no files, file list is empty -> " << filesList_.size() << endl;
	  exit(-1);
	}
      return 0;
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::makeStationBuffers(void)
    {
      if (filesList_.size() == 0)
	{
	  cout << __PRETTY_FUNCTION__ << "\tYou need to make the files list first!" << endl;
	  return -1;
	}
      stationBuffers_.clear();
      //  for(multimap<string,string>::iterator fileIt=filesList_.begin(); fileIt != filesList_.end(); fileIt++)
      //  {
      //    cout << __PRETTY_FUNCTION__ << fileIt->first << ":" << fileIt->second << endl;
      //  }
      for (unsigned int station = 0; station < stationNames_.size(); station++)
	{
	  pair<multimap<string, string>::iterator, multimap<string, string>::iterator> stationRange;
	  stationRange = filesList_.equal_range(stationNames_[station]);
	  string stationBuffer = "";
	  for (multimap<string, string>::iterator fileIt = stationRange.first; fileIt != stationRange.second; fileIt++)
	    {
	      //cout << __PRETTY_FUNCTION__ <<"Station: "<< fileIt->first << "-> File: " << fileIt->second << endl;
	      ifstream file;
	      file.open(fileIt->second.c_str());
	      if (!file.is_open())
		{
		  cout << __PRETTY_FUNCTION__ << "Can't open file: " << fileIt->second.c_str() << endl;
		}
	    
	      std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	      int dataLength = 0;
	    
	      //cout << __PRETTY_FUNCTION__ << stationNames_[station] << " data length: " << data.size() << endl;
	      std::vector<char>::iterator dataIt = data.begin();
	      if (stationNames_[station].find("FER") != string::npos || stationNames_[station].find("fed") != string::npos)
		dataLength = data.size();
	      else
		{
		  unsigned char blockLengthC;
		  int blockLength = -1;						   //Tells you how many 8 bytes words are written in a block
		  int cdatasize = static_cast<int>(data.size()); //To compare blockLength to data.size without type comparison issues
		  while (blockLength != 0 && blockLength < cdatasize)
		    {
		      blockLengthC = *dataIt;
		      blockLength = (int)blockLengthC;
		      //cout << __PRETTY_FUNCTION__ << blockLength << endl;
		      if (blockLength > 0)
			{
			  dataIt += blockLength * sizeof(unsigned long long) + 1;
			  dataLength += blockLength * sizeof(unsigned long long) + 1;
			  //file.seekg(dataLength);
			}
		    }
		}
	      //cout << __PRETTY_FUNCTION__ << stationNames_[station] << " calc length: " << dataLength << endl;
	      file.seekg(0, ios::beg);
	      char *charBuffer = new char[dataLength];
	      //cout << __PRETTY_FUNCTION__ << "Data length: " << dataLength << endl;
	      file.read(charBuffer, dataLength);
	      stationBuffer.append(charBuffer, dataLength);
	      delete charBuffer;
	      file.close();
	    }
	  stationBuffers_[stationNames_[station]] = stationBuffer;
	  //    break;
	}
      return 0;
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::merge(void)
    {
    
      //const bool         	         print_ = false;
      //    const int wrapAround = 1048576; //2^20
      //    const unsigned int maxEvents = -1;
    
      //    const unsigned int maxSeconds = 100;
      //    unsigned char blockLengthC;
      //    int blockLength = -1;
      //string                       runNumber;
      //stringstream runN; runN.str(""); runN << runNumber;
      //    unsigned long long currentTimestamp = 0;
      //    DataDecoder theDataDecoder;
      string stationName;
      //int                	         correction;
      map<string, vector<EventMonicelli *>> stationsEvents;
      map<string, unsigned int> eventCounters;
      std::map<uint32_t, bool> TriggerToRemove;
      uint64_t currentBCO_ = 0; // David
      std::map<int, std::map<uint64_t, PxEvent *>> memory_;
      unsigned char lastSeqId_ = 0;
      for (map<string, string>::iterator buffer = stationBuffers_.begin(); buffer != stationBuffers_.end(); buffer++)
	eventCounters[buffer->first] = 0;
      theRun.reset();
      std::vector<int> stripBco1;
      std::vector<int> stripBco2;
      std::vector<int> stripBco3;
      std::vector<int> currentTrigbco;
      //unsigned int totTrigger;
      int t1396counter = 0;
      ofstream myfile;
      std::string summary = "RunSummary.txt";
      myfile.open(summary);
    
      for (map<string, string>::iterator buffer = stationBuffers_.begin(); buffer != stationBuffers_.end(); buffer++)
	{
	  stationName = buffer->first;
	  //	unsigned int bufferPos = 0;
	  //	EventMonicelli *currentEvent = 0;
	  //	unsigned int currentTrigger = 0;
	  //	unsigned int lastTrigger = (unsigned int)-1;
	  //	uint64_t triggerBCO_ = 0;
	  //uint64_t previousTriggerBCO_ = 0;
	  //unsigned int  uniqueTrigger  = 0;
	  //uint64_t 	    HWcurrentBCO_;
	  //uint64_t	    lowBCOHW_;
	  bool printErrorInData = false;
	  //vector<EventMonicelli*>* stationEvents  = 0;
	  stationsEvents[buffer->first] = vector<EventMonicelli *>();
	  //stationEvents = &(stationsEvents[buffer->first]);
	  int station = -1;
	  if (stationName == "FER0")
	    station = 0;
	  else if (stationName == "FER1")
	    station = 1;
	  else if (stationName == "FER2")
	    station = 2;
	  else if (stationName == "FER3")
	    station = 3;
	  else
	    {
	      cout << __PRETTY_FUNCTION__ << "\tUnrecognized station name: " << stationName << endl;
	      exit(EXIT_FAILURE);
	    }
	  cout << __PRETTY_FUNCTION__ << "Building events for station name: " << stationName << " which is station #: " << station << endl;
	  if (true && (station == 0 || station == 1 || station == 2 || station == 3))
	    {
	      int dataTriggerDelay = 0; //85;//-0x58; //Time for trigger to reach FPGA
	      int stationDelay = 0; //trigger delay between stations
	      if (station == 0 || station == 1) {
		dataTriggerDelay = 2; 
		stationDelay     = 0; //85;//-0x58; //Time for trigger to reach FPGA
	      }
	      else if (station == 2 || station == 3) {
		dataTriggerDelay = 1; 
		stationDelay     = 4; //85;//-0x58; //Time for trigger to reach FPGA
	      }
	    
	      uint32_t word = 0;
	      uint32_t bco = 0;
	      uint32_t previousWord;
	      uint32_t previousBco;
	    
	      uint32_t triggerNumber_ = -1;
	      uint32_t previousTriggerNumber_ = -1;
	    
	      uint64_t veryFirstTrigger_ = 0;
	      uint64_t highBco = 0;
	      bool getout = false;
	      bool isFirstTrigger = true;
	    
	      std::cout << __PRETTY_FUNCTION__ << "File size: " << buffer->second.size() << std::endl;
	    
	      int type, dataType;
	    
	      string dataName;
	    
	      unsigned int increment = sizeof(uint64_t);
	      unsigned int offset = 8;
	      int len = buffer->second.size();
	    
	      unsigned char seqId;
	    
	      //for ( int i=0; i<len; i+=increment)
	      //for ( int i=offset; i<100;)
	      //	std::cout << buffer->second[i] << std::endl;
	      //for (int i = offset; i < 2000;)
	      for (int i = offset; i < len;)
		{
		  if (getout)
		    break;
		  //if((int)triggerNumber_ > 10) break;
		  unsigned char blockLengthC = buffer->second[i];
		  unsigned int numberOfQuadWords = (unsigned int)blockLengthC; //*2;
		  //std::cout << "Quad words: " << numberOfQuadWords << " i " << i << " len " << len << " nextI "
		  //		<< i + numberOfQuadWords*8 + 3 << std::endl;
		
		  seqId = buffer->second[i + 2];
		  if (!(lastSeqId_ + 1 == seqId ||
			(lastSeqId_ == 255 && seqId == 0)))
		    {
		      std::cout << "Station: " << station << " Missed packet?? " << (unsigned int)lastSeqId_ << " v " << (unsigned int)seqId << std::endl;
		    }
		
		  lastSeqId_ = seqId;
		  //Skip 2 bytes + the byte with number of quads
		  i += 3;
		  for (unsigned int block = 0; block < numberOfQuadWords; block++, i += increment)
		    {
		      previousWord = word;
		      previousBco = bco;
		      word = (((buffer->second[i]) & 0xff)) + (((buffer->second[i + 1]) & 0xff) << 8) + (((buffer->second[i + 2]) & 0xff) << 16) + (((buffer->second[i + 3]) & 0xff) << 24);
		      bco = (((buffer->second[i + 4]) & 0xff)) + (((buffer->second[i + 5]) & 0xff) << 8) + (((buffer->second[i + 6]) & 0xff) << 16) + (((buffer->second[i + 7]) & 0xff) << 24);
		    
		      type = word & 0xf;
		      dataType = (word >> 4) & 0xf;
		      if (type == 8 && dataType == 0x1)
			dataName = "Low BCO       ";
		      else if (type == 8 && dataType == 0x2)
			dataName = "High BCO      ";
		      else if (type == 8 && dataType == 0xa)
			dataName = "Low trigger   ";
		      else if (type == 8 && dataType == 0xb)
			dataName = "High trigger  ";
		      else if (type == 8 && dataType == 0xc)
			dataName = "Trigger type c";
		      else if (type == 8 && dataType == 0xd)
			dataName = "Trigger type d";
		      else if (type == 8 && dataType == 0xe)
			dataName = "Trigger type e";
		      else if (type == 8 && dataType == 0xf)
			dataName = "Trigger type f";
		      else if ((type & 0x1) == 1)
			dataName = "Strip";
		      else
			dataName = "Unknown type";
		    
		      if (false && dataName != "Strip" && dataName != "Trigger type f")
			cout << i << hex << "--> word: " << word << " bco:" << bco << dec << " name: " << dataName << endl;
		    
		      if ((type & 0x1) == 1) //Strip Hit Data Word
			{
			  if (word == previousWord && bco != previousBco)
			    {
			      cout << i << hex << " ERROR: Data: " << word << " bco: " << bco << " prev bco: " << previousBco << dec << endl;
			      continue;
			    }
			
			  Hit hit;
			
			  hit.data = word;
			  unsigned long long rocType = 1; //Bits 63->60 are the ROC type => 0 pixels 1 strip
			  unsigned long long longStation = station;
			  unsigned long long stripTriggerNumber = 0xfffff; //must be fixed
			  unsigned long long reset = 0x00000000ffffffff;
			
			  hit.data &= reset;
			  hit.data += ((rocType & 0xf) << 60) + ((longStation & 0xf) << 56) + ((stripTriggerNumber & 0xfffff) << 32); //Adding station number to the final merged data
			
			  unsigned long long tmpData = 0;
			  unsigned long long orderedData = 0;
			  int dataSize = sizeof(unsigned long long);
			  for (int byte = 0; byte < dataSize; byte++)
			    {
			      tmpData = ((hit.data >> ((dataSize - byte - 1) * 8)) & 0xff) << (byte * 8);
			      orderedData += tmpData;
			    }
			  hit.data = orderedData;
			  hit.plaq = (word >> 27) & 0x07;
			  hit.cid = (word >> 24) & 0x07;
			  int set = (word >> 12) & 0x1f;
			  int strip = (word >> 17) & 0x0f;
			  hit.bco = (word >> 4) & 0xff;
			  hit.adc = (word >> 1) & 0x7;
			  hit.station = station;
			  hit.row = stib::Stib::SensorStrip(hit.cid, set, strip);
			
			  uint64_t eventBCO   = stationDelay + highBco + (bco & 0xffffff00) + (hit.bco & 0xff);
			  uint64_t generalBCO = stationDelay + highBco + (currentBCO_ & 0xffffffff00000000ULL) + (bco & 0xffffffff);
			
			  uint64_t timeblock[3];
			  timeblock[0] = eventBCO - 0x100;
			  timeblock[1] = eventBCO;
			  timeblock[2] = eventBCO + 0x100;
			
			  int binid = 0;
			  int diff[3];
			  for (int j = 0; j < 3; j++)
			    {
			      diff[j] = generalBCO - timeblock[j];
			      if (abs(diff[j]) < abs(diff[binid]))
				binid = j;
			    }
			
			  //std::cout << "Bin: " << binid << std::endl;
			  eventBCO = timeblock[binid];
			
			  if (binid == 0)
			    stripminus++;
			  if (binid == 1)
			    stripnochange++;
			  if (binid == 2)
			    stripplus++;
			
			  if (station == 2 || station == 3)
			    eventBCO *= 2;
			  PxEvent *event=0;
			  //UNCOMMENT TO SEE THE DATA BCO!!!!!!
			  //UNCOMMENT TO SEE THE DATA BCO!!!!!!
			  //UNCOMMENT TO SEE THE DATA BCO!!!!!!
			  if (false && triggerNumber_ % 10000 == 0)
			    {
			      cout << "[" << __LINE__ << "] EVENT DATA BCO: " << hex << eventBCO << " - "
				   << " generalBCO: " << generalBCO << " Strip: " << hit.bco << dec << endl;
			      //cout << "DATA:" << hex << hit.data << dec << endl;
			    }
			
			  if (memory_[station].find(eventBCO) == memory_[station].end() && memory_[station].find(eventBCO - 1) == memory_[station].end() && memory_[station].find(eventBCO + 1) == memory_[station].end() && memory_[station].find(eventBCO - 2) == memory_[station].end() && memory_[station].find(eventBCO + 2) == memory_[station].end() && memory_[station].find(eventBCO + 3) == memory_[station].end() && memory_[station].find(eventBCO - 3) == memory_[station].end() && memory_[station].find(eventBCO + 4) == memory_[station].end() && memory_[station].find(eventBCO - 4) == memory_[station].end() && memory_[station].find(eventBCO + 5) == memory_[station].end() && memory_[station].find(eventBCO - 5) == memory_[station].end() && memory_[station].find(eventBCO + 6) == memory_[station].end() && memory_[station].find(eventBCO - 6) == memory_[station].end() && memory_[station].find(eventBCO + 7) == memory_[station].end() && memory_[station].find(eventBCO - 7) == memory_[station].end() && memory_[station].find(eventBCO + 8) == memory_[station].end() && memory_[station].find(eventBCO - 8) == memory_[station].end() && memory_[station].find(eventBCO + 9) == memory_[station].end() && memory_[station].find(eventBCO - 9) == memory_[station].end() && memory_[station].find(eventBCO + 10) == memory_[station].end() && memory_[station].find(eventBCO - 10) == memory_[station].end())
			    {
			      memory_[station][eventBCO] = new PxEvent(eventBCO);
			      event = memory_[station][eventBCO];
			    }
			  else
			    {
			      //cout << hex << "!! MATCH -> Delta: " << currentBCO_ - eventBCO << " EventMonicelli(strip) BCO: " << eventBCO << dec << endl;
			      if (memory_[station].find(eventBCO) != memory_[station].end())
				event = memory_[station][eventBCO];
			      else if (memory_[station].find(eventBCO - 1) != memory_[station].end())
				event = memory_[station][eventBCO - 1];
			      else if (memory_[station].find(eventBCO + 1) != memory_[station].end())
				event = memory_[station][eventBCO + 1];
			      else if (memory_[station].find(eventBCO - 2) != memory_[station].end())
				event = memory_[station][eventBCO - 2];
			      else if (memory_[station].find(eventBCO + 2) != memory_[station].end())
				event = memory_[station][eventBCO + 2];
			      else if (memory_[station].find(eventBCO - 3) != memory_[station].end())
				event = memory_[station][eventBCO - 3];
			      else if (memory_[station].find(eventBCO + 3) != memory_[station].end())
				event = memory_[station][eventBCO + 3];
			      else if (memory_[station].find(eventBCO - 4) != memory_[station].end())
				event = memory_[station][eventBCO - 4];
			      else if (memory_[station].find(eventBCO + 4) != memory_[station].end())
				event = memory_[station][eventBCO + 4];
			      else if (memory_[station].find(eventBCO - 5) != memory_[station].end())
				event = memory_[station][eventBCO - 5];
			      else if (memory_[station].find(eventBCO + 5) != memory_[station].end())
				event = memory_[station][eventBCO + 5];
			      else if (memory_[station].find(eventBCO - 6) != memory_[station].end())
				event = memory_[station][eventBCO - 6];
			      else if (memory_[station].find(eventBCO + 6) != memory_[station].end())
				event = memory_[station][eventBCO + 6];
			      else if (memory_[station].find(eventBCO - 7) != memory_[station].end())
				event = memory_[station][eventBCO - 7];
			      else if (memory_[station].find(eventBCO + 7) != memory_[station].end())
				event = memory_[station][eventBCO + 7];
			      else if (memory_[station].find(eventBCO - 8) != memory_[station].end())
				event = memory_[station][eventBCO - 8];
			      else if (memory_[station].find(eventBCO + 8) != memory_[station].end())
				event = memory_[station][eventBCO + 8];
			      else if (memory_[station].find(eventBCO - 9) != memory_[station].end())
				event = memory_[station][eventBCO - 9];
			      else if (memory_[station].find(eventBCO + 9) != memory_[station].end())
				event = memory_[station][eventBCO + 9];
			      else if (memory_[station].find(eventBCO - 10) != memory_[station].end())
				event = memory_[station][eventBCO - 10];
			      else if (memory_[station].find(eventBCO + 10) != memory_[station].end())
				event = memory_[station][eventBCO + 10];
			      //else    cout << "####Not assigned!!!! Station: " << station << endl;
			    }
			  if (event) {
			    event->setRawHit(word);
			    event->setDecodedHit(hit);
			    event->setCurrentBCO(eventBCO);
			  }
			}
		      else if (type == 8)
			{
			  dataType = (word >> 4) & 0xf; // Unnecessary since already done a few lines up????
			  if (dataType == 1)			  // HIGH BCO --- PKA LOW BCO
			    {
			      if (station == 0 || station == 1)
				{
				  highBco = (((uint64_t)((word >> 8) & 0x00ffff)) << 32);
				  std::cout << "[" << __LINE__ << "]"
					    << "Low  bco-> word: " << hex << word << " bco: " << bco << dec << std::endl;
				}
			    }
			  else if (dataType == 2) //HIGH BCO
			    {
			    
			      if (station == 2 || station == 3)
				{
				  highBco = (((uint64_t)((word >> 16) & 0x00ffff)) << 32);
				  // std::cout << "[" << __LINE__ << "]"
				  // 		  << "High bco-> word: " << hex << word << " bco: " << bco << dec << std::endl;
				}
			    }
			  else if (dataType == 0xa) //LOW TRIGGER
			    {
			      previousTriggerNumber_ = triggerNumber_;
			      triggerNumber_ = (word >> 16) & 0xffff;
			      //			    triggerBCO_ = (word >> 8) & 0xff;
			    }
			  else if (dataType == 0xf) //HIGH TRIGGER
			    {
			    
			      triggerNumber_ = ((word >> 8) & 0xffffff);
			      //myfile <<  t1396counter << " " << currentTimestamp << "\n";
			      if (triggerNumber_ - previousTriggerNumber_ > 1)
				{
				  if (printErrorInData)
				    std::cout << __PRETTY_FUNCTION__ << "I skipped some triggers!!!Previous trigger was: " << previousTriggerNumber_ << " while now: " << triggerNumber_ << std::endl;
				  //exit(0);
				}
			    
			      uint64_t eventBCO = stationDelay + (highBco & 0xffffffff00000000ULL) + bco;
			      eventBCO -= dataTriggerDelay;
			    
			      if (station == 2 || station == 3)
				eventBCO *= 2;
			      PxEvent *event = 0;
			      if (false && triggerNumber_ % 10000 == 0)
				{
				  std::cout << "[" << __LINE__ << "] EVENT DATA BCO: " << hex << eventBCO << " - bco: " << bco << " - " << dec << bco << " Trigger number: " << dec << triggerNumber_ << std::endl;
				}
			      if (isFirstTrigger) //triggerNumber_ == 0)
				{
				  isFirstTrigger = false;
				  std::cout << "[" << __LINE__ << "]First trigger: " << triggerNumber_ << " bco: " << hex << eventBCO << dec << std::endl;
				  if (triggerNumber_ != 0)
				    std::cout << "[" << __LINE__ << "]ERROR - First trigger should be 0 and instead is : " << triggerNumber_ << " bco: " << hex << eventBCO << dec << std::endl;
				  veryFirstTrigger_ = triggerNumber_;
				}
			      triggerNumber_ -= veryFirstTrigger_;
			    
			      if (memory_[station].find(eventBCO) == memory_[station].end() && memory_[station].find(eventBCO - 1) == memory_[station].end() && memory_[station].find(eventBCO + 1) == memory_[station].end() && memory_[station].find(eventBCO - 2) == memory_[station].end() && memory_[station].find(eventBCO + 2) == memory_[station].end() && memory_[station].find(eventBCO + 3) == memory_[station].end() && memory_[station].find(eventBCO - 3) == memory_[station].end() && memory_[station].find(eventBCO + 4) == memory_[station].end() && memory_[station].find(eventBCO - 4) == memory_[station].end() && memory_[station].find(eventBCO + 5) == memory_[station].end() && memory_[station].find(eventBCO - 5) == memory_[station].end() && memory_[station].find(eventBCO + 6) == memory_[station].end() && memory_[station].find(eventBCO - 6) == memory_[station].end() && memory_[station].find(eventBCO + 7) == memory_[station].end() && memory_[station].find(eventBCO - 7) == memory_[station].end() && memory_[station].find(eventBCO + 8) == memory_[station].end() && memory_[station].find(eventBCO - 8) == memory_[station].end() && memory_[station].find(eventBCO + 9) == memory_[station].end() && memory_[station].find(eventBCO - 9) == memory_[station].end() && memory_[station].find(eventBCO + 10) == memory_[station].end() && memory_[station].find(eventBCO - 10) == memory_[station].end())
				{
				  //if(true)   cout << "NO MATCH FOR TRIGGER!!!!!!-> Delta: " << (int) currentBCO_ - (int) eventBCO << " EventMonicelli BCO: " << eventBCO << " Trigger Number: " << triggerNumber_ << " # unmatched: " << ++unmatchedTriggers << endl;
				  memory_[station][eventBCO] = new PxEvent(eventBCO);
				  event = memory_[station][eventBCO];
				}
			      else
				{
				  //cout << hex << "!! MATCH -> Delta: " << eventBCO - currentBCO_ << " -- Trigger number " << triggerNumber_ << " EventMonicelli BCO: " << eventBCO << dec << endl;
				  //extraDelta = (int)eventBCO - currentBCO_;
				  //std::cout << dec << extraDelta << std::endl;
				  //cout << "Matched BCO! For trigger Number " << triggerNumber << endl;
				  if (memory_[station].find(eventBCO) != memory_[station].end())
				    event = memory_[station][eventBCO];
				  else if (memory_[station].find(eventBCO - 1) != memory_[station].end())
				    event = memory_[station][eventBCO - 1];
				  else if (memory_[station].find(eventBCO + 1) != memory_[station].end())
				    event = memory_[station][eventBCO + 1];
				  else if (memory_[station].find(eventBCO - 2) != memory_[station].end())
				    event = memory_[station][eventBCO - 2];
				  else if (memory_[station].find(eventBCO + 2) != memory_[station].end())
				    event = memory_[station][eventBCO + 2];
				  else if (memory_[station].find(eventBCO - 3) != memory_[station].end())
				    event = memory_[station][eventBCO - 3];
				  else if (memory_[station].find(eventBCO + 3) != memory_[station].end())
				    event = memory_[station][eventBCO + 3];
				  else if (memory_[station].find(eventBCO - 4) != memory_[station].end())
				    event = memory_[station][eventBCO - 4];
				  else if (memory_[station].find(eventBCO + 4) != memory_[station].end())
				    event = memory_[station][eventBCO + 4];
				  else if (memory_[station].find(eventBCO - 5) != memory_[station].end())
				    event = memory_[station][eventBCO - 5];
				  else if (memory_[station].find(eventBCO + 5) != memory_[station].end())
				    event = memory_[station][eventBCO + 5];
				  else if (memory_[station].find(eventBCO - 6) != memory_[station].end())
				    event = memory_[station][eventBCO - 6];
				  else if (memory_[station].find(eventBCO + 6) != memory_[station].end())
				    event = memory_[station][eventBCO + 6];
				  else if (memory_[station].find(eventBCO - 7) != memory_[station].end())
				    event = memory_[station][eventBCO - 7];
				  else if (memory_[station].find(eventBCO + 7) != memory_[station].end())
				    event = memory_[station][eventBCO + 7];
				  else if (memory_[station].find(eventBCO - 8) != memory_[station].end())
				    event = memory_[station][eventBCO - 8];
				  else if (memory_[station].find(eventBCO + 8) != memory_[station].end())
				    event = memory_[station][eventBCO + 8];
				  else if (memory_[station].find(eventBCO - 9) != memory_[station].end())
				    event = memory_[station][eventBCO - 9];
				  else if (memory_[station].find(eventBCO + 9) != memory_[station].end())
				    event = memory_[station][eventBCO + 9];
				  else if (memory_[station].find(eventBCO - 10) != memory_[station].end())
				    event = memory_[station][eventBCO - 10];
				  else if (memory_[station].find(eventBCO + 10) != memory_[station].end())
				    event = memory_[station][eventBCO + 10];
				  //else    cout << "####Not assigned!!!! Station: " << station << endl;
				}
			  
			      if (event) {
				event->setTriggerEventBCO(eventBCO);
				event->setTriggerBCO(eventBCO);
				event->setTriggerNumber(triggerNumber_);
				event->addTriggerNumber(station, triggerNumber_);
				event->setCurrentBCO(eventBCO);
			      }
			    }
			}
		    
		    } //END of BLOCK
		}
	      cout << "If station 0, 1, 2 or 3 must match  Last Trigger #: " << dec << triggerNumber_ << "-------------" << endl;
	      //cout << " Number of unmatched triggers: " << dec << unmatchedTriggers << endl;
	    }
	}
    
      EventMonicelli *currentEvent = 0;
      //int k                 = 1;
      for (unsigned int s = 0; s < 4; s++)
	for (std::map<uint64_t, PxEvent *>::iterator it = memory_[s].begin(); it != memory_[s].end(); it++)
	  {
	    if (it->second->getTriggerNumber() != -1) //  &&  TriggerToRemove.find(it->second->getTriggerNumber())!=TriggerToRemove.end())
	      {
		PxEvent &anEvent = *(it->second);
		//for(unsigned int b=0; b<anEvent.getNumberOfHits(); b++,k++)
		//{
		// if(k>0 && k<100)
		// {
		//	 std::cout << __PRETTY_FUNCTION__ << "Station: " << anEvent.getDecodedHit(1).station  << " monicelli trigger number:  " << anEvent.getTriggerNumber() << " BCO: " << anEvent.getBCONumber() << std::endl;
		// }
		//}
		anEvent.addTriggerNumberToHits();
		//cout << "Trigger number: " << anEvent.getTriggerNumber() << " Number of triggers: " << anEvent.getNumberOfTriggers() << endl;
		anEvent.printTriggerInfo();
		currentEvent = &theRun.getEvent(anEvent.getTriggerNumber());
		//currentEvent->setTimestamp(currentTimestamp);
		currentEvent->setTriggerNumber(anEvent.getTriggerNumber());
		//	  currentEvent->setHardwareTriggerNumber(anEvent.getTriggerNumber());
		if ((int)currentEvent->getCurrentBCONumber() == -1)
		  currentEvent->setCurrentBCONumber(it->first);
		for (unsigned int e = 0; e < anEvent.getNumberOfHits(); e++)
		  {
		    currentEvent->addHit(anEvent.getDecodedHit(e));
		  }
	      }
	    delete it->second;
	  }
      memory_.clear();
      cout << __PRETTY_FUNCTION__ << "Done! Total number of events: " << theRun.getNumberOfEvents() << endl;
      //ofstream myfile;
    
      //std::string summary = "RunSummary.txt";
      //myfile.open (summary);
      //myfile << "Run number: " << runNumber << "\n";
      myfile << "END\n";
      myfile << "Total number of events: " << theRun.getNumberOfEvents() << "\n";
      myfile << "Total number of triggers: " << t1396counter << "\n";
      myfile.close();
    
      return 0;
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::writeMergedFiles(string filesDirectory, string fileName)
    {
      if (theRun.getNumberOfEvents() == 0)
	{
	  cout << __PRETTY_FUNCTION__ << "There are no events in this run!" << endl;
	  return -1;
	}
      //std::cout << "Run Number " << runNumber << std::endl;
      if (fileName == "")
	fileName = "Run" + runNumber_ + "_" + subrunNumber_ + "_Merged.dat";
      cout << "[" << __LINE__ << "]" << __PRETTY_FUNCTION__ << " Saving events in file: " << filesDirectory + fileName << endl;
    
      ofstream outFile((filesDirectory + fileName).c_str(), ios::binary);
      const uint64_t ones = -1;
      uint64_t triggerNumber;
      //    uint64_t timestamp;
      uint64_t data;
      uint64_t bco;
      for (map<unsigned int, EventMonicelli>::iterator eventsIt = theRun.getEvents().begin(); eventsIt != theRun.getEvents().end(); eventsIt++)
	{
	  outFile.write((char *)&ones, sizeof(uint64_t));
	  triggerNumber = eventsIt->second.getTriggerNumber();
	  bco           = eventsIt->second.getCurrentBCONumber();
	  outFile.write((char *)&triggerNumber, sizeof(uint64_t));
	  outFile.write((char *)&bco,           sizeof(uint64_t));
	  //if(triggerNumber < 100)
	  //	std::cout << "[" << __LINE__ << "] Trigger: " << dec << triggerNumber << " bco: " << hex << bco << dec << std::endl;
	  for (unsigned int hit = 0; hit < eventsIt->second.getNumberOfHits(); hit++)
	    {
	      data = eventsIt->second.getHit(hit).data;
	      outFile.write((char *)&data, sizeof(uint64_t));
	    }
	}
    
      outFile.close();
    
      return 0;
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::readMergedFiles(string fileName)
    {
      int maxNumberOfEvents = 10; //Irene, before it was 30
      cout << __PRETTY_FUNCTION__ << "\tReading file: " << fileName << endl;
      //Reading the file
      // 8 Bytes: all 1's
      // 8 Bytes: time stamp in computer seconds
      // All hits in event
      // the high byte reserved bits (63:56) indicate station number
      // the low 7 bytes of hit quad word are saved (55:0)
      //Quad Word:
      //reserved(63:52) | trigger number(51:32) | plaquette(31:29) | data0/debug1(28) | chipid(27:23) | row(22:16) | col(15:10) | pulseheight(9:0)
      bool writeOutFile = true;
      ofstream outputFile;
      if (writeOutFile)
	{
	  string outFileName = fileName;
	  outputFile.open(outFileName.replace(outFileName.find(".dat"), 4, ".txt").c_str());
	}
    
      bool timeStamp = false;
      uint64_t ones = -1;
      uint64_t data;
      unsigned int uniqueTrigger = 0;
    
      theRun.reset();
    
      ifstream inFile(fileName.c_str(), ios::binary);
      if (!inFile.is_open())
	{
	  cout << "Can't open file: " << fileName << endl;
	  return -1;
	}
      EventMonicelli anEvent;
      DataDecoder theDataDecoder;
    
      while (!inFile.eof())
	{
	  inFile.read((char *)(&data), sizeof(uint64_t));
	  cout << "Data: " << (data >> 32) << (data & 0xffffffff) << endl;
	  if (data == ones)
	    {
	      outputFile << "fffffffffffffffffffffffffffffffffffff" << endl;
	      timeStamp = true;
	      continue;
	    }
	  if (timeStamp || inFile.eof())
	    {
	      ++uniqueTrigger;
	      theDataDecoder.setTimestamp(data);
	      anEvent.setTimestamp(theDataDecoder.getTimestamp());
	      outputFile << dec << "Time: " << (data >> 32) << (data & 0xffffffff) << endl;
	      timeStamp = false;
	      //MP 01/14/2018
	      //if(true)
	      if (anEvent.getNumberOfHits() != 0)
		{
		  cout << __PRETTY_FUNCTION__ << "New event " << uniqueTrigger << " nHits =  " << anEvent.getNumberOfHits() << endl;
		  theRun.addEvent(anEvent);
		  anEvent.reset();
		  if (theRun.getNumberOfEvents() >= (unsigned int)maxNumberOfEvents)
		    break;
		}
	    }
	  else
	    {
	      theDataDecoder.setData(data);
	      if (theDataDecoder.isData())
		{
		  anEvent.setTriggerNumber(theDataDecoder.decodeTrigger());
		  //	      anEvent.setTriggerNumber(uniqueTrigger);
		  //	      anEvent.setHardwareTriggerNumber(theDataDecoder.decodeTrigger());
		  cout << __PRETTY_FUNCTION__ << "Trigger number: " << theDataDecoder.decodeTrigger() << endl;
		  cout << __PRETTY_FUNCTION__
		       << "data " << setw(8) << setfill('0') << hex
		       << ((data >> 32) & 0xffffffff)
		       << setw(8) << setfill('0')
		       << (data & 0xffffffff) << dec
		       << endl;
		  anEvent.addHit(theDataDecoder.decodeHit());
		  if (writeOutFile)
		    {
		      Hit tmpHit = theDataDecoder.decodeHit();
		      outputFile << dec
				 << "data: "
				 << setw(8) << setfill('0')
				 << ((data >> 32) & 0xffffffff)
				 << setw(8) << setfill('0')
				 << (data & 0xffffffff)
			//			     << " trig: "      << anEvent.getHardwareTriggerNumber()
				 << " trig: " << anEvent.getTriggerNumber()
				 << " station: " << tmpHit.station
				 << " plaq: " << tmpHit.plaq
				 << " chip: " << tmpHit.cid
				 << " row: " << tmpHit.row
				 << " col: " << tmpHit.col
				 << " adc: " << tmpHit.adc
				 << endl;
		    }
		}
	    }
	}
      inFile.close();
    
      if (writeOutFile)
	{
	  outputFile.close();
	}
      cout << __PRETTY_FUNCTION__ << "Final number of events: " << theRun.getNumberOfEvents() << endl;
      return 0;
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::readStationBuffers(void)
    {
      //Reading the file
      // 1 Byte (unsigend char) = Number of 8 bytes words in the following block (max 183)
      // 8 Bytes: the first word in a block is the time stamp in computer seconds
      // 8 Bytes * nWords in a block-1: hits
      unsigned char blockLengthC;
      int blockLength = -1;
      long long data;
      int trig;
      unsigned long long orderedData = 0;
      unsigned long long timeStamp;
    
      bool writeOutFile = true;
    
      ofstream outputFile;
      if (writeOutFile)
	{
	  outputFile.open("summary.txt");
	}
    
      DataDecoder theDataDecoder;
      Hit tmpHit;
      Status tmpStatus;
      struct tm *timeinfo;
      for (map<string, string>::iterator buffer = stationBuffers_.begin(); buffer != stationBuffers_.end(); buffer++)
	{
	  unsigned int bufferPos = 0;
	  //      for(unsigned int a=0; a<20; a++){
	  //        cout << __PRETTY_FUNCTION__ << hex << "Single: " << (unsigned int)buffer->second[a] << endl;
	  //      }
	  while (bufferPos < buffer->second.size())
	    {
	      //    for(int f=0; f<100; f++){
	      if (writeOutFile)
		{
		  outputFile << "-----------------------------------------------" << endl;
		}
	      blockLengthC = buffer->second[bufferPos];
	      bufferPos += sizeof(char);
	      blockLength = (int)blockLengthC;
	      if (writeOutFile)
		{
		  outputFile << dec << blockLength << endl;
		}
	      data = 0;
	      for (int nWords = 0; nWords < blockLength; nWords++)
		{
		  //	cout << __PRETTY_FUNCTION__ << "Buffer pos: " << bufferPos << endl;
		  if (nWords == 0)
		    {
		      theDataDecoder.setTimestamp(buffer->second.substr(bufferPos, sizeof(unsigned long long)));
		      timeStamp = theDataDecoder.getTimestamp();
		    
		      timeinfo = localtime((time_t *)(&timeStamp));
		      if (writeOutFile)
			{
			  outputFile << dec << "Time: " << (timeStamp >> 32) << (timeStamp & 0xffffffff)
				     << " which correspond to: " << asctime(timeinfo)
				     << "--- Packet number: " << theDataDecoder.getPacketNumber()
				     << endl;
			}
		    }
		  else
		    {
		      theDataDecoder.setData(buffer->second.substr(bufferPos, sizeof(unsigned long long)));
		      if (theDataDecoder.isData())
			{
			  data = theDataDecoder.getData();
			  tmpHit = theDataDecoder.decodeHit();
			  trig = theDataDecoder.decodeTrigger();
			  if (writeOutFile)
			    {
			      outputFile
				<< hex
				<< "data: "
				<< setw(8) << setfill('0')
				<< ((data >> 32) & 0xffffffff)
				<< setw(8) << setfill('0')
				<< (data & 0xffffffff)
				<< " ordered:"
				<< setw(8) << setfill('0')
				<< ((orderedData >> 32) & 0xffffffff)
				<< setw(8) << setfill('0')
				<< (orderedData & 0xffffffff)
				<< dec
				<< " Block: " << setw(3) << blockLength
				<< " Time: " << (timeStamp >> 32) << (timeStamp & 0xffffffff)
				<< " trig: " << setw(3) << trig
				<< " row: " << setw(2) << tmpHit.row
				<< " col: " << setw(2) << tmpHit.col
				<< " chip: " << tmpHit.cid
				<< " plaq: " << tmpHit.plaq
				<< " adc: " << setw(3) << tmpHit.adc
				<< " station: " << tmpHit.station
				<< endl;
			    }
			}
		      else
			{
			  data = theDataDecoder.getData();
			  tmpStatus = theDataDecoder.decodeStatus();
			  if (writeOutFile)
			    {
			      outputFile
				<< "Data: " << std::hex << (tmpStatus.data >> 32) << (tmpStatus.data & 0xffffffff) << std::dec
				<< " error : " << tmpStatus.error
				<< " repErr: " << tmpStatus.repErr
				<< " tokenInError: " << tmpStatus.tokenInError
				<< " tokenOutError: " << tmpStatus.tokenOutError
				<< " stError: " << tmpStatus.stError
				<< " plaquette: " << tmpStatus.plaquette
				<< " tokenIn: " << tmpStatus.tokenIn
				<< " tokenOut: " << tmpStatus.tokenOut
				<< " trigger: " << tmpStatus.trigger
				<< " ultraBlack: " << tmpStatus.ultraBlack
				<< endl;
			    }
			}
		    }
		  bufferPos += sizeof(unsigned long long);
		}
	    }
	
	  if (writeOutFile)
	    {
	      outputFile.close();
	    }
	
	  break;
	}
    
      return 0;
    }
  
    //-----------------------------------------------------------------------
    int FilesMerger::readFile(void)
    {
      //Reading the file
      // 1 Byte (unsigend char) = Number of 8 bytes words in the following block (max 183)
      // 8 Bytes: the first word in a block is the time stamp in computer seconds
      // 8 Bytes * nWords in a block-1: hits
      // All hits in event
      // the high byte reserved bits (63:56)
      // the low 7 bytes of hit quad word are saved (55:0)
      // Quad Word:
      // reserved(63:52) | trigger number(51:32) | plaquette(31:29) | data0/debug1(28) | chipid(27:23) | row(22:16) | col(15:10) | pulseheight(9:0)
      struct Hit
      {
	int detId;
	int station;
	int plaq;
	int cid;
	int row;
	int col;
	int adc;
      };
      unsigned char blockLengthC;
      int blockLength = -1;
      long long data;
      int dataSize = 8;
      //int                dataDebug;
      int trig;
      unsigned long long orderedData = 0;
      unsigned long long tmpData;
      long long timeStamp = 0;
      bool writeOutFile = true;
    
      ofstream outputFile;
      if (writeOutFile)
	{
	  outputFile.open("summary1.txt");
	}
    
      Hit tmpHit;
      ifstream file("test1.dat");
      if (!file.is_open())
	{
	  cout << __PRETTY_FUNCTION__ << "Can't open file test1.dat" << endl;
	  return -1;
	}
    
      while (!file.eof() && blockLength != 0)
	{
	  //  for(int f=0; f<100; f++){
	  if (writeOutFile)
	    {
	      outputFile << "-----------------------------------------------" << endl;
	    }
	  file.read((char *)&blockLengthC, sizeof(char));
	  blockLength = (int)blockLengthC;
	  if (writeOutFile)
	    {
	      outputFile << dec << blockLength << endl;
	    }
	  for (int nWords = 0; nWords < blockLength; nWords++)
	    {
	      file.read((char *)&data, sizeof(unsigned long long));
	      if (nWords == 0)
		{
		  timeStamp = data;
		  if (writeOutFile)
		    {
		      //	    outputFile << dec << "Time: " << (timeStamp >> 32) << (timeStamp & 0xffffffff) << endl;
		    }
		}
	      else
		{
		  orderedData = 0;
		  for (int byte = 0; byte < dataSize; byte++)
		    {
		      tmpData = ((data >> ((dataSize - byte - 1) * 8)) & 0xff) << (byte * 8);
		      orderedData += tmpData;
		    }
		  tmpHit.adc = orderedData & 0x3ff;
		  tmpHit.col = (orderedData >> 10) & 0x3f;
		  tmpHit.row = (orderedData >> 16) & 0x7f;
		  tmpHit.cid = (orderedData >> 23) & 0xf;
		  //dataDebug     = (orderedData >> 28) & 0x1;
		  tmpHit.plaq = (orderedData >> 29) & 0x7;
		  trig = (orderedData >> 32) & 0xfffff;
		  tmpHit.station = (orderedData >> 56) & 0xf;
		  if (writeOutFile)
		    {
		      outputFile
			//		   << hex
			//		   << "data: "
			//		   << setw(8) << setfill('0')
			//		   << ((data>>32) & 0xffffffff)
			//		   << setw(8) << setfill('0')
			//		   << (data & 0xffffffff)
			//		   << " ordered:"
			//		   << setw(8) << setfill('0')
			//		   << ((orderedData>>32) & 0xffffffff)
			//		   << setw(8) << setfill('0')
			//		   << (orderedData & 0xffffffff)
			<< dec
			<< "Block: " << setw(3) << blockLength
			<< " Time: " << (timeStamp >> 32) << (timeStamp & 0xffffffff)
			<< " trig: " << setw(3) << trig
			<< " row: " << setw(2) << tmpHit.row
			<< " col: " << setw(2) << tmpHit.col
			<< " chip: " << tmpHit.cid
			<< " plaq: " << tmpHit.plaq
			<< " adc: " << setw(3) << tmpHit.adc
			<< " station: " << tmpHit.station
			<< endl;
		    }
		}
	    }
	}
      if (writeOutFile)
	{
	  outputFile.close();
	}
      return 0;
    }
  
    //-----------------------------------------------------------------------
    std::string FilesMerger::BitString(const std::vector<uint32_t> &cData, uint32_t pOffset, uint32_t pWidth)
    {
      std::ostringstream os;
    
      for (uint32_t i = 0; i < pWidth; ++i)
	{
	  uint32_t pos = i + pOffset;
	  uint32_t cWordP = pos / 32;
	  uint32_t cBitP = pos % 32;
	
	  if (cWordP >= cData.size())
	    break;
	
	  //os << ((cbcData[cByteP] & ( 1 << ( 7 - cBitP ) ))?"1":"0");
	  os << ((cData[cWordP] >> (31 - cBitP)) & 0x1);
	}
    
      return os.str();
    }
  
    uint64_t FilesMerger::mortonEncode(uint32_t pEvenWord, uint32_t pOddWord)
    {
    
      // this method is specific to the d19c Firmware as it spits out the data
      // sensor by sensor, so even channels first and then odd channels
      // the process is called morton encoding and the code comes from here:
      // https://stackoverflow.com/questions/22101273/how-to-combine-two-16bit-words-into-one-32bit-word-bit-by-bit-efficiently
      //
      uint64_t A, B;
      A = ((pOddWord & 0x00000000FFFF0000ull) << 16) | (pOddWord & 0x000000000000FFFFull);
      A = ((A & 0x0000FF000000FF00ull) << 8) | (A & 0x000000FF000000FFull);
      A = ((A & 0xF0F0F0F0F0F0F0F0ull) << 4) | (A & 0x0F0F0F0F0F0F0F0Full);
      A = ((A & 0xCCCCCCCCCCCCCCCCull) << 2) | (A & 0x0333333333333333ull);
      A = ((A & 0xAAAAAAAAAAAAAAAAull) << 1) | (A & 0x5555555555555555ull);
    
      B = ((pEvenWord & 0x00000000FFFF0000ull) << 16) | (pEvenWord & 0x000000000000FFFFull);
      B = ((B & 0x0000FF000000FF00ull) << 8) | (B & 0x000000FF000000FFull);
      B = ((B & 0xF0F0F0F0F0F0F0F0ull) << 4) | (B & 0x0F0F0F0F0F0F0F0Full);
      B = ((B & 0xCCCCCCCCCCCCCCCCull) << 2) | (B & 0x0333333333333333ull);
      B = ((B & 0xAAAAAAAAAAAAAAAAull) << 1) | (B & 0x5555555555555555ull);
    
      uint64_t cResultWord = A | (B << 1);
      return cResultWord;
    }
  
    void FilesMerger::printWord(uint32_t w)
    {
      for (unsigned int i = 0; i < 32; ++i)
	{
	  printf("%d ", ((w << i) & 0x80000000) >> 31);
	}
      std::cout << std::bitset<32>(w) << std::endl;
      printf("\n");
    }
  
    void FilesMerger::printWord(unsigned long long w)
    {
      for (unsigned int i = 0; i < 64; ++i)
	{
	  printf("%llu ", ((w << i) & 0x80000000) >> 63);
	}
      printf("\n");
    }
  
    uint32_t FilesMerger::reverseWord(uint32_t w)
    {
    
      uint32_t r = w; // r will be reversed bits of w; first get LSB of w
      int s = 32 - 1; // extra shift needed at end
    
      for (w >>= 1; w; w >>= 1)
	{
	  r <<= 1;
	  r |= w & 1;
	  s--;
	}
      r <<= s; // shift when w's highest bits are zero
    
      return r;
    }

  } // end namespace merger
  
} // end namespace ssd
