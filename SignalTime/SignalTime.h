////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SignalTime functions 
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SignalTime_H
#define SignalTime_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace emph {
namespace st {

  class SignalTime {

  public:

    SignalTime(); //Default constructor
    ~SignalTime() {}; //Destructor

  private:

    bool _fIsLoaded;
    int _signaltime[48];

  public:
    // Getters
    const bool IsTimeMapLoaded() const { return _fIsLoaded; }
    const int* TimeMap() const { return _signaltime; }

    int SigTime(const int& index) const {return _signaltime[index]; }
    int SigTime(const int& board, const int& channel) const{
        int index = board*8 + channel;
        return _signaltime[index];
    }
    void LoadMap(int fRun);
  };

}
}

#endif // SignalTime_H
