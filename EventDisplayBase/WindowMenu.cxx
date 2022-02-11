///
/// \file  WindowMenu.cxx
/// \brief Implement the pull down window menu
///
/// \version $Id: WindowMenu.cxx,v 1.4 2011-07-11 19:35:05 brebel Exp $
/// \author  messier@indiana.edu
///
#include <cstdlib>
#include <string>
#include <iostream>
#include "TGMsgBox.h"
#include "TGMenu.h"
#include "TGLayout.h"

#include "EventDisplayBase/WindowMenu.h"
#include "EventDisplayBase/evdb.h"
#include "EventDisplayBase/DisplayWindow.h"
#include "EventDisplayBase/ListWindow.h"
#include "EventDisplayBase/ScanWindow.h"

namespace evdb{

//......................................................................

  WindowMenu::WindowMenu(TGMenuBar* menubar, TGMainFrame* /*mf*/)
  {
    fWindowMenu = new TGPopupMenu(gClient->GetRoot());
    fLayout     = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    // Create the list of functions. Associate each which a command code
    unsigned int cnt = 2;
    unsigned int i=0;
    const std::vector<std::string>& names = DisplayWindow::Names();
    for (; i<names.size(); ++i) {
      fWindowMenu->AddEntry(names[i].c_str(), cnt+i);
    }
    fWindowMenu->AddSeparator();

    // Create the list of functions. Associate each which a command code
    const std::vector<std::string>& lnames = ListWindow::Names();
    for (unsigned int j=0; j<lnames.size(); ++j) {
      fWindowMenu->AddEntry(lnames[j].c_str(), cnt+i+j);
    }
    fWindowMenu->AddSeparator();
    fWindowMenu->AddEntry("&Scan Window",  0);
    fWindowMenu->AddSeparator();
    //fWindowMenu->AddEntry("&MC Information",   -2);
    fWindowMenu->AddEntry("&ROOT Browser",    1);
    fWindowMenu->Connect("Activated(Int_t)",
			 "evdb::WindowMenu",this,"HandleMenu(int)");
  
    // Attach the menu to the menu bar
    menubar->AddPopup("&Window",fWindowMenu,fLayout);
  }

  //......................................................................

  WindowMenu::~WindowMenu() 
  {
    if (fLayout)     { delete fLayout;     fLayout     = 0; }
    if (fWindowMenu) { delete fWindowMenu; fWindowMenu = 0; }
  }

  //......................................................................

  void WindowMenu::HandleMenu(int menu) 
  {
    if( menu == 0 ){
      new ScanWindow();
      return;
    }
    //
    // Check if the menu has selected one of the named "user" windows
    //
    if (menu-2 < (int)DisplayWindow::Names().size()) {
      int aok = DisplayWindow::OpenWindow(menu-2);
      if (aok<0) this->NoImpl("Error openning requested window");
      return;
    }
  }

  //......................................................................

  int WindowMenu::NoImpl(const char* method) 
  {
    std::string s;
    s = "Sorry action '"; s += method; s+= "' is not implemented.\n";
    // Why isn't this a memory leak? Dunno, but its seems the TG classes
    // are all managed by TGClient which takes care of deletion
    new TGMsgBox(evdb::TopWindow(),
		 evdb::TopWindow(),
		 "No implementation",s.c_str(),kMBIconExclamation);
    return 0;
  }

  //......................................................................

  int WindowMenu::No3DViewer()
  {
    new TGMsgBox(evdb::TopWindow(), 
		 evdb::TopWindow(),
		 "Not for this view",
		 "This display does not implement a 3D viewer",
		 kMBIconExclamation);
    return 0;
  }

} // namespace
////////////////////////////////////////////////////////////////////////
