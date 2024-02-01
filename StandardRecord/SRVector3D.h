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
      SRVector3D(float x, float y, float z);
      /// Easy conversion from TVector3
      SRVector3D(const TVector3& v);

      void SetXYZ(float x, float y, float z);

      /// Easy conversion back to TVector3
      operator TVector3() const;

      void SetX(float _x){x = _x;}
      void SetY(float _y){y = _y;}
      void SetZ(float _z){z = _z;}

      float X() const {return x;}
      float Y() const {return y;}
      float Z() const {return z;}

      // The more common TVector3 operations, mostly for use with TTree::Draw
      //
      // NB: you need to specify the initial "rec." when using these
      float Mag2() const {return x*x+y*y+z*z;}
      float Mag() const {return sqrt(Mag2());}
      float Dot(const SRVector3D& v) const {return x*v.x + y*v.y + z*v.z;}
      SRVector3D Unit() const
      {
	const float m = Mag();
	return SRVector3D(x/m, y/m, z/m);
      }
#endif
      
      float x;
      float y;
      float z;
    };

} // end namespace

#ifndef __castxml__
std::ostream &operator<<(std::ostream &stream, const caf::SRVector3D &vec);
#endif

#endif // SRVECTOR3D_H
/////////////////////////////////////////////////////////////////////////////
