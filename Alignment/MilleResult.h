////////////////////////////////////////////////////////////////////////
/// \brief   a Millepede-II measurement, and a contanier.. We keep things in memory
///          Works in conjunction with SingleTrackAlignmentV2 
///          and, to execute Millepe, MilleRun class. 
/// \date
/// 
/// \author  $Author: Paul Lebrun $ lebrun@fnal.gov
///    
////////////////////////////////////////////////////////////////////////
#ifndef MILLERECSULT_H
#define MILLERECSULT_H

#include <vector>
#include <string>

// assume by station.. 3 derivatives.. 

namespace emph {

  namespace align {
    
    class MilleParam {
      public:
        explicit MilleParam(bool isFixed, int label, double val, double err, int glCorr);  
        const bool isFixed_; // this is the pre-sigma, but, in our context, it can only have 0 or -1 values. 
        const int label_; 
        const double val_;
        const double err_;
        const int glCorr_; // Not sure how the global correlation is defined.
    };
    
    class MilleParamByStation : public MilleParam {
      public: 
        explicit MilleParamByStation (bool isFixed, int label, double val, double err, int glCorr); 
        const int station_;
	std::string glVar_;  //Should be const, but it does not let me.. 
    };      
      
    class MilleParamBySensor : public MilleParam {
      public: 
        explicit MilleParamBySensor(bool isFixed, int label, double val, double err, int glCorr);
	const bool is4P_;
        const int station_, plane_, sensor_; 
	std::string glVar_;  
    };      
    class MilleParamBySensor4P : public MilleParam {
      public: 
        explicit MilleParamBySensor4P(bool isFixed, int label, double val, double err, int glCorr);
	const bool is4P_;
        const int station_, plane_, sensor_; 
	std::string glVar_;  
    };      
    //
    // Now a container for these parameters. 
    //
    class MilleResult {
    
    public:
    
      explicit MilleResult(bool isByStation, const std::string &fNameRes, bool is4P=false) ;
      explicit MilleResult(bool isByStation, const MilleResult &r1, const MilleResult &r2, bool is4P=false) ;
    
    private:
    
      bool isByStation_; 
      int numRec_; 
      int numRejected_; 
      double chi2Ndf_;
      std::vector<MilleParamByStation> dataByStation_; 
      std::vector<MilleParamBySensor> dataBySensor_; 
      std::vector<MilleParamBySensor4P> dataBySensor4P_; 
 
    public:
     
      inline bool IsByStations() const { return isByStation_; } 
      inline size_t NumParams() const {
        if (isByStation_) return dataByStation_.size();
	else {
	  if (dataBySensor_.size() > dataBySensor4P_.size()) {
	    return dataBySensor_.size();
	  } else {
	    return dataBySensor4P_.size();
	  }
	}
      }
      inline std::vector<MilleParamByStation>::const_iterator CBeginSt() const { return dataByStation_.cbegin(); } 
      inline std::vector<MilleParamByStation>::const_iterator CEndSt() const { return dataByStation_.cend(); } 
      inline std::vector<MilleParamBySensor>::const_iterator CBeginSe() const { return dataBySensor_.cbegin(); } 
      inline std::vector<MilleParamBySensor>::const_iterator CEndSe() const { return dataBySensor_.cend(); } 
      inline std::vector<MilleParamBySensor4P>::const_iterator CBeginSe4P() const { return dataBySensor4P_.cbegin(); } 
      inline std::vector<MilleParamBySensor4P>::const_iterator CEndSe4P() const { return dataBySensor4P_.cend(); } 
      inline double Chi2Norm() const { return  chi2Ndf_; } 
      inline double NumRecords() const { return  numRec_; } 
      inline double NumRejected() const { return  numRejected_; } 
      
    private:
      void snifOutDataFromLog();  
      
    };
  } // align name space 
} // emph

#endif // MILLERECSULT_H
