////////////////////////////////////////////////////////////////////////
/// \file  WindowMenu.h
/// \brief Pull down menu for launching new windows
///
/// \version $Id: WindowMenu.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_WINDOWMENU_H
#define EVDB_WINDOWMENU_H
#include "TQObject.h"
#include "RQ_OBJECT.h"

class TGMainFrame;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;

namespace evdb {
  class WindowMenu {
    RQ_OBJECT("evdb::WindowMenu")
    
  public:
    WindowMenu(TGMenuBar* menubar, TGMainFrame* mf);
    virtual ~WindowMenu();
    
    // slots
    void HandleMenu(int menu);
    
  private:
    int NoImpl(const char* m);
    int No3DViewer();
    
  private:
    TGPopupMenu*   fWindowMenu; // The file menu
    TGLayoutHints* fLayout;     // How to layout the menu
  };
}

#endif // WINDOWMENU_H
////////////////////////////////////////////////////////////////////////
