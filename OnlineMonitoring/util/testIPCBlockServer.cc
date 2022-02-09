#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/IPCBlock.h"
#include <iostream>
#include "TH1F.h"
#include "TH2F.h"

static TH1F* h1 = new TH1F("histo1ds","histo1ds",5,0,5);
static TH1F* h2 = new TH1F("histo1db","histo1db",10,0,10);
static TH2F* h3 = new TH2F("histo2ds","histo1ds",4,0,4,4,0,4);
static TH2F* h4 = new TH2F("histo2db","histo1db",10,0,10,10,0,10);

static const std::string h1s("histo1ds");
static const std::string h2s("histo1db");
static const std::string h3s("histo2ds");
static const std::string h4s("histo2db");

class MyIPC : public emph::onmon::IPC 
{
public:
  MyIPC(int m) : emph::onmon::IPC(m,"IPCT") { }
private:
  TH1F* FindTH1F(const char* nm) {
    if (h1s==nm) return h1;
    if (h2s==nm) return h2;
    return 0;
  }
  TH2F* FindTH2F(const char* nm) {
    if (h3s==nm) return h3;
    if (h4s==nm) return h4;
    return 0;
  }
};

//......................................................................

int main(void)
{
  MyIPC ipc(emph::onmon::kIPC_SERVER);

  std::string name;
  for (unsigned int i=0;;++i) {

    //
    // Look for requests for histograms and try to satisfy them
    //
    ipc.HandleRequests();

    //
    // Keep filling histograms
    //
    h1->Fill(i%5);
    h2->Fill(i%10);
    h3->Fill(i%4,i%4);
    h4->Fill(i%10,i%10);
    
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////
