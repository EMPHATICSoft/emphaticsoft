////////////////////////////////////////////////////////////////////////
/// \file  FileMenu.h
/// \brief The file pull down menu
///
/// \version $Id: FileMenu.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_FILEMENU_H
#define EVDB_FILEMENU_H
#include "TQObject.h"
#include "RQ_OBJECT.h"

class TGMainFrame;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;

namespace evdb {
  class FileMenu {
    RQ_OBJECT("evdb::FileMenu")

  public:
    FileMenu(TGMenuBar* menubar, TGMainFrame* mf);
    virtual ~FileMenu();
  
    // slots
    void HandleFileMenu(int menu);
  
  private:
    int Open();
    int Save();
    int SaveAs();
    int Close();
    int Print();
    int Quit();
    int NoImpl(const char* m);
    
  private:
    TGMainFrame*   fMainFrame;  ///< Main graphics frame
    TGPopupMenu*   fFileMenu;   ///< The file menu
    TGLayoutHints* fLayout;     ///< How to layout the menu

  };
}
#endif // EVDB_FILEMENU_H
////////////////////////////////////////////////////////////////////////
