#ifndef PlotOrientation_h
#define PlotOrientation_h
//
// Allowed values of the plot orientation of the event display
//

#include "EventDisplay/EnumToStringSparse.h"

namespace emph {
  
  class PlotOrientationDetail {
  public:
    enum enum_type { unknown = -1, portrait, landscape };
    
    static std::string const& typeName();
    
    static std::map<enum_type, std::string> const& names();
  };
  
  typedef EnumToStringSparse<PlotOrientationDetail> PlotOrientation;
}

#endif
