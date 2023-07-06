////////////////////////////////////////////////////////////////////////
// \file    SRSSDHits.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSSDHits_H
#define SRSSDHits_H

namespace caf
{
  /// SRSSDHits is a simple descriptor for an SSDHits.
  class SRSSDHits
  {
  public:
    SRSSDHits(); // Default constructor
    // SSDHit(const emph::rawdata::SSDRawDigit &ssdraw, const emph::geo::Detector &st); // Default constructor
    virtual ~SRSSDHits(); //Destructor

    float FER;
    float Module;
    float Chip;
    float Set;
    float Strip;
    float Time;
    float ADC;
    float TrigNum;
    float Row;

    virtual void setDefault();
  };

} // end namespace

#endif // SRSSDHits_H
