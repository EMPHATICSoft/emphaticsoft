////////////////////////////////////////////////////////////////////////
/// \file  HelpMenu.h
/// \brief The help pull down menu
///
/// \version $Id: HelpMenu.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_HELPMENU_H
#define EVDB_HELPMENU_H
#include "TQObject.h"
#include "RQ_OBJECT.h"
class TGMainFrame;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;

namespace evdb {
  class HelpMenu {
    RQ_OBJECT("evdb::HelpMenu")

  public:
    HelpMenu(TGMenuBar* menubar, TGMainFrame* mf);
    virtual ~HelpMenu();
    
    // slots
    void HandleMenu(int menu);
    
  private:
    int Contents();
    int ReleaseNotes();
    int About();
    int NoImpl(const char* m);
    
  private:
    TGMainFrame*   fMainFrame; // Main graphics frame
    TGPopupMenu*   fHelpMenu;  // The file menu
    TGLayoutHints* fLayout;    // How to layout the menu
  };
}
#endif // EVDB_FILEMENU_H
////////////////////////////////////////////////////////////////////////
