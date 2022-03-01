////////////////////////////////////////////////////////////////////////
/// \file  JobMenu.h
/// \brief The job pull down menu
///
/// \version $Id: JobMenu.h,v 1.2 2011-05-26 13:30:34 brebel Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_JOBMENU_H
#define EVDB_JOBMENU_H
#include <vector>
#include "TObject.h"
#include "TQObject.h"
#include "RQ_OBJECT.h"

class TGMainFrame;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;

namespace evdb {
  /// The job pull dow menu
  class JobMenu 
  {
    RQ_OBJECT("evdb::JobMenu")
    
  public:
    JobMenu(TGMenuBar* menubar, TGMainFrame* mf);
    virtual ~JobMenu();
    void SetWorkers(const std::vector<std::string>& w);
    void SetServices(const std::vector<std::string>& w);

    // slots
    void HandleMenu(int menu);
    void EditConfig(int cfg);
    void EditService(int cfg);
    
  private:
    int  OpenJob();
    void ResetJob();
    
  private:
    // TGMainFrame*   fMainFrame;   ///< Main graphics frame - never used in .cxx file except to set it
    TGPopupMenu*   fJobMenu;     ///< The file menu
    TGPopupMenu*   fConfigMenu;  ///< The module configuration menu
    TGPopupMenu*   fServiceMenu; ///< The user service configuration menu
    TGLayoutHints* fLayout;      ///< How to layout the menu
  };
}
#endif // EVDB_FILEMENU_H
////////////////////////////////////////////////////////////////////////
