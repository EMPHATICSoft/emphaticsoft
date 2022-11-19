////////////////////////////////////////////////////////////////////////
/// \brief   A V1720 waveform, as well as a weighted averaged x, y, position of the shower 
///          generated in the one of the 9 blocks taken from the TOPAZ detector. 
///
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_TOPAZLGHit_H
#define SIM_TOPAZLGHit_H

#include <vector>

namespace sim {
  /// A sum of the contribution due to a given secondary produced from the primary particle 
  class TOPAZLGHit {

  public:
    TOPAZLGHit();
    ~TOPAZLGHit() {}
    
    int        GetBlockNumber() const {return fBlock; }
    double     GetEDeposited() const {return fEDeposited; } // sum of the photons, perhaps, time a calibration fector. Depending on usage.. 
    double     GetMeanX() const {return fX[0]; } // average over the whole shower, in a given block. 
    double     GetMeanY() const {return fX[1]; }
    double     GetMeanZ() const {return fX[2]; }
    int        GetNumPhot() const {return fNPhots; } // at the PMT cathode, integrated over the waveform 
    
    int        GetNumStep() const {return fNSteps; } 

    int        GetAncestorPId() const ; // pdg value for the track ID, which is a secondary coming our of the target 
                                                      // or the primary itself. Pick the one with the highest number visible photons at the PMT cathode.  

    int        GetAncestorTrackNum() const; // Corresponding 

    size_t GetSizeWaveform() const { return fWaveForm.size();} // Assuming a 250 MHz digitization ! 
    std::vector<int>::const_iterator GetItWaveBegin() const { return fWaveForm.cbegin(); } 
    std::vector<int>::const_iterator GetItWaveEnd() const { return fWaveForm.cend(); } 
//    size_t GetSizeTracksAndPid() const { return fTrackAmpls.size();} // Assuming a 250 MHz digitization ! 
//    std::vector<std::pair<int, int> >::const_iterator GetItTrackAndPidsBegin() const { return fTrackAmpls.cbegin(); } 
//    std::vector<std::pair<int, int> >::const_iterator GetItTrackAndPidsEnd() const { return fTrackAmpls.cend(); } 
//
    void SetBlockNumber(int n) { fBlock = n; }
 
//
// public method to compute averages for positions and renormalize the waveform, if need be.    
//
  void Reset(); // to be invoked at the beginning of each event 
                        // We add the pid and the G4Tack number of the ancestor (actually, the child of the primary track, of the primary track number, 
			// which is one.
  void AddSomePhotons(int numPhot, double x, double y, double z, double t); // to be called by the user action upon a new G4Step. 
  void AddToAncestorTrack(int ancestorTrackNum); // for each non-vanishing contribution, above 1 MeV, 
                        // !!! modal: add to the last entered element of fTrackAmpls
    // for each stepif the energy of the chargec particle energy is such that the Cerenkov angle is say, greater than 60 degree, 
    // we generate number of photons detected at the photocathode
    // not implemented yet.. We first wnat to identify the primary particle.   
  void FinalizeAndCalibrate(double calFact);         

  private:
  
    int fBlock; // See the gdml nmerology for Lead glass. Either 0,1,2,10,11,12,20,21,22.  If 9, sum of signals. 
    int fAncestorTrackID;
    double fEDeposited; 
    int fNSteps;
    std::vector<double> fX; // dimensioned to 3. Compute averages in FinalizeAndCalibrate,  Irrespectively of the trackNum/PiD that created it.   
    int fNPhots; // Total, Irrespectively of the G4 track number & PiD
    int fNunSteps; 
    
    std::vector<int> fWaveForm;
// Too complicated, Root pukes on this.. classes_def.xml difficult to build 
//    std::vector<std::pair<int, int> > fTrackAmpls;
//    std::vector<std::pair<int, int> >::reverse_iterator fItTrackLastUsed;
    //

  };

}
#endif
////////////////////////////////////////////////////////////////////////
