////////////////////////////////////////////////////////////////////////
/// \brief   Hit time for ARICH
///
/// \author  wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_ARICHHit_H
#define SIM_ARICHHit_H

#include <vector>

namespace sim {
  /// A sum of the contribution due to a given secondary produced from the primary particle 
  class ARICHHit {

  public:
    ARICHHit();
    ~ARICHHit() {}
    
    int        GetBlockNumber() const {return fBlock; }
    int        GetAncestorTrackNum() const {return fAncestorTrackID;} // Corresponding 
    int        GetTime() const {return fTime;} 

 
  void Reset(); // to be invoked at the beginning of each event 

  void SetBlockNumber(int n) { fBlock = n; }
  void AddToAncestorTrack(int ancestorTrackNum){ fAncestorTrackID=ancestorTrackNum;} // for each non-vanishing contribution, above 1 MeV, 
  void SetTime(double t) { fTime= t; }
  
  private:
  
    int fBlock; // See the gdml nmerology for ARICH
    int fAncestorTrackID;
    double fTime; // Hit time
    
  };

}
#endif
////////////////////////////////////////////////////////////////////////
