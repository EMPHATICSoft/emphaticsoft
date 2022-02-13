/////////////////////////////////////////////////////////////////////////////
/// \file    DrawingPad.cxx
/// \brief   Base class for all event display drawing pads
/// \author  messier@indiana.edu
/// \version $Id: DrawingPad.cxx,v 1.7 2011-10-31 15:32:18 greenc Exp $:
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>

#include "TPad.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "EventDisplay/DrawingPad.h"
//#include "EventDisplay/HeaderDrawer.h"
#include "EventDisplay/GeometryDrawer.h"
//#include "EventDisplay/SimulationDrawer.h"
//#include "EventDisplay/RecoBaseDrawer.h"
//#include "EventDisplay/RawDataDrawer.h"
#include "EventDisplayBase/evdb.h"

namespace evd
{
  /// Create a drawing pad for the event display
  ///
  /// @param nm : Name of the TPad
  /// @param ti : Title of the TPad
  /// @param x1 : Relative x position (0-1) of lower left  corner
  /// @param y1 : Relative y position (0-1) of lower left  corner
  /// @param x2 : Relative x position (0-1) of upper right corner
  /// @param y2 : Relative y position (0-1) of upper right corner
  ///
  DrawingPad::DrawingPad(const char* nm,
			 const char* ti, 
			 double x1, double y1,
			 double x2, double y2) :
    //    fHeaderDraw(0),
    fGeometryDraw(0)
    //    fSimulationDraw(0),
    //    fRawDataDraw(0),
    //    fRecoBaseDraw(0)
  {
    fPad = new TPad(nm,ti,x1,y1,x2,y2);
    fPad->Draw();
    fPad->cd();
  }

  //......................................................................
  DrawingPad::~DrawingPad() 
  {
    //    if (fHeaderDraw)     { delete fHeaderDraw;     fHeaderDraw     = 0; }
    if (fGeometryDraw)   { delete fGeometryDraw;   fGeometryDraw   = 0; }
    //    if (fSimulationDraw) { delete fSimulationDraw; fSimulationDraw = 0; }
    //    if (fRawDataDraw)    { delete fRawDataDraw;    fRawDataDraw    = 0; }
    //    if (fRecoBaseDraw)   { delete fRecoBaseDraw;   fRecoBaseDraw   = 0; }
    if (fPad)            { delete fPad;            fPad = 0;            }
  }

  //......................................................................
  ///
  /// Provide access to the drawer for the event header
  ///
  /*
  HeaderDrawer* DrawingPad::HeaderDraw() 
  {
    if (fHeaderDraw==0) fHeaderDraw = new HeaderDrawer();
    return fHeaderDraw;
  }
  */

  ///
  /// Provide access to the drawer for the detector geometry
  ///
  GeometryDrawer* DrawingPad::GeometryDraw() 
  {
    if (fGeometryDraw==0) fGeometryDraw = new GeometryDrawer();
    return fGeometryDraw;
  }

  ///
  /// Provide access to the drawer for the Simulation classes
  ///
  /*
  SimulationDrawer* DrawingPad::SimulationDraw() 
  {
    if (fSimulationDraw==0) fSimulationDraw = new SimulationDrawer();
    return fSimulationDraw;
  }
  */

  ///
  /// Provide access to the drawer for the RawData classes
  ///
  /*
  RawDataDrawer* DrawingPad::RawDataDraw() 
  {
    if (fRawDataDraw==0) fRawDataDraw = new RawDataDrawer();
    return fRawDataDraw;
  }
  */

  //......................................................................
  ///
  /// Provide access to the drawer for RecoBase classes
  ///
  /*
  RecoBaseDrawer* DrawingPad::RecoBaseDraw() 
  {
    if (fRecoBaseDraw==0) fRecoBaseDraw = new RecoBaseDrawer();
    return fRecoBaseDraw;
  }
  */

} // end namespace evd
////////////////////////////////////////////////////////////////////////
