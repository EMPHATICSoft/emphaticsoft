///
/// \file    Display3DPad.cxx
/// \brief   Drawing pad showing a 3D rendering of the detector
/// \author  messier@indiana.edu
/// \version $Id: Display3DPad.cxx,v 1.6 2011-10-21 20:37:50 gsdavies Exp $
///
#include <iostream>

#include "TPad.h"
#include "TView3D.h"
#include "TGLViewer.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "EventDisplay/Display3DPad.h"
#include "EventDisplayServices/GeometryDrawingOptions.h"
//#include "EventDisplay/HeaderDrawer.h"
#include "EventDisplay/GeometryDrawer.h"
//#include "EventDisplay/RawDataDrawer.h"
//#include "EventDisplay/RecoBaseDrawer.h"
//#include "EventDisplay/SimulationDrawer.h"
#include "EventDisplayBase/View3D.h"
#include "EventDisplayBase/EventHolder.h"
#include "Geometry/Geometry.h"

namespace evd
{
  ///
  /// Create a pad to show a 3D rendering of the detector and events
  /// @param nm : Name of the pad
  /// @param ti : Title of the pad
  /// @param x1 : Location of left  edge of pad (0-1)
  /// @param x2 : Location of right edge of pad (0-1)
  /// @param y1 : Location of bottom edge of pad (0-1)
  /// @param y2 : Location of top    edge of pad (0-1)
  /// @param opt: Options. Currently just a place holder
  ///
  Display3DPad::Display3DPad(const char* nm, const char* ti,
			     double x1, double y1,
			     double x2, double y2,
			     const char* /*opt*/) :
    DrawingPad(nm, ti, x1, y1, x2, y2)
  {
    // this->Pad()->Draw();
    // this->Pad()->cd();
    fView = new evdb::View3D();
  }

  //......................................................................
  Display3DPad::~Display3DPad() 
  {
    if (fView) { delete fView; fView = 0; }
  }

  //......................................................................
  void Display3DPad::Draw() 
  {
    fView->Clear();

    // grab the event from the singleton
    const art::Event *evt = evdb::EventHolder::Instance()->GetEvent();
    if (evt==0) return;

    this->GeometryDraw()->  DetOutline3D         (*evt, fView);
    //    art::ServiceHandle<geo::Geometry> geo;
    /*
    this->HeaderDraw()->    Header               (fView);
    this->RawDataDraw()->   RawDigit3D           (*evt, fView);
    this->SimulationDraw()->MCTruthVertices3D    (*evt, fView);
    this->SimulationDraw()->MCTruthVectors3D     (*evt, fView);
    this->SimulationDraw()->MCTruthTrajectories3D(*evt, fView);
    this->SimulationDraw()->FLSHit3D             (*evt, fView);
    this->RecoBaseDraw()->  CellHit3D            (*evt, fView);
    this->RecoBaseDraw()->  Cluster3D            (*evt, fView);
    this->RecoBaseDraw()->  Prong3D              (*evt, fView);
    */
    int irep;
    this->Pad()->Clear();
    this->Pad()->cd();
    if (fPad->GetView()==0) {
      double rmin[]={ -0.5, // -geo->DetHalfWidth(),
		      -0.5, // -geo->DetHalfHeight(),
		      -0.1 //(0.5-0.45)*geo->DetLength()
      };
      double rmax[]={ 0.5, // geo->DetHalfWidth(),
		      0.5, // geo->DetHalfHeight(),
		      3. // (0.5+0.25)*geo->DetLength()
      };
      TView3D* v = new TView3D(1,rmin,rmax);
      v->SetPerspective();
      v->SetView(-125.0,-115.0,135.0,irep);
      fPad->SetView(v); // ROOT takes ownership of object *v
    }
    art::ServiceHandle<evd::GeometryDrawingOptions> opt;
    fPad->GetView()->SetView(opt->fTheta,opt->fPhi,opt->fPsi, irep);
    fView->Draw();
    fPad->Update();
  }

} // end namespace evd
////////////////////////////////////////////////////////////////////////
