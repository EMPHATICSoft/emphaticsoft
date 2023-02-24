#include <string>
#include "RunHistory/RunHistory.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    std::cout << "Usage: emph_runHistory [run]" << std::endl;
  }

  int runNum = atoi(argv[1]);
  
  runhist::RunHistory rh(runNum);
  rh.SetQEURL("https://dbdata0vm.fnal.gov:9443/QE/emphatic_prod/");

  float beamMom = rh.BeamMom();
  std::string targetStr = rh.Target();

  std::cout << "Run " << runNum << ": " << beamMom << " GeV/c, " << targetStr << " target"
	    << std::endl;

  return 0;

}
