/////////////////////////////////////////////////////////////////////////////
/// \file    Display3DView.cxx
/// \brief   The "main" event display view that most people will want to use
/// \author  messier@indiana.edu
/// \version $Id: Display3DView.cxx,v 1.4 2011-10-31 15:32:18 greenc Exp $
/////////////////////////////////////////////////////////////////////////////
#include "TCanvas.h"
#include "TGLViewer.h"
#include "TGeoManager.h"

#include "art/Framework/Principal/Event.h"

#include "EventDisplay/Display3DView.h"
#include "EventDisplay/Display3DPad.h"

#include "Geometry/GeometryService.h"

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
    /*
    static double r13d[3] = {-55.0,-30.0, -600.0};
    static double r23d[3] = { 55.0, 80.0,  400.0};
    */

    fDisplay3DPad->Draw();
    evdb::Canvas::fCanvas->Update();

    // Eventually it would be nice to embed this into one of the normal
    // canvases, but for now just open ROOT's OpenGL viewer
    /*
    static TGLViewer* fGLViewer = 0;
    if (fGLViewer==0) {
      fGLViewer = (TGLViewer*)fDisplay3DPad->Pad()->GetViewer3D("ogl");
      fGLViewer->PreferLocalFrame();
      fGLViewer->SetResetCamerasOnUpdate(0);
    }
    else {
      fDisplay3DPad->Pad()->GetViewer3D()->PadPaint(fDisplay3DPad->Pad());
    }
    */

    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto geoM  = geo->Geo()->ROOTGeoManager();

    auto world_n = (TGeoNode*)geoM->GetTopNode();
    auto world_v = (TGeoVolume*)world_n->GetVolume();
    geoM->SetVisLevel(3);
    world_v->Draw();

  }

} // end namespace evd
////////////////////////////////////////////////////////////////////////
