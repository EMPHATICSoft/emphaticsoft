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
      
      bool TranslateAStation(int iStation, float x, float y, float z); // allow for Z motion. 
      bool PushPullAStation(int iStation, float z); // only along Z 
      bool RotateAStation(int iStation, float phi);
      bool TranslateASensor(int iStation, int iPlane, int iSensor, float x, float y); 
        // No change in Z, constrained by construction. 
      bool RotateASensor(int iStation, int iPlane, int iSensor, float phi);
      bool generateAlignedGDML(const std::string &fNameMilleRes, const std::string &newGDMLName); 
     
    };
  }
}

#endif // ModGDML_H 
