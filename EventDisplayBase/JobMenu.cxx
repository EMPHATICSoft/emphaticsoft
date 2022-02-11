////////////////////////////////////////////////////////////////////////
/// \file  JobMenu.cxx
/// \brief The job pull down menu
///
/// \version $Id: JobMenu.cxx,v 1.9 2011-05-26 13:30:34 brebel Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <string>
#include <iostream>
#include "TROOT.h"
#include "TSystem.h"
#include "TGMsgBox.h"
#include "TGMenu.h"
#include "TGLayout.h"
#include "TGFileDialog.h"
#include "TTimer.h"

#include "EventDisplayBase/JobMenu.h"
#include "EventDisplayBase/evdb.h"
#include "EventDisplayBase/EventDisplay.h"

namespace evdb{

  // Define ID codes for the actions on the file menu
  enum {
    kM_JOB_OPENXML            = 99001,
    kM_JOB_EDITCONFIG         = 99100,
    kM_JOB_RESETJOB           = 99003,
    kM_JOB_EDITSERVICE        = 99200
  };

  //......................................................................

  JobMenu::JobMenu(TGMenuBar* menubar, TGMainFrame* /*mf*/) 
  //: fMainFrame(mf)
  {
    //======================================================================
    // Build the help menu
    //======================================================================
    fJobMenu = new TGPopupMenu(gClient->GetRoot());
    fLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  
    fConfigMenu  = new TGPopupMenu();
    fServiceMenu = new TGPopupMenu();
  
    // Create the list of functions. Associate each which a command code
    fJobMenu->AddEntry("&Load job",  kM_JOB_OPENXML);
    fJobMenu->AddEntry("&Reset Job", kM_JOB_RESETJOB);
    fJobMenu->AddSeparator();
    fJobMenu->AddPopup("&Configure Module",  fConfigMenu);
    fJobMenu->AddSeparator();
    fJobMenu->AddPopup("&Configure Service", fServiceMenu);
  
    // Connect only the fJobMenu - the other pop-ups that are embedded in 
    // it do not get their own signals for some reason known only to the 
    // ROOT developers
    fJobMenu->Connect("Activated(Int_t)",
		      "evdb::JobMenu",
		      this,
		      "HandleMenu(int)");

    // Attach the menu to the menu bar
    menubar->AddPopup("&Job",fJobMenu,fLayout);
  }

  //......................................................................

  JobMenu::~JobMenu() 
  {
    //======================================================================
    // Delete the job menu
    //======================================================================
    if (fLayout)  { delete fLayout;  fLayout  = 0; }
    if (fJobMenu) { delete fJobMenu; fJobMenu = 0; }
  }

  //......................................................................

  void JobMenu::SetWorkers(const std::vector<std::string>& w)
  {
    // Wipe out the existing menus and lists
    for (unsigned int i=0;;++i) {
      TGMenuEntry* m = fConfigMenu->GetEntry(i);
      if (m) fConfigMenu->DeleteEntry(i);
      else   break;
    }
  
    // Rebuild the list
    for (unsigned int i=0; i<w.size(); ++i) {
      fConfigMenu->AddEntry(w[i].c_str(), kM_JOB_EDITCONFIG+i);
    }
  }

  //......................................................................

  void JobMenu::SetServices(const std::vector<std::string>& w)
  {
    // Wipe out the existing menus and lists
    for (unsigned int i=0;;++i) {
      TGMenuEntry* m = fConfigMenu->GetEntry(i);
      if (m) fConfigMenu->DeleteEntry(i);
      else   break;
    }
  
    // Rebuild the list
    for (unsigned int i=0; i<w.size(); ++i) {
      fServiceMenu->AddEntry(w[i].c_str(), kM_JOB_EDITSERVICE+i);
    }
  }

  //......................................................................

  void JobMenu::HandleMenu(int menu) 
  {
    //======================================================================
    // Take care of menu events
    //======================================================================
    switch(menu) {
    case kM_JOB_OPENXML: 
      this->OpenJob(); 
      break;
    case kM_JOB_RESETJOB: 
      this->ResetJob(); 
      break;
    default:
      if(menu >= kM_JOB_EDITCONFIG && 
	 menu < kM_JOB_EDITSERVICE)       this->EditConfig(menu);
      else if(menu >= kM_JOB_EDITSERVICE) this->EditService(menu);
      break;
    }
  }

  //......................................................................

  void JobMenu::EditConfig(int /*i*/)
  {  
    // make sure to subtract off the offset used to distinguish between
    // module and service configs
    // art::ServiceHandle<evdb::EventDisplay> evd;
    // evd->EditWorkerParameterSet(i-kM_JOB_EDITCONFIG);
  }

  //......................................................................

  void JobMenu::EditService(int /*i*/)
  {  
    // make sure to subtract off the offset used to distinguish between
    // module and service configs
    // art::ServiceHandle<evdb::EventDisplay> evd;
    // evd->EditServiceParameterSet(i-kM_JOB_EDITSERVICE);
  }

  //......................................................................

  int JobMenu::OpenJob() 
  {
    // not every experiment uses SRT, so be sure to have
    // a non-SRT option for the directory
    static TString dir("./");
    char* dirchar = getenv("SRT_PRIVATE_CONTEXT");
    if(dirchar) dir = dirchar;    
    const char* filetypes[] = {
      "Configuration Files", "*.fcl",
      0,                     0
    };
  
    TGFileInfo finfo;
    finfo.fIniDir    = StrDup(dir.Data());
    finfo.fFileTypes = filetypes;

    new TGFileDialog(gClient->GetRoot(), 
		     gClient->GetRoot(),
		     kFDOpen,
		     &finfo);
    if (finfo.fFilename == 0) return 0;

    return 0;
  }

  //......................................................................

  void JobMenu::ResetJob() 
  {
    // jobc::Stack::Instance().CleanUp();
  }

}// namespace
////////////////////////////////////////////////////////////////////////
