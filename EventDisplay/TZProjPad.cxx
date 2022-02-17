///
/// \file    TZProjPad.cxx
/// \brief   Drawing pad for X-Z or Y-Z projections of events
/// \author  messier@indiana.edu
/// \version $Id: TZProjPad.cxx,v 1.25 2012-12-05 02:43:06 bckhouse Exp $
///
#include "TH2F.h"
#include "TPad.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "EventDisplay/TZProjPad.h"
#include "EventDisplayServices/GeometryDrawingOptions.h"
//#include "EventDisplayServices/SliceNavigator.h"
#include "EventDisplay/GeometryDrawer.h"
#include "EventDisplay/GeoTransform.h"
//#include "EventDisplay/SimulationDrawer.h"
//#include "EventDisplay/RawDataDrawer.h"
//#include "EventDisplay/RecoBaseDrawer.h"
#include "EventDisplayBase/View2D.h"
#include "EventDisplayBase/EventHolder.h"
#include "Geometry/Geometry.h"
//#include "GeometryObjects/CellGeo.h"
//#include "RecoBase/CellHit.h"
#include "Geometry/GeometryService.h"

namespace evd
{

  //......................................................................

  THUnZoomable::THUnZoomable(const char* a, const char* b,
                             int nx, double x0, double x1,
                             int ny, double y0, double y1)
    : TH2F(a, b, nx, x0, x1, ny, y0, y1)
  {
  }

  //......................................................................

  void THUnZoomable::UnZoom()
  {
    GetXaxis()->UnZoom();
    GetYaxis()->UnZoom();
  }

  //......................................................................

  // Numeric flags for the XZ or ZY views
  static const int kX = 0;
  static const int kY = 1;

  static const double kZsf = 1.015; // Show z range larger than detector
  static const double kTsf = 1.035; // Show x/y range larger than detector

  ///
  /// Create a pad showing a single X-Z or Y-Z projection of the detector
  /// \param nm : Name of the pad
  /// \param ti : Title of the pad
  /// \param x1 : Location of left  edge of pad (0-1)
  /// \param x2 : Location of right edge of pad (0-1)
  /// \param y1 : Location of bottom edge of pad (0-1)
  /// \param y2 : Location of top    edge of pad (0-1)
  /// \param opt : Options. Include 'x', 'y' for xz or yz projections
  ///
  TZProjPad::TZProjPad(const char* nm,
		       const char* ti,
		       double x1, double x2,
		       double y1, double y2,
		       const char* opt) :
    DrawingPad(nm, ti, x1, x2, y1, y2),
    fLastRun(999999),
    fLastEvt(999999),
    //    fLastSlice(999999),
    fLastZoom(999999)
  {
  
    //    art::ServiceHandle<geo::Geometry> geo;

    //    fDetId = geo->DetId();

    this->Pad()->SetBit(TPad::kCannotMove);
    this->Pad()->cd();

    if (std::string(opt)=="x") fXorY = kX;
    if (std::string(opt)=="y") fXorY = kY;
    if (fXorY==kX) {
      this->Pad()->SetLeftMargin  (0.050);
      this->Pad()->SetRightMargin (0.010);
      this->Pad()->SetTopMargin   (0.110);
      this->Pad()->SetBottomMargin(0.005);
    }
    if (fXorY==kY) {
      this->Pad()->SetLeftMargin  (0.050);
      this->Pad()->SetRightMargin (0.010);
      this->Pad()->SetTopMargin   (0.005);
      this->Pad()->SetBottomMargin(0.120);
    }

    fHisto = 0; // Force function to create us a new histogram
    this->LayoutHisto();

    fView = new evdb::View2D();
  }

  //......................................................................

  TZProjPad::~TZProjPad() 
  {
    if (fHisto) { delete fHisto; fHisto = 0; }
    if (fView)  { delete fView;  fView  = 0; }
  }

  //......................................................................

  void TZProjPad::Draw(const char* , bool* rezoom)
  {
    bool temp;
    if(!rezoom) rezoom = &temp;

    fView->Clear();
  
    evdb::View2D* vx = 0;
    evdb::View2D* vy = 0;
    if (fXorY==kX) vx = fView; // fView is an XZ projection
    if (fXorY==kY) vy = fView; // fView is a  YZ projection

    // grab the singleton holding the art::Event
    const art::Event *evt = evdb::EventHolder::Instance()->GetEvent();
    this->LayoutHisto();

    if (evt) {
      this->GeometryDraw()->  DetOutline2D         (*evt, vx, vy);
      /*
      this->GeometryDraw()->  DrawCells2D          (*evt, vx, vy);
      this->GeometryDraw()->  DrawDCMBoxes         (*evt, vx, vy);
      this->GeometryDraw()->  FiducialVolumeUser2D (*evt, vx, vy);
      this->GeometryDraw()->  DrawBadBoxesPretty   (*evt, vx, vy);  // nice looking badboxes but not quite exact
      */
      //      this->GeometryDraw()->  DrawBadBoxesExact    (*evt, vx, vy); // switch in to see exactly where the bad boxes are
      /*
      this->SimulationDraw()->FLSHit2D             (*evt, vx, vy);
      this->SimulationDraw()->MCTruthVertices2D    (*evt, vx, vy);
      this->SimulationDraw()->MCTruthVectors2D     (*evt, vx, vy);
      this->SimulationDraw()->MCTruthTrajectories2D(*evt, vx, vy);
      this->RawDataDraw()->   RawDigit2D           (*evt, vx, vy);
      this->RecoBaseDraw()->  CellHit2D            (*evt, vx, vy);
      this->GeometryDraw()->  DrawHighlightCell    (*evt, vx, vy);
      this->RecoBaseDraw()->  Cluster2D            (*evt, vx, vy);
      this->RecoBaseDraw()->  OfflineChans2D       (*evt, vx, vy);
      this->RecoBaseDraw()->  HoughResult2D        (*evt, vx, vy);
      this->RecoBaseDraw()->  Prong2D              (*evt, vx, vy);
      this->RecoBaseDraw()->  Track2D              (*evt, vx, vy);
      this->RecoBaseDraw()->  Vertex2D             (*evt, vx, vy);
      */
    }

    art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
    //    art::ServiceHandle<SliceNavigator> nav;
    fPad->cd();
    fPad->Clear();
    
    int grid = ((drawopt->fOutline & 
		 evd::GeometryDrawingOptions::kOUTLINE_GRID)!=0);
    fPad->SetGridx(grid);
    fPad->SetGridy(grid);

    this->LayoutHisto();
    if (!fHisto) return;

    if (fXorY==kX) fHisto->Draw("X+");
    else           fHisto->Draw("");

    // Check if we should zoom the displays
    *rezoom = false;
    if(evt){
      if (fLastRun   != evt->run())          *rezoom = true;
      if (fLastEvt   != evt->id().event())   *rezoom = true;
      //      if (fLastSlice != nav->CurrentSlice()) *rezoom = true;
    }
    if (fLastZoom != drawopt->fZoom)    *rezoom = true;
    /*
    if (drawopt->fZoom == evd::GeometryDrawingOptions::kZOOM_SLICE &&
        int(fLastSlice) != nav->CurrentSlice())
      *rezoom = true;
      */
    
    if (*rezoom) {
      switch (drawopt->fZoom) {
	/*
      case evd::GeometryDrawingOptions::kZOOM_TRUTH:
        if (evt) AutoZoomTruth(evt);
	break;
      case evd::GeometryDrawingOptions::kZOOM_SLICE:
        if (evt) AutoZoomSlice(evt);
	break;
      case evd::GeometryDrawingOptions::kZOOM_HITS:
        AutoZoom();
	break;
	*/
      case evd::GeometryDrawingOptions::kZOOM_PARTIAL:
	ShowPartial();
	break;
	/*
      case evd::GeometryDrawingOptions::kZOOM_SLICEBOX:
	AutoZoomBox(evt);
	break;
	*/
      default:
	ShowFull();
      }
    }
    
    if(evt){
      fLastRun   = evt->run();
      fLastEvt   = evt->id().event();
    }
    //    fLastSlice = nav->CurrentSlice();
    fLastZoom  = drawopt->fZoom;

    fView->Draw();
    // Axes can get overdrawn by pad objects
    // This somehow causes an infinite loop in the FD, comment out for now.
    //    fHisto->Draw("axis same");
  }

  //......................................................................

  ///
  /// Automatically zoom the view to a size just larger than the
  /// events. Also ensures that the aspect ratio is the same for the XZ
  /// and YZ projections.
  ///
  /*
  void TZProjPad::AutoZoom()
  {
    double xmin, ymin, zmin;
    double xmax, ymax, zmax;
    this->RawDataDraw()->GetLimits(&xmin, &xmax, 
				   &ymin, &ymax, 
				   &zmin, &zmax);

    AutoZoomHelper(xmin, xmax, ymin, ymax, zmin, zmax, .1);
  }
  */

  //......................................................................
  /*
  void TZProjPad::AutoZoomTruth(const art::Event* evt)
  {
    assert(evt);

    std::vector<art::Handle<std::vector<rb::CellHit>>> chits;
    evt->getManyByType(chits);

    std::set<geo::OfflineChan> hmap;
    for(unsigned int i = 0; i < chits.size(); ++i){
      for(unsigned int j = 0; j < chits[i]->size(); ++j){
        const art::Ptr<rb::CellHit> chit(chits[i], j);
        hmap.insert(geo::OfflineChan(chit->Plane(), chit->Cell()));
      }
    }

    double xmin, xmax, ymin, ymax, zmin, zmax;
    SimulationDraw()->GetLimits(evt, xmin, xmax, ymin, ymax, zmin, zmax, hmap);

    AutoZoomHelper(xmin, xmax, ymin, ymax, zmin, zmax, .05);
  }
  */
  //......................................................................
  /*
  void TZProjPad::AutoZoomSlice(const art::Event* evt)
  {
    assert(evt);

    art::ServiceHandle<evd::SliceNavigator> nav;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    // Drop 10% of outlier hits from each axis
    nav->GetBounds(*evt, xmin, xmax, ymin, ymax, zmin, zmax, .1);

    // But then add in 20% of padding to compensate
    AutoZoomHelper(xmin, xmax, ymin, ymax, zmin, zmax, .2);
  }

  void TZProjPad::AutoZoomBox(const art::Event* evt)
  {
    assert(evt);

    art::ServiceHandle<evd::SliceNavigator> nav;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    // Drop 10% of outlier hits from each axis
    nav->GetBox(*evt, xmin, xmax, ymin, ymax, zmin, zmax, .1);

    // But then add in 20% of padding to compensate
    AutoZoomHelper(xmin, xmax, ymin, ymax, zmin, zmax, .2);
  }
  */

  //......................................................................

  void TZProjPad::AutoZoomHelper(double xmin, double xmax,
                                 double ymin, double ymax,
                                 double zmin, double zmax, double safety)
  {
    const double dxy = std::max(xmax-xmin, ymax-ymin);
    const double dz = zmax-zmin;

    const double xavg = (xmin+xmax)/2;
    const double yavg = (ymin+ymax)/2;

    xmin = xavg-(.5+safety)*dxy;
    xmax = xavg+(.5+safety)*dxy;
    ymin = yavg-(.5+safety)*dxy;
    ymax = yavg+(.5+safety)*dxy;
    zmin -= safety*dz;
    zmax += safety*dz;

    fHisto->GetXaxis()->SetRangeUser(zmin, zmax);
    if(fXorY == kX) fHisto->GetYaxis()->SetRangeUser(xmin, xmax);
    else            fHisto->GetYaxis()->SetRangeUser(ymin, ymax);
  }

  //......................................................................

  void TZProjPad::ShowPartial()
  {
    art::ServiceHandle<emph::geo::GeometryService> g;
    art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;

    double xmin = -kTsf*g->Geo()->WorldWidth();
    double xmax = +kTsf*g->Geo()->WorldWidth();
    double ymin = -kTsf*g->Geo()->WorldHeight();
    double ymax = +kTsf*g->Geo()->WorldHeight();
    double zmin = -(kZsf-1.0)*g->Geo()->WorldLength();
    double zmax = +(kZsf    )* drawopt->fZRange;

    GeoTransform::XYZ(&xmin, &ymin, &zmin);
    GeoTransform::XYZ(&xmax, &ymax, &zmax);
    
    fHisto->GetXaxis()->SetRangeUser(zmin,zmax);
    if (fXorY==kX) fHisto->GetYaxis()->SetRangeUser(xmin,xmax);
    else           fHisto->GetYaxis()->SetRangeUser(ymin,ymax);
  }
  
  //......................................................................

  void TZProjPad::ShowFull()
  {
    art::ServiceHandle<emph::geo::GeometryService> g;

    double xmin = -kTsf*g->Geo()->WorldWidth();
    double xmax = +kTsf*g->Geo()->WorldWidth();
    double ymin = -kTsf*g->Geo()->WorldHeight();
    double ymax = +kTsf*g->Geo()->WorldHeight();
    double zmin = -(kZsf-1.0)*g->Geo()->WorldLength();
    double zmax = +(kZsf    )*g->Geo()->WorldLength();

    GeoTransform::XYZ(&xmin, &ymin, &zmin);
    GeoTransform::XYZ(&xmax, &ymax, &zmax);

    fHisto->GetXaxis()->SetRangeUser(zmin,zmax);
    if (fXorY==kX) fHisto->GetYaxis()->SetRangeUser(xmin,xmax);
    else           fHisto->GetYaxis()->SetRangeUser(ymin,ymax);
  }

  //......................................................................
  void TZProjPad::GetWrange(int* i1, int* i2) const
  {
    *i1 = fHisto->GetYaxis()->GetFirst();
    *i2 = fHisto->GetYaxis()->GetLast();
  }

  //......................................................................

  void TZProjPad::GetZrange(int* i1, int* i2) const 
  {
    *i1 = fHisto->GetXaxis()->GetFirst();
    *i2 = fHisto->GetXaxis()->GetLast();
  }

  //......................................................................
  void TZProjPad::GetWrangeCm(float* i1, float* i2) const
  {
    TAxis *axis = fHisto->GetYaxis();
    *i1 = axis->GetBinCenter( axis->GetFirst() );
    *i2 = axis->GetBinCenter( axis->GetLast() );
  }

  //......................................................................

  void TZProjPad::GetZrangeCm(float* i1, float* i2) const 
  {
    TAxis *axis = fHisto->GetXaxis();
    *i1 = axis->GetBinCenter( axis->GetFirst() );
    *i2 = axis->GetBinCenter( axis->GetLast() );
  }

  //......................................................................

  void TZProjPad::GetZBoundCm(float* i1, float* i2) const 
  {
    TAxis *axis = fHisto->GetXaxis();
    *i1 = axis->GetXmin();
    *i2 = axis->GetXmax();
  }

  //......................................................................

  void TZProjPad::GetWBoundCm(float* i1, float* i2) const 
  {
    TAxis *axis = fHisto->GetYaxis();
    *i1 = axis->GetXmin();
    *i2 = axis->GetXmax();
  }

  //......................................................................


  void TZProjPad::SetZrange(int i1, int i2)
  {
    fHisto->GetXaxis()->SetRange(i1,i2);
  }
  
  //......................................................................

  void TZProjPad::SetZrangeCm(float i1, float i2)
  {
    TAxis *axis = fHisto->GetXaxis();
    axis->SetRange( axis->FindBin(i1) , axis->FindBin(i2) );
  }
  
  //......................................................................

  void TZProjPad::SetWrange(int i1, int i2)
  {
    fHisto->GetYaxis()->SetRange(i1,i2);
  }

  //......................................................................

  void TZProjPad::SetWrangeCm(float i1, float i2)
  {
    TAxis *axis = fHisto->GetYaxis();
    axis->SetRange( axis->FindBin(i1) , axis->FindBin(i2) );
  }

  //......................................................................

  void TZProjPad::LayoutHisto()
  {
    //
    // Check if the current layout is correct. That is, we have a
    // histogram and the detector ID matches the detector ID for the
    // current geometry.
    //
    art::ServiceHandle<emph::geo::GeometryService> geo;

    // Figure out the z range for axis. The z-range may optionally run
    // from north to south or from south to north.
    //
    double zlo = -(kZsf-1.0)*geo->Geo()->WorldLength();
    double zhi =  (kZsf    )*geo->Geo()->WorldLength();
    GeoTransform::Z(&zlo);
    GeoTransform::Z(&zhi);
    if (zlo>zhi) { double ztmp = zlo; zlo = zhi; zhi = ztmp; }
    
    //
    // Figure out how to label the axes. We may be asked to label
    // compass directions and have to account for axis flips
    //
    art::ServiceHandle<evd::GeometryDrawingOptions> geoopt;
    bool flipx = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_X;
    bool flipz = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_Z;
    const char* xlabel = ";;x (cm)";
    const char* ylabel = ";z (cm);y (cm)";
    if (geoopt->fLabel&evd::GeometryDrawingOptions::kLABEL_COMPASS) {
      if (flipx) xlabel = ";;#leftarrow west    x (cm)    east #rightarrow";
      else       xlabel = ";;#leftarrow east    x (cm)    west #rightarrow";
      if (flipz) ylabel = ";#leftarrow north    z (cm)    south #rightarrow;y (cm)";
      else       ylabel = ";#leftarrow south    z (cm)    north #rightarrow;y (cm)";
    }

    delete fHisto; fHisto = 0;    
    if (fXorY == kX){
      fHisto = new THUnZoomable("fTZXHisto",
                                xlabel,
                                2000, 
                                zlo,
                                zhi,
                                1000,
                                -kTsf*geo->Geo()->WorldWidth(),
                                +kTsf*geo->Geo()->WorldWidth());
    }
    if (fXorY == kY){    
      fHisto = new THUnZoomable("fTZYHisto",
                                ylabel,
                                2000, 
                                zlo,
                                zhi,
                                1000,
                                -kTsf*geo->Geo()->WorldHeight(),
                                +kTsf*geo->Geo()->WorldHeight());
    }

    fHisto->GetYaxis()->CenterTitle();
    fHisto->GetYaxis()->SetNdivisions(505);
    fHisto->SetTitleOffset(0.5, "Y");
  
    fHisto->GetXaxis()->SetNoExponent();
  }

} // end namespace evd
////////////////////////////////////////////////////////////////////////
