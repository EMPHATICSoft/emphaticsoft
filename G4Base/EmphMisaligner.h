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
namespace g4b {  

  class EmphMisaligner {

  public:

    /// Standard constructor and destructor for an FMWK module.
    explicit EmphMisaligner(const std::string &fNameIn, unsigned int aSeed);

    void doIt(int aModelNum, double gapDoubleSSD);
    void writeIt(const char* fName) const;
    void dumpRawMisAlignParams(const char* fName) const;

  private:
  
    int fModelNumber;
    double fGapDoubleSSD;
    std::vector<double> fTransShiftsRaw; // sorted by X, 1-8, Y 1-8, U 1-2, V 1-4 total length 22 
    std::vector<double> fRollsRaw; // same ordering. 
    std::vector<double> fYawsPitchsRaw; // same ordering. Blank for now, gave up on making this thing hopelessly complicate
    std::vector<std::string> fLines;

    void readIt(const char* fName);
    void doSSDRolls(double sigmaRolls);
    void doSSDYawPitchOnStations(double sigmaYawPitch);
    void doSSDZTranslationOnStations(double sigmaZShifts); // on stations only
    void doSSDTransOffsetOnPlanes(double sigmaTrShifts, double gapDoubleSSD); // on individual planes. Could induce volume overlaps! 
    double getValue(const std::string &line, const std::string &key=std::string("value=")); // from a line.    
    std::string getName(const std::string &line); // from a line.    
    double getRandomShift(double sig); 
    // For checking the solution, after SSD align run 
  };

} // namespace g4b

#endif // G4BASE_EMPHMISALIGNER_H
