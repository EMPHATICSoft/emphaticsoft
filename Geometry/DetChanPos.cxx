////////////////////////////////////////////////////////////////////////
/// \file    DetChanPos.cxx
/// \brief   Class that converts channel information into a position
/// \version 
/// \author  jpaley@fnal.gov 
////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <fstream>
#include <stdio.h>

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"
#include "cetlib/search_path.h"

#include "Geometry/DetChanPos.h"

namespace emph {
  namespace geo {
    
    //----------------------------------------------------------------------
    
    DetChanPos::DetChanPos() :
      fRun(0)
    {

    }
    
    //--------------------------------------------------------------------------------
    
    bool DetChanPos::SetRun(int run) 
    {
 
      if (run == fRun) {
	if (run == 0) {
	  mf::LogWarning("SetRun") << "cannot set run to 0\n";
	  return false;
	}
	else return true;
      }
      
      if (run < 1000) { // Phase 1a Runs
	fRun = run;
	return true;
      }
      else {
	mf::LogWarning("SetRun") << "unrecognized run number\n";
	fRun = 0;
	return false;
      }

      //      std::ifstream geoFile;
      //      std::string file_path;
      //      std::string fname;
      
      /*
      file_path = getenv ("CETPKG_SOURCE");
      fname = file_path + "/Geometry/" + fGDMLFile;
      geoFile.open(fname.c_str());
      if (!geoFile.is_open()) {
	throw cet::exception("GeometryFileLoad")
	  << "cannot find GDML file " << fname << " bail ungracefully\n"
	  << __FILE__ << ":" << __LINE__ << "\n";
	return false;
      }
      geoFile.close();
      */
            
      
      return true;
    }
    
  } // end namespace geo
} // end namespace emph
