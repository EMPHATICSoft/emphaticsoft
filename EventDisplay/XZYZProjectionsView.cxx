//////////////////////////////////////////////////////////////////////////////
/// \file    XZYZProjectionsView.cxx
/// \brief   The "main" event display view that most people will want to use
/// \author  messier@indiana.edu
/// \version $Id: XZYZProjectionsView.cxx,v 1.13 2012-03-10 02:09:41 bckhouse Exp $
/////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>

#include "TAxis.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TTimer.h"

//#include "EventDisplay/HeaderPad.h"
//#include "EventDisplay/MCBriefPad.h"
//#include "EventDisplay/SliceButtonBar.h"
#include "EventDisplay/TZProjPad.h"
//#include "EventDisplay/TQPad.h"
#include "EventDisplay/XZYZProjectionsView.h"

#include "TRootEmbeddedCanvas.h"

namespace evd
{
  //...................................................................
  XZYZProjectionsView::XZYZProjectionsView(TGMainFrame* mf) : 
    evdb::Canvas(mf),
    fX0(0.0),
    fX1(0.135),
    fX3(1.0),
    fX2(0.5*(fX1+fX3)),
    fY0(0.0),
    fY1(0.14),
    fY2(0.17),
    fY4(1.0),
    fY3(0.5*(fY2+fY4))
  {
    /*
    evdb::Canvas::fCanvas->cd();
    fHeaderPad = new HeaderPad("fHeaderPad","Header",fX0,fY0,fX1,fY2,"");
    fHeaderPad->Draw();
  
    evdb::Canvas::fCanvas->cd();
    fRawT = new TQPad("fTPad", "Hit Times",fX1,fY0,fX2,fY1,"T");
    fRawT->Draw();
  
    evdb::Canvas::fCanvas->cd();
    fRawQ = new TQPad("fQPad", "Hit Charges",fX2,fY0,fX3,fY1,"Q");
    fRawQ->Draw();
  
    evdb::Canvas::fCanvas->cd();
    fMC = new MCBriefPad("fMCPad","MC Info.",fX1,fY1,fX3,fY2,"");
    fMC->Draw();
    */
  
    evdb::Canvas::fCanvas->cd();
    fXview = new TZProjPad("fXView","xz view",fX0,fY3,fX3,fY4,"x");
    fXview->Draw();
  
    evdb::Canvas::fCanvas->cd();
    fYview = new TZProjPad("fYView","yz view",fX0,fY2,fX3,fY3,"y");
    fYview->Draw();
  
    fXview->Pad()->Connect("RangeChanged()",
			   "evd::XZYZProjectionsView",
			   this,
			   "RangeChanged()");
    fYview->Pad()->Connect("RangeChanged()",
			   "evd::XZYZProjectionsView",
			   this,
			   "RangeChanged()");

    // Want the slice bar top, but the canvas got there first. Remove it...
    //    mf->RemoveFrame(fEmbCanvas);
    // ...add the bar...
    //    fSliceButtonBar = new SliceButtonBar(mf);
    //    mf->AddFrame(fSliceButtonBar,
    //                 new TGLayoutHints(kLHintsTop | kLHintsExpandX,
    //                                   0, 0, 1, 0));
    // ...and put the canvas back
    //    mf->AddFrame(fEmbCanvas, fLayout);
    
    fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "evd::XZYZProjectionsView",
		     this, "AspectRatioHandler(int, int, int, TObject*)");
    
    evdb::Canvas::fCanvas->Update();
  }
  
  //......................................................................

  XZYZProjectionsView::~XZYZProjectionsView() 
  {
    //    if (fRawT)  { delete fRawT;  fRawT  = 0; }
    //    if (fRawQ)  { delete fRawQ;  fRawQ  = 0; }
    //    if (fMC)    { delete fMC;    fMC    = 0; }
    if (fXview) { delete fXview; fXview = 0; }
    if (fYview) { delete fYview; fYview = 0; }
    //    if (fSliceButtonBar) { delete fSliceButtonBar; fSliceButtonBar = 0; }
  }

  //......................................................................
  //
  // Adjust the aspect ratios of the x and y views to reflect the size
  // of the detector
  //
  void XZYZProjectionsView::SetAspectRatio()
  {
    if(!fXview->fHisto || !fYview->fHisto) return;

    static double lastnx = 0.0;
    static double lastny = 0.0;
  
    double nx = 
      fXview->fHisto->GetYaxis()->GetXmax()-
      fXview->fHisto->GetYaxis()->GetXmin();
    double ny = 
      fYview->fHisto->GetYaxis()->GetXmax()-
      fYview->fHisto->GetYaxis()->GetXmin();
  
    //
    // Is the current aspect ratio correct?
    //
    if (lastnx == nx && lastny==ny) return;
    static const double tol = 0.001;
    if (fabs(lastnx-nx)<tol && fabs(lastny-ny)<tol) return;
  
    lastnx = nx;
    lastny = ny;
  
    //
    // Reposition the boundary (fY3) between the X and Y views so that
    // the distance between y3 and y4 scales with nx and the distance
    // between y2 and y3 scales with ny
    //
    fY3 = (nx*fY2+ny*fY4)/(nx+ny);
  
    fXview->Pad()->SetPad(fX0,fY3,fX3,fY4);
    fYview->Pad()->SetPad(fX0,fY2,fX3,fY3);
  
    //
    // Along with scaling the views, we need to rescale the fonts sizes
    // and offsets to keep them lined up
    //
    double sfx = (fY3-fY2)/(fY4-fY2);
    double sfy = (fY4-fY3)/(fY4-fY2);
  
    TAxis* xvx = fXview->fHisto->GetXaxis();
    TAxis* xvy = fXview->fHisto->GetYaxis();
    xvx->SetTickLength(0.01);
    xvy->SetTickLength(0.01);
  
    xvx->SetTitleSize(0.15*sfx);
    xvy->SetTitleSize(0.15*sfx);
  
    xvx->SetTitleOffset(0.8);
    xvy->SetTitleOffset(0.65*sfy);
    xvx->SetLabelSize(0.12*sfx);
    xvy->SetLabelSize(0.12*sfx);
    xvx->SetLabelOffset(0.025*sfx);
  
    TAxis* yvx = fYview->fHisto->GetXaxis();
    TAxis* yvy = fYview->fHisto->GetYaxis();
  
    yvx->SetTickLength(0.01);
    yvy->SetTickLength(0.01);
    yvx->SetTitleSize(0.15*sfy);
    yvy->SetTitleSize(0.15*sfy);
    yvx->SetTitleOffset(0.8);
    yvy->SetTitleOffset(0.65*sfx);
  
    yvx->SetLabelSize(0.12*sfy);
    yvy->SetLabelSize(0.12*sfy);
    yvx->SetLabelOffset(0.025*sfy);
  }

  //......................................................................

  void XZYZProjectionsView::Draw(const char* opt) 
  {
    evdb::Canvas::fCanvas->cd();
  
    //    fHeaderPad->Draw();
  
    //    fRawT->Draw();
    //    fRawQ->Draw();
    //    fMC->Draw();

    bool rezoom = false;

    fXview->Draw(opt, &rezoom);
    fYview->Draw(opt, &rezoom);

    // Any auto-zooming should always wind up in a state with correct aspect
    // ratio.
    if(rezoom) FixAspectRatio();

    //    fSliceButtonBar->Update();
  
    this->SetAspectRatio();
  
    evdb::Canvas::fCanvas->Update();
  }

  //......................................................................

  class Refresher: public TTimer
  {
  public:
    Refresher() : TTimer(0, kTRUE) {}
    virtual Bool_t Notify()
    {
      gPad->Update();
      TurnOff();
      return kFALSE;
    }
  } gRefresher;

  //......................................................................

  void XZYZProjectionsView::RangeChanged() 
  {
    static int ilolast = -1;
    static int ihilast = -1;
  
    int i1lo, i1hi;
    int i2lo, i2hi;
  
    fXview->GetZrange(&i1lo, &i1hi);
    fYview->GetZrange(&i2lo, &i2hi);
  
    bool axis1changed = (i1lo!=ilolast)||(i1hi!=ihilast);
    bool axis2changed = (i2lo!=ilolast)||(i2hi!=ihilast);

    if (axis1changed) {
      fYview->SetZrange(i1lo, i1hi);
      fYview->Pad()->Modified();

      ilolast = i1lo;
      ihilast = i1hi;
    }
    if (axis2changed) {
      fXview->SetZrange(i2lo, i2hi);
      fXview->Pad()->Modified();

      ilolast = i2lo;
      ihilast = i2hi;
    }

    if(axis1changed || axis2changed){
      // We need to call gPad->Update() to get these new axis ranges
      // drawn. Unfortunately, this function is called in the middle of another
      // gPad->Update(), and ROOT prevents two calls from happening at
      // once. Instead, arrange matters so that it will be called just as soon
      // as we return to the event loop. This is the neatest way I could figure
      // out to get a one-short timer (I can't make TTimer::SingleShot() work
      // right).
      gRefresher.TurnOn();
    }
  }

 //......................................................................

  void XZYZProjectionsView::AspectRatioHandler( int event, int key, int,  TObject *)
  {

    if (event != kKeyPress || key != '=' )
      return;

    FixAspectRatio();
  }

 //......................................................................

  void XZYZProjectionsView::FixAspectRatio()
  {
    float xmin, xmax, ymin, ymax, zmin, zmax;
    fXview->GetWrangeCm(&xmin, &xmax);
    fYview->GetWrangeCm(&ymin, &ymax);
    fXview->GetZrangeCm(&zmin, &zmax);

    float xcm = xmax - xmin;
    float ycm = ymax - ymin;
    float zcm = zmax - zmin;

    int xpix  = fXview->Pad()->GetWh()*fXview->Pad()->GetHNDC();
    int ypix  = fYview->Pad()->GetWh()*fYview->Pad()->GetHNDC();
    int zpix  = fXview->Pad()->GetWw()*fXview->Pad()->GetWNDC();

    float xpixcm = xpix/xcm;
    float ypixcm = ypix/ycm;
    float zpixcm = zpix/zcm;

    float xboundmin, xboundmax, yboundmin, yboundmax, zboundmin, zboundmax; 

    fXview->GetZBoundCm( &zboundmin, &zboundmax);
    fXview->GetWBoundCm( &xboundmin, &xboundmax);
    fYview->GetWBoundCm( &yboundmin, &yboundmax);

    if( xpixcm < ypixcm && xpixcm < zpixcm ){
      // x is more zoomed out, zoom out on y,z as well.

      std::vector<float> zrange = GetRightAxesRange( zpix, zcm, zmin, zmax, 
						     zboundmin, zboundmax,
						     xpix, xcm);

      std::vector<float> yrange = GetRightAxesRange( ypix, ycm, ymin, ymax, 
						     yboundmin, yboundmax,
						     xpix, xcm);
      fXview->SetZrangeCm( zrange[0], zrange[1]);
      fYview->SetZrangeCm( zrange[0], zrange[1]);
      fYview->SetWrangeCm( yrange[0], yrange[1]);
      fXview->Pad()->Modified();
      fYview->Pad()->Modified();
    }
    
    else if( ypixcm < xpixcm && ypixcm < zpixcm ){

      // y is more zoomed out, zoom out on x,z as well.

      std::vector<float> zrange = GetRightAxesRange( zpix, zcm, zmin, zmax, 
						     zboundmin, zboundmax,
						     ypix, ycm);
     
      std::vector<float> xrange = GetRightAxesRange( xpix, xcm, xmin, xmax, 
						     xboundmin, xboundmax,
						     ypix, ycm);

      fXview->SetZrangeCm( zrange[0], zrange[1]);
      fYview->SetZrangeCm( zrange[0], zrange[1]);
      fXview->SetWrangeCm( xrange[0], xrange[1]);
      fXview->Pad()->Modified();
      fYview->Pad()->Modified();
    }

    else if( zpixcm < ypixcm && zpixcm < xpixcm ){

      // x is more zoomed out, zoom out on y,z as well.

      std::vector<float> xrange = GetRightAxesRange( xpix, xcm, xmin, xmax, 
						     xboundmin, xboundmax,
						     zpix, zcm);

      std::vector<float> yrange = GetRightAxesRange( ypix, ycm, ymin, ymax, 
						     yboundmin, yboundmax,
						     zpix, zcm);

      fXview->SetWrangeCm( xrange[0], xrange[1]);
      fYview->SetWrangeCm( yrange[0], yrange[1]);
      fXview->Pad()->Modified();
      fYview->Pad()->Modified();
    }

    else if( ypixcm == xpixcm && ypixcm < zpixcm ){
      // y,x are more zoomed out, zoom out on z as well.

      std::vector<float> zrange = GetRightAxesRange( zpix, zcm, zmin, zmax, 
						     zboundmin, zboundmax,
						     ypix, ycm);

      fXview->SetZrangeCm( zrange[0], zrange[1]);
      fYview->SetZrangeCm( zrange[0], zrange[1]);
      fXview->Pad()->Modified();
      fYview->Pad()->Modified();
    }

    else if( zpixcm == xpixcm && zpixcm < ypixcm ){
      // y,x are more zoomed out, zoom out on z as well.

      std::vector<float> yrange = GetRightAxesRange( ypix, ycm, ymin, ymax, 
						     yboundmin, yboundmax,
						     zpix, zcm);

      fYview->SetWrangeCm( yrange[0], yrange[1]);
      fYview->Pad()->Modified();
    }

    else if( zpixcm == ypixcm && zpixcm < xpixcm ){
      // y,x are more zoomed out, zoom out on z as well.

      std::vector<float> xrange = GetRightAxesRange( xpix, xcm, xmin, xmax, 
						     xboundmin, xboundmax,
						     zpix, zcm);
      fXview->SetWrangeCm( xrange[0], xrange[1]);
      fXview->Pad()->Modified();
    }

    fCanvas->Update();
  }
  
 //......................................................................
  
  std::vector<float> XZYZProjectionsView::GetRightAxesRange( int pix, float cm, float min, 
							     float max, float boundmin, 
							     float boundmax, int tpix, float tcm)
  {
    std::vector<float> range;

    float zoom = (tcm * pix / tpix ) -cm;
    float lo = min - (zoom/2);
    float hi = max + (zoom/2);
    
    if( lo < boundmin ){
      float shift = boundmin - lo;
      lo = boundmin;
      hi = hi + shift;
    }

    if( hi > boundmax ){
      float shift = hi - boundmax;
      lo = lo - shift;
      hi = boundmax;
    }
    range.push_back( lo );
    range.push_back( hi );

    return range;
  }

} // end namespace evd
////////////////////////////////////////////////////////////////////////
