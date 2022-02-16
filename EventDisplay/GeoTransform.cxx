///
/// \file GeoTransform.cxx
//
/// \version $Id: Style.h,v 1.1.1.1 2010-11-10 19:44:54 p-novaart Exp $
/// \author messier@indiana.edu
///
#include "TVector3.h"
#include "EventDisplay/GeoTransform.h"
#include "EventDisplayServices/GeometryDrawingOptions.h"

using namespace evd;

void GeoTransform::XYZ(double* xyz) 
{
  GeoTransform::XYZ(xyz,xyz+1,xyz+2);
}

//......................................................................

void GeoTransform::XYZ(double* x, double* y, double* z) 
{
  GeoTransform::X(x);
  GeoTransform::Y(y);
  GeoTransform::Z(z);
}

//......................................................................

void GeoTransform::XYZ(TVector3& v)
{
  art::ServiceHandle<evd::GeometryDrawingOptions> geoopt;
  
  bool flipx = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_X;
  bool flipy = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_Y;
  bool flipz = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_Z;
  
  if (flipx) v.SetX(-v.X());
  if (flipy) v.SetY(-v.Y());
  if (flipz) v.SetZ(-v.Z());
}

//......................................................................

void GeoTransform::X(double* x) 
{
  art::ServiceHandle<evd::GeometryDrawingOptions> geoopt;
  bool flipx = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_X;
  if (flipx) *x = -(*x);
}

//......................................................................

void GeoTransform::Y(double* y) 
{
  art::ServiceHandle<evd::GeometryDrawingOptions> geoopt;
  bool flipy = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_Y;
  if (flipy) *y = -(*y);
}

//......................................................................

void GeoTransform::Z(double* z) 
{
  art::ServiceHandle<evd::GeometryDrawingOptions> geoopt;
  bool flipz = geoopt->fFlip & evd::GeometryDrawingOptions::kFLIP_Z;
  if (flipz) *z = -(*z);
}

////////////////////////////////////////////////////////////////////////
