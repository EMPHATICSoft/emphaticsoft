///
/// \file    Display3DView.h
/// \brief   A view showing a 3D rendering of the detector
/// \author  messier@indiana.edu
/// \version $Id: Display3DView.h,v 1.1.1.1 2010-11-10 19:44:54 p-novaart Exp $
///
#ifndef EVD_DISPLAY3DVIEW_H
#define EVD_DISPLAY3DVIEW_H
#include "EventDisplayBase/Canvas.h"

namespace evd {
  class Display3DPad;

  /// View of event shoing the XZ and YZ readout planes
  class Display3DView : public evdb::Canvas {
  public:
    Display3DView(TGMainFrame* mf);
    ~Display3DView();
    
    const char* Description() const { return "3D Detector Display"; }
    const char* PrintTag()    const { return "emph3d";              }
    void Draw(const char* opt="");

  private:
    Display3DPad* fDisplay3DPad; /// Pad showing 3D view of the detector
  };
}

#endif
////////////////////////////////////////////////////////////////////////
