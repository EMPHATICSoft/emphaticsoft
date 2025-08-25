////////////////////////////////////////////////////////////////////////
/// \brief   RunHistory class
/// \author  jpaley@fnal.gov, wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "RunHistory/RunHistory.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <filesystem>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "Database/include/query_engine_api.h"
//#include "art_cpp_db_interfaces/query_engine_api.h"
#pragma GCC diagnostic pop

namespace runhist{
   
  //----------------------------------------------------------------------
  
  RunHistory::RunHistory(int run) :
    _isLoaded(false),
    _isConfig(false),
    _runNumber(run)
  {
    _QEURL = "";
    _beamMom = 0.;
    _target = "Unknown";
    _useFixedAlignFile = false;
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
  
  std::string RunHistory::SSDAlignFile()
  {
    if (!_isConfig) LoadConfig();
    return _ssdAlignFile;

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

    //    _ssdAlignFile = "";

    if(_runNumber >= 436 && _runNumber <= 605){
      _geoFile=file_path+"Geometry/phase1a.gdml";
      _chanFile=file_path+"ChannelMap/ChannelMap_Jan22_Run436.txt";
      _calibVer=1;
    }
    else if(_runNumber > 605 && _runNumber <= 1386){
      _geoFile=file_path+"Geometry/phase1b.gdml";
      _chanFile=file_path+"ChannelMap/ChannelMap_Jun22.txt";
      _ssdAlignFile=file_path+"Align/SSDAlign_1b.txt";
      _calibVer=2;
    }
    else if(_runNumber >= 2000){
      _geoFile=file_path+"Geometry/phase1c_"+this->Target();
      if (_runNumber < 2179) _geoFile += "_7S";
      if (!_magnetIn) 
	_geoFile += "_nomag.gdml";
      else 
	_geoFile += ".gdml";
      _chanFile=file_path+"ChannelMap/ChannelMap_Mar23.txt";
      if (_useFixedAlignFile) {
	if (_ssdAlignFile.empty())
	  _ssdAlignFile=file_path+"Align/SSDAlign_1c.txt";
      }
      else {
	// find alignment file with nearest run number
	std::string alignPath = file_path+"Align/";
	std::string tfilename;
	std::vector<std::string> alignFile;
	std::vector<int> runs;
	std::string fnamePrefix="SSDAlign_1c_";
	for (const auto& entry : std::filesystem::directory_iterator(alignPath))
	  {
	    // Check if the entry is a regular file
	    if (std::filesystem::is_regular_file(entry.status())) {
	      tfilename = entry.path().filename().string();
	      if (tfilename.find(fnamePrefix) != std::string::npos) {
		try {
		  runs.push_back(std::stoi(tfilename.substr(fnamePrefix.length(),4)));
		  alignFile.push_back(tfilename);
		} 
		catch (const std::invalid_argument& e) {}
		catch (const std::out_of_range& e) {}	     
	      }
	      
	    }	    
	  }
	std::sort(alignFile.begin(),alignFile.end());
	std::sort(runs.begin(),runs.end());
	for (size_t ir=0; ir<runs.size() && runs[ir]<=_runNumber; ++ir) {
	  _ssdAlignFile=file_path+"Align/"+alignFile[ir];
	}
	std::cout << "Using " << _ssdAlignFile << " for SSD alignment." << std::endl;
      }	
      _calibVer=2;
      std::cout << "%%%%% SSD alignment file = " << _ssdAlignFile << std::endl;
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

    QueryEngine<std::string,double,std::string,std::string> runquery(_QEURL,"emphatic_prd","emph","runs","nsubruns","momentum","target","magnet_in");
    runquery.where("run","eq",_runNumber);

    auto result = runquery.get();

    if (result.size() == 0) return false;
    std::string magStr;

    for (auto& row : result) {
      //      std::cout << "(" << column<0>(row) << "," << column<1>(row) << "," << column<2>(row) << ")" << std::endl;
      if (!strcmp(column<0>(row).c_str(),"None"))
	_nSubrun = -1;
      else
	_nSubrun = stoi(column<0>(row));
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
