////////////////////////////////////////////////////////////////////////
/// \file  ObjListCanvas.cxx
/// \brief A base class for defining a detector display
///
/// \version $Id: ObjListCanvas.cxx,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
///
/// Revised 16-Apr-2009 seligman@nevis.columbia.edu
/// Allow dynamic re-sizing of the canvas if the user drags the window's
/// size box.

#include "EventDisplayBase/ObjListCanvas.h"

#include "TCanvas.h"
#include "TGFrame.h"
#include "TGLayout.h"
// #include "TRootEmbeddedCanvas.h"

#include <iostream>
#include <string>

namespace evdb{

  //......................................................................

  ///
  /// Perform the basic setup for a drawing canvas
  ///
  ObjListCanvas::ObjListCanvas(TGMainFrame* mf) 
  {
    TGDimension sz;     // Size of the main frame
  
    sz           = mf->GetSize();
    fXsize       = sz.fWidth  - 10; // Leave small margin on left and right 
    fYsize       = sz.fHeight - 58; // Leave small margin on top and bottom
    fAspectRatio = (float)fYsize/(float)fXsize;
  
    // This frame is apparently for holding the buttons on the top; it's
    // not used for anything else.
    fFrame  = new TGCompositeFrame(mf, 60, 60, kHorizontalFrame); 

    // Define a layout for placing the canvas within the frame.
    fLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
				kLHintsExpandY, 5, 5, 5, 5);
  
    // Careful about clashes with root's naming of canvases. Make a name
    // like "tpcEVDCanvas" using the print tag
    std::string name(this->PrintTag());
    name += "evdb::ObjListCanvas";

    /*
    // Create the embedded canvas within the main ROOT graphics frame.
    fEmbCanvas = new TRootEmbeddedCanvas(name.c_str(), mf, fXsize, fYsize,
    kSunkenFrame, 0);
    mf->AddFrame(fEmbCanvas, fLayout);
    */
    mf->AddFrame(fFrame);
  
    // Extract the graphical Canvas from the embedded canvas.  The user
    // will do most of their drawing in this.
    //  fCanvas = fEmbCanvas->GetCanvas();

    //  this->Update();
  }

  //......................................................................

  void ObjListCanvas::Connect() 
  {
    // Make connections for drawing and printing
    // IoModule::Instance()->Connect("NewEvent()",
    // "evdb::ObjListCanvas",this,"Draw()");
    Printable::AddToListOfPrintables(this->Description(),this);
  }

  //......................................................................

  ObjListCanvas::~ObjListCanvas() 
  {
    // IoModule::Instance()->Disconnect(0,this,0);
    //  delete fEmbCanvas;
    delete fLayout;    
    delete fFrame;
  }

  //......................................................................

  void ObjListCanvas::Print(const char* /*f*/) { /*fCanvas->Print(f);*/ }

}//namespace
////////////////////////////////////////////////////////////////////////

