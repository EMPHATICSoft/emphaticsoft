#ifndef OM_HISTODATA_H
#define OM_HISTODATA_H
#include <string>
#include <vector>

namespace om {
  //
  // Enumerate the supported types of detectors.
  //
  typedef enum _det_types {
    kEMPH       = 1,
    kSSD        = 2,
    kALLDET     = 100,
    kUnknownDet = 999999
  } Detector_t;

  //
  // Enumerate the supported types of histograms
  //
  typedef enum _histo_types {
    kUnknownType,
    kTH1F,
    kTH2F
  } Histo_t;

  //......................................................................

  ///
  /// A complete line of data describing a histogram used in the online
  /// monitoring.
  ///
  class HistoData
  {
  public:
    bool IsValid() const;
  public:
    std::vector<std::string> fCategory; ///< Categories this hist. belongs to
    unsigned int             fReset;    ///< Reset schedule (see TickerSubscriber.h)
    unsigned int             fLookBack; ///< How many copies to save in history
    Detector_t               fDetector; ///< Detector these settings apply to
    std::string              fTriggers; ///< Triggers to which histo applies
    Histo_t                  fType;     ///< What kind of histogram is this?
    std::string              fName;     ///< C++ name of this histogram
    std::string              fTitle;    ///< Titles for histogram
    int                      fNx;       ///< Number of bins in x
    double                   fX1;       ///< Low edge of x range
    double                   fX2;       ///< High edge of x range
    double                   fNy;       ///< Number of bins in y
    double                   fY1;       ///< Low edge of y range
    double                   fY2;       ///< High edge of y range
    std::vector<std::string> fOption;   ///< Booking/drawing options
    std::string              fCaption;  ///< What does this histogram show?
  };
}
#endif
////////////////////////////////////////////////////////////////////////
