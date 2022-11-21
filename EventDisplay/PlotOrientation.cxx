//
// Allowed values of the instance names for Intersection objects,
// as both enum values and strings.
//

#include "EventDisplay/PlotOrientation.h"

std::string const&
emph::PlotOrientationDetail::typeName()
{
  static std::string type("PlotOrientation");
  return type;
}

std::map<emph::PlotOrientationDetail::enum_type, std::string> const&
emph::PlotOrientationDetail::names()
{

  static const std::map<enum_type, std::string> nam = {
    {unknown, "unknown"}, {portrait, "portrait"}, {landscape, "landscape"}};

  return nam;
}
