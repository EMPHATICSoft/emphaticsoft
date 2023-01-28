////////////////////////////////////////////////////////////////////////
/// \brief  The input data for the SSD aligner 
///          Used by main SSD Aligner Algo1 package.   
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>

#include "BTAlignInput.h"

namespace emph {
  namespace rbal {
  
     BTAlignInput::BTAlignInput() { ; }
       
     size_t BTAlignInput::FillItFromFile(int nEvtExpected, const char *fName, int selSpill) {
       std::ifstream fIn(fName, std::ios::in | std::ios::binary);
       if ((!fIn.is_open()) || (!fIn.good())) {
         std::cerr << " BTAlignInput::FillItFromFile, file with name " << std::string(fName) << " ain't there, or no good. Fatal " << std::endl;
	 exit(2);
       }
       int nEvtRead = 0;
       int aKey, numDoublePerEvt, spill, evt;
       std::vector<double> XViewData, YViewData, UViewData, VViewData;
       double stripInfo[2];
       while (fIn.good()) {
         XViewData.clear(); YViewData.clear(); UViewData.clear(); VViewData.clear();
         fIn.read(reinterpret_cast<char*>(&aKey), sizeof(int)); 
	 if (aKey != static_cast<int>(fKey)) {
	   std::cerr << " BTAlignInput::FillItFromFile, 1rst word in file " << std::string(fName) 
	            << " has key " << aKey << " expecting " << static_cast<int>(fKey) << " fatal, quit here and now " << std::endl;
	   exit(2);
	 }
	 fIn.read(reinterpret_cast<char*>(&numDoublePerEvt), sizeof(int) );
	 if (nEvtRead == 0) std::cerr << " BTAlignInput::FillItFromFile, 1rst event,  expecting " 
	                               << nEvtExpected << " events with " <<  numDoublePerEvt << " doubles per Evts " << std::endl;
	  	  
	 fIn.read(reinterpret_cast<char*>(&spill), sizeof(int) ); fIn.read(reinterpret_cast<char*>(&evt), sizeof(int) );
	 for (int k=0; k != fNumStations+2; k++) {
	   fIn.read(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   XViewData.push_back(stripInfo[0]); XViewData.push_back(stripInfo[1]); 
	 }
	 for (int k=0; k != fNumStations+2; k++) {
	   fIn.read(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   YViewData.push_back(stripInfo[0]); YViewData.push_back(stripInfo[1]); 
	 }
	 for (int k=0; k != 2; k++) {
	   fIn.read(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   UViewData.push_back(stripInfo[0]); UViewData.push_back(stripInfo[1]); 
	 }
	 for (int k=0; k != 4; k++) {
	   fIn.read(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   VViewData.push_back(stripInfo[0]); VViewData.push_back(stripInfo[1]); 
	 }
	 if ((selSpill != INT_MAX) && (spill != selSpill)) continue;
	 BeamTrackCluster aBT(spill, evt, XViewData, YViewData, UViewData, VViewData);
	 fDat.push_back(aBT);
	 nEvtRead++;
	 if (nEvtRead == nEvtExpected) {
	   if (nEvtRead == 0) std::cerr << " .... reached event count " 
	                               << nEvtRead << " all god.. "  << std::endl;
           break;
	 }
       } // reading.. 
       fIn.close();
       std::cerr << " ... Done, " << fDat.size() << " events accepted " << std::endl;
       return fDat.size();
     }
     
     void BTAlignInput::Remove(int spill, int evt) { // very slow!!! 
       for (std::vector<BeamTrackCluster>::iterator it=fDat.begin(); it != fDat.end(); it++) {
         if ((it->Spill() == spill) && (it->EvtNum() == evt)) { fDat.erase(it); return; }
       }
       std::cerr << " BTAlignInput::Remove, event " << evt  << " from spill " << spill << " not found, ignore for now " << std::endl;
     }
     // Serialize, de Serialize. a bit tedious, done once for the job. 
     void BTAlignInput::AddFromMPIData(double *dat) {
       double *ptr = dat;
       int spill = static_cast<int>(*ptr); ptr++;
       int evt = static_cast<int>(*ptr); ptr++;
       std::vector<double> XViewData, YViewData, UViewData, VViewData;
       for (int k=0; k != 2*(fNumStations+2); k++) { XViewData.push_back(*ptr); ptr++; } 
       for (int k=0; k != 2*(fNumStations+2); k++) { YViewData.push_back(*ptr); ptr++; } 
       for (int k=0; k != 4; k++) { UViewData.push_back(*ptr); ptr++; } 
       for (int k=0; k != 8; k++) { VViewData.push_back(*ptr); ptr++; }
       BeamTrackCluster aBT(spill, evt,  XViewData, YViewData, UViewData, VViewData);
       fDat.push_back(aBT);
     }
     
     
      size_t BTAlignInput::FillForMPITransfer(std::vector<emph::rbal::BeamTrackCluster>::const_iterator it, std::vector<double> &evtRaw) const { 
       
       evtRaw.clear();
       evtRaw.push_back(static_cast<double>(it->Spill()));
       evtRaw.push_back(static_cast<double>(it->EvtNum()));
       size_t nW = 2;// We do not include fKeep, no point, it is used only in the context of MPI
       for (int k=0; k != fNumStations+2; k++) { 
         evtRaw.push_back(it->TheAvStrip('X', k));
         evtRaw.push_back(it->TheRmsStrip('X', k));
       }
       nW += 2*(fNumStations+2);
        for (int k=0; k != fNumStations+2; k++) { 
         evtRaw.push_back(it->TheAvStrip('Y', k));
         evtRaw.push_back(it->TheRmsStrip('Y', k));
       }
       nW += 2*(fNumStations+2);
       for (int k=0; k != 2; k++) { 
         evtRaw.push_back(it->TheAvStrip('U', k));
         evtRaw.push_back(it->TheRmsStrip('U', k));
       }
       nW += 4;
        for (int k=0; k != 4; k++) { 
         evtRaw.push_back(it->TheAvStrip('V', k));
         evtRaw.push_back(it->TheRmsStrip('V', k));
       }
       nW += 8;
       return nW;
     }
     ///
     void BTAlignInput::DumpCVSForR(int myRank, char view, const std::string &aNameToken) const {
       std::string aName("./BTInput_Rank"); std::ostringstream rrStrStr; 
       rrStrStr << myRank; aName += rrStrStr.str() + std::string("_");
       aName += aNameToken; aName += std::string("_");
       aName += std::string(1, view); aName += std::string("_V1.txt");
       size_t numDat = 0;
       
       switch (view) {
         case 'X': 
         case 'Y': 
	 {
	    numDat = fNumStations + 2;
	    break;
	 }
	 case 'U':
	 {
	    numDat = 2;
	    break;
	 }
	 case 'V':
	 case 'W':
	 {
	    numDat = 4;
	    break;
	 }
	 default: {
	   std::cerr << " BTAlignInput::DumpCVSForR, unknown view " << view << " skip... " << std::endl; return;
	 }
       } 
       std::ofstream fOut(aName.c_str());
       // header 
       fOut << " spill evt";
       for (size_t k=0; k != numDat; k++) fOut << " av" << k << " rms" << k; 
       fOut << " " << std::endl;
       // data 
       for (std::vector<BeamTrackCluster>::const_iterator it = fDat.cbegin(); it != fDat.cend(); it++) {
         if (!it->Keep()) continue;
         fOut << " " << it->Spill() << " " << it->EvtNum();
	 for (size_t k=0; k != numDat; k++) {
	   fOut << " " << it->TheAvStrip(view, k) << " " << it->TheRmsStrip(view, k);
	 }
	 fOut << " " << std::endl;
       }
       fOut.close();
     }   
     
  } // rbal 
} // emph
