///
/// \file    GeometryDrawer.cxx
/// \brief   Class to aid in the rendering of Geometry objects
/// \author  messier@indiana.edu
/// \version $Id: GeometryDrawer.cxx,v 1.12 2012-12-05 02:24:46 bckhouse Exp $
///
#include "TBox.h"
#include "TLine.h"
#include "TMarker3DBox.h"
#include "TPolyLine.h"
#include "TPolyLine3D.h"
#include "TText.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "EventDisplay/GeometryDrawer.h"
#include "EventDisplay/GeoTransform.h"
#include "EventDisplayServices/GeometryDrawingOptions.h"
#include "EventDisplayBase/View2D.h"
#include "EventDisplayBase/View3D.h"
#include "EventDisplayBase/Colors.h"
//#include "Geometry/Geometry.h"
#include "Geometry/GeometryService.h"

using namespace evd;

//......................................................................

GeometryDrawer::GeometryDrawer() { }

//......................................................................

GeometryDrawer::~GeometryDrawer() { }

//......................................................................
/*
void GeometryDrawer::GetBox(double* xlo, double* ylo, double* zlo,
			    double* xhi, double* yhi, double* zhi)
{
  art::ServiceHandle<geo::Geometry> geo;
  
  //
  // Explore the cell locations to find the smallest box that contains
  // them all in the x-z and y-z views
  //
  unsigned int i, j;
  *xlo   =  1e9;
  *ylo   =  1e9;
  *zlo   =  1e9;
  *xhi   = -1e9;
  *yhi   = -1e9;
  *zhi   = -1e9;
  for (i=0; i<geo->NPlanes(); ++i) {
    const geo::PlaneGeo* p = geo->Plane(i);
    for (j=0; j<p->Ncells(); ++j) {
      const geo::CellGeo* c = p->Cell(j);
      if (c==0) continue;
      double xyz[3];
      c->GetCenter(xyz);
      if (p->View()==geo::kX) {
	if (xyz[0]-c->HalfW()<*xlo) *xlo = xyz[0]-c->HalfW();
	if (xyz[0]+c->HalfW()>*xhi) *xhi = xyz[0]+c->HalfW();
      }
      else if (p->View()==geo::kY) {
	if (xyz[1]-c->HalfW()<*ylo) *ylo = xyz[1]-c->HalfW();
	if (xyz[1]+c->HalfW()>*yhi) *yhi = xyz[1]+c->HalfW();
      }
      if (xyz[2]-c->HalfW()<*zlo) *zlo = xyz[2]-c->HalfD();
      if (xyz[2]+c->HalfW()>*zhi) *zhi = xyz[2]+c->HalfD();
    }
  }
  double wallt = 0.3; // PVC wall thickness in cm
  *xlo -= wallt;
  *ylo -= wallt;
  *zlo -= wallt;
  *xhi += wallt;
  *yhi += wallt;
  *zhi += wallt;
}  
*/

//......................................................................

void GeometryDrawer::DetOutline2D(const art::Event& , // evt,
				  evdb::View2D*        xzview,
				  evdb::View2D*        yzview)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  if ( (drawopt->fOutline & 
	evd::GeometryDrawingOptions::kOUTLINE_DETECTOR)==0) {
    return;
  }
  
  art::ServiceHandle<emph::geo::GeometryService> geo;

  double xlo, ylo, zlo, xhi, yhi, zhi;
  xlo = -1.*geo->Geo()->WorldWidth();
  ylo = -1.*geo->Geo()->WorldHeight();
  xhi = -1.*xlo;
  yhi = -1.*ylo;
  zlo = -1.*geo->Geo()->WorldLength();
  zhi = -1.*zlo;

  //  this->GetBox(&xlo, &ylo, &zlo, &xhi, &yhi, &zhi);
  GeoTransform::XYZ(&xlo, &ylo, &zlo);
  GeoTransform::XYZ(&xhi, &yhi, &zhi);
  
  int c = 16;
  int s = 1;
  int w = 1;
  if (xzview) {
    TPolyLine& l = xzview->AddPolyLine(5, c, w, s);
    l.SetPoint(0, zlo, xlo);
    l.SetPoint(1, zhi, xlo);
    l.SetPoint(2, zhi, xhi);
    l.SetPoint(3, zlo, xhi);
    l.SetPoint(4, zlo, xlo);
    l.SetBit(kCannotPick);
  }
  if (yzview) {
    TPolyLine& l = yzview->AddPolyLine(5, c, w, s);
    l.SetPoint(0, zlo, ylo);
    l.SetPoint(1, zhi, ylo);
    l.SetPoint(2, zhi, yhi);
    l.SetPoint(3, zlo, yhi);
    l.SetPoint(4, zlo, ylo);
    l.SetBit(kCannotPick);
  }
}

//......................................................................

void GeometryDrawer::DetOutline3D(const art::Event& , // *evt,
				  evdb::View3D*        view)
{
  art::ServiceHandle<emph::geo::GeometryService> geo;
  
  double xlo, ylo, zlo, xhi, yhi, zhi;
  xlo = -1.*geo->Geo()->WorldWidth();
  ylo = -1.*geo->Geo()->WorldHeight();
  xhi = -1.*xlo;
  yhi = -1.*ylo;
  zlo = -1.*geo->Geo()->WorldLength();
  zhi = -1.*zlo;

  //  this->GetBox(&xlo, &ylo, &zlo, &xhi, &yhi, &zhi);

  art::ServiceHandle<evdb::Colors> colors;
  bool isBonW = (colors->Foreground(0) == kBlack) ? 1 : 0;

  int c = isBonW ? 1 : 0;
  int s = 1;
  int w = 1;

  TPolyLine3D& top = view->AddPolyLine3D(5, c, w, s);
  top.SetPoint(0, xlo, yhi, zlo);
  top.SetPoint(1, xhi, yhi, zlo);
  top.SetPoint(2, xhi, yhi, zhi);
  top.SetPoint(3, xlo, yhi, zhi);
  top.SetPoint(4, xlo, yhi, zlo);
    
  TPolyLine3D& front = view->AddPolyLine3D(3, c, w, s);
  front.SetPoint(0, xlo, yhi, zlo);
  front.SetPoint(1, xlo, ylo, zlo);
  front.SetPoint(2, xhi, ylo, zlo);
  front.SetPoint(3, xhi, yhi, zlo);
  front.SetPoint(4, xlo, yhi, zlo);
  
  TPolyLine3D& wside = view->AddPolyLine3D(5, c, w, s);
  wside.SetPoint(0, xhi, yhi, zlo);
  wside.SetPoint(1, xhi, ylo, zlo);
  wside.SetPoint(2, xhi, ylo, zhi);
  wside.SetPoint(3, xhi, yhi, zhi);
  wside.SetPoint(4, xhi, yhi, zlo);  

}


//......................................................................
/*
void GeometryDrawer::FiducialVolumeUser2D(const art::Event&,  // evt,
					  evdb::View2D* xzview,
					  evdb::View2D* yzview)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  if ( (drawopt->fOutline & 
	evd::GeometryDrawingOptions::kOUTLINE_FIDUCIAL_USER)==0 ||
       (drawopt->fFiducialBounds).size() !=6 ) {
    return;
  }

  art::ServiceHandle<geo::Geometry> geom;
  std::vector< float > bounds = drawopt->fFiducialBounds;
  TVector3 lo( bounds[0], bounds[1], bounds[2]);
  TVector3 hi( bounds[3], bounds[4], bounds[5]);

  if(xzview){
    TPolyLine& l = xzview->AddPolyLine(5, kBlue-9, 2, 0);
    l.SetPoint(0, lo.Z(), lo.X());
    l.SetPoint(1, hi.Z(), lo.X());
    l.SetPoint(2, hi.Z(), hi.X());
    l.SetPoint(3, lo.Z(), hi.X());
    l.SetPoint(4, lo.Z(), lo.X());
    l.SetBit(kCannotPick);
  }
  if(yzview){
    TPolyLine& l = yzview->AddPolyLine(5, kBlue-9, 2, 0);
    l.SetPoint(0, lo.Z(), lo.Y());
    l.SetPoint(1, hi.Z(), lo.Y());
    l.SetPoint(2, hi.Z(), hi.Y());
    l.SetPoint(3, lo.Z(), hi.Y());
    l.SetPoint(4, lo.Z(), lo.Y());
    l.SetBit(kCannotPick);
  }
}
*/

//......................................................................

/*
void GeometryDrawer::DrawCells2D(const art::Event& evt,
				 evdb::View2D*     xzview,
				 evdb::View2D*     yzview,
				 double xlo, double xhi,
				 double ylo, double yhi,
				 double zlo, double zhi)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  bool draw_cells =
    (drawopt->fOutline&evd::GeometryDrawingOptions::kOUTLINE_CELLS)!=0;
  bool label_cells =
    (drawopt->fLabel&evd::GeometryDrawingOptions::kLABEL_PLANECELL)!=0;
  if (!draw_cells && !label_cells) return;

  art::ServiceHandle<geo::Geometry>         geo;

  unsigned int ip, ic;
  const geo::PlaneGeo* plane;
  const geo::CellGeo*  cell;
  for (ip=0; ip<geo->NPlanes(); ++ip) {
    plane = geo->Plane(ip);
    
    geo::View_t view = plane->View();
    if (view==geo::kX && xzview==0) continue;
    if (view==geo::kY && yzview==0) continue;

    evdb::View2D* v2d = 0;
    if (view==geo::kX && xzview) { v2d = xzview; }
    if (view==geo::kY && yzview) { v2d = yzview; }
    if (v2d==0) continue;
    
    for (ic=0; ic<plane->Ncells(); ++ic ) {
      cell = plane->Cell(ic);

      //
      // Cell location and size
      //
      double pos[3];
      cell->GetCenter(pos);
      GeoTransform::XYZ(pos);

      //
      // Skip channels that are out of view range
      //
      if (pos[2]<zlo || pos[2]>zhi)    continue;
      if (view==geo::kX && pos[0]<xlo) continue;
      if (view==geo::kX && pos[0]>xhi) continue;
      if (view==geo::kY && pos[1]<ylo) continue;
      if (view==geo::kY && pos[1]>yhi) continue;

      //
      // Corner locations
      //
      double halfw = cell->HalfW();
      double halfd = cell->HalfD();
      double x0    = pos[2]-halfd;
      double x1    = pos[2]+halfd;
      double y0    = pos[view]-halfw;
      double y1    = pos[view]+halfw;
      
      if (draw_cells) {
	//
	// Change style for bad channels
	//
	static const int kBox=1;
	static const int kXed=2;
	int style = kBox;
	int color = drawopt->fEnabledColor;
	
	if (style==kBox) {
	  TBox& b = v2d->AddBox(x0, y0, x1, y1);
	  b.SetFillStyle(0);
	  b.SetLineColor(color);
	  b.SetLineStyle(1);
	  b.SetLineWidth(1);
	  b.SetBit(kCannotPick);
	}
	if (style==kXed) {
	  TLine& l1 = v2d->AddLine(x0,y0,x1,y1);
	  TLine& l2 = v2d->AddLine(x0,y1,x1,y0);
	  
	  l1.SetLineColor(color);
	  l1.SetLineStyle(1);
	  l1.SetLineWidth(1);
	  l1.SetBit(kCannotPick);
	  
	  l2.SetLineColor(color);
	  l2.SetLineStyle(1);
	  l2.SetLineWidth(1);
	  l2.SetBit(kCannotPick);
	}
      } // if (draw_cells) ...
      
      if (label_cells && (ip%5)==0) {
	this->ShowCellAndPlaneNumbers(v2d,
				      ip,
				      ic,
				      0.5*(x1+x0),
				      0.5*(x1-x0),
				      0.5*(y1+y0),
				      0.5*(y1-y0));
      }
    } // loop on cells
  } // loop on planes
}
*/

//......................................................................

/*
void GeometryDrawer::DrawHighlightCell(const art::Event&, // evt,
                                       evdb::View2D*     xzview,
                                       evdb::View2D*     yzview)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  const int planeId =  drawopt->fHighlightPlane;
  const int cellId =  drawopt->fHighlightCell;

  art::ServiceHandle<geo::Geometry> geom;

  const geo::PlaneGeo* plane;
  const geo::CellGeo* cell;
  try{
    plane = geom->Plane(planeId);
    if(!plane) return;

    cell = plane->Cell(cellId);
    if(!cell) return;
  }
  catch(...){
    return;
  }

  const geo::View_t view = plane->View();
  evdb::View2D* v2d = view == geo::kX ? xzview : yzview;
  if(!v2d) return;

  // Cell location and size
  double pos[3];
  cell->GetCenter(pos);
  GeoTransform::XYZ(pos);

  const double dx = cell->HalfD();
  const double dy = cell->HalfW();
  const double x0 = pos[2]-dx;
  const double x1 = pos[2]+dx;
  const double y0 = pos[view]-dy;
  const double y1 = pos[view]+dy;

  TBox& b = v2d->AddBox(x0, y0, x1, y1);
  b.SetFillStyle(0);
  b.SetLineColor(kRed);
  b.SetLineStyle(1);
  b.SetLineWidth(1);
  b.SetBit(kCannotPick);
}
*/
//......................................................................
/*
std::map<int, GeometryDrawer::Box> GeometryDrawer::
GetDCMBoxes(geo::View_t view) const
{
  art::ServiceHandle<cmap::CMap>    cmap;
  art::ServiceHandle<geo::Geometry> geo;

  //
  // This map will hold the geographic extent of each DCM
  //
  std::map<int, Box> ret;

  unsigned int ip, ic;
  for (ip=0; ip<geo->NPlanes(); ++ip) {
    if(geo->Plane(ip)->View() != view) continue;
    for (ic=0; ic<geo->Plane(ip)->Ncells(); ++ic) {
      geo::View_t v;
      double pos[3], dpos[3];
      geo->CellInfo(ip, ic, &v, pos, dpos);

      const daqchannelmap::lchan lc  = cmap->Map()->encodeLChan(geo->DetId(),ip,ic);
      const daqchannelmap::dchan dc  = cmap->Map()->encodeDChan(lc);
      const uint32_t             dib = cmap->Map()->getDiBlock(dc);
      const uint32_t             dcm = cmap->Map()->getDCM(dc);

      // Create a unique identifier for this DCM
      unsigned int dcmid = dcm+dib*1000;

      double xylo = pos[v]-dpos[v];
      double zlo  = pos[2]-dpos[2];

      double xyhi = pos[v]+dpos[v];
      double zhi  = pos[2]+dpos[2];

      //
      // If we've never seen this DCM before, initialize its values
      //
      if (ret.find(dcmid)==ret.end()) {
        ret[dcmid].xylo =  9E9;
        ret[dcmid].zlo  =  9E9;
        ret[dcmid].xyhi = -9E9;
        ret[dcmid].zhi  = -9E9;
      }
      //
      // Record the minimum and maximum spatial extent of this DCM
      //
      if (xylo < ret[dcmid].xylo) ret[dcmid].xylo = xylo;
      if (zlo  < ret[dcmid].zlo)  ret[dcmid].zlo  = zlo;
      if (xyhi > ret[dcmid].xyhi) ret[dcmid].xyhi = xyhi;
      if (zhi  > ret[dcmid].zhi)  ret[dcmid].zhi  = zhi;
    }
  }

  return ret;
}
*/
//......................................................................
/*
void GeometryDrawer::DrawDCMBoxes(const art::Event&    evt,
				  evdb::View2D*        xzview,
				  evdb::View2D*        yzview)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  if ( (drawopt->fOutline & 
	evd::GeometryDrawingOptions::kOUTLINE_DCMS) == 0) {
    return;
  }

  art::ServiceHandle<geo::Geometry> geo;
  
  //
  // These maps will hold the geographic extent of each DCM
  //
  static std::map<int, Box> xboxes, yboxes;
  
  static bool ifirst = true;
  
  //
  // First time through, discover the extent of each DCM
  //
  if (ifirst) {
    xboxes = GetDCMBoxes(geo::kX);
    yboxes = GetDCMBoxes(geo::kY);

    ifirst = false;
  }

  for(geo::View_t view: {geo::kX, geo::kY}){
    evdb::View2D* v2d = (view == geo::kX) ? xzview : yzview;
    if(!v2d) continue;

    for(auto it: (view == geo::kX) ? xboxes : yboxes){
      Box box = it.second;

      double ll[3]; // Box position, lower left
      double ur[3]; // Box position, upper right

      ll[view] = box.xylo;
      ur[view] = box.xyhi;
      ll[2] = box.zlo;
      ur[2] = box.zhi;

      GeoTransform::XYZ(ll);
      GeoTransform::XYZ(ur);

      TBox& b = v2d->AddBox(ll[2],ll[view],ur[2],ur[view]);

      b.SetFillStyle(0);
      b.SetLineColor(kGreen-8);
      b.SetBit(kCannotPick);
    }
  }
}
*/
//......................................................................
///
/// Draw text indicating the plane and cell numbers on the display
///
/// \param view - A 2D drawing view
/// \param ip   - Plane number
/// \param ic   - Cell number
/// \param z    - z center of the cell (cm)
/// \param dz   - z width of the cell (cm)
/// \param t    - transverse location (x or y) of the cell (cm)
/// \param dt   - transverse width of the cell (cm)
///
/*
void GeometryDrawer::ShowCellAndPlaneNumbers(evdb::View2D* view,
					     int ip,
					     int ic,
					     double z,
					     double dz,
					     double t,
					     double ) // dt)
{
  int    f = 42;    // Font style
  int    c = 33;    // Text color
  double s = 0.035; // Text size
  
  if (ic%4==0) {
    char buff[256];
    sprintf(buff,"%d-",ic);
    TText& t1 = view->AddText(z-1.1*dz,t, buff);
    t1.SetTextAlign(32);
    t1.SetTextSize(s);
    t1.SetTextFont(f);
    t1.SetTextColor(c);
    t1.SetBit(kCannotPick);
  }
  if ((ic+5)%4==0) {
    char buff[256];
    sprintf(buff,"  -");
    TText& t1 = view->AddText(z-1.1*dz,t, buff);
    t1.SetTextAlign(32);
    t1.SetTextSize(s);
    t1.SetTextFont(f);
    t1.SetTextColor(c);
    t1.SetBit(kCannotPick);
  }
  if ((ic+5)%4==0) {
    char buff[256];
    sprintf(buff,"..%d..",ip);
    TText& t2 = view->AddText(z+1.6*dz,t, buff);
    t2.SetTextAngle(-90);
    t2.SetTextAlign(22);
    t2.SetTextSize(s);
    t2.SetTextFont(f);
    t2.SetTextColor(c);
  }
}
*/

//......................................................................
/*
void GeometryDrawer::DrawBadBoxesExact(const art::Event&, // evt, // less pretty but exactly accurate
                                         evdb::View2D* xzview,
                                         evdb::View2D* yzview)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  if ( (drawopt->fOutline &
	evd::GeometryDrawingOptions::kOUTLINE_BADBOX)==0) {
    return;
  }

  art::ServiceHandle<geo::LiveGeometry> livegeom;

  if(xzview){
    for(int i=0; i < livegeom->NumBadBoxesX(); i++) {
      TPolyLine& l = xzview->AddPolyLine(5, kRed, 2, 0);
      l.SetPoint(0, livegeom->GetBadBoxCorner(false,3,i), livegeom->GetBadBoxCorner(false,1,i));
      l.SetPoint(1, livegeom->GetBadBoxCorner(false,4,i), livegeom->GetBadBoxCorner(false,1,i));
      l.SetPoint(2, livegeom->GetBadBoxCorner(false,4,i), livegeom->GetBadBoxCorner(false,2,i));
      l.SetPoint(3, livegeom->GetBadBoxCorner(false,3,i), livegeom->GetBadBoxCorner(false,2,i));
      l.SetPoint(4, livegeom->GetBadBoxCorner(false,3,i), livegeom->GetBadBoxCorner(false,1,i));
      l.SetBit(kCannotPick);
    }
  }
  if(yzview){
    for(int i=0; i < livegeom->NumBadBoxesY(); i++) {
      TPolyLine& l = yzview->AddPolyLine(5, kRed, 2, 0);
      l.SetPoint(0, livegeom->GetBadBoxCorner(true,3,i), livegeom->GetBadBoxCorner(true,1,i));
      l.SetPoint(1, livegeom->GetBadBoxCorner(true,4,i), livegeom->GetBadBoxCorner(true,1,i));
      l.SetPoint(2, livegeom->GetBadBoxCorner(true,4,i), livegeom->GetBadBoxCorner(true,2,i));
      l.SetPoint(3, livegeom->GetBadBoxCorner(true,3,i), livegeom->GetBadBoxCorner(true,2,i));
      l.SetPoint(4, livegeom->GetBadBoxCorner(true,3,i), livegeom->GetBadBoxCorner(true,1,i));
      l.SetBit(kCannotPick);
    }
  }
}
*/

//......................................................................
/*
void GeometryDrawer::DrawBadBoxesPretty(const art::Event& ,// evt, // Chris's pretty drawer
                                  evdb::View2D* xzview,
                                  evdb::View2D* yzview)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> drawopt;
  if ( (drawopt->fOutline &
	evd::GeometryDrawingOptions::kOUTLINE_BADBOX)==0) {
    return;
  }

  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<geo::LiveGeometry> livegeom;

  static std::map<int, Box> xboxes, yboxes;

  static std::map<int, double> zavg[2]; // [view][dib] -> lower Z

  static bool once = true;
  if(once){
    once = false;

    xboxes = GetDCMBoxes(geo::kX);
    yboxes = GetDCMBoxes(geo::kY);

    // Lines drawn to literal box edges don't join up due to intervening plane
    // in the other view and also tilt of planes. Calculate an average z for
    // each diblock boundary and use that instead.
    for(geo::View_t view: {geo::kX, geo::kY}){
      std::map<int, int> zcount;
      const std::map<int, Box>& boxes = (view == geo::kX) ? xboxes : yboxes;
      for(auto it: boxes){
        const int dib = it.first/1000;
        const Box& box = it.second;
        zavg[view][dib] += box.zlo;
        zavg[view][dib+1] += box.zhi;
        ++zcount[dib];
        ++zcount[dib+1];
      }
      // Divide by the counts to get the average
      for(auto it: zcount) zavg[view][it.first] /= it.second;
    } // end for view
  } // end if once

  for(geo::View_t view: {geo::kX, geo::kY}){
    evdb::View2D* v2d = (view == geo::kX) ? xzview : yzview;
    if(!v2d) continue;

    const std::map<int, Box>& boxes = (view == geo::kX) ? xboxes : yboxes;
    for(auto it: boxes){
      const int dcmid = it.first;
      const int dib = dcmid/1000;
      const int dcm = dcmid%1000;
      const Box box = it.second;

      for(int status: {0, 2}){
        if(livegeom->DCMStatus(dib, dcm) != status) continue;
        const int color = (status == 0) ? kViolet : kRed;

        // What delta in DCM number is "up" on the screen?
        const int updir = (view == geo::kX) ? +1 : -1;

        // For each DCM of the correct state, if the neighbouring DCM is a
        // different state, or is off the edge of the detector, then draw a
        // line there. Together these form the outline of the region.

        // Lower edge
        if(boxes.find(dib*1000+dcm-updir) == boxes.end() ||
           livegeom->DCMStatus(dib, dcm-updir) != status){

          TLine& l = v2d->AddLine(zavg[view][dib],   box.xylo,
                                  zavg[view][dib+1], box.xylo);
          l.SetLineColor(color);
          l.SetLineWidth(1);
          l.SetBit(kCannotPick);
        }

        // Upper edge
        if(boxes.find(dib*1000+dcm+updir) == boxes.end() ||
           livegeom->DCMStatus(dib, dcm+updir) != status){

          TLine& l = v2d->AddLine(zavg[view][dib],   box.xyhi,
                                  zavg[view][dib+1], box.xyhi);
          l.SetLineColor(color);
          l.SetLineWidth(1);
          l.SetBit(kCannotPick);
        }

        // Upstream
        if(boxes.find((dib-1)*1000+dcm) == boxes.end() ||
           livegeom->DCMStatus(dib-1, dcm) != status){

          TLine& l = v2d->AddLine(zavg[view][dib], box.xylo,
                                  zavg[view][dib], box.xyhi);
          l.SetLineColor(color);
          l.SetLineWidth(1);
          l.SetBit(kCannotPick);
        }

        // Downstream
        if(boxes.find((dib+1)*1000+dcm) == boxes.end() ||
           livegeom->DCMStatus(dib+1, dcm) != status){

          TLine& l = v2d->AddLine(zavg[view][dib+1], box.xylo,
                                  zavg[view][dib+1], box.xyhi);
          l.SetLineColor(color);
          l.SetLineWidth(1);
          l.SetBit(kCannotPick);
        }
      } // end for status
    } // end for it (boxes)
  } // end for view
}
*/
////////////////////////////////////////////////////////////////////////
