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
  chanState->SetDataType("data");
  chanState->SetCondbURL("https://dbdata2vm.fnal.gov:9443/emphatic_con_prod/app");
  chanState->SetRunSubrun(runNum,subrunNum);

  auto stateMap = chanState->StateMap(emph::geo::SSD);

  int nDead = 0; 
  int nHot = 0;
  for (auto state : stateMap) {
    emph::ChannelStateType st = static_cast<emph::ChannelStateType>(state.second);
    if (st != emph::ChannelStateType::kGood) {
      if (st == emph::ChannelStateType::kHot) ++nHot;
      if (st == emph::ChannelStateType::kDead) ++nDead;
    }
  }
  std::cout << "N(Dead) = " << nDead << std::endl;
  std::cout << "N(Hot) = " << nHot << std::endl;
  std::cout << "N(Good) = " << (stateMap.size() - nDead - nHot) << std::endl;

  delete chanState;

  return 0;

}
