///
/// \file  FileMenu.cxx
/// \brief The file pull down menu
///
/// \version $Id: FileMenu.cxx,v 1.3 2011-04-11 20:13:56 messier Exp $
/// \author  messier@indiana.edu
///
#include "EventDisplayBase/FileMenu.h"
#include "EventDisplayBase/PrintDialog.h"
#include "EventDisplayBase/evdb.h"

#include <cstdlib>
#include <string>
#include <iostream>

#include "TGMsgBox.h"
#include "TGMenu.h"
#include "TGLayout.h"
#include "TGFileDialog.h"

namespace evdb{
  // Define ID codes for the actions on the file menu
  enum {
    kM_FILE_OPEN,
    kM_FILE_SAVE,
    kM_FILE_SAVEAS,
    kM_FILE_CLOSE,
    kM_FILE_PRINT,
    kM_FILE_QUIT
  };
  
  
  //......................................................................
  
  FileMenu::FileMenu(TGMenuBar* menubar, TGMainFrame* mf) :
    fMainFrame(mf)
  {
    fFileMenu = new TGPopupMenu(gClient->GetRoot());
    fLayout   = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
    
    // Create the list of functions. Associate each which a command code
    fFileMenu->AddEntry("&Open File...", kM_FILE_OPEN);
    fFileMenu->AddEntry("&Save",         kM_FILE_SAVE);
    fFileMenu->AddEntry("S&ave as...",   kM_FILE_SAVEAS);
    fFileMenu->AddSeparator();
    fFileMenu->AddEntry("&Print",        kM_FILE_PRINT);
    fFileMenu->AddSeparator();
    fFileMenu->AddEntry("&Quit",         kM_FILE_QUIT);
    
    fFileMenu->Connect("Activated(Int_t)",
		       "evdb::FileMenu",this,"HandleFileMenu(int)");
    
    // Attach the menu to the menu bar
    menubar->AddPopup("&File",fFileMenu,fLayout);
  }
  
  //......................................................................
  
  FileMenu::~FileMenu() 
  {
    if (fLayout)   { delete fLayout;   fLayout   = 0; }
    if (fFileMenu) { delete fFileMenu; fFileMenu = 0; }
  }
  
  //......................................................................
  
  void FileMenu::HandleFileMenu(int menu) 
  {
    switch(menu) {
    case kM_FILE_OPEN:   this->Open();   break;
    case kM_FILE_SAVE:   this->Save();   break;
    case kM_FILE_SAVEAS: this->SaveAs(); break;
    case kM_FILE_PRINT:  this->Print();  break;
    case kM_FILE_QUIT:   this->Quit();   break;
    default: this->NoImpl("??"); break;
    }
  }
  
  //......................................................................
  
  int FileMenu::Open() 
  {
    static TString dir(""); // Static so that directory remembers where
    // we were last time
    const char* filetypes[] = { "ROOT files",  "*.root",
				"All files",   "*",
				0,             0 };
    TGFileInfo finfo;
    
    finfo.fFileTypes = filetypes;
    finfo.fIniDir    = StrDup(dir.Data());
    
    new TGFileDialog(evdb::TopWindow(),evdb::TopWindow(),kFDOpen, &finfo);
    
    return 1;
  }
  
  //......................................................................
  
  int FileMenu::Save() 
  {
    this->NoImpl("Save");
    return 0;
  }
  
  //......................................................................
  
  int FileMenu::SaveAs() 
  {
    this->NoImpl("SaveAs");
    return 0;
  }
  
  //......................................................................
  
  int FileMenu::Print() 
  {
    new PrintDialog();
    return 1;
  }
  
  //......................................................................
  
  int FileMenu::Quit()
  {
    std::cout << "\n";
    exit(0);
  }
  
  //......................................................................
  
  int FileMenu::NoImpl(const char* method) 
  {
    std::string s;
    s = "Sorry action '"; s += method; s+= "' is not implemented.\n";
    // Why isn't this a memory leak? Dunno, but its seems the TG classes
    // are all managed by TGClient which takes care of deletion
    new TGMsgBox(evdb::TopWindow(), fMainFrame,
		 "No implementation",s.c_str(),kMBIconExclamation);
    return 0;
  }
}// end evdb namespace
////////////////////////////////////////////////////////////////////////
