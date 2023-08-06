////////////////////////////////////////////////////////////////////////
/// \file  EmphMisaligner.h
/// \brief Generate a misaligned gdml file to simulate the loss of acceptance & momentum resolution due to 
///        misalignements and/or survey uncertainties... 
///
/// \version $Id: EmphMisaligner.h,v 1.15 2012-12-03 23:29:50 rhatcher Exp $
/// \author  lebrun@fnal.gov
///  August 1 2023:  Upgrade the geometry, support rolls of stations.. 
////////////////////////////////////////////////////////////////////////

///

#ifndef G4BASE_EMPHMISALIGNER_H
#define G4BASE_EMPHMISALIGNER_H

#include <vector>
#include <string> 
#include <iostream>
# 
/// This is a stand alone class, but it must follow the XML based GDML protocalls. 
//  Not the cleanest solution, but hopefully, it will do 
// August 5 2023 : modify now the perl script, not the gdml file. 
namespace g4b {  

  class EmphMisaligner {

  public:

    /// Standard constructor and destructor for an FMWK module.
    explicit EmphMisaligner(const std::string &fNameIn, unsigned int aSeed); 
      // First argument can be empty string , take the standart generate_gdml.pl, from Phase1b. 
      // if Phase1c declare, well, do this Phase1c.   

    void doIt(int aModelNum, double gapDoubleSSD); 
    void writeIt(const char* aName) const;
    void dumpRawMisAlignParams(const char* fName) const; // Obsolete, we can just look at the perl script. 

  private:
  
    std::string fNameIn;
    int fModelNumber;
    double fGapDoubleSSD, fSigZ, fSigRoll, fSigTr, fSigYP;
    std::vector<double> fZShiftsRaw; // On station, sorted as in 
    std::vector<double> fXTransShiftsRaw; // sorted as in the perl script. 
    std::vector<double> fYTransShiftsRaw; //
    std::vector<double> fRollsRaw; 
    std::vector<double> fYawsRaw; 
    std::vector<double> fPitchesRaw;
    std::vector<std::string> fLines; // The perl script, read and modified in  method readAndModifyIt
    std::string fLine_SSD_station_shift; // For DZTranslationOnStations
    std::string fLine_SSD_shift; // for what I called rotation centers, 
    std::string fLine_SSD_mount_rotation; // For Pitch and Yaw 
    std::string fLine_SSD_angle; 

    void readAndModifyIt(); // 
    void doSSDRolls();
    void doSSDYawPitchOnMounts();
    void doSSDZTranslationOnStations(); // on stations only
    void doSSDTransOffsetOnPlanes(); // on individual planes. Could induce volume overlaps! 
    double getValue(const std::string &line, const std::string &key=std::string("value=")); // from a line.  Obsolete  
    std::string getName(const std::string &line); // from a line.  Obsolete    
    double getRandomShift(double sig); 
    // For checking the solution, after SSD align run 
  };

} // namespace g4b

#endif // G4BASE_EMPHMISALIGNER_H
