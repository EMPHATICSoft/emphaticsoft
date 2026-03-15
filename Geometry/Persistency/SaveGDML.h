////////////////////////////////////////////////////////////////////////
/// \brief   Definitions of detector types and constants
/// \author  lebrun@fnal.gov based on the suggestion of  jpaley@fnal.gov
/// \date
/// The idea is to strenth our Provenance work flow: Suppose we run 
/// our Monte-Carlo, G4EMPH, etc,.. in production mode, and we stash the 
/// resulting files on our server. Days, weeks, months passes by... 
/// We then reconstruct the data, to estimate our acceptance.. 
/// But we need to know what Geometry (we have many of themm) was used. 
/// And, since it our geometry file are ASCII, that is editable, the safest
/// option is to save the entire file, in the MC production root file. 
/// This classes simply "globs" in memory, via the use of a single stl vector
/// and has a minumm set of public const accessors, and a method to 
/// to write the content to a file. 
/// It is written such that is consistent with rootIO, and thereby 
/// our default Persistency mechanism in art, canvas-root-io 
/// At least, I hope... 
/// But may be we also want this file stored in our CAF file... 
////////////////////////////////////////////////////////////////////////
#ifndef SAVEGDML_H
#define SAVEGDML_H

#include <vector>
#include <string>

namespace emph {

  namespace geo {
    
    class SaveGDML {
    
    public:
    
    explicit SaveGDML(const std::string &fileName) ;
    
    void SaveAgain(const std::string &newFileName) const; 
    
    private:
      
      std::string ffileName;
      std::vector<std::string> fData;
    
    public:
      
      inline size_t size() const { return fData.size(); } 
      inline std::vector<std::string>::const_iterator cbegin() const { return fData.cbegin(); } 
      inline std::vector<std::string>::const_iterator cend() const { return fData.cend(); } 
      
    };
  }
}

#endif // SAVEGDML_H
