///
/// \file    DrawingPad.h
/// \brief   Base class for all event display drawing pads
/// \author  messier@indiana.edu
/// \version $Id: DrawingPad.h,v 1.3 2011-01-29 19:15:20 p-novaart Exp $:
///
#ifndef EVD_DRAWINGPAD_H
#define EVD_DRAWINGPAD_H
class TPad;

///nova event display
namespace evd {
  //  class HeaderDrawer;
  class GeometryDrawer;
  //  class SimulationDrawer;
  //  class RawDataDrawer;
  //  class RecoBaseDrawer;

  /// Base class for event display drawing pads
  class DrawingPad {
  public:
    DrawingPad(const char* nm,
	       const char* ti, 
	       double x1, double y1,
	       double y2, double x2);
    ~DrawingPad();
    TPad* Pad() { return fPad; }
    
    // Access to the drawing utilities
    //    HeaderDrawer*     HeaderDraw();
    GeometryDrawer*   GeometryDraw();
    //    SimulationDrawer* SimulationDraw();
    //    RawDataDrawer*    RawDataDraw();
    //    RecoBaseDrawer*   RecoBaseDraw();
    
  protected:
    TPad*             fPad;            ///< The ROOT graphics pad
    //    HeaderDrawer*     fHeaderDraw;     ///< Drawer for event header info
    GeometryDrawer*   fGeometryDraw;   ///< Drawer for detector geometry
    //    SimulationDrawer* fSimulationDraw; ///< Drawer for simulation objects
    //    RawDataDrawer*    fRawDataDraw;    ///< Drawer for raw data
    //    RecoBaseDrawer*   fRecoBaseDraw;   ///< Drawer for recobase objects
  };
}
#endif
////////////////////////////////////////////////////////////////////////
