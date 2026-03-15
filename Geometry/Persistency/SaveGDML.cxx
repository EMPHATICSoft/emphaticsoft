////////////////////////////////////////////////////////////////////////
/// \file    SaveGDML.cxx
/// \brief   Class that save the GDML file into memory, implementation. 
/// \version 
/// \author  lebrun@fnal.gov, based on asuggestion of jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Geometry/Persistency/SaveGDML.h"

namespace emph {
  namespace geo {
    
    //----------------------------------------------------------------------
    
    SaveGDML::SaveGDML(const std::string &fileName) :
      ffileName(fileName) {
      
      if (fileName.length() < 2) return;
      std::ifstream fIn(fileName.c_str());
      if (!fIn.is_open()) {
        std::cerr << " SaveGDML::SaveGDML file is not open, check file name " << std::endl;   
	std::cerr << " ... should not happen! EMPHATIC can't run with a GDML file.. Quit here and now " << std::endl; 
	exit(2);
      }
      int nl=0;
      char aLineChar[4096];
      while (fIn.good()) {
       fIn.getline(aLineChar, 4096);
       std::string aLineStr(aLineChar);
       if (fIn.good()) fData.push_back(aLineStr);
      }
      fIn.close();   
   }
   //
   void SaveGDML::SaveAgain(const std::string &newFileName) const {
     std::ofstream fOut(newFileName.c_str());
     for (auto it = fData.cbegin(); it != fData.cend(); it++) fOut << (*it) << std::endl;
     fOut.close();
   } 
 }
}
