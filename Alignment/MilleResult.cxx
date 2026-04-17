////////////////////////////////////////////////////////////////////////
/// \file    MilleResult.cxx
/// \brief   Class and container to save a Mille record..  
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
#include <array>
#include <iomanip>
#include <cmath>
#include <cfloat>
#include "Alignment/MilleResult.h"

namespace emph {

  namespace align {
    
    //----------------------------------------------------------------------
    MilleParam::MilleParam(bool isFixed, int label, double val, double err, int glCorr):
      isFixed_(isFixed), label_(label), val_(val), err_(err), glCorr_(glCorr) { ; } 
      
    MilleParamByStation::MilleParamByStation(bool isFixed, int label, double val, double err, int glCorr):
      MilleParam(isFixed, label, val, err, glCorr), station_(label/10) {
        switch (label - station_*10) {
	  case 1: {glVar_ = std::string("DeltaX"); break; } 
	  case 2: {glVar_ = std::string("DeltaY"); break; } 
	  case 3: {glVar_ = std::string("DeltaPhi"); break; }
	  default : { 
	    std::cerr << "  MilleParam::MilleParamByStation, unexpected label " << label << " fatal " << std::endl; 
	    exit(2);
	  }
        }
     } 
      
    MilleParamBySensor::MilleParamBySensor(bool isFixed, int label, double val, double err, int glCorr):
      MilleParam(isFixed, label, val, err, glCorr), is4P_(false), station_(label/1000), 
      plane_((label - 1000*station_)/100),
      sensor_((label -  1000*station_ - 100*plane_)/10) {
        switch (label - 1000*station_ - 100*plane_ - 10*sensor_) {
	  case 1: {glVar_ = std::string("DeltaX"); break; } 
	  case 2: {glVar_ = std::string("DeltaY"); break; } 
	  case 3: {glVar_ = std::string("DeltaPhi"); break; } 
	  default : { 
	    std::cerr << "  MilleParam::MilleParamBySensor, unexpected label " << label << " fatal " << std::endl; 
	    exit(2);
	  }
        }
    }
    
    MilleParamBySensor4P::MilleParamBySensor4P(bool isFixed, int label, double val, double err, int glCorr):
      MilleParam(isFixed, label, val, err, glCorr), is4P_(true), station_(label/1000), 
      plane_((label - 1000*station_)/100),
      sensor_((label -  1000*station_ - 100*plane_)/10) {
        switch (label - 1000*station_ - 100*plane_ - 10*sensor_) {
	  case 1: {glVar_ = std::string("DeltaX"); break; } 
	  case 2: {glVar_ = std::string("DeltaY"); break; } 
	  case 3: {glVar_ = std::string("DeltaZ"); break; } 
	  case 4: {glVar_ = std::string("DeltaPhi"); break; } 
	  default : { 
	    std::cerr << "  MilleParam::MilleParamBySensor4P, unexpected label " << label << " fatal " << std::endl; 
	    exit(2);
	  }
        }
    }
    MilleResult::MilleResult(bool isByStation, const std::string &fNameRes, bool is4P) :
       isByStation_(isByStation), numRec_(-1), numRejected_(999999), chi2Ndf_(9.0e9)
    {
      this->snifOutDataFromLog(); 
      std::cerr << " MilleResult::MilleResult, storing it, "; 
      if (isByStation) std::cerr << " by station "; 
      else std::cerr << " by sensor ";  std::cerr << std::endl; 
      std::ifstream fIn(fNameRes.c_str());
      if (!fIn.is_open()) {
        std::cerr << " MilleResult::MilleResult result file is not open, check file name " << std::endl;   
	exit(2);
      }
      int nL=0;
      char aLineChar[4096];
      while (fIn.good()) {
       fIn.getline(aLineChar, 4096);
       std::string aLineStr(aLineChar);
       std::istringstream aLineIStr(aLineStr); 
       int label, glCorr; double val, valCheck, fixFlag, err;
       if (nL == 0) { nL++; continue; } // header line 
       aLineIStr >> label >> val >> fixFlag;
       const bool isFixed = (std::abs(fixFlag + 1.) < 0.1);
       if (!fIn.good()) break;
       if (isFixed) { err = 9.0e9;  glCorr= 0; } 
       else { aLineIStr >> valCheck >> err >> glCorr; }
       if (isByStation) {
         MilleParamByStation aM(isFixed, label, val, err, glCorr);
	 dataByStation_.push_back(aM); 
       } else {
         if (!is4P) { 
           MilleParamBySensor aM(isFixed, label, val, err, glCorr);
	   dataBySensor_.push_back(aM); 
	 } else { 
           MilleParamBySensor4P aM(isFixed, label, val, err, glCorr);
	   dataBySensor4P_.push_back(aM); 
	 }
       }
       nL++;
     }
     std::cerr << " Done... num Param lines " << nL << std::endl; 
   }
   // 
   // combine results.. if need be.. 
   //
    MilleResult::MilleResult(bool isByStation, const MilleResult &r1, const MilleResult &r2, bool is4P_) :
       isByStation_(isByStation), numRec_(r2.NumRecords()),
        numRejected_(r2.NumRejected()), chi2Ndf_(r2.Chi2Norm()) 
 {
       if (isByStation_) { 
         for (auto it1 = r1.CBeginSt(); it1 != r1.CEndSt(); it1++) {
	   for (auto it2 = r2.CBeginSt(); it2 != r2.CEndSt(); it2++) {
	     if (it1->label_ != it2->label_) continue;
	       double val, err; 
	       if (it1->glVar_ != std::string("DeltaPhi")) { 
	         val = it1->val_ + it2->val_; 
	         err = std::sqrt(it1->err_ * it1->err_ + it2->err_ * it2->err_); // Hum !!!? 
	       } else {
	         val = it2->val_; err =it2->err_;  // Assuming mode 10.  We simply shift the pull, leaving phi od the strip unchanged. 
	       }
	       MilleParamByStation aM(it2->isFixed_, it1->label_, val, err, it2->glCorr_); 
	       dataByStation_.push_back(aM);
	   }
	 } 
      } else {
         if (!is4P_) { 
           for (auto it1 = r1.CBeginSe(); it1 != r1.CEndSe(); it1++) {
	     for (auto it2 = r2.CBeginSe(); it2 != r2.CEndSe(); it2++) {
	       if (it1->label_ != it2->label_) continue;
	         double val = it1->val_ + it2->val_; 
	         double err = std::sqrt(it1->err_ * it1->err_ + it2->err_ * it2->err_); // Hum !!!?
	         MilleParamBySensor aM(it2->isFixed_, it1->label_, val, err, it2->glCorr_); 
	         dataBySensor_.push_back(aM);
	       } 
	   }
         } else {
           for (auto it1 = r1.CBeginSe4P(); it1 != r1.CEndSe4P(); it1++) {
	     for (auto it2 = r2.CBeginSe4P(); it2 != r2.CEndSe4P(); it2++) {
	       if (it1->label_ != it2->label_) continue;
	         double val = it1->val_ + it2->val_; 
	         double err = std::sqrt(it1->err_ * it1->err_ + it2->err_ * it2->err_); // Hum !!!?
	         MilleParamBySensor4P aM(it2->isFixed_, it1->label_, val, err, it2->glCorr_); 
	         dataBySensor4P_.push_back(aM);
	       } 
	     }
	 }
      }
    }
    
   //
    
   void MilleResult::snifOutDataFromLog() {
      
      std::ifstream fIn("./millepede.log");
      
      int nL=0;
      char aLineChar[4096];
      int numOK = 0;
      while (fIn.good()) {
       if (numOK == 3) break; // we got what we need... 
       fIn.getline(aLineChar, 4096);
       std::string aLineStr(aLineChar);
       size_t iPos1 = aLineStr.find("NREC =");   
       if (iPos1 != std::string::npos) {
         std::string tmp1(aLineStr.substr(iPos1 + 8)); 
         std::istringstream tmp1IStr(tmp1); tmp1IStr >> numRec_; numOK++; continue;  
       }
       size_t iPos2 = aLineStr.find("Data records rejected"); 
       if (iPos2 != std::string::npos) { 
         fIn.getline(aLineChar, 4096);
         std::string tmp2(aLineChar);
         std::istringstream tmp2IStr(tmp2); tmp2IStr >> numRejected_; numOK++; continue;
       }
       size_t iPos3 = aLineStr.find("Sum(Chi^2)/Sum(Ndf)"); 
       if (iPos3 != std::string::npos) { 
         fIn.getline(aLineChar, 4096); fIn.getline(aLineChar, 4096);
         std::string tmp3(aLineChar);
         size_t iPos3b = aLineStr.find("="); 
	 std::string tmp3b(tmp3.substr(iPos3b+1));
	 std::istringstream tmp3bIStr(tmp3b); tmp3bIStr >> chi2Ndf_; numOK++; continue; 
       } 
    }
   }  
 } // name space align 
} // name space emph 
