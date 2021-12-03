#ifndef OM_OSTREAM_H
#define OM_OSTREAM_H
#include <sstream>
namespace om { class LogViewer; }

namespace om {
  ///
  /// Define an output stream the streams to the log viewer
  ///
  class OStream {
  public:
    OStream();
    void Flush();
    void SetLogViewer(om::LogViewer* lv);

  private:
    std::ostringstream fBuf;
    om::LogViewer*     fLogViewer;
    
  public:
    template <class T> OStream& operator<<(const T& a) {
      fBuf.str("");
      fBuf << a;
      this->Flush();
      return *this;
    }
  };
  
  extern OStream cout;
  extern OStream cerr;
}
#endif
////////////////////////////////////////////////////////////////////////
