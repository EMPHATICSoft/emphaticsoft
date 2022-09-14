////////////////////////////////////////////////////////////////////////
/// \brief   RunHistory class
/// \author  jpaley@fnal.gov, wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "RunHistory/RunHistory.h"

#include <iomanip>
#include <iostream>
#include <cassert>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "art_cpp_db_interfaces/query_engine_api.h"
#pragma GCC diagnostic pop

namespace runhist{
   
  //----------------------------------------------------------------------
  
  RunHistory::RunHistory() :   
    _isLoaded(false), _isConfig(false), _runNumber(0), _nSubrun(0), _beamMom(0.), _geoFile(""), _chanFile(""), _calibVer(0), _nTrig(0), _QEURL("")
  {
    _det.clear();

  }
  
  //----------------------------------------------------------------------
  
  RunHistory::RunHistory(int run) :
    _isLoaded(false),
    _isConfig(false),
    _runNumber(run)
  {
    _QEURL = "";
    
  }
  
  //----------------------------------------------------------------------
  
  bool RunHistory::SetRun(int run)
  {
    if (run != _runNumber) {
      _runNumber = run;
      _isLoaded = false;
      _isConfig = false;
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
  
  std::string RunHistory::GeoFile()
  {
    if (!_isConfig) LoadConfig();
    return _geoFile;

  }

  //----------------------------------------------------------------------
  
  std::string RunHistory::ChanFile()
  {
    if (!_isConfig) LoadConfig();
    return _chanFile;

  }

  //----------------------------------------------------------------------
  
  int RunHistory::CalibVer()
  {
    if (!_isConfig) LoadConfig();
    return _calibVer;

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

  bool RunHistory::LoadConfig()
  {

	 if(_runNumber >= 436 && _runNumber <= 605){
		 _geoFile="phase1a.gdml";
		 _chanFile="ChannelMap_Jan22_Run436.txt";
		 _calibVer=1;
	 }
	 else if(_runNumber >= 605 && _runNumber <= 1385){
		 _geoFile="phase1b.gdml";
		 _chanFile="ChannelMap_Jun22.txt";
		 _calibVer=2;
	 }
	 else{
		 std::cout << "Run " << _runNumber << " is not in the database." << std::endl;
	    std::abort();
	 }

	 return true;
  }

  bool RunHistory::LoadFromDB()
  {

    if (_QEURL.empty()) return false;

    QueryEngine<int,int,double> runquery(_QEURL,"emph_prod","runs","nsubrun","ntrig","beammom");
    runquery.where("run","eq",_runNumber);

    auto result = runquery.get();

    for (auto& row : result) {
      //      std::cout << "(" << column<0>(row) << "," << column<1>(row) << "," << column<2>(row) << ")" << std::endl;
      _nSubrun = column<0>(row);
      _nTrig  = column<1>(row);
      _beamMom = column<2>(row);

    }

	 _isLoaded = true;
    return true;

  }
  
} // end namespace runhist

//////////////////////////////////////////////////////////////////////////////
