// Extra utility functions from SRVector3DAddons.h

float X() const {return x;}
float Y() const {return y;}
float Z() const {return z;}

operator TVector3() const {return TVector3(x, y, z);}

float Mag2() const {return x*x+y*y+z*z;}
float Mag() const {return sqrt(Mag2());}
float Dot(const Proxy& v) const {return x*v.x + y*v.y + z*v.z;}
float Dot(const TVector3& v) const {return x*v.X() + y*v.Y() + z*v.Z();}
TVector3 Unit() const
{
  const float m = Mag();
  return TVector3(x/m, y/m, z/m);
}
