////////////////////////////////////////////////////////////////////////
/// \brief   a class that execute one, or two, depending on the size of the pulls
///          a Millepede-II run
///           
/// \date
/// 
/// \author  $Author: Paul Lebrun $ lebrun@fnal.gov
///    
////////////////////////////////////////////////////////////////////////
#ifndef MILLERUN_H
#define MILLERUN_H

#include <array>
#include <vector>
#include <string>
#include "Alignment/MilleResult.h"
#include "Alignment/MilleRecord.h"

namespace emph {

  namespace align {
    
    class MilleRun {
    
    public:
    
      explicit MilleRun(bool byStation, int mode, const std::string &token ); 
      
    private:
      bool byStations_; // assumes that the placement of all individual sensor on a station mount is very precise
      bool fixRotation0X_, fixRotation0Y_, fixRotation4X_, fixRotation4Y_;
      int mode_; // a place holder, for tweaking algorithm(s);
      int phase_;
      short maxStation_; // internal.. 
      double scaleErrors_;
      std::string fNameSteerFile_; 
      std::string token_; 
      std::string fNameBinary_; 
      std::map<short, double> roughPulls_; // could be volatile.. 
      std::vector<MilleResult> allResults_; 
     
    public:  
     
      inline void SetScaleErrors(double factor) { scaleErrors_ = factor; }
      
      inline void fixRotation(short iStation, char view) {
        if (iStation == 0) {
	  if ((view == 'X') || (view == 'x')) fixRotation0X_ = true;
	  if ((view == 'Y') || (view == 'y')) fixRotation0Y_ = true;
	} else if (iStation == 4) {
	  if ((view == 'X') || (view == 'x')) fixRotation4X_ = true;
	  if ((view == 'Y') || (view == 'y')) fixRotation4Y_ = true;
        }
      }
     
      bool doIt(MilleRecords &myMrs, 
                const bool fixStationRotations,  const int fixAllStationPosButOne, const int fixAllStationPosRotButOne, 
                const int fixAllFreeOneRotStation, int newMode=-1 ); 
		
      bool doIt4Param(int newMode= 400 ); // 4 parameter per measurement, could include Z .. as per Jon & Robert.. 	
		 // not a const.. we will re-center if need be...Saving memory.. 
		 // also too many arguments.. 
      // returns true if a Mille*.res" has been generated..  
      //
      // Accessor for analysis.. 
      size_t numResults() const { return allResults_.size(); } 
      inline std::vector<MilleParamByStation>::const_iterator CBeginFinalResultSt() const {
         return allResults_[allResults_.size()-1].CBeginSt() ; 
      } 
      inline std::vector<MilleParamByStation>::const_iterator CEndFinalResultSt() const { 
         return allResults_[allResults_.size()-1].CEndSt(); 
      } 
      inline std::vector<MilleParamBySensor>::const_iterator CBeginFinalResultSe() const 
      { 
        return allResults_[allResults_.size()-1].CBeginSe(); 
      } 
      inline std::vector<MilleParamBySensor>::const_iterator CEndFinalResultSe() const { 
        return allResults_[allResults_.size()-1].CEndSe();; 
      } 
 
      inline double Chi2Norm() const { return  allResults_[allResults_.size()-1].Chi2Norm(); } 
      inline double NumRecords() const { return  allResults_[allResults_.size()-1].NumRecords(); } 
      inline double NumRejected() const { return allResults_[allResults_.size()-1].NumRejected();  } 

    private:
      
      void editSteerFile( const bool fixStationRotations,  const int fixAllStationPosButOne, const int fixAllStationPosRotButOne, 
                const int fixAllFreeOneRotStation) ; // too many sed command sto compose.. simple to write the damm file sfrom scratch, with 
		                                     // no comment. 

      void composeAndWriteSteerFile(const std::string &aNewFNameSteer, 
                const bool fixStationRotations,  const int fixAllStationPosButOne, const int fixAllStationPosRotButOne, 
                const int fixAllFreeOneRotStation) ;	

      bool runItNow(const std::string &aNameSteer);
      
      bool fillResult(bool is4P = false); 
    };
  } // name space align 
} // name space emph 

#endif //  MILLERUN 
