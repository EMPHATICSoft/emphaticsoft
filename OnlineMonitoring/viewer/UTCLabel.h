#ifndef OM_UTCLABEL_H
#define OM_UTCLABEL_H
#include <list>

class TText;
class TLine;

namespace om {
  ///
  /// Draw a nicely formatted overlay on UTC plots
  ///
  class UTCLabel
  {
  public:
    UTCLabel();
    ~UTCLabel();

    void Draw();
    void Clear();

  private:
    std::list<TText*> fText;
    std::list<TLine*> fLine;

  };
}

#endif
////////////////////////////////////////////////////////////////////////
