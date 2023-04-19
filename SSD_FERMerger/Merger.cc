#include "SSD_FERMerger/FilesMerger.h"
#include <map>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;
using namespace ssd;
using namespace ssd::merger;

const bool writeOutFile = true;

//////////////////////////////////////////////////////////////////////////////////
//                    WARNING
// We are assuming:
// 1) The file names must be like this: Run1_ . That is they start with Run, then there is the run number and then an underscore (int FilesMerger::makeFilesList(string runNumber,std::string dir))
// 2) The 3 stations must be named dut, downstream and upstream                                                                  (int FilesMerger::makeFilesList(string runNumber,std::string dir))
// 3) The files are contained in 3 subdirectories called CC_DUT/Burst Archive, CC_DS/Burst Archive, CC_US/Burst Archive          (int FilesMerger::makeFilesList(string runNumber,std::string dir))
// 4)The station numbers are dut => station = 4, upstream => station = 2 and downstream => station = 0                           (int FilesMerger::merge(void))
//
///////////////////////////////////////////////////////////////////////////////////

//const string pixelDutName        = "dut";
//const string pixelDigitalName    = "digital";
//const string pixelDownstreamName = "downstream";
//const string pixelUpstreamName   = "upstream";
const string fer0Name = "FER0";
const string fer1Name = "FER1";
const string fer2Name = "FER2";
const string fer3Name = "FER3";
const string fer4Name = "FER4";
//const string RD53MiddlewareName  = "RD53Middleware";
//const string OTEastName     	   = "fed000";
//const string OTWestName     	   = "fed001";
//const string nim                 = "Nim";
const int MaxSubRun = 1000;

int main(int argc, char **argv)
{

  if (getenv("MERGER_INPUT_DIR") == 0 || getenv("MERGER_OUTPUT_DIR") == 0)
  {
    cout << __PRETTY_FUNCTION__ << "You need to source setup.sh" << endl;
    exit(EXIT_FAILURE);
  }

  const string inputFilesDir = getenv("MERGER_INPUT_DIR");
  const string outputFilesDir = getenv("MERGER_OUTPUT_DIR");

  int firstRunNumber, lastRunNumber;

  if (argc < 2 || argc > 3)
  {
    cout << __PRETTY_FUNCTION__ << "\tUsage: ./Merger runNumber or ./Merger firstRunNumber lastRunNumber" << endl;
    exit(EXIT_FAILURE);
  }
  else if (argc == 2)
  {
    firstRunNumber = atoi(argv[1]);
    lastRunNumber = atoi(argv[1]);
  }
  else if (argc == 3)
  {
    firstRunNumber = atoi(argv[1]);
    lastRunNumber = atoi(argv[2]);
  }

  //////
  FilesMerger theFilesMerger;
  vector<string> stationNames;
  //stationNames.push_back(pixelDownstreamName);
  stationNames.push_back(fer0Name);
  stationNames.push_back(fer1Name);
  stationNames.push_back(fer2Name);
  stationNames.push_back(fer3Name);
  stationNames.push_back(fer4Name);
  //stationNames.push_back(fer1Name);
  //stationNames.push_back(RD53MiddlewareName);

  theFilesMerger.setStationNames(stationNames);

  for (int runNumber = firstRunNumber; runNumber <= lastRunNumber; runNumber++)
  {
	  cout << __PRETTY_FUNCTION__ << "\tMerging run " << runNumber << endl;
	  for (int subrunNumber = 1; subrunNumber <= MaxSubRun; subrunNumber++){
		  if (theFilesMerger.makeFilesList(runNumber, subrunNumber, inputFilesDir) == -1)
			  continue;
		  cout << __PRETTY_FUNCTION__ << "\tMerging subrun " << subrunNumber << endl;
		  theFilesMerger.makeStationBuffers();
		  theFilesMerger.merge();
		  theFilesMerger.writeMergedFiles(outputFilesDir, "");
	  }
  }

  return EXIT_SUCCESS;
}
