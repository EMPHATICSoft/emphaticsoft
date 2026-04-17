////////////////////////////////////////////////////////////////////////
/// \brief   a class that studies the tail of distribution and covrainces
//             of linear fits for a given Plane, along with the rotation angle
//             of the orthogonal plane. 
//             Linear regression are done with Gnu Scientific Library. 
///           
/// \date
/// 
/// \author  $Author: Paul Lebrun $ lebrun@fnal.gov
///    
////////////////////////////////////////////////////////////////////////
#ifndef GSLSTUDIES_H
#define GSLSTUDIES_H

#include <array>
#include <vector>
#include <string>
#include "Alignment/MilleRecord.h" 
#include "Alignment/MilleResult.h"

namespace emph {

  namespace align {
  
    class aGSLLinFitResult { // simple struct.. to hold the results of one linear fit 
      public :
        int numEvts_; // exclude the rejected.. 
	short station_, plane_, sensor_, jCase_;
        double shift_; 
	double slope_;
	double chiSq_; 
	std::array<double, 3> covMat_; 
	
        aGSLLinFitResult(int numEvts, short station, short plane, short sensor, double c0, double c1, 
	                 std::array<double,3> covMat, double chisq); 
  
    };
        
    class GSLStudies {
    
    public:
    
      explicit GSLStudies(bool byStation, int mode, const std::string &token ); 
      
    private:
      bool byStations_; // Place holder, always done byStations. 
      int mode_; // a place holder, for tweaking algorithm(s);
      int phase_; // 
      double sigmaCut_;
      double maxPull_; // absolute value... 
      const double basicSigma_; // 60 microns/sqrt(12) 
      int nIterMax_; 
      std::string token_; // Should we use a CSV file for studies & debugging.. 
      std::vector<aGSLLinFitResult> allResults_;  // on coupled sensors, for a given station.
      //
      // final values of interest.. 
      //
      double finalOffsetXSly_, finalSlopeXSly_, finalChiSqXSly_;
      std::array<double, 3> finalCovMatXSly_, finalInvCovMatXSly_; 
      double finalOffsetYSlx_, finalSlopeYSlx_, finalChiSqYSlx_;
      std::array<double, 3> finalCovMatYSlx_, finalInvCovMatYSlx_; 
      
      

    // internal, volatile.. since GSL is C, use parallel array of double and ints. 
      int curIter_;
      bool debug_;
      std::vector<aGSLLinFitResult> tmpResults_;  // on iteration.
      double curOffset_, curSlope_, curChiSq_;
      std::array<double, 3> curCovMat_, curInvCovMat_; 
      std::vector<double> xVals_, yVals_;
      std::vector<std::vector<MilleRecord>::const_iterator > rejected_;  // spill evt..station, etc...  
      std::vector<double> resids_; // from our linear regression fit, to constriain offset and relative (X, Y) rotation angles. 
      
     
    public:
    
      void StudyByStationXY(const MilleRecords &mData, short iStation, short sensor = 0);
       // ignoring the W planes for now.. if iStation =999, do all of them.    
     
      inline void SetSigmaCut(double factor) { sigmaCut_ = factor; } // while pruning.. 
      inline void SetNIterMax(int n) { nIterMax_ = n; } // while pruning.. 
      inline void SetMaxPull(double mP) { maxPull_ = mP; } // while pruning.. 
      inline void SetDebugOn() { debug_ = true; } 
          
      bool doIt(const MilleRecords &myMrs, int newMode=-1 ); 
      // returns true if a sensorGSLLinFitResult has been generated.., and store in the above vector. 
      //
      // Accessor for analysis.. 
      size_t numResults() const { return allResults_.size(); } 
      inline std::vector<aGSLLinFitResult>::const_iterator CBegin() const {
         return allResults_.cbegin() ; 
      } 
      inline std::vector<aGSLLinFitResult>::const_iterator CEnd() const { 
         return allResults_.cend(); 
      } 
 
      inline double Chi2Norm(short iStation, short jCase, short kSensor, size_t index) const { // if 9999, the last one.. 
         if (allResults_.size() == 0) return 9.0e9; 	 
         for (auto itLf = allResults_.cbegin(); itLf != allResults_.cbegin(); itLf++) {
	   if ((itLf->station_ != iStation) || (itLf->jCase_ != jCase) || (itLf->sensor_ != kSensor)) continue;
	   size_t nnEvt = itLf->numEvts_;
	   if (nnEvt == 0) return 9.0e9;
	   const double chiSq = itLf->chiSq_/(nnEvt - 2);
	   return  chiSq;  
	 }
         return  9.0e9;
      }
      inline bool gotResult(short iStation, short jCase, short kSensor) const { // if 9999, the last one.. 
//         if (debug_) std::cerr << " emph::Align::GSLStudies:: got result for station " 
//	           << iStation << " jCase " << jCase << " sensor " <<  kSensor << std::endl;
         if (allResults_.size() == 0) return false; 	 	 
         for (auto itLf = allResults_.cbegin(); itLf != allResults_.cend(); itLf++) {
	   if ((itLf->station_ != iStation) || (itLf->jCase_ != jCase) || (itLf->sensor_ != kSensor)) continue;
	   return true; 
	 }
         return  false;
      }
      std::vector<aGSLLinFitResult>::const_iterator finalResult(short iStation, short jCase, short kSensor) const {
         if (allResults_.size() == 0) return allResults_.cend();
         for (auto itLf = allResults_.cbegin(); itLf != allResults_.cend(); itLf++) {
	   if ((itLf->station_ != iStation) || (itLf->jCase_ != jCase) || (itLf->sensor_ != kSensor)) continue;
	   size_t nnRes = itLf->numEvts_;
	   if (nnRes == 0) return allResults_.cend();	 
           return itLf;
	 }
	 return allResults_.cend();
      }

    private:
    
      
    
      size_t doOneStationXYFit(const MilleRecords &mData, short jCase, 
                              short iStation, short kSensor);  
			      // return the number of of events (data pts in the fit)  
      size_t pruneIt(const MilleRecords &mData, short jCase, short iStation, short kSensor, double numSigmas);
      size_t pruneItMaxPull (const MilleRecords &mData, short jCase, short iStation, short kSensor);
      // returns the number of rejected events  
      bool fillInvCovs(); 
       
      
    };
  } // name space align 
} // name space emph 

#endif //  GSLSTUDIES 
