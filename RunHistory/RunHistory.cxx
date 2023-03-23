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
#include "Database/include/query_engine_api.h"
//#include "art_cpp_db_interfaces/query_engine_api.h"
#pragma GCC diagnostic pop

namespace runhist{
   
  //----------------------------------------------------------------------
  /*  
  RunHistory::RunHistory() :   
    _isLoaded(false), _isConfig(false), _runNumber(0), _nSubrun(0), _beamMom(0.), _geoFile(""), _chanFile(""), _alignFile(""), _calibVer(0), _nTrig(0), _QEURL("")
  {
    _det.clear();

  }
  */

  //----------------------------------------------------------------------
  
  RunHistory::RunHistory(int run) :
    _isLoaded(false),
    _isConfig(false),
    _runNumber(run)
  {
    _QEURL = "";
    _beamMom = 0.;
    _target = "Unknown";

  }
  
  //----------------------------------------------------------------------
  
  bool RunHistory::SetRun(int run)
  {
    if (run != _runNumber) {
      _runNumber = run;
      _isLoaded = false;
      _isConfig = false;
      _beamMom = 0.;
      _target = "Unknown";
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
    if (!_isLoaded) LoadFromDB();
    //assert(LoadFromDB());
    return _beamMom;

  }

  //----------------------------------------------------------------------
  
  std::string RunHistory::Target()
  {
    if (!_isLoaded) LoadFromDB();
    //assert(LoadFromDB());
    return _target;

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
  
  std::string RunHistory::AlignFile()
  {
    if (!_isConfig) LoadConfig();
    return _alignFile;

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

	  std::string file_path;
	  file_path = getenv ("CETPKG_SOURCE");
	  file_path = file_path + "/ConstBase/" ;

	  if(_runNumber >= 436 && _runNumber <= 605){
		  _geoFile=file_path+"Geometry/phase1a.gdml";
		  _chanFile=file_path+"ChannelMap/ChannelMap_Jan22_Run436.txt";
          //_alignFile=file_path+"Calibration/SSDAlignment.dat"; //No alignment file for run 1a yet
		  _calibVer=1;
	  }
	  else if(_runNumber > 605 && _runNumber <= 1386){
		  _geoFile=file_path+"Geometry/phase1b.gdml";
		  _chanFile=file_path+"ChannelMap/ChannelMap_Jun22.txt";
          _alignFile=file_path+"Calibration/SSDAlignment.dat";
		  _calibVer=2;
	  }
	  else{
		  std::cout << "Run " << _runNumber << " is not in the database." << std::endl;
		  std::abort();
	  }

	  return true;
  }

  //----------------------------------------------------------------------

  bool RunHistory::LoadFromDB()
  {
    if (_QEURL.empty()) return false;

    QueryEngine<int,double,std::string,std::string> runquery(_QEURL,"emphatic_prd","emph","runs","nsubruns","momentum","target","magnet_in");
    runquery.where("run","eq",_runNumber);

    auto result = runquery.get();

    if (result.size() == 0) return false;
    std::string magStr;

    for (auto& row : result) {
      //      std::cout << "(" << column<0>(row) << "," << column<1>(row) << "," << column<2>(row) << ")" << std::endl;
      _nSubrun = column<0>(row);
      _beamMom = column<1>(row);
      _target = column<2>(row);
      magStr = column<3>(row);
      if (magStr == "True")
	_magnetIn = true;
      else
	_magnetIn = false;
    }
    
    _isLoaded = true;
    return true;

  }
  
} // end namespace runhist

//////////////////////////////////////////////////////////////////////////////
