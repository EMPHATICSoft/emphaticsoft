///
/// \file  EventDisplay_service.cc
/// \brief The interactive event display
///
/// \version $Id: EventDisplay.cxx,v 1.25 2012-03-05 20:37:14 brebel Exp $
/// \author  messier@indiana.edu
///
#include "EventDisplayBase/EventDisplay.h"
// ROOT includes
#include "TROOT.h"
#include "TApplication.h"
#include "TText.h"
#include "TCanvas.h"
// ART includes framework includes
#include "art_root_io/RootInput.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "cetlib_except/exception.h"
// Local includes
#include "EventDisplayBase/ServiceTable.h"
#include "EventDisplayBase/DisplayWindow.h"
#include "EventDisplayBase/Canvas.h"
#include "EventDisplayBase/RootEnv.h"
#include "EventDisplayBase/EventHolder.h"
#include "EventDisplayBase/NavState.h"

#include <wordexp.h>

namespace evdb
{
  //
  // Build this as soon as the library is loaded to ensure that our
  // interactive session is started before other services that might use
  // ROOT get a chance to make their own.
  //
  static evdb::RootEnv gsRootEnv(0,0);

  //......................................................................

  class testCanvas1 : public evdb::Canvas
  {
  public:
    testCanvas1(TGMainFrame* mf) : evdb::Canvas(mf) {
      evdb::Printable::AddToListOfPrintables(this->PrintTag(),this);
    }
    ~testCanvas1() {
      evdb::Printable::RemoveFromListOfPrintables(this);
    }
    const char* Description() const { return "Test Canvas 1"; }
    const char* PrintTag()    const { return "Test1"; }
    void Draw(const char* /* opt */) {
      static TText* t = new TText(0.5,0.5,"-");
      static int count = 0;
      char buff[256];
      sprintf(buff,"%d",count);
      mf::LogWarning("EventDisplayBase") << buff;
      t->SetText(0.5,0.5,buff);
      t->Draw();
      ++count;
      fCanvas->Update();
    }
  };

  //......................................................................
  // Uncomment this static method for testing purposes
  // static evdb::Canvas* mk_canvas1(TGMainFrame* mf) {
  //   return new testCanvas1(mf);
  // }

  //......................................................................

  EventDisplay::EventDisplay(fhicl::ParameterSet const& pset,
                             art::ActivityRegistry& reg) :
  fAutoPrintCount(0)
  {
    //   evdb::DisplayWindow::Register("Test1","Test display #1",600,900,mk_canvas1);
    //   evdb::DisplayWindow::OpenWindow(0);

    this->reconfigure(pset);

    reg.sPostBeginJob.watch       (this, &EventDisplay::postBeginJob);
    reg.sPostBeginJobWorkers.watch(this, &EventDisplay::postBeginJobWorkers);
    reg.sPreProcessEvent.watch    (this, &EventDisplay::preProcessEvent);
    reg.sPostProcessEvent.watch   (this, &EventDisplay::postProcessEvent);
  }

  //......................................................................

  void EventDisplay::reconfigure(fhicl::ParameterSet const& pset)
  {
    fAutoAdvanceInterval = pset.get<unsigned int>("AutoAdvanceInterval" );
    fAutoPrintMax        = pset.get<int         >("AutoPrintMax",     0 );
    fAutoPrintPattern    = pset.get<std::string >("AutoPrintPattern", "");
    fEchoPrint           = pset.get<bool        >("EchoPrint",        false);
    fEchoPrintFile       = pset.get<std::string >("EchoPrintFile",    "$HOME/evt_echo.gif");
    // Sanitize filename: root's OK with env variables, straight system
    // calls are not.  So, force a substitution of env. variables in the
    // filename so we can do atomic-write "renames" later using a tmp file
    if (fEchoPrint) {
      wordexp_t p;
      char** w;
      wordexp( fEchoPrintFile.c_str(), &p, 0 );
      w = p.we_wordv;
      fEchoPrintFile = std::string(*w);
      // the tempfile has to end with the same extension (eg, ".gif") as
      // the specified filename.  root's printing takes the format of the file
      // from that extension.  So, we have to construct a name with the same
      // path, and extension: need to insert a "tmp" before the final .gif
      // Sp, simply grab the file extension and stick it back on the end
      std::string::size_type idx;
      std::string extension;
      idx = fEchoPrintFile.rfind('.');
      if(idx != std::string::npos) {
        extension = fEchoPrintFile.substr(idx);
        fEchoPrintTempFile = std::string(*w) + ".tmp" + extension;
      } else {
        // No extension found, can't do this
        fEchoPrint = false;
        fEchoPrintTempFile = "";
        mf::LogWarning("EventDisplayBase")
        << "No file extension given to EchoPrintFile "
        << fEchoPrintFile
        << " so cannot determine file format, disabling EchoPrint\n";
      }
      wordfree(&p);
    } else {
      fEchoPrintTempFile = "";
    }
  }

  //......................................................................

  void EventDisplay::postBeginJobWorkers(art::InputSource* input_source,
                                         std::vector<art::Worker*> const&)
  {
    fInputSource = input_source;
  }

  //......................................................................

  void EventDisplay::postBeginJob()
  {
    DisplayWindow::SetServicesAll();
  }

  //......................................................................

  void EventDisplay::preProcessEvent(art::Event const & evt, art::ScheduleContext)
  {
    evdb::DisplayWindow::SetRunEventAll(evt.id().run(), evt.id().event());
  }

  //......................................................................

  void EventDisplay::postProcessEvent(art::Event const& evt, art::ScheduleContext)
  {
    // stuff the event into the holder
    evdb::EventHolder *holder = evdb::EventHolder::Instance();
    holder->SetEvent(&evt);

    evdb::DisplayWindow::DrawAll();

    if(fAutoPrintMax == 0){
      TApplication* app = gROOT->GetApplication();

      // Hold here for user input from the GUI...
      app->Run(kTRUE);
    }

    //
    // Apply edits to any services that may have been reconfigured
    //
    ServiceTable::Instance().ApplyEdits();

    if(fAutoPrintMax > 0){
      ++fAutoPrintCount;
      std::map<std::string, Printable*>& ps = Printable::GetPrintables();
      for(std::map<std::string,Printable*>::iterator it = ps.begin(); it != ps.end(); ++it){
        Printable* p = it->second;
          // Ensure the format string is well-formed
        if(fAutoPrintPattern.find("%s") == std::string::npos)
          throw cet::exception("EventDisplay") << "Cannot find AutoPrintPattern"
          << " format for %s";
        if(fAutoPrintPattern.find("%d") == std::string::npos)
          throw cet::exception("EventDisplay") << "Cannot find AutoPrintPattern"
          << " format for %d";
          // png doesn't seem to work for some reason
        p->Print(TString::Format(fAutoPrintPattern.c_str(), p->PrintTag(), evt.event()));
      }
      if(fAutoPrintCount >= fAutoPrintMax) exit(0);
    }

    // if fEchoPrint is set, do so
    if (fEchoPrint){
      std::map<std::string, Printable*>& ps = Printable::GetPrintables();
      for(std::map<std::string,Printable*>::iterator it = ps.begin(); it != ps.end(); ++it){
        Printable* p = it->second;
          // lack of more parameters to Print() call means use the file format
          // that's specified by the file name extension
        p->Print(fEchoPrintTempFile.c_str());
      }
        // move temporary file to final file.  This makes the creation of the
        // newly printed file close to atomic
      int result;
      result=rename(fEchoPrintTempFile.c_str(),fEchoPrintFile.c_str());
      if (result==0)
        MF_LOG_DEBUG("EventDisplayBase") << fEchoPrintTempFile
        << " tempfile successfully renamed to "
        << fEchoPrintFile;
      else
        mf::LogWarning("EventDisplayBase") << "Error renaming file "
                                           << fEchoPrintTempFile
                                           << " to " << fEchoPrintFile
                                           << " " << strerror(errno) <<"\n";
    }

    art::RootInput* rootInput = dynamic_cast<art::RootInput*>(fInputSource);

    if(!rootInput && NavState::Which() != kSEQUENTIAL_ONLY){
      NavState::Set(kSEQUENTIAL_ONLY);
      mf::LogWarning("EventDisplayBase")
      << "Random access for the EventDisplay requires a RootInput source for proper operation.\n"
      << "You do not have a RootInput source so only sequential access works.\n";
    }


    // Figure out where to go in the input stream from here
    switch (NavState::Which()) {
    case kSEQUENTIAL_ONLY: break;
    case kNEXT_EVENT: {
      // Contrary to appearances, this is *not* a NOP: it ensures run and
      // subRun are (re-)read as necessary if we've been doing random
      // access. Come the revolution ...
      //
      // 2011/04/10 CG.
      art::EventID eid(evt.id().run(),evt.id().subRun(),evt.id().event()+1);
      if(rootInput) rootInput->seekToEvent(eid);
      break;
    }
    case kPREV_EVENT: {
      art::EventID eid(evt.id().run(),evt.id().subRun(),evt.id().event()-1);
      if(rootInput) rootInput->seekToEvent(eid);
      break;
    }
    case kRELOAD_EVENT: {
      if(rootInput) rootInput->seekToEvent(evt.id());
      break;
    }
    case kGOTO_EVENT: {
      art::EventID id(art::SubRunID::invalidSubRun(art::RunID(NavState::TargetRun())), NavState::TargetEvent());
      if(rootInput){
        if (!rootInput->seekToEvent(id)) { // Couldn't find event
          mf::LogWarning("EventDisplayBase") << "Unable to find "
                                             << id
                                             << " -- reloading current event.";
            // Reload current event.
          rootInput->seekToEvent(evt.id());
        }
      }// end if not a RootInput
      break;
    }
    default: {
      throw art::Exception(art::errors::LogicError)
      << "EvengtDisplay in unhandled state "
      << NavState::Which()
      << ".\n";
    }
    } // end switch statement

  }

}//namespace


////////////////////////////////////////////////////////////////////////
