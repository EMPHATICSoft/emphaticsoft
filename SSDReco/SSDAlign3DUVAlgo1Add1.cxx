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
    
    void SSDAlign3DUVAlgo1::dumpCompactEvt(int spill, int evt, bool strictY6St, bool strictX6St, 
                                           const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
      if (!fFOutCompact.is_open()) {
       std::ostringstream aFOutCompactStrStr; 
       if ((!strictY6St) && (!strictX6St)) aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1b.dat";
       else {
         if ((strictY6St) && (!strictX6St)) aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1c2.dat";
         if ((strictX6St) && (!strictY6St)) aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1d.dat";
//         if ((strictX6St) && (strictY6St)) aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1e.dat";
// With track number 
         if ((strictX6St) && (strictY6St)) aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1g.dat";
       
       }
       std::string aFOutCompactStr(aFOutCompactStrStr.str());
       fFOutCompact.open(aFOutCompactStr.c_str(),  std::ios::binary | std::ios::out);
       this->writeNominalCoords();
       std::cerr << " Opening CompactEvt file " << aFOutCompactStr << " andkeep going  .. " << std::endl; 
       if (fDoUseTightClusters) std::cerr << " ........ Selecting narrow low de clusters.. " << std::endl;
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
      std::vector< int> nClXInBeam(fNumStations+2, 0);  
      std::vector< int> nClYInBeam(fNumStations+2, 0); 
      std::vector< int> nClUInBeam(2, 0);  
      std::vector< int> nClVInBeam(4, 0); 
      std::vector< int> nClX(fNumStations+2, 0);  
      std::vector< int> nClY(fNumStations+2, 0); 
      std::vector< int> nClU(2, 0);  
      std::vector< int> nClV(4, 0); 
      std::vector< int> nClXOutBeam(fNumStations+2, 0);  
      std::vector< int> nClYOutBeam(fNumStations+2, 0); 
      std::vector< int> nClUOutBeam(2, 0);  
      std::vector< int> nClVOutBeam(4, 0); 
     const double stripAvNone = 0.; const double stripRmsNone = 1.0e9;
      const bool debugIsOn =  (fSubRunNum == 10) && (fEvtNum < 200);
      if (debugIsOn)  std::cerr << " Check for event 10 from spill 10, cluster table " << std::endl;
      for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	if ((fDoUseTightClusters) && ((itCl->Width() > 2) ||  (itCl->AvgADC() > 1100.)) ) continue; // cut only on Width.. 
        const size_t kSt = itCl->Station(); const size_t kSe = itCl->Sensor(); 
	if (debugIsOn) 
	  std::cerr << " Station " <<  kSt << " Sensor " << kSe << " View " 
	            << itCl->View() << " strip " << itCl->WgtAvgStrip() 
		    << " rms " << itCl->WgtRmsStrip() << " Width " << itCl->Width() << " Avg ADC " << itCl->AvgADC() << std::endl;
	size_t kStI = kSt;
	  //
	  // Concocted based on real data run 1055.. 
          //	 ... but it seems correct, despite being a bit confusing.. March 27 2023
	  // 
	if (itCl->View() == geo::X_VIEW) {
	  if ((kSt == 4) && (kSe % 2 == 1)) kStI = 5;
	  if (kSt == 5) { if (kSe % 2 == 0) kStI = 6; else kStI = 7; }
	  if (fDoAllowSideClusters) {
	    if (this->IsInPencilBeamRegion(itCl))  { nClXInBeam[kStI]++; nClX[kStI]++; mySSDClsPtrsX[kStI] = itCl;} 
	    else  nClXOutBeam[kStI]++;
	  } else { 
	    nClX[kStI]++;
	    mySSDClsPtrsX[kStI] = itCl;
	  }
	} else if (itCl->View() == geo::Y_VIEW) {
	  if ((kSt == 4) && (kSe % 2 == 1)) kStI = 5;
	  if (kSt == 5) { if (kSe % 2 == 0) kStI = 6; else kStI = 7; }
	  if (fDoAllowSideClusters) {
	    if (this->IsInPencilBeamRegion(itCl))  { nClYInBeam[kStI]++; nClY[kStI]++; mySSDClsPtrsY[kStI] = itCl;} 
	    else  nClYOutBeam[kStI]++;
	  } else { 
	    nClY[kStI]++;
	    mySSDClsPtrsY[kStI] = itCl;
	  }
	} else if (itCl->View() == geo::U_VIEW) {
	  if (fDoAllowSideClusters) {
	    if (this->IsInPencilBeamRegion(itCl))  { nClUInBeam[kSt-2]++; nClU[kSt-2]++; mySSDClsPtrsU[kSt-2] = itCl;} 
	    else  nClUOutBeam[kSt-2]++;
	  } else { 
	    nClU[kSt-2]++;
	    mySSDClsPtrsU[kSt-2] = itCl;
	  }
	} else if (itCl->View() == geo::W_VIEW) {
	  kStI = 2*(kSt-4) + kSe - 4;
	  if (kStI > 3) {
	    std::cerr << " SSDAlign3DUVAlgo1::dumpCompactEvt, inconsistent W view with station and sensor index " 
	              << kSt << " / " << kSe << " skip this cluster.. " << std::endl;
	    continue;
	  }
	  if (fDoAllowSideClusters) {
	    if (this->IsInPencilBeamRegion(itCl))  { nClVInBeam[kStI]++; nClV[kStI]++; mySSDClsPtrsV[kStI] = itCl;} 
	    else nClVOutBeam[kStI]++;
	  } else { 
	    nClV[kStI]++;
	    mySSDClsPtrsV[kStI] = itCl;
	  }
	}
      }
      if (debugIsOn) {
         std::cerr << " nClX "; 
        for (size_t kk=0; kk != nClX.size(); kk++)  std::cerr << " " << nClX[kk] << ", ";
	 std::cerr << std::endl;
         std::cerr << " nClY "; 
        for (size_t kk=0; kk != nClY.size(); kk++)  std::cerr << " " << nClY[kk] << ", ";
	 std::cerr << std::endl;
	if (fDoAllowSideClusters) {
           std::cerr << " nClX In Beam "; 
           for (size_t kk=0; kk != nClXInBeam.size(); kk++)  std::cerr << " " << nClXInBeam[kk] << ", ";
	   std::cerr << std::endl;
           std::cerr << " nClY InBeam"; 
           for (size_t kk=0; kk != nClYInBeam.size(); kk++)  std::cerr << " " << nClYInBeam[kk] << ", ";
	   std::cerr << std::endl;
           std::cerr << " nClX Out Beam "; 
           for (size_t kk=0; kk != nClXOutBeam.size(); kk++)  std::cerr << " " << nClXOutBeam[kk] << ", ";
	   std::cerr << std::endl;
           std::cerr << " nClY Out Beam"; 
           for (size_t kk=0; kk != nClYOutBeam.size(); kk++)  std::cerr << " " << nClYOutBeam[kk] << ", ";
	   std::cerr << std::endl;
	 }
      }
      //
      // allow for 1 missing hit missing in all views, but no extra cluster  
      //
      for (size_t kSt=0; kSt != fNumStations+2; kSt++) if ( (nClX[kSt] > 1) || (nClY[kSt] > 1)) return ;
      //
      if (strictY6St) {
        // This was to avoid the poor statistics in some of the double sensor, real data, run 1055 
//        if ((nClY[4] > 0) || (nClY[6] > 0)) return;
        for (size_t kSt=0; kSt != 4; kSt++) if (nClY[kSt] == 0) return;
	if ((nClY[4] + nClY[5]) == 0) return;    
	if ((nClY[6] + nClY[7]) == 0) return;    
//        if ((nClY[5] == 0) || (nClY[7] == 0)) return;
//        if (nClY[4] != 0) return; // We also veto the low multiplicity sensor, concentrated onsensor 5 and 7 of station 4 & 5. 
//        if (nClY[6] != 0) return; // We also veto the low multiplicity sensor, concentrated onsensor 5 and 7 of station 4 & 5. 
      }
      if (strictX6St) {
//        if ((nClX[4] > 0) || (nClX[6] > 0)) return;
        for (size_t kSt=0; kSt != 4; kSt++) if (nClX[kSt] == 0) return;
//        if ((nClX[5] == 0) || (nClX[7] == 0)) return;
//        if (nClX[4] != 0) return; // We also veto the low multiplicity sensor, concentrated onsensor 5 and 7 of station 4 & 5. 
//        if (nClX[6] != 0) return; // We also veto the low multiplicity sensor, concentrated onsensor 5 and 7 of station 4 & 5. 
	if ((nClX[4] + nClX[5]) == 0) return;    
	if ((nClX[6] + nClX[7]) == 0) return;    
      }

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
      if (strictY6St) key++;
      if (strictX6St) key += 2;
      int numDouble = ( 2 * (2*(fNumStations+2) + 2 + 4)) ; // fir 2 is mean & RMS, then, X + Y + U + V
      // a bit of memory waste if strictY6 is true, 4 doubles being either 0 or DBL_MAX. 
      // but keep the code relatively simple. 
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
    //
    bool SSDAlign3DUVAlgo1::IsInPencilBeamRegion(std::vector<rb::SSDCluster>::const_iterator itCl) {
      if (fPosRawPencilBeam.size() == 0) {
        for (size_t k=0; k != 60; k++) fPosRawPencilBeam.push_back(1.0e9); // index is station*10 + sensor. See dumpClusters 
        // Based on run 1043, running DumpClusters.. 
        fPosRawPencilBeam[1] = 365.; // XView, Station 0, sensor 1 
        fPosRawPencilBeam[11] = 390.; // XView, Station 1, sensor 1 
        fPosRawPencilBeam[22] = 400.; // XView, Station 2, sensor 2   Tuned by hand, looking at histogram  
        fPosRawPencilBeam[32] = 410.; // XView, Station 3, sensor 2   Tuned by hand,  
        fPosRawPencilBeam[41] = 610.; // XView, Station 4, sensor 1   Tuned by hand,  
        fPosRawPencilBeam[51] = 620.; // XView, Station 5, sensor 1   Tuned by hand,  
	//
	// Y View ... 
	//
        fPosRawPencilBeam[0] = 390. ; // YView, Station 0, sensor 0 
        fPosRawPencilBeam[10] = 385.; // YView, Station 1, sensor 0 
        fPosRawPencilBeam[21] = 415.; // YView, Station 2, sensor 1   
        fPosRawPencilBeam[31] = 420.; // YView, Station 3, sensor 2   Tuned by hand,  
        fPosRawPencilBeam[43] = 555.; // YView, Station 4, sensor 3   Tuned by hand,  
        fPosRawPencilBeam[53] = 590.; // YView, Station 5, sensor 3   Tuned by hand,  
	//
	// U View ... 
	//
        fPosRawPencilBeam[20] = 325.; // UView, Station 2, sensor 0 
        fPosRawPencilBeam[30] = 325.; // UView, Station 3, sensor 0 
	//
	// V View ... 
	//
        fPosRawPencilBeam[45] = 550.; // UView, Station 2, sensor 0 
        fPosRawPencilBeam[55] = 640.; // UView, Station 3, sensor 0 
	
      }
      size_t iPos = 10*itCl->Station() + itCl->Sensor();
      if (iPos > 59) return false;
      if (fPosRawPencilBeam[iPos] > 1000.) return false;
      if (std::abs(itCl->WgtAvgStrip() - fPosRawPencilBeam[iPos] ) > 125.) return false;
      return true;
      
    }
  } // ssdr
}
