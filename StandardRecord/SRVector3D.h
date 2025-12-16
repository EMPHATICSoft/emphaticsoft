#ifndef SRVECTOR3D_H
#define SRVECTOR3D_H

#ifndef __castxml__
#include <cmath>
#include <iostream>
#include "TVector3.h"
#endif

namespace caf
{
  /// A 3-vector with more efficient storage than TVector3
    class SRVector3D
    {
    public:
      SRVector3D();
      virtual ~SRVector3D();

#ifndef __castxml__
      SRVector3D(double x, double y, double z);
      /// Easy conversion from TVector3
      SRVector3D(const TVector3& v);

      void SetXYZ(double x, double y, double z);

      /// Easy conversion back to TVector3
      operator TVector3() const;

      void SetX(double _x){x = _x;}
      void SetY(double _y){y = _y;}
      void SetZ(double _z){z = _z;}

      double X() const {return x;}
      double Y() const {return y;}
      double Z() const {return z;}

      // The more common TVector3 operations, mostly for use with TTree::Draw
      //
      // NB: you need to specify the initial "rec." when using these
      double Mag2() const {return x*x+y*y+z*z;}
      double Mag() const {return sqrt(Mag2());}
      double Dot(const SRVector3D& v) const {return x*v.x + y*v.y + z*v.z;}
      SRVector3D Cross(const SRVector3D& v){
	 double cx = y*v.z - z*v.y;
	 double cy = -(x*v.z - z*v.x);
	 double cz = x*v.y - y*v.x;
	 SRVector3D c(cx,cy,cz);
	 return c;
      }
      SRVector3D Unit() const
      {
	const double m = Mag();
	return SRVector3D(x/m, y/m, z/m);
      }
#endif
      
      double x;
      double y;
      double z;
    };

} // end namespace

#ifndef __castxml__
std::ostream &operator<<(std::ostream &stream, const caf::SRVector3D &vec);
#endif

#endif // SRVECTOR3D_H
/////////////////////////////////////////////////////////////////////////////
