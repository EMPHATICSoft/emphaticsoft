#ifndef OM_MENU_BAR_H
#define OM_MENU_BAR_H
#include "TGMenu.h"
class TGPopupMenu;

namespace emph { 
  namespace onmon {
    class MenuBar : public TGMenuBar {
    public:
      MenuBar(TGWindow* w);
      ~MenuBar();

      Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t p);

      int FileOpen();
      int FileOpenReference();
      int FilePrint();
      int FileQuit();

      int ResetLocks();

      int HelpAbout();

    private:
      TGPopupMenu* fFile;
      TGPopupMenu* fUpdate;
      TGPopupMenu* fHelp;
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
