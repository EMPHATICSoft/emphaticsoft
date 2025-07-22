// \brief: An evd::UserHighlight lets ANYONE highlight objects in the event display
//       from ANYWHERE in the framework without recompiling the event display.
//       You can also add a short text comment that will be displayed when
//       the object is moused over.
// \author: Andrew Olivier aolivie4@nd.edu
// \date: 9/12/2024

//emphaticsoft includes
#include "EventDisplay/WebDisplay/UserHighlight.h"

namespace evd
{
  UserHighlight::UserHighlight(): fHexColor(0), fComment()
  {
  }

  UserHighlight::UserHighlight(const uint32_t hexColor, const std::string& comment): fHexColor(hexColor), fComment(comment)
  {
  }

  const std::string& UserHighlight::Comment() const
  {
    return fComment;
  }

  const uint32_t UserHighlight::Color() const
  {
    return fHexColor;
  }

  void UserHighlight::SetComment(const std::string& comment)
  {
    fComment = comment;
  }

  void UserHighlight::SetColor(const uint32_t color)
  {
    fHexColor = color;
  }
} //namespace evd
