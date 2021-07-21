////////////////////////////////////////////////////////////////////////
/// \brief   RunHistory class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RunHistory/RunHistory.h"

#include <iomanip>
#include <iostream>
#include <cassert>

#include "art_cpp_db_interfaces/query_engine_api.h"

namespace runhist{
  
  //----------------------------------------------------------------------
  
  RunHistory::RunHistory() :   
    _isLoaded(false), _runNumber(0), _nSubrun(0), _beamMom(0.), _nTrig(0)
  {
    _det.clear();
  }
  
  //----------------------------------------------------------------------
  
  RunHistory::RunHistory(int run) :
    _isLoaded(false),
    _runNumber(run)
  {

  }
  
  //----------------------------------------------------------------------
  
  bool RunHistory::SetRun(int run)
  {
    if (run != _runNumber) {
      _runNumber = run;
      _isLoaded = false;
    }

    return true;
  }
  
  //----------------------------------------------------------------------
  
  int RunHistory::NSubrun()
  {
    if (!_isLoaded) assert(LoadFromDB());
    return _nSubrun;

  }

  //----------------------------------------------------------------------
  
  double RunHistory::BeamMom()
  {
    if (!_isLoaded) assert(LoadFromDB());
    return _beamMom;

  }

  //----------------------------------------------------------------------
  
  std::vector<std::string> RunHistory::DetectorList()
  {
    if (!_isLoaded) assert(LoadFromDB());
    return _det;

  }

  //----------------------------------------------------------------------
  
  int RunHistory::NTriggers()
  {
    if (!_isLoaded) assert(LoadFromDB());
    return _nTrig;

  }

  //----------------------------------------------------------------------

  bool RunHistory::LoadFromDB()
  {
    _isLoaded = true;
    return true;
  }
  
} // end namespace runhist

//////////////////////////////////////////////////////////////////////////////
