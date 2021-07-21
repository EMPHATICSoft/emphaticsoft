////////////////////////////////////////////////////////////////////////
/// \brief   Run history data class
/// \author  jpaley@fnal.gov
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
    RunHistory(); // Default constructor
    RunHistory(int run);
    virtual ~RunHistory() {}; //Destructor
    
  private:
    
    bool _isLoaded;
    int _runNumber;
    int _nSubrun;
    double _beamMom;
    std::vector<std::string> _det;
    int _nTrig;

    bool LoadFromDB();

  public:

    // Getters (although data members are public)    
    int        Run() const { return _runNumber; }
    int        NSubrun();
    double     BeamMom();
    std::vector<std::string> DetectorList();
    int        NTriggers();

    // Setters
    bool       SetRun(int run);

  };
  
}

#endif // RUNHISTORY_H
