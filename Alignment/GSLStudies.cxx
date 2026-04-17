////////////////////////////////////////////////////////////////////////
/// \file    GSLStudies.cxx
/// \brief   Class and container for Cross-Plane studies, based on  
/// \version 
/// \author  lebrun@fnal.gov, to create interface c++ ->c -> f90.. 
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <iomanip>
#include <cfloat>
#include <gsl/gsl_fit.h>
#include "Alignment/MilleRecord.h"
#include "Alignment/GSLStudies.h"

emph::align::aGSLLinFitResult::aGSLLinFitResult(int numEvts, short station, short plane, short sensor, double c0, double c1, 
	                 std::array<double,3> covMat, double chiSq) :
        numEvts_(numEvts), station_(station), plane_(plane), sensor_(sensor), shift_(c0), slope_(c1), 
	covMat_(covMat), chiSq_(chiSq)   
   {
  
    ; 
  
  }

emph::align::GSLStudies::GSLStudies(bool byStation, int mode, const std::string &token) :  
      byStations_(byStation), mode_(mode), phase_(0),  sigmaCut_(5.0), maxPull_(4.0), 
      basicSigma_(0.06/std::sqrt(12.)), nIterMax_(15), token_(token) {
     debug_ = false; ; 
   }
    
void emph::align::GSLStudies::StudyByStationXY(const MilleRecords &mData, short iStation, short sensor) { //Phase 1c only.. 

   for (short jPlStu = 0; jPlStu != 2; jPlStu++) {
     if (debug_) std::cerr << std::endl << " emph::align::GSLStudies::StudyByStationXY, Station " 
                           << iStation << " case " << jPlStu << " sensor " << sensor << std::endl;
     double residMax = DBL_MAX; rejected_.clear();
     size_t nKept, nRej;
     tmpResults_.clear(); rejected_.clear();
     int nIter = 0;
     while (true) { // until we converge.. 
       curIter_ = nIter;
       nKept = this->doOneStationXYFit(mData, jPlStu, iStation, sensor );
       if (nKept < 5) break; // no or too few selected events...
       if (nIter == 0) nRej = this->pruneItMaxPull(mData, jPlStu, iStation, sensor);
       else nRej = this->pruneIt(mData, jPlStu, iStation, sensor, sigmaCut_);
       if (debug_) {
         std::cerr << " ... At iter " << nIter << " nKept " << nKept << " Rejected " 
	           << nRej << " check nRej " <<rejected_.size() <<  std::endl; 
	 std::cerr << " ... currend Offset " << curOffset_ << " slope " << curSlope_ 
	           << " cov " << curCovMat_[0] << ", " << curCovMat_[1] << ", " << curCovMat_[2] 
		   << " chiSq " <<  curChiSq_ << std::endl;
       }
       if (nRej == 0) break; // Nothing beyond 5 sigma, o. k., good enough 
       if (tmpResults_.size() > 1) {
         size_t iPrev = tmpResults_.size()-2;
         if ((std::abs(curOffset_ - tmpResults_[iPrev].shift_) < 0.002) && 
	    (std::abs(curSlope_ - tmpResults_[iPrev].slope_) < 1.0e-5)) break; // good enough accuracy.  
       }
       nIter++;
       if (nIter > nIterMax_) {
         if (debug_) std::cerr << " .. Stop iterating, too many of them.. " << std::endl;
	 break;
       }
     } // convergence.
     if (tmpResults_.size() > 0) {
       size_t iResFinal = tmpResults_.size()-1;
       allResults_.push_back(tmpResults_[iResFinal]);
     }
       
   } // on case, X vs Y 
          
}      

size_t emph::align::GSLStudies::doOneStationXYFit(const MilleRecords &mData, short jCase, 
                              short iStation, short kSensor) {  // return the number of iteration. 
   std::ofstream fOutTmp;
   if (debug_) {
     std::ostringstream aNameStrStr; aNameStrStr << "./GSLStudiesXY_Case_" << jCase 
                                    << "_Station" << iStation << "_Sensor" << kSensor << "_Iter" << curIter_ << ".txt"; 
     std::string aNameStr(aNameStrStr.str());
     fOutTmp.open(aNameStr.c_str());
     fOutTmp << " evt pull  coordOrtho " << std::endl;
   }	
			      
   short jPlX, jPlY;
   switch (iStation) {
     case 0: case 1: case 4: {
         jPlX = 1; jPlY = 0; break; 
     } 
     case 2: case 3: {
         jPlX = 2; jPlY = 1; break; 
     } 
     case 5: case 6: case 7: {
         jPlX = 0; jPlY = 1; break; 
     }
   }
   if (debug_) std::cerr << " emph::align::GSLStudies::doOneStationXYFit jPlX " << jPlX << ", jPlY " << jPlY << std::endl;
   xVals_.clear(); yVals_.clear();
   curOffset_ = 0.;  curSlope_ = 0.;
   for (auto it = mData.CBegin(); it != mData.CEnd(); it++) {
     if ((it->station_ != iStation ) || (it->sensor_ != kSensor)) continue;
     bool keepIt = true;
     for (auto itR=rejected_.cbegin(); itR != rejected_.cend(); itR++) 
        if (it == (*itR)) {keepIt = false; break; }
     if (!keepIt) continue; 
     if ((jCase == 0) && (it->plane_ != jPlX)) continue;
     if ((jCase == 1) && (it->plane_ != jPlY)) continue;
     xVals_.push_back(it->coordOrtho_); yVals_.push_back(it->pull_); 
     if (debug_) fOutTmp << it->evtNum_ << " " <<  it->pull_ << " " << it->coordOrtho_ << std::endl; 
   }
   if (fOutTmp.is_open()) fOutTmp.close();
   if (xVals_.size() < 5 ) return 0;
   gsl_fit_linear (&xVals_[0], 1, &yVals_[0], 1 , xVals_.size(), &curOffset_, &curSlope_, 
                   &curCovMat_[0], &curCovMat_[1], &curCovMat_[2], &curChiSq_);
   if (debug_) { 
     std::cerr << " emph::align::GSLStudies::doOneStationXYFit, station " << iStation 
               << " sensor " << kSensor << " Plane " << jPlX << " offset " << curOffset_ << "slope " << curSlope_ << std::endl;
	       
//     if (iStation == 5) std::cerr << " ... and quit for now " << std::endl; exit(2);
   }
   short aPlane = (jCase == 0) ? jPlX : jPlY;		   
   aGSLLinFitResult aRes((int) xVals_.size(), iStation, aPlane, kSensor, curOffset_, curSlope_, 
	                 curCovMat_, curChiSq_);
   aRes.jCase_ = jCase;			 
   tmpResults_.push_back(aRes);			 		   
   return xVals_.size();
}  
size_t emph::align::GSLStudies::pruneIt(const MilleRecords &mData, short jCase, short iStation, short kSensor, double numSigmas) {

   if (debug_) std::cerr << " emph::align::GSLStudies::pruneIt, jCase " << jCase << " Station " 
                         <<  iStation << " sensor " <<  kSensor << "  nSig " << numSigmas << std::endl;
			 
//   if (!this->fillInvCovs()) return 0; // should not happen.. 
   short jPlX, jPlY;
   switch (iStation) {
     case 0: case 1: case 4: {
         jPlX = 1; jPlY = 0; break; 
     } 
     case 2: case 3: {
         jPlX = 2; jPlY = 1; break; 
     } 
     case 5: case 6: case 7: {
         jPlX = 0; jPlY = 1; break; 
     }
   }
   size_t nObs = 0;
   size_t nRej = 0;
   for (auto it = mData.CBegin(); it != mData.CEnd(); it++) {
     if ((it->station_ != iStation ) || (it->sensor_ != kSensor)) continue;
     bool keepIt = true;
     size_t nRef = 0;
     for (auto itR=rejected_.cbegin(); itR != rejected_.cend(); itR++) 
        if (it == (*itR)) {keepIt = false; break; } 
     if (!keepIt) continue; 
     if ((jCase == 0) && (it->plane_ != jPlX)) continue;
     if ((jCase == 1) && (it->plane_ != jPlY)) continue;
     double xv = it->coordOrtho_; double yv = it->pull_;
     nObs++;
     const double resid = yv - (curOffset_ + xv*curSlope_); 
     const double residSq = resid*resid;
     const double errProjSq = curCovMat_[2] + 2.0*curCovMat_[1]*curSlope_*xv + curCovMat_[0]*xv*xv;
     const double err = std::sqrt(std::abs(errProjSq + basicSigma_*basicSigma_));
     if (debug_ && (it->evtNum_ < 10)) std::cerr << " resid " << resid << " errProj " 
                                                 << std::sqrt(std::abs(errProjSq))
						 <<  " nSig " << std::abs(resid/err) << std::endl;
     if (std::abs(resid/err) > numSigmas) {
       nRej++;
       rejected_.push_back(it); 
     }
   }
   return nRej;
}
size_t emph::align::GSLStudies::pruneItMaxPull(const MilleRecords &mData, short jCase, short iStation, short kSensor) {

   if (debug_) std::cerr << " emph::align::GSLStudies::pruneIt, jCase " << jCase << " Station " 
                         <<  iStation << " sensor " <<  kSensor << " maxPull " << maxPull_ << std::endl;
			 
//   if (!this->fillInvCovs()) return 0; // should not happen.. 
   short jPlX, jPlY;
   switch (iStation) {
     case 0: case 1: case 4: {
         jPlX = 1; jPlY = 0; break; 
     } 
     case 2: case 3: {
         jPlX = 2; jPlY = 1; break; 
     } 
     case 5: case 6: case 7: {
         jPlX = 0; jPlY = 1; break; 
     }
   }
   size_t nObs = 0;
   size_t nRej = 0;
   for (auto it = mData.CBegin(); it != mData.CEnd(); it++) {
     if ((it->station_ != iStation ) || (it->sensor_ != kSensor)) continue;
     bool keepIt = true;
     size_t nRef = 0;
     for (auto itR=rejected_.cbegin(); itR != rejected_.cend(); itR++) 
        if (it == (*itR)) {keepIt = false; break; } 
     if (!keepIt) continue;
     if (std::abs(it->pull_) > maxPull_) keepIt = false;
     if (!keepIt) {   
       nRej++;
       rejected_.push_back(it); 
     }
   }
   return nRej;
}
/* 
bool emph::align::GSLStudies::fillInvCovs() { 
  const double det = curCovMat_[0] * curCovMat_[2] - curCovMat_[1] * curCovMat_[1];
  if (std::abs(det) < 1.0e-20) {
    std::cerr << " emph::align::GSLStudies::fillInvCovs, determinant is too small "  << det << std::endl; 
    std::cerr << " .... covariance matrix  "  << curCovMat_[0] << ", " << curCovMat_[1] << ", " << curCovMat_[2] <<  std::endl; 
    return false;
  }
  curInvCovMat_[0] = curCovMat_[2]/det;
  curInvCovMat_[2] = curCovMat_[0]/det;
  curInvCovMat_[1] = -1.0*curCovMat_[1]/det;
  //
  // Check...
  //
  const double matIdentity00 = curInvCovMat_[0]*curCovMat_[0] + curInvCovMat_[1]*curCovMat_[1];
  const double matIdentity11 = curInvCovMat_[1]*curCovMat_[1] + curInvCovMat_[2]*curCovMat_[2];
  const double matIdentity10 = curInvCovMat_[0]*curCovMat_[1] + curInvCovMat_[1]*curCovMat_[2];
  if (debug_) {
     std::cerr << " emph::align::GSLStudies::fillInvCovs, curInvCovMat_ " 
               << curInvCovMat_[0] << ",  " << curInvCovMat_[1] << ", " <<  curInvCovMat_[2] << std::endl;
     std::cerr << " ......  check 00 " 
                        << matIdentity00 << " 11 " << matIdentity11 << " off-diag " << matIdentity10 << std::endl;
  } 
  return true;
} 
*/   
    
