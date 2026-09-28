////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in the target
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TARGETHIT_H
#define TARGETHIT_H

#include <vector>
#include <string>

namespace sim {
  /// A single unit of energy deposition in the liquid scintillator
  class TargetHit {
  public:
    TargetHit();
    ~TargetHit() {};
    
    double     PreX() const {return fPreX[0]; }
    double     PreY() const {return fPreX[1]; }
    double     PreZ() const {return fPreX[2]; }
    double     PostX() const {return fPostX[0]; }
    double     PostY() const {return fPostX[1]; }
    double     PostZ() const {return fPostX[2]; }

    double     PrePx() const {return fPreP[0]; }
    double     PrePy() const {return fPreP[1]; }
    double     PrePz() const {return fPreP[2]; }
    double     PostPx() const {return fPostP[0]; }
    double     PostPy() const {return fPostP[1]; }
    double     PostPz() const {return fPostP[2]; }

    double     DE() const {return fDE; }

    int        PId() const {return fPId; } // pdg value

    int        TrackID()      const {return fTrackID;}
    std::string Process() const {return fProcess; }
    void       SetPreX(double x[3]) {for (int i=0; i<3; ++i) fPreX[i] = x[i];}
    void       SetPreP(double p[3]) {for (int i=0; i<3; ++i) fPreP[i] = p[i];}
    void       SetPostX(double x[3]) {for (int i=0; i<3; ++i) fPostX[i] = x[i];}
    void       SetPostP(double p[3]) {for (int i=0; i<3; ++i) fPostP[i] = p[i];}
    void       SetDE(double de) {fDE = de; }
    void       SetProcess(std::string str) {fProcess = str; }
    void       SetPId(int id) { fPId = id; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }

  private:
    double fPreX[3];
    double fPreP[3];
    double fPostX[3];
    double fPostP[3];
    std::string fProcess;
    double fDE; // amount of energy deposited
    int    fPId;

    int    fTrackID;
  };

}
#endif
////////////////////////////////////////////////////////////////////////
