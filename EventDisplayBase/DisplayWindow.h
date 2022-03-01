///
/// \file  DisplayWindow.h
/// \brief A window, dressed with menus and buttons, that displays
/// dector information
///
/// \version $Id: DisplayWindow.h,v 1.4 2011-05-26 13:30:34 brebel Exp $
/// \author  messier@indiana.edu
///
#ifndef EVDB_DISPLAYWINDOW_H
#define EVDB_DISPLAYWINDOW_H
#include <vector>
#include <string>
#include "TQObject.h"
#include "RQ_OBJECT.h"


class TGMainFrame;
namespace art { 
  class Worker; 
}
namespace evdb {
  class MenuBar;
  class ButtonBar;
  class StatusBar;
  class Canvas;
  typedef Canvas* (*CanvasCreator_t)(TGMainFrame* mf);
}

//......................................................................

namespace evdb {
  /// An event display window
  class DisplayWindow {
    RQ_OBJECT("evdb::DisplayWindow")
    
  public:
    static void Register(const char* name,
			 const char* description,
			 unsigned int h,
			 unsigned int w,
			 CanvasCreator_t creator);
    static const std::vector<std::string>& Names();
    static int   OpenWindow(int type=0);
    static void  SetRunEventAll(int run, int event);
    static void  SetServicesAll();
    static void  DrawAll(const char* opt=0);

  public:
    DisplayWindow(int window=0);
    virtual ~DisplayWindow();

    virtual void Draw(const char* opt="");
    virtual void CloseWindow();
    void         Raise();
    void         SetRunEvent(int run, int event);
    void         SetServices();

  private:
    TGMainFrame* fMain;      ///< Main window
    MenuBar*     fMenuBar;   ///< Top menu bar
    ButtonBar*   fButtonBar; ///< Top button bar
    StatusBar*   fStatusBar; ///< Status bar running along the bottom
    Canvas*      fDisplay;   ///< Display of detector event information
  };
}
#endif
////////////////////////////////////////////////////////////////////////
