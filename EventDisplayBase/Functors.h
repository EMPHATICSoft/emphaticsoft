////////////////////////////////////////////////////////////////////////
// $Id: Functors.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
//
// Define a set of useful functions for managing lists of drawable
// objects
//
// messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDFUNCTORS_H
#define EVDFUNCTORS_H
#include "TObject.h"
#include "TMarker.h"
#include "TPolyMarker.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TArc.h"
#include "TBox.h"
#include "TMarker3DBox.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TText.h"
#include "TLatex.h"

#include <iostream>

struct draw_tobject      {
  void operator()(TObject* x) {
    // Do not draw poly lines if they have no points. Otherwise ROOT crashes
    if (dynamic_cast<TPolyLine*>(x)) {
      if (dynamic_cast<TPolyLine*>(x)->GetN() < 2) return;
    }
    else if (dynamic_cast<TPolyLine3D*>(x)) {
      if (dynamic_cast<TPolyLine3D*>(x)->GetN() < 2) return;
    }

    x->Draw();
  }
};

struct delete_marker     {void operator()(TMarker* x)     { delete x; }};
struct delete_polymarker {void operator()(TPolyMarker* x) { delete x; }};
struct delete_line       {void operator()(TLine* x)       { delete x; }};
struct delete_polyline   {void operator()(TPolyLine* x)   { delete x; }};
struct delete_arc        {void operator()(TArc* x)        { delete x; }};
struct delete_box        {void operator()(TBox* x)        { delete x; }};
struct delete_text       {void operator()(TText* x)       { delete x; }};
struct delete_latex      {void operator()(TLatex* x)      { delete x; }};

struct delete_marker3dbox {void operator()(TMarker3DBox* x) {delete x;}};
struct delete_polymarker3d{void operator()(TPolyMarker3D* x){delete x;}};
struct delete_polyline3d  {void operator()(TPolyLine3D* x)  {delete x;}};

#endif
////////////////////////////////////////////////////////////////////////
