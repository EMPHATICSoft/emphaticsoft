#ifndef OM_OSTREAM_H
#define OM_OSTREAM_H
#include <sstream>
namespace emph { namespace onmon { class LogViewer; } }

namespace emph { 
  namespace onmon {
    ///
    /// Define an output stream the streams to the log viewer
    ///
    class OStream {
    public:
      OStream();
      void Flush();
      void SetLogViewer(emph::onmon::LogViewer* lv);

    private:
      std::ostringstream fBuf;
      emph::onmon::LogViewer*     fLogViewer;
      
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
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
