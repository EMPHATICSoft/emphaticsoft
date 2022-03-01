////////////////////////////////////////////////////////////////////////
/// \file  ListWindow.h
/// \brief A window, dressed with menus and buttons, that displays
/// dector information
///
/// \version $Id: ListWindow.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  jpaley@anl.gov
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_LISTWINDOW_H
#define EVDB_LISTWINDOW_H
#include <vector>
#include <string>
#include "TQObject.h"
#include "RQ_OBJECT.h"
class TGMainFrame;
namespace evdb {
  class MenuBar;
  class ButtonBar;
  class StatusBar;
  class ObjListCanvas;
  typedef ObjListCanvas* (*ObjListCanvasCreator_t)(TGMainFrame* mf);
}

//......................................................................

namespace evdb {
  /// An event display window
  class ListWindow {
    RQ_OBJECT("evdb::ListWindow")
    
  public:
    static void Register(const char* name,
			 const char* description,
			 unsigned int h,
			 unsigned int w,
			 ObjListCanvasCreator_t creator);
    static const std::vector<std::string>& Names();
    static int OpenWindow(int type=0);
    
    ListWindow(int window=0);
    virtual ~ListWindow();

    virtual void Draw(const char* opt="");
    virtual void CloseWindow();
    void         Raise();
    
  private:
    TGMainFrame* fMain;      ///< Main window
    MenuBar*     fMenuBar;   ///< Top menu bar
    ButtonBar*   fButtonBar; ///< Top button bar
    StatusBar*   fStatusBar; ///< Status bar running along the bottom
    ObjListCanvas* fDisplay;   ///< Display of detector event information
  };
}
#endif
////////////////////////////////////////////////////////////////////////
