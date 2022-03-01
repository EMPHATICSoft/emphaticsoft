///
/// \file    EditMenu.h
/// \brief   The edit pull down menu
/// \author  messier@indiana.edu
///
#ifndef EVDB_EDITMENU_H
#define EVDB_EDITMENU_H
#include "TQObject.h"
#include "RQ_OBJECT.h"
class TGMainFrame;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;

namespace evdb {
  ///
  /// \brief The edit pull down menu
  ///
  class EditMenu {
    RQ_OBJECT("evdb::EditMenu")

  public:
    EditMenu(TGMenuBar* menubar, TGMainFrame* mf);
    virtual ~EditMenu();

    void SetServices();

    void MenuSelect(int which);
    void WipeMenu(TGPopupMenu* m);

  private:
    // TGMainFrame*   fMainFrame;   ///< Main graphics frame - apparently not used in .cxx
    TGPopupMenu*   fEditMenu;    ///< The file menu
    TGPopupMenu*   fDrawingMenu; ///< Drawing options
    TGPopupMenu*   fExpMenu;     ///< Experiment services
    TGLayoutHints* fLayout;      ///< How to layout the menu
  };
}

#endif // EVDB_EDITMENU_H
////////////////////////////////////////////////////////////////////////
