////////////////////////////////////////////////////////////////////////
/// \file  RootEnv.h
/// \brief Setup the root environment
///
/// \version $Id: RootEnv.h,v 1.2 2011-04-05 20:50:42 messier Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_ROOTENV_H
#define EVDB_ROOTENV_H
class TRint;

namespace evdb {
  /// Configure the ROOT environment
  class RootEnv {
  public:
    RootEnv(int argc, char** argv);
    ~RootEnv();

    int Run();
    
  private:
    void SetStyle();
    void InterpreterConfig();
    void SignalConfig();
    void LoadIncludes();
    void LoadClasses();
    
  private:
  };
}
#endif // EVDROOTENV_H
////////////////////////////////////////////////////////////////////////
