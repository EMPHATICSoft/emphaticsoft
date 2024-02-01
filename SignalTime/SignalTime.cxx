////////////////////////////////////////////////////////////////////////
/// \brief   SignalTime class for loading V1720 expected signal time
/// \author  jmirabit@bu.ed
/// \date
////////////////////////////////////////////////////////////////////////
#include "SignalTime.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>

namespace emph{
namespace st{

  //----------------------------------------------------------------------
  
  SignalTime::SignalTime()
  {
    _fIsLoaded = false;
    for (int i=0; i<48; ++i) {_signaltime[i] = -999; }
  }


  //----------------------------------------------------------------------
  void SignalTime::LoadMap(int fRun)
  {
    std::ifstream SignalTimeFile; 
    std::string file_path = getenv ("CETPKG_SOURCE");
    file_path = file_path + "/ConstBase/Calibration/SignalTimes.dat";
    SignalTimeFile.open(file_path);
    std::cout<<"Opening Signal Time Map from "<<file_path<<std::endl;
    if (!SignalTimeFile.is_open()) {
        std::cout<<"Error: Signal Time Map not found"<<std::endl;
        std::abort();
    }

    std::string line;
    std::string label;
    int val;
    int index =0;

    while (getline(SignalTimeFile, line)){
        std::stringstream lineStr(line);
        lineStr >> label;
        if (label[0]=='#') continue; //skip commented out lines
        if (label=="Run"){
            index = 0;
            lineStr >> val;
            if (fRun < val) break; //exit map
        }
        if (label=="V1720Index") continue; //skip index labels
        if (label=="SignalTime"){
            while (lineStr >> val){
                _signaltime[index]=val;
                index+=1;
            }
        }
    }
    SignalTimeFile.close();
    for( int i=0; i<48; ++i){
    }
    _fIsLoaded=true;
  }
}
}


