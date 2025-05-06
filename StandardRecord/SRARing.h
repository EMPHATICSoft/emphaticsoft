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
  };

} // end namespace

#endif // SRARING_H
