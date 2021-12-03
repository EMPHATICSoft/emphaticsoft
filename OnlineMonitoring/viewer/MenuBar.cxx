#include "OnlineMonitoring/viewer/MenuBar.h"
#include "TApplication.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/util/IPC.h"
using namespace om;

enum _menu {
  // File menu entries
  kFILE_MENU_OPEN,
  kFILE_MENU_OPENREF,
  kFILE_MENU_PRINT,
  kFILE_MENU_QUIT,
  // Update menu entries
  kUPDATE_MENU_RESETLOCKS,
  // Help menu entries
  kHELP_MENU_ABOUT
};

//......................................................................

MenuBar::MenuBar(TGWindow* w) :
  TGMenuBar(w, Layout::fMenuBarSizeX, Layout::fMenuBarSizeY, kHorizontalFrame)
{
  fFile = new TGPopupMenu(gClient->GetRoot());
  fFile->AddEntry("Open",           kFILE_MENU_OPEN);
  fFile->AddEntry("Open reference", kFILE_MENU_OPENREF);
  fFile->AddSeparator();
  fFile->AddEntry("Print",          kFILE_MENU_PRINT);
  fFile->AddSeparator();
  fFile->AddEntry("Quit",           kFILE_MENU_QUIT);
  fFile->Associate(this);
  this->AddPopup("&File", fFile, 0);

  fUpdate = new TGPopupMenu(gClient->GetRoot());
  fUpdate->AddEntry("Reset Locks", kUPDATE_MENU_RESETLOCKS);
  fUpdate->Associate(this);
  this->AddPopup("&Update", fUpdate, 0);

  fHelp = new TGPopupMenu(gClient->GetRoot());
  fHelp->AddEntry("About", kHELP_MENU_ABOUT);
  this->AddPopup("&Help", fHelp, 0);
}

//......................................................................

MenuBar::~MenuBar() { }

//......................................................................

Bool_t MenuBar::ProcessMessage(Long_t msg, Long_t parm1, Long_t p __attribute__((unused))) 
{
  if ((GET_MSG(msg)==kC_COMMAND) && (GET_SUBMSG(msg)==kCM_MENU)) {
    switch (parm1) {
    case kFILE_MENU_OPEN:         this->FileOpen();          break;
    case kFILE_MENU_OPENREF:      this->FileOpenReference(); break;
    case kFILE_MENU_PRINT:        this->FilePrint();         break;
    case kFILE_MENU_QUIT:         this->FileQuit();          break;
    case kUPDATE_MENU_RESETLOCKS: this->ResetLocks();        break;
    case kHELP_MENU_ABOUT:        this->HelpAbout();         break;
    }
  }
  return kTRUE;
}

//......................................................................

int MenuBar::FileOpen() { return 0; }

//......................................................................

int MenuBar::FileOpenReference() { return 0; }

//......................................................................

int MenuBar::FilePrint()
{
  GUIModel::Instance().SendPrint();
  return 0;
}

//......................................................................

int MenuBar::FileQuit() { gApplication->Terminate(0); return 0;}

//......................................................................

int MenuBar::ResetLocks()
{
  const std::string& src =
    GUIModel::Instance().Data().fHistogramSource.substr(0,4);

  if (src.find(".shm")==src.npos) return 0;

  std::string hndl = src.substr(0,4);

  IPC ipc(om::kIPC_CLIENT,hndl.c_str());
  ipc.ResetAllLocks();

  return 0;
}

//......................................................................

int MenuBar::HelpAbout() { return 0; }

////////////////////////////////////////////////////////////////////////
