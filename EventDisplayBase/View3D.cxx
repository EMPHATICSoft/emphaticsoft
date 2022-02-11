#include <algorithm>
#include "EventDisplayBase/View3D.h"
#include "EventDisplayBase/Functors.h"

namespace evdb{

  //......................................................................

  std::list<TMarker3DBox*>  View3D::fgMarker3DBoxL;
  std::list<TPolyMarker3D*> View3D::fgPolyMarker3DL;
  std::list<TPolyLine3D*>   View3D::fgPolyLine3DL;
  std::list<TText*>         View3D::fgText3DL;

  //......................................................................

  View3D::View3D() 
  {
  }

  //......................................................................

  View3D::~View3D() 
  {
    // Make sure to return all our objects to where they came from
    Clear();
  }

  //......................................................................

  void View3D::Draw()
  {
    for_each(fMarker3DBoxL.begin(), fMarker3DBoxL.end(), draw_tobject());
    for_each(fPolyMarker3DL.begin(),fPolyMarker3DL.end(),draw_tobject());
    for_each(fPolyLine3DL.begin(),  fPolyLine3DL.end(),  draw_tobject());
    for_each(fText3DL.begin(),      fText3DL.end(),      draw_tobject());
  }

  //......................................................................

  void View3D::Clear() 
  {
    // Empty each of our lists, appending them back onto the static ones
    fgMarker3DBoxL.splice(fgMarker3DBoxL.end(), fMarker3DBoxL);
    fgPolyMarker3DL.splice(fgPolyMarker3DL.end(), fPolyMarker3DL);
    fgPolyLine3DL.splice(fgPolyLine3DL.end(), fPolyLine3DL);
    fgText3DL.splice(fgText3DL.end(), fText3DL);
  }

  //......................................................................

  TMarker3DBox& View3D::AddMarker3DBox(double x,  double y,  double z,
				       double dx, double dy, double dz,
				       double th, double ph)
  {
    // See comment in View2D::AddMarker()
    TMarker3DBox* m = 0;
    if(fgMarker3DBoxL.empty()){
      m = new TMarker3DBox(x,y,z,dx,dy,dz,th,ph);
      m->SetBit(kCanDelete,kFALSE);
    }
    else {
      m = fgMarker3DBoxL.back();
      fgMarker3DBoxL.pop_back();

      m->SetPosition(x,y,z);
      m->SetSize(dx,dy,dz);
    }

    fMarker3DBoxL.push_back(m);
    return *m;
  }

  //......................................................................

  TPolyMarker3D& View3D::AddPolyMarker3D(int n, int c, int st, double sz)
  {
    TPolyMarker3D* pm = 0;
    if(fgPolyMarker3DL.empty()){
      pm = new TPolyMarker3D(n);
      pm->SetBit(kCanDelete,kFALSE);
      pm->SetMarkerColor(c);
      pm->SetMarkerStyle(st);
      pm->SetMarkerSize(sz);
    }
    else {
      pm = fgPolyMarker3DL.back();
      fgPolyMarker3DL.pop_back();

      // The first call to SetPolyMarker3D with the 0
      // deletes the current set of points before trying
      // to make a new set
      pm->SetPolyMarker(0,(double*)0,1,"");
      pm->SetPolyMarker(n,(double*)0,1,"");
      pm->SetMarkerColor(c);
      pm->SetMarkerStyle(st);
      pm->SetMarkerSize(sz);
    }

    fPolyMarker3DL.push_back(pm);
    return *pm;
  }

  //......................................................................

  TPolyLine3D& View3D::AddPolyLine3D(int n, int c, int w, int s)
  {
    TPolyLine3D* pl = 0;
    if(fgPolyLine3DL.empty()){
      pl = new TPolyLine3D(n);
      pl->SetBit(kCanDelete,kFALSE);
      pl->SetLineColor(c);
      pl->SetLineWidth(w);
      pl->SetLineStyle(s);
    }
    else {
      pl = fgPolyLine3DL.back();
      fgPolyLine3DL.pop_back();

      // The first call to SetPolyMarker3D with the 0
      // deletes the current set of points before trying
      // to make a new set
      pl->SetPolyLine(0,(double*)0,"");
      pl->SetPolyLine(n,(double*)0,"");
      pl->SetLineColor(c);
      pl->SetLineWidth(w);
      pl->SetLineStyle(s);
    }

    fPolyLine3DL.push_back(pl);
    return *pl;
  }

  //......................................................................

  TText& View3D::AddText(double x, double y, const char* text)
  {
    TText* itxt = 0;
    if(fgText3DL.empty()){
      itxt = new TText(x,y,text);
      itxt->SetBit(kCanDelete,kFALSE);
    }
    else {
      itxt = fgText3DL.back();
      fgText3DL.pop_back();

      itxt->SetText(x,y,text);
    }

    fText3DL.push_back(itxt);
    return *itxt;
  }

} // namespace
////////////////////////////////////////////////////////////////////////
