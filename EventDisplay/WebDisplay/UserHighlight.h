// \brief: An evd::UserHighlight lets ANYONE highlight objects in the event display
//       from ANYWHERE in the framework without recompiling the event display.
//       You can also add a short text comment that will be displayed when
//       the object is moused over.
// \author: Andrew Olivier aolivie4@nd.edu
// \date: 9/12/2024

#ifndef EVD_USERHIGHLIGHT_H
#define EVD_USERHIGHLIGHT_H

//c++ includes
#include <string>

namespace evd
{
  class UserHighlight
  {
    public:
      UserHighlight();
      UserHighlight(const short int hexColor, const std::string& comment);
      virtual ~UserHighlight() = default;

      // Getters
      const std::string& Comment() const;
      const short int Color() const;

      // Setters
      void SetComment(const std::string& comment);
      void SetColor(const short int color);

    private:
      short int fHexColor; //32-bit "web color".  Last 4 bits represent transparency
      std::string fComment; //Text that the event display will show when object is highlighted with the mouse
  };
}

#endif //EVD_USERHIGHLIGHT_H
