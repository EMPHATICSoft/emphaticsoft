////////////////////////////////////////////////////////////////////////
/// \file  Misaligner.h
/// \brief Generate a misaligned gdml file to simulate the loss of acceptance & momentum resolution due to 
///        misalignements and/or survey uncertainties... 
///
/// \version $Id: Misaligner.h,v 1.15 2012-12-03 23:29:50 rhatcher Exp $
/// \author  lebrun@fnal.gov
///  August 1 2023:  Upgrade the geometry, support rolls of stations.. 
////////////////////////////////////////////////////////////////////////

///

#ifndef G4BASE_Misaligner_H
#define G4BASE_Misaligner_H

#include <vector>
#include <string> 
#include <iostream>
# 
/// This is a stand alone class, but it must follow the XML based GDML protocalls. 
//  Not the cleanest solution, but hopefully, it will do 
// August 5 2023 : modify now the perl script, not the gdml file. 
namespace g4b {  

  class Misaligner {

  public:

    /// Standard constructor and destructor for an FMWK module.
    explicit Misaligner(const std::string &fNameIn, unsigned int aSeed); 
      // First argument can be empty string , take the standart generate_gdml.pl, from Phase1b. 
      // if Phase1c declare, well, do this Phase1c.   

    void doIt(int aModelNum, double gapDoubleSSD); 
    void writeIt(const char* aName) const;
    std::string runIt(const std::string &suffix) const; // return a complete Unix path name, the gdml file to use in the 
    // in the G4EMPH DetectorConstruction constructor. 
    void dumpRawMisAlignParams(const char* fName) const; // Obsolete, we can just look at the perl script. 

  private:
  
    std::string fNameIn;
    bool fDoOnlyYTrans, fDoOnlyXTrans;
    int fModelNumber;
    unsigned int fSeed;
    double fGapDoubleSSD, fSigZ, fSigRoll, fSigTr, fSigYP;
    std::vector<char> fViews; 
    std::vector<size_t> fViewIndicesY, fViewIndicesX, fViewIndicesU, fViewIndicesV; 
    //with above, to organize the parameters along while printing the truth values. 
    // To compare with minimization.. 
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
//
    void SetModelNumber(int am) { fModelNumber = am; } 
    void SetGapDoubleSSD(double d) { fGapDoubleSSD = d; } 
    void readToModifyIt(); //
    void setViewIndices();  
    void doSSDRolls();
    void doSSDYawPitchOnMounts();
    void doSSDZTranslationOnStations(); // on stations only
    void doSSDTransOffsetOnPlanes(); // on individual planes. Could induce volume overlaps! 
    double getValue(const std::string &line, const std::string &key=std::string("value=")); // from a line.  Obsolete  
    std::string getName(const std::string &line); // from a line.  Obsolete    
    double getRandomShift(double sig); 

   public:
    void SetDoOnlyYTrans(bool t=true) { fDoOnlyYTrans = t; }
    void SetDoOnlyXTrans(bool t=true) { fDoOnlyXTrans = t; }
  };

} // namespace g4b

#endif // G4BASE_Misaligner_H
