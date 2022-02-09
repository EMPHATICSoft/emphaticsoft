#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/IPCBlock.h"
#include <iostream>
#include <cstring>
extern "C" {
#include "unistd.h"  
}
#include "TH1F.h"
#include "TH2F.h"

void dumph1(TH1F* h) 
{
  if (h==0) {
    std::cout << "TH1F is NULL" << std::endl;
    return;
  }
  int i;
  std::cout << "GOT TH1F " 
	    << h->GetName() << " : " << h->GetNbinsX() 
	    << std::endl;
  for (i=0; i<h->GetNbinsX(); ++i) {
    std::cout << i << ": " << h->GetBinContent(i+1) << std::endl;
  }
}

//......................................................................

void dumph2(TH2F* h) 
{
  if (h==0) {
    std::cout << "TH2F is NULL" << std::endl;
    return;
  }
  int i, j;
  std::cout << "GOT TH2F " 
	    << h->GetName() << " : " << h->GetNbinsX() 
	    << std::endl;
  for (i=0; i<h->GetNbinsX(); ++i) {
    for (j=0; j<h->GetNbinsY(); ++j) {
      std::cout << i << "," << j << ": " << h->GetBinContent(i+1,j+1) << " ";
    }
    std::cout << std::endl;
  }
}

//......................................................................

int main(void)
{
  emph::onmon::IPC ipc(emph::onmon::kIPC_CLIENT,"IPCT");

  TH1F* h1;
  TH2F* h2;
  
  h1 = ipc.RequestTH1F("histo1ds");
  dumph1(h1);
  if (h1) delete h1;

  h1 = ipc.RequestTH1F("histo1db");
  dumph1(h1);
  if (h1) delete h1;

  h2 = ipc.RequestTH2F("histo2ds");
  dumph2(h2);
  if (h2) delete h2;

  h2 = ipc.RequestTH2F("hsito2db");
  dumph2(h2);
  if (h2) delete h2;

  return 0;
}
////////////////////////////////////////////////////////////////////////
