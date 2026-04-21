////////////////////////////////////////////////////////////////////////
/// \brief   a Millepede-II measurement, and a contanier.. We keep things in memory
///          Works in conjunction with SingleTrackAlignmentV2 
///          and, to execute Millepe, MilleRun class. 
/// \date
/// 
/// \author  $Author: Paul Lebrun $ lebrun@fnal.gov
///    
////////////////////////////////////////////////////////////////////////
#ifndef MILLERECORD_H
#define MILLERECORD_H

#include <array>
#include <vector>
#include <map>
#include <string>

// assume by station.. 3 derivatives.. 

namespace emph {

  namespace align {
    
    class MilleRecord {
    
    public:
    
    explicit MilleRecord(int evtNum, short spill, const short station, const short plane, const short sensor, 
                         const std::array<float,3> dervis, const float coordOrtho, const float pull, const float err) ;
      int evtNum_;
      short spill_;
      short station_;
      short plane_;
      short sensor_;
      float coordOrtho_; //  Track model data.  
      float pull_;  
      float err_;
      std::array<float,3> derivs_; 
      
      friend std::ostream& operator << (std::ostream& o, const MilleRecord& h);     
    
    };
    // Now a container for these ... 
    class MilleRecords {
    
    public:
    
       MilleRecords(); 
       
    private:
    
       std::vector<MilleRecord> fData;
    
    public:
      
      inline void clear() { fData.clear(); } 
      inline void add(MilleRecord &mr) { fData.push_back(mr); } 
      
      void SaveIt(bool byStation, const std::string &newFileName) const; 
        
      inline size_t numRecords() const {return fData.size(); } 
      inline std::vector<MilleRecord>::const_iterator CBegin() const { return fData.cbegin(); } 
      inline std::vector<MilleRecord>::const_iterator CEnd() const { return fData.cend(); } 
       
      double CenterPulls(short station, short plane, short sensor); // take the average, for one ssensor at a time, and re-center it. 
      void CenterPulls(short station, short plane, short sensor, double v); // take the average, for one ssensor at a time, and re-center it. 
      void CenterPulls(const std::map<short, double> &vals ); // Not sure what that is for...  
      void ShiftCenterPulls(short station, double x, double y ); // To test the idea to run first with scalerrors = 10, then, shift, then 
                                                                 // run pede with no scalefactors.  
       
    
    };
  } // align name space 
} // emph

#endif // MILLERECORD_H
