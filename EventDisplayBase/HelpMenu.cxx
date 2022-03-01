////////////////////////////////////////////////////////////////////////
/// \file  HelpMenu.cxx
/// \brief Implementation of the help pull down menu
///
/// \todo This needs a lot of work if it is to actually provide help...
///
/// \version $Id: HelpMenu.cxx,v 1.2 2012-09-20 21:38:32 greenc Exp $
/// \author  messier@indiaa.edu
////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <string>
#include <iostream>

#include "TROOT.h"
#include "TSystem.h"
#include "TGMsgBox.h"
#include "TGMenu.h"
#include "TGLayout.h"

#include "EventDisplayBase/HelpMenu.h"
#include "EventDisplayBase/evdb.h"

namespace evdb{

  // Define ID codes for the actions on the file menu
  enum {
    kM_HELP_CONTENTS,
    kM_HELP_RELEASENOTES,
    kM_HELP_ABOUT
  };

  //......................................................................

  HelpMenu::HelpMenu(TGMenuBar* menubar, TGMainFrame* mf) :
    fMainFrame(mf)
  {
    //======================================================================
    // Build the help menu
    //======================================================================
    fHelpMenu = new TGPopupMenu(gClient->GetRoot());
    fLayout   = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    // Create the list of functions. Associate each which a command code
    fHelpMenu->AddEntry("&Contents",      kM_HELP_CONTENTS);
    fHelpMenu->AddEntry("&Release Notes", kM_HELP_RELEASENOTES);
    fHelpMenu->AddSeparator();
    fHelpMenu->AddEntry("&About",         kM_HELP_ABOUT);

    // fHelpMenu->Connect("Activated(Int_t)",
    // "evdb::HelpMenu",this,"HandleMenu(int)");
  
    // Attach the menu to the menu bar
    menubar->AddPopup("&Help",fHelpMenu,fLayout);
  }

  //......................................................................

  HelpMenu::~HelpMenu() 
  {
    //======================================================================
    // Delete the help menu
    //======================================================================
    if (fLayout)   { delete fLayout;   fLayout   = 0; }
    if (fHelpMenu) { delete fHelpMenu; fHelpMenu = 0; }
  }

  //......................................................................

  void HelpMenu::HandleMenu(int menu) 
  {
    //======================================================================
    // Take care of menu events
    //======================================================================
    switch(menu) {
    case kM_HELP_CONTENTS:     this->Contents();     break;
    case kM_HELP_RELEASENOTES: this->ReleaseNotes(); break;
    case kM_HELP_ABOUT:        this->About();        break;
    default: this->NoImpl("??"); break;
    }
  }

  //......................................................................

  int HelpMenu::Contents() 
  {
    //======================================================================
    // Start a help browser
    //======================================================================
    this->NoImpl("Contents");
    return 0;
  }

  //......................................................................

  int HelpMenu::ReleaseNotes() 
  {
    //======================================================================
    // Print information about this release of the event display
    //======================================================================
    const char* releaseNotes = "This is a pre-release version of event display";
    new TGMsgBox(evdb::TopWindow(), evdb::TopWindow(),
		 "Release notes",releaseNotes,kMBIconExclamation);
    return 0;
  }

  //......................................................................

  int HelpMenu::About() 
  {
    //======================================================================
    // Pop open a window containing information about versions of things
    // used in the event display.
    //======================================================================
    std::string about;
  
    about =  "MIPP Event Display\n\n";

    about += "  Version: "; 
    about += "$Id: HelpMenu.cxx,v 1.2 2012-09-20 21:38:32 greenc Exp $";
    about += "\n";

    about += "  ";
    about += gSystem->GetBuildArch();
    about += "\n";

    about += "  ";
    about += gSystem->GetBuildNode();
    about += "\n";

    about += "  Based on ROOT version: ";
    about += gROOT->GetVersion();
    about += "\n";
  
    new TGMsgBox(evdb::TopWindow(), fMainFrame,
		 "Release notes",about.c_str(),kMBIconExclamation);
    return 0;
  }

  //......................................................................

  int HelpMenu::NoImpl(const char* method) 
  {
    std::string s;
    s = "Sorry action '"; s += method; s+= "' is not implemented.\n";
    // Why isn't this a memory leak? Dunno, but its seems the TG classes
    // are all managed by TGClient which takes care of deletion
    new TGMsgBox(evdb::TopWindow(), fMainFrame,
		 "No implementation",s.c_str(),kMBIconExclamation);
    return 0;
  }

}//namespace
////////////////////////////////////////////////////////////////////////
