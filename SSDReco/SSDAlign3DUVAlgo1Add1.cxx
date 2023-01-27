/// \brief   3D aligner, U and V sensor offset setting. 
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
//

#include "SSDReco/SSDHotChannelList.h" 
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/SSDAlign3DUVAlgo1.h" 

 using namespace emph;

namespace emph {
  namespace ssdr {
     void SSDAlign3DUVAlgo1::writeNominalCoords() {
	
       std::ofstream aFOut("./NominalCoordsFor2ndOrderAlignment.txt");
       aFOut << "NominalCoordsFor2ndOrderAlignment" << std::endl;
       aFOut << " " << fZCoordXs.size() << " " << fZCoordYs.size() << " " << fZCoordUs.size() << " " << fZCoordVs.size() << std::endl;
       for (size_t k=0; k != fZCoordXs.size(); k++) aFOut << " " << fZCoordXs[k]; 
       aFOut << std::endl;
       for (size_t k=0; k != fZCoordYs.size(); k++) aFOut << " " << fZCoordYs[k]; 
       aFOut << std::endl;
       for (size_t k=0; k != fZCoordUs.size(); k++) aFOut << " " << fZCoordUs[k]; 
       aFOut << std::endl;
       for (size_t k=0; k != fZCoordVs.size(); k++) aFOut << " " << fZCoordVs[k]; 
       aFOut << std::endl;
       
       aFOut.close();
    }
    
    void SSDAlign3DUVAlgo1::dumpCompactEvt(int spill, int evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
      if (!fFOutCompact.is_open()) {
       std::ostringstream aFOutCompactStrStr; aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1b.dat";
       std::string aFOutCompactStr(aFOutCompactStrStr.str());
       fFOutCompact.open(aFOutCompactStr.c_str(),  std::ios::binary | std::ios::out);
       this->writeNominalCoords();
      }
      //
      // Require at most one cluster per Sensor.. There can be at most two missing hits. 
      //
      if (aSSDClsPtr->size() < 10) return;
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsX(fNumStations+2, aSSDClsPtr->cend());  // station 0 to 5,
      //   4 and 5 have 2 sensors, but they do not overlap in phase1b.  But they will in  phase1c.
      //  So, the code  is written such possible tiling is allowed. 
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsY(fNumStations+2, aSSDClsPtr->cend());// station 0 to 5 
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsU(2, aSSDClsPtr->cend());  // station 2 nd 3 
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsV(4, aSSDClsPtr->cend());// station 4 and 5, two adjacent sensors.  
      // double sensors are disjoint, a track can't be at two place at the same time 
      std::vector< int> nClX(fNumStations+2, 0);  
      std::vector< int> nClY(fNumStations+2, 0); 
      std::vector< int> nClU(2, 0);  
      std::vector< int> nClV(4, 0); 
      const double stripAvNone = 0.; const double stripRmsNone = 1.0e9; 
      if ((fSubRunNum == 10) && (fEvtNum == 10))  std::cerr << " Check for event 10 from spill 10, cluster table " << std::endl;
      for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
        const size_t kSt = itCl->Station(); const size_t kSe = itCl->Sensor(); 
	if ((fSubRunNum == 10) && (fEvtNum == 10)) 
	  std::cerr << " Station " <<  kSt << " Sensor " << kSe << " View " 
	            << itCl->View() << " strip " << itCl->WgtAvgStrip() << " rms " << itCl->WgtRmsStrip() << std::endl;
	size_t kStI = kSt;
	if (itCl->View() == rb::X_VIEW) {
	  if ((kSt == 4) && (kSe % 2 == 1)) kStI = 5;
	  if (kSt == 5) { if (kSe % 2 == 0) kStI = 6; else kStI = 7; }
	  nClX[kStI]++;
	  mySSDClsPtrsX[kStI] = itCl;
	} else if (itCl->View() == rb::Y_VIEW) {
	  if ((kSt == 4) && (kSe % 2 == 1)) kStI = 5;
	  if (kSt == 5) { if (kSe % 2 == 0) kStI = 6; else kStI = 7; }
	  nClY[kStI]++;
	  mySSDClsPtrsY[kStI] = itCl;
	} else if (itCl->View() == rb::U_VIEW) {
	  nClU[kSt-2]++;
	  mySSDClsPtrsU[kSt-2] = itCl;
	} else if (itCl->View() == rb::W_VIEW) {
	  kStI = 2*(kSt-4) + kSe - 4;
	  if (kStI > 3) {
	    std::cerr << " SSDAlign3DUVAlgo1::dumpCompactEvt, inconsistent W view with station and sensor index " 
	              << kSt << " / " << kSe << " skip this cluster.. " << std::endl;
	    continue;
	  }
	  nClV[kStI]++;
	  mySSDClsPtrsV[kStI] = itCl;
	}
      }
      //
      // allow for 1 missing hit missing in all views, but no extra cluster  
      //
      for (size_t kSt=0; kSt != fNumStations+2; kSt++) if ( (nClX[kSt] > 1) || (nClY[kSt] > 1)) return ;
      int aNumX = 0; for (size_t kSt=0; kSt != fNumStations+2; kSt++) aNumX += nClX[kSt];
      int aNumY = 0; for (size_t kSt=0; kSt != fNumStations+2; kSt++) aNumY += nClY[kSt];
      if ((aNumX < 5) || (aNumY < 5)) return;
      // 
      // for the U and V view, if we have more than 1 hit in a given sensor, we set the pointer to nothing, stripRMS is infinite. 
      for (size_t kSt=0; kSt != 2; kSt++) if (nClU[kSt] > 1) mySSDClsPtrsU[kSt] = aSSDClsPtr->cend();
      for (size_t kSt=0; kSt != 4; kSt++) if (nClV[kSt] > 1) mySSDClsPtrsV[kSt] = aSSDClsPtr->cend();
      //
      // event selected for higher order alignments. 
      //
      int key=687400;
      int numDouble = 2 * (2*(fNumStations+2) + 2 + 4); // fir 2 is mean & RMS, then, X + Y + U + V
      fFOutCompact.write(reinterpret_cast<char*>(&key), sizeof(int)); 
      fFOutCompact.write(reinterpret_cast<char*>(&numDouble), sizeof(int));
      fFOutCompact.write(reinterpret_cast<char*>(&spill), sizeof(int)); 
      fFOutCompact.write(reinterpret_cast<char*>(&evt), sizeof(int));
      double stripInfo[2];
      for (size_t kSt=0; kSt != fNumStations+2; kSt++) {
	 stripInfo[0] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsX[kSt]->WgtAvgStrip(); 
	 stripInfo[1] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsX[kSt]->WgtRmsStrip();
         fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
      }
      for (size_t kSt=0; kSt != fNumStations+2; kSt++) {
	 stripInfo[0] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsY[kSt]->WgtAvgStrip(); 
	 stripInfo[1] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsY[kSt]->WgtRmsStrip();
         fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
      }
      for (size_t kSt=0; kSt != 2; kSt++) {
	 stripInfo[0] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsU[kSt]->WgtAvgStrip(); 
	 stripInfo[1] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsU[kSt]->WgtRmsStrip();
         fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
      }
      for (size_t kSt=0; kSt != 4; kSt++) {
	 stripInfo[0] = (mySSDClsPtrsV[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsV[kSt]->WgtAvgStrip(); 
	 stripInfo[1] = (mySSDClsPtrsV[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsV[kSt]->WgtRmsStrip();
         fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
      }
      fNEvtsCompact++;
    }
  } // ssdr
}
