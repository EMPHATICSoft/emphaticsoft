////////////////////////////////////////////////////////////////////////
/// \brief   Implemantation of TOPAZLGHit
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <climits>
#include <cfloat>
#include <iostream>
#include <fstream>

#include "Simulation/TOPAZLGHit.h"

namespace sim {

  TOPAZLGHit::TOPAZLGHit() :
  fBlock(INT_MAX), // See LeadGlass block numerology in the GDML file. 
  fAncestorTrackID(0), // A track id, set when a high track is entering the block  Not yet implemented. 
  fEDeposited(0.), // in GeV, or some pre-calibrated random units. 
  fNSteps(0),  
  fX(3, DBL_MAX), // dimensioned to 3. Compute averages in FinalizeAndCalibrate  
  fNPhots(0),
  fWaveForm(50, 0.) // 50 4 ns bin, too much for one promt shwer, but O.K.  Perhaps we ought simulate multi-bucket, or 
                                      // unexpectdely slow Cerenkohv ( Even perhaps Dark Cerenkov radiation + break down of Lorentz Invariance) 
//  fTrackAmpls(),
//  fItTrackLastUsed(fTrackAmpls.rbegin())
  {

  }
  void TOPAZLGHit::Reset() {
    fEDeposited = 0.;
    for (size_t k=0; k != fX.size(); k++) fX[k] = 0.;
    fNPhots = 0; fNSteps=0;
    for (size_t k=0; k != fWaveForm.size(); k++) fWaveForm[k] = 0.;
//    fTrackAmpls.clear(); fItTrackLastUsed = fTrackAmpls.rbegin();
  }
  /*
  int TOPAZLGHit::GetAncestorTrackNum() const { // Take the one with the highest photon count. 
    if (fTrackAmpls.size() == 0) return 0;
    std::vector<std::pair<int, int> >::const_iterator itSel = fTrackAmpls.cbegin();
    int nPhotSel=0;
    for (std::vector<std::pair<int, int> >::const_iterator it = fTrackAmpls.cbegin(); it != fTrackAmpls.cend(); it++) {
      if (it->second > nPhotSel) { nPhotSel = it->second; itSel = it; } 
    }
    return itSel->first;
  }
  void TOPAZLGHit::AddAncestorTrack(int ancestorTrackNum) {
  
    for (std::vector<std::pair<int, int> >::iterator it = fTrackAmpls.begin(); it != fTrackAmpls.end(); it++) {
      if (it->first == ancestorTrackNum) { // Already got it.. 
         for (std::vector<std::pair<int, int> >::reverse_iterator itR = fTrackAmpls.rbegin(); itR != fTrackAmpls.rend(); itR++) {
	   if (itR->first == ancestorTrackNum)  { fItTrackLastUsed = itR; return; } 
	 } // double loop.. Hopefully, not many of them.  For Energy resolution studies, only one, the primary electron.
      }
    } 
    std::pair<int, int>  aTr; aTr.first = ancestorTrackNum; aTr.second = 0;
    fTrackAmpls.push_back(aTr);
    fItTrackLastUsed = fTrackAmpls.rbegin();
  }
  */
  void TOPAZLGHit::AddSomePhotons(int numPhot, double x, double y, double z, double t) { // To the current track ! 
    fNPhots += numPhot;
    fNSteps++;
    fX[0] += x*numPhot;
    fX[1] += y*numPhot;
    fX[2] += z*numPhot;
    size_t itTime = static_cast<size_t>(4.0*t); // should add here noise due to the V1720 time digitizer. Assume 250 MHz. 
    if (itTime >= fWaveForm.size()) {
//       std::cerr << " Out of time in ADC " << t << " itTime " << itTime << std::endl;
       return; // Out of time condition.. 
    }
   fWaveForm[itTime] += numPhot;
//    if ((fTrackAmpls.size() == 0) || (fItTrackLastUsed == fTrackAmpls.rbegin())) {
//      std::cerr << " TOPAZLGHit::AddSomePhotons error, can not add photon to an unknow track.  Use Add AddTrackAncestorTrack first.. Fatal " << std::endl;
//      exit(2);
//    }
//    std::vector<<std::pair<int, int> >::reverse_iterator itR =  fTrackAmpls.rbegin();
//    fItTrackLastUsed->second += numPhot;
  
  }  // to called by the user action upon a new G4Step, after setting the Ancestor track number  
  void TOPAZLGHit::FinalizeAndCalibrate(double cFact) {
  
    for (size_t k=0; k != 3; k++) fX[k] /= fNPhots;
    fEDeposited = fNPhots * cFact;
    
  }
}

