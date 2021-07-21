////////////////////////////////////////////////////////////////////////
/// \brief   Definition of basic raw digits
/// \author  jpaley@indiana.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef DAQHEADER_H
#define DAQHEADER_H

#include <iosfwd>
#include <vector>

namespace rawdata {

  // Detector ID is identified in NovaDAQConventions package as an int

  class DAQHeader 
  {
  public:
    DAQHeader(); // Default constructor
    DAQHeader(int detId, unsigned int status, int TotalMicroSlices = -1);
    DAQHeader(const DAQHeader& ); // Copy constructor
    
    ~DAQHeader();
    
    unsigned int                 Status() const { return fStatus; }
    int                          DetId() const  { return fDetId; }
    int                          TotalMicroSlices() const { return fTotalMicroSlices; }
    
    void  SetStatus(unsigned int                i) { fStatus = i;}
    void  SetDetId(int i) { fDetId = i;}
    void  SetTotalMicroSlices(int i){ fTotalMicroSlices = i;}
    
  private:
    unsigned int       fStatus;
    int                fDetId;
    int                fTotalMicroSlices;
    
  };
}

#endif // DAQHEADER_H
////////////////////////////////////////////////////////////////////////
