////////////////////////////////////////////////////////////////////////
// \file    SRARing.h
////////////////////////////////////////////////////////////////////////
#ifndef SRARING_H
#define SRARING_H

namespace caf
{
  /// An SRAring is a simple descriptor for an ARing.
  class SRARing{
  public:
    SRARing();
    virtual  ~SRARing(); 
    float     center[2];  ///< center position of aring [cm?]
    float     radius;  ///< aring radius
    int 	nhit;	
    virtual void setDefault();

    void SetCenter(float _center[2]){for(int p=0;p < 2;p++) this->center[p] = _center[p];}
  };

} // end namespace

#endif // SRARING_H
