/////////////////////////////////////////////////////////////////////////////
/// \file    Display3DView.cxx
/// \brief   The "main" event display view that most people will want to use
/// \author  messier@indiana.edu
/// \version $Id: Display3DView.cxx,v 1.4 2011-10-31 15:32:18 greenc Exp $
/////////////////////////////////////////////////////////////////////////////
#include "TCanvas.h"
#include "TGLViewer.h"

#include "art/Framework/Principal/Event.h"

#include "EventDisplay/Display3DView.h"
#include "EventDisplay/Display3DPad.h"

namespace evd
{
  //...............................................................
  Display3DView::Display3DView(TGMainFrame* mf) : evdb::Canvas(mf)
  {
    evdb::Canvas::fCanvas->cd();
    fDisplay3DPad = new Display3DPad("fDisplay3DPad","3D Display",
				     0.0, 0.0, 1.0, 1.0, "");
    fDisplay3DPad->Draw();
  
    evdb::Canvas::fCanvas->Update();
  }

  //......................................................................
  Display3DView::~Display3DView() 
  {
  }
    
  //......................................................................
  void Display3DView::Draw(const char* /*opt*/) 
  {
    fDisplay3DPad->Draw();
    evdb::Canvas::fCanvas->Update();

    // Eventually it would be nice to embed this into one of the normal
    // canvases, but for now just open ROOT's OpenGL viewer

    static TGLViewer* fGLViewer = 0;
    if (fGLViewer==0) {
      fGLViewer = (TGLViewer*)fDisplay3DPad->Pad()->GetViewer3D("ogl");
      fGLViewer->PreferLocalFrame();
      fGLViewer->SetResetCamerasOnUpdate(0);
    }
    else {
      fDisplay3DPad->Pad()->GetViewer3D()->PadPaint(fDisplay3DPad->Pad());
    }

  }

} // end namespace evd
////////////////////////////////////////////////////////////////////////
