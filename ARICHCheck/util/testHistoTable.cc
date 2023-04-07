#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/HistoData.h"
#include <cassert>
#include <iostream>

int main(void)
{
  emph::onmon::HistoTable& t = emph::onmon::HistoTable::Instance();

  t.ReadFile("test-histos.csv", emph::onmon::kEMPH);

  assert(t["Test1D"].      fName=="Test1D");
  assert(t["Test2D"].      fName=="Test2D");
  assert(t["TestRegExp_1"].fName=="TestRegExp_*");
  assert(t["TestRegExp_2"].fName=="TestRegExp_*");
  assert(t["TestEMPH"].    fNx==10);

  std::cout << "All tests passed." << std::endl;
  return 0;
}
////////////////////////////////////////////////////////////////////////
