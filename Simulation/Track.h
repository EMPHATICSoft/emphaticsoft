////////////////////////////////////////////////////////////////////////
/// \brief   
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_Track_H
#define SIM_Track_H

#include <vector>

namespace sim {
  /// A single unit of energy deposition in the liquid scintillator
  class Track {
  public:
    Track();
    ~Track() {}
    
    double     GetX() const {return fX[0]; }
    double     GetY() const {return fX[1]; }
    double     GetZ() const {return fX[2]; }

    double     GetPx() const {return fP[0]; }
    double     GetPy() const {return fP[1]; }
    double     GetPz() const {return fP[2]; }

    int        GetPId() const {return fPId; } // pdg value

    int        GetTrackID()      const {return fTrackID;}
    int        GetParentTrackID()      const {return fParentTrackID;}

    void       SetPosition(double x, double y, double z) { fX[0] = x; fX[1] = y; fX[2] = z;}
    void       SetMomentum(double px, double py, double pz) {fP[0] = px; fP[1] = py; fP[2] = pz;}
    void       SetPId(int id) { fPId = id; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }
    void       SetParentTrackID(int trkId) { fParentTrackID = trkId; }

  private:
    std::vector<double> fX;
    std::vector<double> fP;
    int    fPId;

    int    fTrackID;
    int    fParentTrackID;
  };

}
#endif
////////////////////////////////////////////////////////////////////////
