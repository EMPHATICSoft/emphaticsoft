////////////////////////////////////////////////////////////////////////
/// \brief   Definitions of detector types and constants
/// \author  lebrun@fnal.gov 
/// \date
/// 
//    
////////////////////////////////////////////////////////////////////////
#ifndef MODGDML_H
#define MODGDML_H

#include <vector>
#include <string>

namespace emph {

  namespace geo {
    
    class ModGDML {
    
    public:
    
    explicit ModGDML(const std::string &fileName) ;
    
    void SaveIt(const std::string &newFileName) const; 
    
    private:
      
      std::string fFileName;
      std::vector<std::string> fData;
      
    
    public:
      
      bool TranslateAStation(int iStation, float x, float y, float z);
      bool RotateAStation(int iStation, float phi);
      bool RotateASensor(int iStation, int iPlane, int iSensor, float phi);
     
    };
  }
}

#endif // ModGDML_H 
