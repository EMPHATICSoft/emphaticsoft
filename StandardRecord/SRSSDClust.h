////////////////////////////////////////////////////////////////////////
// \file    SRSSDClust.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSSDCLUST_H
#define SRSSDCLUST_H

namespace caf
{
  /// An SRSSDClust is a simple descriptor for an SSDCluster.
  class SRSSDClust
  {
  public:
    SRSSDClust();
    virtual ~SRSSDClust();

    int station;
    int sens;
    double avgstrip;  ///< average strip
    double wgtavgstrip;
    double wgtrmsstrip;
    int minstrip;
    int maxstrip;
    double avgadc; //average adc
    double avgtime;
    int mintime;
    int maxtime;
    int timerange;
    int width;
    unsigned int ndigits;
    virtual void setDefault();
  };

} // end namespace

#endif // SRSSDCLUST_H
