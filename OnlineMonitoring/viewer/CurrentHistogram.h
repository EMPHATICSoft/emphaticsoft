#ifndef OM_CURRENTHISTOGRAM_H
#define OM_CURRENTHISTOGRAM_H
#include <string>
#include <deque>

namespace om {
  ///
  /// Hold the current and past state of the current histogram setting.
  ///
  class CurrentHistogram 
  {
  public:
    CurrentHistogram();
    
    int         Set(const char* nm);
    int         Forward();
    int         Reload();
    int         Backward();

    const char* Current()    const;
    bool        ForwardOK()  const;
    bool        BackwardOK() const;
    bool        ReloadOK()   const;
    
  private:
    ///< How deep do we allow the history to go?
    static const unsigned int kMaxHistory=16;
    
  private:
    std::deque<std::string>           fHistory;
    std::deque<std::string>::iterator fPlace;
  };
}

#endif
////////////////////////////////////////////////////////////////////////
