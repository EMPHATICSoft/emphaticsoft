///
/// \file    TZProjPad
/// \brief   Drawing pad showing a single X-Z or Y-Z projection of an event
/// \author  messier@indiana.edu
/// \version $Id: TZProjPad.h,v 1.7 2012-07-23 23:50:52 bckhouse Exp $
///
#ifndef EVD_TZPROJPAD_H
#define EVD_TZPROJPAD_H
#include "EventDisplay/DrawingPad.h"

#include "TH2F.h"

namespace art {class Event;}
namespace evdb { class View2D;   }

namespace evd 
{
  //  class RawDataDrawer;
  //  class RecoBaseDrawer;

  /// Implement an UnZoom menu item
  class THUnZoomable: public TH2F
  {
  public:
    THUnZoomable(const char* a, const char* b, int nx, double x0, double x1, int ny, double y0, double y1);
    void UnZoom(); // *MENU*
  protected:
    ClassDef(THUnZoomable, 0);
  };
  
  /// A drawing pad for an XZ or ZY
  class TZProjPad : public DrawingPad {
  public:
    TZProjPad(const char* nm, const char* ti,
	      double x1, double y1,
	      double x2, double y2,
	      const char* opt);
    ~TZProjPad();
    // \a rezoom is an out-param, whether the display rezoomed this time
    void Draw(const char* opt=0, bool* rezoom = 0);

    void GetWrange(int *i1, int *i2) const;
    void GetZrange(int *i1, int *i2) const;
    void GetWrangeCm(float *i1, float *i2) const;
    void GetZrangeCm(float *i1, float *i2) const;
    void GetZBoundCm(float *i1, float *i2) const;
    void GetWBoundCm(float *i1, float *i2) const;

    void SetZrange(int i1, int i2);
    void SetWrange(int i1, int i2);
    void SetZrangeCm(float i1, float i2);
    void SetWrangeCm(float i1, float i2);

  private:
    //    void AutoZoom();
    //    void AutoZoomTruth(const art::Event* evt);
    //    void AutoZoomSlice(const art::Event* evt);
    //    void AutoZoomBox(const art::Event* evt);
    void AutoZoomHelper(double xmin, double xmax,
                        double ymin, double ymax,
                        double zmin, double zmax, double safety);
    void ShowFull();
    void ShowPartial();
    void LayoutHisto();
    
  public:
    unsigned int  fLastRun;  ///< Last run number shown
    unsigned int  fLastEvt;  ///< Last event number shown
    //    int           fLastSlice;///< Last slice number shown
    int           fLastZoom; ///< Last zoom option applied
    //    int           fDetId;    ///< Detector display is configured for
    int           fXorY;     ///< Which view? 0 = X vs. Z, 1 = Y vs. Z
    TH2F*         fHisto;    ///< Histogram to draw objects on
    evdb::View2D* fView;     ///< Collection of graphics objects to render
  };
}

#endif
////////////////////////////////////////////////////////////////////////
