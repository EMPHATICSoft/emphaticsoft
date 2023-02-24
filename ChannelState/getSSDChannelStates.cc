#include <string>
#include "ChannelState/ChannelState.h"

int main(int argc, char **argv)
{
  if (argc != 3) {
    std::cout << "Usage: getSSDChannelStates [run] [subrun]" << std::endl;
  }

  int runNum = atoi(argv[1]);
  int subrunNum = atoi(argv[2]);
  
  emph::ChannelState* chanState = new emph::ChannelState();
  chanState->SetLoadSSDFromDB(true);
  chanState->SetCondbURL("https://dbdata2vm.fnal.gov:9443/emphatic_con_prod/app/");
  chanState->SetRunSubrun(runNum,subrunNum);

  auto stateMap = chanState->StateMap(emph::geo::SSD);

  std::cout << "stateMap.size() = " << stateMap.size() << std::endl;
  
  std::cout << "All other SSD channels are assumed to be good." << std::endl;

  delete chanState;

  return 0;

}
