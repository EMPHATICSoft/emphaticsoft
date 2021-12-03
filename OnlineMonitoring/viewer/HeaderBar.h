#ifndef OM_HEADER_BAR_H
#define OM_HEADER_BAR_H
#include "TGFrame.h"

class TGLabel;
class TTimer;

namespace om {
  class HeaderBar : public TGGroupFrame {
  public:
    HeaderBar(TGWindow* w);
    ~HeaderBar();

    Bool_t HandleTimer(TTimer *timer __attribute__((unused)));

  public:
    TGLabel* fProdLabel;
    TGLabel* fDateLabel;

  private:
    TTimer* fTimer;
  };
}
#endif
////////////////////////////////////////////////////////////////////////
