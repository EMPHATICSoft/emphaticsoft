////////////////////////////////////////////////////////////////////////
/// \file  RootEnv.cxx
/// \brief Configure the ROOT environment
///
/// \version $Id: RootEnv.cxx,v 1.7 2011-10-31 14:41:40 greenc Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#include "EventDisplayBase/RootEnv.h"

#include <iostream>
#include <string>
#include <cstdlib>

#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TApplication.h"
#include "TRootApplication.h"
#include "TGClient.h"
#include "TRint.h"
#include "TSystem.h"
#include "TSysEvtHandler.h"
#include "TInterpreter.h"

#include "cetlib_except/exception.h"

namespace evdb{

  RootEnv::RootEnv(int /*argc*/, char** /*argv*/) 
  {
    //======================================================================
    // Setup the root environment for a program started with command line
    // options argc and argv
    //======================================================================
    TApplication* app = ROOT::GetROOT()->GetApplication();

    // ROOT::GetROOT() should initialize gROOT.    
    if(!gROOT)
      throw cet::exception("RootEnv") << "No ROOT global pointer"; 
    
    if (app == 0) {
      int    largc = 0;
      char** largv = 0;
      TRint* rapp = new TRint("TAPP",&largc, largv, 0, 0, kTRUE);
    
      //     std::string p = gSystem->BaseName(argv[0]); p+= " [%d] ";
      rapp->SetPrompt("evd [%d] ");
    }
    else {
      gROOT->SetBatch(kFALSE);
      if (gClient==0) {
        app->InitializeGraphics();
      }
    }

    this->SetStyle();
    this->SignalConfig();
    this->InterpreterConfig();
    this->LoadIncludes();
    this->LoadClasses();
  }

  //......................................................................

  RootEnv::~RootEnv() 
  {
    // ROOT takes care of the following delete so don't do it twice
    // if (fTheApp) { delete fTheApp; fTheApp = 0; }
  }

  //......................................................................

  int RootEnv::Run() 
  {
    //======================================================================
    // Turn control of the application over to ROOT's event loop
    //======================================================================
    TApplication* app = ROOT::GetROOT()->GetApplication();
    if (app) {
      app->Run(kFALSE); // kTRUE == "Return from run" request...
      return 1;
    }
    return 0;
  }

  //......................................................................

  void RootEnv::InterpreterConfig()
  {
    //======================================================================
    // Configure the root interpreter
    //======================================================================
    if (gInterpreter) { // gInterpreter from TInterpreter.h
      gInterpreter->SaveContext();
      gInterpreter->SaveGlobalsContext();
    }
  }

  //......................................................................

  void RootEnv::SignalConfig()
  {
    //======================================================================
    // Configure root's signale handlers
    //======================================================================
    return;
    if (gSystem) { // gSystem from TSystem.h
      // Reset ROOT's signal handling to the defaults...
      gSystem->ResetSignal(kSigBus,                  kTRUE);
      gSystem->ResetSignal(kSigSegmentationViolation,kTRUE);
      gSystem->ResetSignal(kSigSystem,               kTRUE);
      gSystem->ResetSignal(kSigPipe,                 kTRUE);
      gSystem->ResetSignal(kSigIllegalInstruction,   kTRUE);
      gSystem->ResetSignal(kSigQuit,                 kTRUE);
      gSystem->ResetSignal(kSigInterrupt,            kTRUE);
      gSystem->ResetSignal(kSigWindowChanged,        kTRUE);
      gSystem->ResetSignal(kSigAlarm,                kTRUE);
      gSystem->ResetSignal(kSigChild,                kTRUE);
      gSystem->ResetSignal(kSigUrgent,               kTRUE);
      gSystem->ResetSignal(kSigFloatingException,    kTRUE);
      gSystem->ResetSignal(kSigTermination,          kTRUE);
      gSystem->ResetSignal(kSigUser1,                kTRUE);
      gSystem->ResetSignal(kSigUser2,                kTRUE);
    }
  }

  //......................................................................

  void RootEnv::LoadIncludes()
  {
    //======================================================================
    // Load include files to make the root session more covenient
    //======================================================================
    TApplication* app = gROOT->GetApplication();
    if (app) {
      // Load a set of useful C++ includes.
      // app->ProcessLine("#include <iostream>"); // Root gets this one itself
      app->ProcessLine("#include <iomanip>");
      app->ProcessLine("#include <string>");
    
      // Load experiment include files
      // Have to be careful here, not every experiment uses
      // SRT, so don't try to load the SRT macro paths 
      // if SRT variables aren't defined.
      std::string mp = gROOT->GetMacroPath();
      std::string ip;
      const char* p;
      bool srtPrivate = false;
      bool srtPublic  = false;
      p = gSystem->Getenv("SRT_PRIVATE_CONTEXT");
      if (p) {
	srtPrivate = true;
	mp += ":";
	mp += p;
	mp += ":";
	mp += p;
	mp += "/macros";
	ip += " -I";
	ip += p;

	std::string dip = ".include ";
	dip += gSystem->Getenv("SRT_PRIVATE_CONTEXT");
	gROOT->ProcessLine(dip.c_str());
      }
      p = gSystem->Getenv("SRT_PUBLIC_CONTEXT");
      if (p) {
	srtPublic = true;
	mp += ":";
	mp += p;
	mp += "/macros";
	ip += " -I";
	ip += p;

	std::string dip = ".include ";
	dip += gSystem->Getenv("SRT_PUBLIC_CONTEXT");
	gROOT->ProcessLine(dip.c_str());
      }

      if(srtPublic || srtPrivate){
	gROOT->SetMacroPath(mp.c_str());
	gSystem->SetIncludePath(ip.c_str());
      }
    }
  }

  //......................................................................

  void RootEnv::LoadClasses()
  {
    //======================================================================
    // Load classes to make the root session more covenient
    //======================================================================
    if (gROOT) {
      gROOT->LoadClass("TGeometry",   "Graf3d");
      gROOT->LoadClass("TTree",       "Tree");
      gROOT->LoadClass("TMatrix",     "Matrix");
      gROOT->LoadClass("TMinuit",     "Minuit");
      gROOT->LoadClass("TPostScript", "Postscript");
      gROOT->LoadClass("TCanvas",     "Gpad");
      gROOT->LoadClass("THtml",       "Html");
    }
  }

  //......................................................................

  void RootEnv::SetStyle() 
  {
    gROOT->SetStyle("Plain");
  
    // Set Line Widths
    gStyle->SetFrameLineWidth(1);
    gStyle->SetFuncWidth(1);
    gStyle->SetHistLineWidth(1);
  
    gStyle->SetFuncColor(2);
    gStyle->SetGridColor(18);
    gStyle->SetGridStyle(1);
    // SetGridWidth expects a short, the default value is 1, just use that
    gStyle->SetGridWidth();
  
    // Set margins -- I like to shift the plot a little up and to the
    // right to make more room for axis labels
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetPadBottomMargin(0.36);
    gStyle->SetPadRightMargin(0.03);
    gStyle->SetPadLeftMargin(0.10);
  
    // Set fonts
    gStyle->SetTextFont(132);
    gStyle->SetLabelFont(132,"XYZ");
    gStyle->SetStatFont(132);
    gStyle->SetTitleFont(132,"XYZ");
  
    gStyle->SetStatFontSize(0.07);
    gStyle->SetTitleFontSize(0.07);
    gStyle->SetLabelSize(0.07,"XYZ");
    gStyle->SetTitleSize(0.07,"XYZ");
    gStyle->SetTextSize(0.07);
  
    gStyle->SetStatW(0.19);
    gStyle->SetStatX(0.90);
    gStyle->SetStatY(0.90);
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
  
    // Set tick marks and turn off grids
    gStyle->SetNdivisions(510,"XYZ");
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
  
    // Set paper size for life in the US
    gStyle->SetPaperSize(TStyle::kUSLetter);
    gStyle->SetPalette(1);
  
    // Force this style on all histograms
    gROOT->ForceStyle();
  }

}// namespace
////////////////////////////////////////////////////////////////////////
