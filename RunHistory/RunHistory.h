////////////////////////////////////////////////////////////////////////
/// \brief   Run history data class
/// \brief   loading run info, calibration, geometry, and channelmap
/// \author  jpaley@fnal.gov, wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef RUNHISTORY_H
#define RUNHISTORY_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <string>

namespace runhist {
  
  class RunHistory {
  public:
    //    RunHistory(); // Default constructor
    explicit RunHistory(int run);
    virtual ~RunHistory() {}; //Destructor
    
  private:
    
    bool _isLoaded;
    bool _isConfig;
    bool _magnetIn;
    int _runNumber;
    int _nSubrun;
    double _beamMom;
    std::string  _geoFile;
    std::string  _chanFile;
    std::string  _ssdAlignFile;
    std::string  _target;
    int  _calibVer;
    std::vector<std::string> _det;
    int _nTrig;
    std::string _QEURL;

    bool LoadFromDB();
    bool LoadConfig();

  public:

    // Getters (although data members are public)    
    int        Run() const { return _runNumber; }
    int        NSubrun();
    double     BeamMom();
    std::string Target();
    std::string GeoFile();
    std::string ChanFile();
    std::string SSDAlignFile();
    int        CalibVer();
    std::vector<std::string> DetectorList();
    int        NTriggers();
    
    // Setters
    bool       SetRun(int run);
    void       SetGeo(std::string geoFile) { _geoFile = geoFile; }
    void       SetChan(std::string chanFile) { _chanFile = chanFile; }
    void       SetCalib(int calibVer) { _calibVer = calibVer; }
    void       SetQEURL(std::string uri) { _QEURL = uri; }
    void       SetSSDAlign(std::string ssdFile) { _ssdAlignFile = ssdFile; }
  };
  
}

#endif // RUNHISTORY_H
