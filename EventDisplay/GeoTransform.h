///
/// \file GeoTransform.cxx
//
/// \version $Id: Style.h,v 1.1.1.1 2010-11-10 19:44:54 p-novaart Exp $
/// \author messier@indiana.edu
///
#ifndef EVD_GEOTRANSFORM_H
#define EVD_GEOTRANSFORM_H
class TVector3;

namespace evd {
  class GeoTransform {
  public:
    static void XYZ(double* xyz);
    static void XYZ(double* x, double* y, double* z);
    static void XYZ(TVector3& v);
    static void X(double* x);
    static void Y(double* y); 
    static void Z(double* z); 
  };
}
#endif
////////////////////////////////////////////////////////////////////////
