////////////////////////////////////////////////////////////////////////
/// \file    PMT.h
/// \brief
/// \version 
/// \author  wanly@bu.edu
////////////////////////////////////////////////////////////////////////

#ifndef ARICH_PMT_H
#define ARICH_PMT_H

#include <utility>
#include <memory>
#include <vector>
#include "TRandom3.h"

class G4VPhysicalVolume;

namespace emph {
  namespace arich_util {

    class PMT {
    public:
      PMT();
      PMT(int n, std::string name, double dark, double qecor, double fxtalk, std::vector<std::pair<double, double> > qe);
      ~PMT();
      PMT(const PMT& other);
      PMT& operator=(const PMT& other);

      int PMTnum() const { return fPMTnum;} 
      std::string Name() const { return fName;} 
      double DarkRate() const {return fDarkRate;}
      double QEcor() const {return fQECorrection;}
      std::vector<std::pair<double, double> > QE() const {return fQE;}
				
      void SetCrossTalk(double t0) {fXTalk = t0;}
      double GetCharge() const;
			
      void SetTriggerWin(int TriggerWin){fTriggerWin = TriggerWin;}
      void SetTransitTime(double t0) {fTransitTime = t0;}
      void SetTransitTimeSpread(double t0) {fTransitTimeSpread = t0;}
      void SetPMTnum(int n) {fPMTnum = n; }
      void SetName(std::string name) {fName = name; }
      void SetDarkRate(double dark) {fDarkRate = dark;}
      void SetQEcor(double qecor) {fQECorrection = qecor;}
      void SetQE(std::vector<std::pair<double, double> > qe) {fQE = qe;}
      double GetDarkRate() const;
      bool ifDet(double wavelength) const;
      int findBlockNumberFromName(std::string name);
      std::vector<float> GetPosfromBlockNumber(int block_number);
      void SetPhysicalVolume(G4VPhysicalVolume* vol){fPhysVolName = vol;}
      void SetPosition(std::vector<float> pos){fPos = pos;}	
      bool IsOn() const {return fON;}	
      double GetTriggerWin() const{return fTriggerWin;}
      double GetTransitTime() const {return fTransitTime;}
      double GetTransitTimeSpread() const {return fTransitTimeSpread;}

      void PrintInfo();
      std::vector<float> GetPos() const {return fPos;}
    private:    
      int fPMTnum;
      G4VPhysicalVolume* fPhysVolName;
      std::string fName;
      double fXTalk;
      double fDarkRate;
      double fQECorrection;
      std::vector<std::pair<double, double> > fQE;
      bool fON;
      double fTransitTimeSpread;
      double fTransitTime;
      double fTriggerWin;
      std::vector<float> fPos;
      TRandom3* rand_gen;
    };

  }  // end namespace geo
} // end namespace emph


#endif
