////////////////////////////////////////////////////////////////////////
/// \file  MenuBar.h
/// \brief The pull down menu bar
///
/// \version $Id: MenuBar.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_MENUBAR_H
#define EVDB_MENUBAR_H
#include "RQ_OBJECT.h"
class TGMainFrame;
class TGLayoutHints;
class TGMenuBar;
namespace evdb {
  class MainWindow;
  class FileMenu;
  class EditMenu;
  class WindowMenu;
  class JobMenu;
  class HelpMenu;
}

namespace evdb {
  /// The pull down menu bar
  class MenuBar {
    RQ_OBJECT("evdb::MenuBar")

  public:
    MenuBar(TGMainFrame* frame);
    virtual ~MenuBar();
    
  public:
    TGMenuBar*     fMenuBar;    ///< Menu bar across top of application
    TGLayoutHints* fLayout;     ///< Layout of menu bar
    FileMenu*      fFileMenu;   ///< File menu
    EditMenu*      fEditMenu;   ///< Edit menu
    WindowMenu*    fWindowMenu; ///< Window menu
    HelpMenu*      fHelpMenu;   ///< Help menu
  };
}
#endif // EVDB_MENUBAR_H
////////////////////////////////////////////////////////////////////////
