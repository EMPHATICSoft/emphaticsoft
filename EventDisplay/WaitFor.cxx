//
// Tell the event display where to look for authorization to
// continue to the event, either input from the keyboard
// or from the mouse.
//

#include "EventDisplay/WaitFor.h"

std::string const&
emph::WaitForDetail::typeName()
{
  static std::string type("WaitFor");
  return type;
}

std::map<emph::WaitForDetail::enum_type, std::string> const&
emph::WaitForDetail::names()
{

  static const std::map<enum_type, std::string> nam = {
    {unknown, "unknown"}, {keyboard, "keyboard"}, {mouse, "mouse"}};

  return nam;
}
