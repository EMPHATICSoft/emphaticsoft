///
/// \file    XZYZProjectionsView.h
/// \brief   A view showing XZ and YZ readout planes
/// \author  messier@indiana.edu
/// \version $Id: XZYZProjectionsView.h,v 1.6 2011-05-03 17:16:39 messier Exp $
///
#ifndef EVD_XZYZPROJECTIONSVIEW_H
#define EVD_XZYZPROJECTIONSVIEW_H
#include "EventDisplayBase/Canvas.h"
#include "RQ_OBJECT.h"

namespace evd 
{
  //  class MCBriefPad;
  class TZProjPad;
  class TQPad;
  //  class HeaderPad;
  //  class SliceButtonBar;
  
  /// View of event showing the XZ and YZ readout planes
  class XZYZProjectionsView : public evdb::Canvas {
  public:
    RQ_OBJECT("evd::XZYZProjectionsView")
  public:
    XZYZProjectionsView(TGMainFrame* mf);
    ~XZYZProjectionsView();
    
    const char* Description() const { return "XZ/YZ Projections"; }
    const char* PrintTag()    const { return "xzyz-proj";         }
    void Draw(const char* opt="");

    void RangeChanged();
    void AspectRatioHandler( int event, int key, int py, TObject *sel);    
    void FixAspectRatio();
    void SetAspectRatio();

  private:
    //    HeaderPad*  fHeaderPad; ///< Show header information
    //    TQPad*      fRawQ;      ///< Histogram of raw charges
    //    TQPad*      fRawT;      ///< Histogram of raw times
    //TQPad*      fCalQ;      ///< Histogram of calibrated charges
    //TQPad*      fCalT;      ///< Histogram of calibrated times
    //    MCBriefPad* fMC;        ///< Short summary of MC event
    //    SliceButtonBar* fSliceButtonBar; ///< Bar with slice controls

    TZProjPad*  fXview;     ///< X - Z projection of the event
    TZProjPad*  fYview;     ///< Y - Z projection of the event

    // X and Y locations which determine the layout
    double fX0;
    double fX1;
    double fX3;
    double fX2;

    double fY0;
    double fY1;
    double fY2;
    double fY4;
    double fY3;
    
    /// A function that calculates the range to zoom out to
    /// on an axis.
    std::vector<float> GetRightAxesRange( int pix, float cm, float min,
			      float max, float boundmin,
			      float boundmax, int tpix, float tcm);
      
  };
}

#endif
////////////////////////////////////////////////////////////////////////
