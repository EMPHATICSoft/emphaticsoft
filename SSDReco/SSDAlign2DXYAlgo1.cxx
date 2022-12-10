////////////////////////////////////////////////////////////////////////
/// \brief   2D aligner, X-Z view or Y-Z view, indepedently from each others. 
///          Algorithm one.  Could beong to SSDCalibration, but, this aligner 
///          requires some crude track reconstruction, as it is based on track residuals, i
///          of SSD strip that are on too often. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <cmath>
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
#include "SSDReco/SSDAlign2DXYAlgo1.h" 

 using namespace emph;

namespace emph {
  namespace ssdr {
  
     SSDAlign2DXYAlgo1::SSDAlign2DXYAlgo1() : 
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fFilesAreOpen(false),
       fView('?'), fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10), fChiSqCut(20.), 
       fTokenJob("undef"), fZCoords(fNumStations, 0.), fNominalOffsets(fNumStations, 0.), 
       fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0), fRMSResiduals(fNumStations, 0),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1)   
     { 
        ; 
     }
     SSDAlign2DXYAlgo1::SSDAlign2DXYAlgo1(char aView) : 
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fFilesAreOpen(false),
       fView(aView), fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10),fChiSqCut(20.), 
       fTokenJob("undef"), fZCoords(fNumStations, 0.), fNominalOffsets(fNumStations, 0.), 
       fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0), fRMSResiduals(fNumStations, 0),  
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1)   
     { 
        if ((aView != 'X') && (aView != 'Y')) {
	     std::cerr << " SSDAlign2DXYAlgo1, setting an unknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	}
     }
     SSDAlign2DXYAlgo1::~SSDAlign2DXYAlgo1() {
      if (fFOutA1.is_open()) fFOutA1.close();
     }
     void  SSDAlign2DXYAlgo1::InitializeCoords(const std::vector<double> &zCoords) {
       if (zCoords.size() != 8) {
         std::cerr << "  SSDAlign2DXYAlgo1::InitailizeCoords Unexpected number of ZCoords, " 
	           << zCoords.size() << " fatal, and that is it! " << std::endl; exit(2); 
       }
       for (size_t k=0; k != 5; k++) fZCoords[k] = zCoords[k]; 
       fZCoords[5] = zCoords[7];
       for (size_t k=0; k != 4; k++)  
         fNominalOffsets[4] = -fHalfWaferWidth; // a bit arbitrary, of course. See the conversion from Strip number to positions. 
       // 
       // Min amd maximum window.. To be defined.. Based on histogramming cluster positions.. 
       //
       
     }
     void  SSDAlign2DXYAlgo1::openOutputCsvFiles() {
       std::ostringstream fNameStrStr; 
       fNameStrStr << "SSDAlign2D_" << fView << "_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutA1.open(fNameStr.c_str());
       fFOutA1 << " spill evt kTr";
       for (size_t kSt=0; kSt != fNumStations; kSt++) fFOutA1 << fView << kSt << " ";
       fFOutA1 << " trOff trOffErr trSl trSlErr chi ";
       for (size_t kSt=0; kSt != fNumStations; kSt++) fFOutA1 << fView << "r" << kSt << " ";
       fFOutA1 << std::endl;
       fFilesAreOpen = true;
     }
     void  SSDAlign2DXYAlgo1::alignIt(const art::Event &evt, const std::vector<rb::SSDCluster> &aSSDcls) {
     
       fRunNum = evt.run();
       if (!fFilesAreOpen) this->openOutputCsvFiles();
       fSubRunNum = evt.subRun(); 
       fEvtNum = evt.id().event();
       int kTrSeqNum = 0;
       std::ostringstream headerStrStr; headerStrStr << " "  << fSubRunNum << " " << fEvtNum;
       std::string headerStr(headerStrStr.str());
       fNEvents++; 
       bool debugIsOn = (fNEvents < 15);
       if (debugIsOn) std::cerr <<  " SSDAlign2DXYAlgo1::alingIt,  number of Cluster " << aSSDcls.size() << std::endl;
       if (aSSDcls. size() < fNumStations) {
         if (debugIsOn) std::cerr <<  " ... Not enough data for a 6 hit fit.. " << std::endl;
	 return;
       }
       std::vector<size_t> nHits(fNumStations,0);
       for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDcls.cbegin(); itCl != aSSDcls.cend(); itCl++) {
         size_t kSt = itCl->Station();
         if (kSt >  fNumStations) continue; // Should not happen 
	 nHits[kSt]++;
       }
       for (size_t kSt=0; kSt != fNumStations; kSt++) {
         if (nHits[kSt] == 0) {
	   if (debugIsOn) std::cerr <<  " ... No enough clusters at all for station " << kSt << " So, No fits " << std::endl;
	   return;
	 }
          if (nHits[kSt] > 4) {
	   if (debugIsOn) std::cerr <<  " ... Too many Clusters ( " << nHits[kSt] << " ) for station  " << kSt << " So, No fits " << std::endl;
	   return;
	 }
       }
       myLinFitResult aRes;
       std::vector<double> tsData(fNumStations, 0.); 
       std::vector<double> tsDataErr(fNumStations, 5.0*fPitch); // At start.. to be reviewed based on sims, and prelim results. And use cluster RMS.  
       for (std::vector<rb::SSDCluster>::const_iterator itCl0 = aSSDcls.cbegin(); itCl0 != aSSDcls.cend(); itCl0++) {
	 if (itCl0->Station() != 0) continue;
	 tsData[0] = getTsFromCluster(0, itCl0->WgtAvgStrip()); 
         for (std::vector<rb::SSDCluster>::const_iterator itCl1 = aSSDcls.cbegin(); itCl1 != aSSDcls.cend(); itCl1++) {
	   if (itCl1->Station() != 1) continue;
	   tsData[1] = getTsFromCluster(1, itCl1->WgtAvgStrip()); 
           for (std::vector<rb::SSDCluster>::const_iterator itCl2 = aSSDcls.cbegin(); itCl2 != aSSDcls.cend(); itCl2++) {
	     if (itCl2->Station() != 2) continue;
	     tsData[2] = getTsFromCluster(2, itCl2->WgtAvgStrip()); 
             for (std::vector<rb::SSDCluster>::const_iterator itCl3 = aSSDcls.cbegin(); itCl3 != aSSDcls.cend(); itCl3++) {
	       if (itCl3->Station() != 3) continue;
	       tsData[3] = getTsFromCluster(3, itCl3->WgtAvgStrip()); 
               for (std::vector<rb::SSDCluster>::const_iterator itCl4 = aSSDcls.cbegin(); itCl4 != aSSDcls.cend(); itCl4++) {
	         if (itCl4->Station() != 4) continue;
	         tsData[4] = getTsFromCluster(4, itCl4->WgtAvgStrip()); 
                 for (std::vector<rb::SSDCluster>::const_iterator itCl5 = aSSDcls.cbegin(); itCl5 != aSSDcls.cend(); itCl5++) {
	           if (itCl5->Station() != 5) continue;
	           tsData[5] = getTsFromCluster(5, itCl5->WgtAvgStrip()); 
		     // Now fit.. a few times keep transfering the residuals 
		     aRes.chiSq = DBL_MAX; std::vector<double> tsDataStart(tsData); int nIter = 0;
                     while  ((nIter < fNumIterMax) && (aRes.chiSq > fChiSqCut)) { // chi-square cut to be decided later on.
		        this->fitLin(tsData, tsDataErr, aRes);
			 for(size_t kSt=1; kSt !=5; kSt++) tsData[1] -= 0.5*aRes.resids[kSt];
			 nIter++;
		     }
		     if (nIter < fNumIterMax) {
		         fFOutA1 << headerStr << " " << kTrSeqNum;
			 for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1 << " " << tsDataStart[kSt];
			 fFOutA1 << " " << aRes.offset << " " << aRes.sigmaOffset << " " 
			         << aRes.slope << " " << aRes.sigmaSlope << " " << aRes.chiSq;
			 for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1 << " " << aRes.resids[kSt];
			 fFOutA1 << std::endl;
			 kTrSeqNum++;
		     }
		   } // itCl5
	         } // itCl4 
	       } // itCl3      
	     }  // itCl2
	   } // itCl1
         } // itCl0
     } 

     void  SSDAlign2DXYAlgo1::fitLin(const std::vector<double> &ts, const std::vector<double> &sigTs, 
                                    myLinFitResult &fitRes ) const  {
       //
       // See good old Numerical Recipes, chapter 15.2, subroutine fit. (yeah, a subroutine!) 
       //
       if (ts.size() != sigTs.size()) {
         std::cerr << " SSDAlign2DXYAlgo1::fitLin mismatch of length between measurement values ( " 
                  << ts.size() << " )  and uncertainties ( " << sigTs.size() << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
       }
       if (ts.size() != fZCoords.size()) {
         std::cerr << " SSDAlign2DXYAlgo1::fitLin mismatch of length between measurement values ( " 
                  << ts.size() << " )  Z positions of planes ( " << fZCoords.size() << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
       }
       fitRes.ndgf = static_cast<int> (ts.size() - 2);
       std::vector<double> ws(ts);
       double sx = 0.; double sy = 0.; double ss = 0.;
       for (size_t k=0; k != ts.size(); k++) {
         ws[k] = 1.0/(sigTs[k]*sigTs[k]);
         sx += fZCoords[k]*ws[k]; sy += ts[k]*ws[k]; ss += ws[k];
       }
       const double sxoss = sx/ss;
       double b = 0.; double st2 = 0.;
       for (size_t k=0; k != ts.size(); k++) {
         const double tmpT = (fZCoords[k] - sxoss)/sigTs[k];
         st2 += tmpT*tmpT;
         b += tmpT*ts[k];
       }
       
       fitRes.slope = b/st2;
       fitRes.offset = (sy - sx*fitRes.slope)/ss;
       fitRes.sigmaOffset = std::sqrt((1. + (sx*sx)/(ss*st2))/ss);   
       fitRes.sigmaSlope = std::sqrt(1./st2);
       fitRes.covOffsetSlope = -1.0 * sx/(ss*st2);
       fitRes.chiSq = 0.;
       fitRes.resids.clear();
       for(size_t k=0; k != ts.size(); k++) {
        const double rr = ts[k] - (fitRes.offset + fitRes.slope*fZCoords[k]);
        fitRes.resids.push_back(rr);
        fitRes.chiSq += (rr*rr)*ws[k];
       }
     
       std::cerr << " SSDAlign2DXYAlgo1::fitLin, done, offset " << fitRes.offset << " +- " 
                 <<  fitRes.sigmaOffset << " but need checking, so, quit now for good  " << std::endl; 
       exit(2);
     
     } // end of fitLin.. 
     
  } // ssdr
} // emph
