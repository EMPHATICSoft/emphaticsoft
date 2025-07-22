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
    int        GetAncestorPDG()	 const {return fAncestorPDG;}
    double     GetAncestorMom() const {return fAncestorMom;}
    //int        GetGrandParentPDG() const {return fGranParentPDG;}
    double     GetTime() const {return fTime;} 
    double     GetEnergyDepo() const{return fEnergyDepo;}
    double 	GetWavelength() const{return fWavelength;}
    int 	GetHiLo();
    int 	GetChannel();  
    double 	GetDirx() const{return fdirx;}
    double      GetDiry() const{return fdiry;}
    double      GetPosx() const{return fposx;}
    double      GetPosy() const{return fposy;}


 	
  void Reset(); // to be invoked at the beginning of each event 

  void SetBlockNumber(int n) { fBlock = n; }
  void AddToAncestorTrack(int ancestorTrackNum) {fAncestorTrackID=ancestorTrackNum;} // for each non-vanishing contribution, above 1 MeV, 
  void SetAncestorPDG(int ancestorPDG) {fAncestorPDG = ancestorPDG ;}
  void SetAncestorMom(double mom) {fAncestorMom = mom;}
//void SetGrandParentPDG(int PDG){fGranParentPDG = PDG;}
  void SetTime(double t) { fTime= t; }
  void SetEnergyDepo(double e){fEnergyDepo = e;} 
  void SetWavelength(double wl) {fWavelength = wl;}
  void SetDirx(double dirx){fdirx = dirx;}
  void SetDiry(double diry){fdiry = diry;}
  void SetPosx(double posx){fposx = posx;}
  void SetPosy(double posy){fposy = posy;}


   inline friend std::ostream& operator<<(std::ostream& os, const ARICHHit& arichhit)
      {
        os << "ARICH HIT: (" << arichhit.fBlock << ", " << arichhit.fTime  << " sec, " <<  arichhit.fWavelength << " nm, "<< arichhit.fEnergyDepo << " MeV) " << std::endl; 

	return os;
        }

  private:
 
   
    int fBlock; // mPMT anode identifier 
    int fAncestorTrackID;
    int fAncestorPDG;
    double fdirx,fdiry;  //direction 
    double fposx, fposy;
    double fAncestorMom;
    double fTime; // Hit time
    double fEnergyDepo;  //Energy Depo 
//    int    fGranParentPDG; 
    double fWavelength;   

 };

}
#endif
////////////////////////////////////////////////////////////////////////
