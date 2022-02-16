////////////////////////////////////////////////////////////////////////
// $Id: MenuBar.cxx,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
//
// The list of menus running across the top of a display
//
// messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "TGLayout.h"
#include "TGMenu.h"

#include "EventDisplayBase/MenuBar.h"
#include "EventDisplayBase/FileMenu.h"
#include "EventDisplayBase/EditMenu.h"
#include "EventDisplayBase/WindowMenu.h"
#include "EventDisplayBase/HelpMenu.h"

namespace evdb{

  //......................................................................

  MenuBar::MenuBar(TGMainFrame* frame) 
  {
    int padleft   = 0;
    int padright  = 0;
    int padtop    = 1;
    int padbottom = 1;

    // Create the menu bar
    fMenuBar = new TGMenuBar(frame, 1, 1, kHorizontalFrame);
    fLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 
				 padleft, padright, padtop, padbottom);
  
    // Add the menus to it
    fFileMenu   = new FileMenu  (fMenuBar, frame);
    fEditMenu   = new EditMenu  (fMenuBar, frame);
    fWindowMenu = new WindowMenu(fMenuBar, frame);
    fHelpMenu   = new HelpMenu  (fMenuBar, frame);

    // Add the menu bar to the main window frame
    frame->AddFrame(fMenuBar,fLayout);
  }

  //......................................................................

  MenuBar::~MenuBar() {
    if (fHelpMenu)   { delete fHelpMenu;   fHelpMenu   = 0; }
    if (fWindowMenu) { delete fWindowMenu; fWindowMenu = 0; }
    if (fEditMenu)   { delete fEditMenu;   fEditMenu   = 0; }
    if (fFileMenu)   { delete fFileMenu;   fFileMenu   = 0; }
  }
}// namespace
////////////////////////////////////////////////////////////////////////
