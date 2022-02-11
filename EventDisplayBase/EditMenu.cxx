///
/// \file   EditMenu.cxx
/// \brief  The edit pull down menu
/// \author messier@indiana.edu
///
#include "EventDisplayBase/EditMenu.h"
#include "TGMenu.h"
#include "EventDisplayBase/ServiceTable.h"

namespace evdb
{
  EditMenu::EditMenu(TGMenuBar* menubar,
                     TGMainFrame* /*mf*/)
  {
    fEditMenu = new TGPopupMenu(gClient->GetRoot());

    fDrawingMenu = new TGPopupMenu();
    fExpMenu     = new TGPopupMenu();

    fEditMenu->AddPopup("Configure &Drawing",             fDrawingMenu);
    fEditMenu->AddPopup("Configure &Experiment Services", fExpMenu);

    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    menubar->AddPopup("&Edit",fEditMenu,fLayout);
  }

  //......................................................................

  EditMenu::~EditMenu()
  {
    if (fLayout)      { delete fLayout;      fLayout      = nullptr; }
    if (fEditMenu)    { delete fEditMenu;    fEditMenu    = nullptr; }
    if (fExpMenu)     { delete fExpMenu;     fExpMenu     = nullptr; }
    if (fDrawingMenu) { delete fDrawingMenu; fDrawingMenu = nullptr; }
  }

  //....................................................................

  void EditMenu::WipeMenu(TGPopupMenu* m)
  {
    for (unsigned int i=0;;++i) {
      TGMenuEntry* me = m->GetEntry(i);
      if (me) m->DeleteEntry(i);
      else break;
    }
  }

  //....................................................................

  void EditMenu::SetServices()
  {
    this->WipeMenu(fDrawingMenu);
    this->WipeMenu(fExpMenu);

    const ServiceTable& st = ServiceTable::Instance();

    unsigned int i;
    for (i=0; i<st.fServices.size(); ++i) {
      const ServiceTableEntry& ste = st.fServices[i];
      if (ste.fCategory==kDRAWING_SERVICE) {
        fDrawingMenu->AddEntry(ste.fName.c_str(), i);
      }
      else if (ste.fCategory==kEXPERIMENT_SERVICE) {
        fExpMenu->AddEntry(ste.fName.c_str(), i);
      }
      fEditMenu->Connect("Activated(Int_t)",
                         "evdb::EditMenu",
                         this,
                         "MenuSelect(int)");
    }
  }

  //......................................................................

  void EditMenu::MenuSelect(int i)
  {
    ServiceTable::Instance().Edit(i);
  }

}// namespace
////////////////////////////////////////////////////////////////////////
