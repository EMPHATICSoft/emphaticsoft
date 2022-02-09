#ifndef OM_PLOTOPTIONS_H
#define OM_PLOTOPTIONS_H
#include "OnlineMonitoring/viewer/UTCLabel.h"
#include <vector>
#include <string>
#include <list>

class TPad;
class TH1;
class TH1F;
class TH2F;
class TPaveText;

namespace emph { namespace onmon { class HistoData; } }

namespace emph { 
  namespace onmon {
    ///
    /// Encapsulate all the plotting options
    ///
    class PlotOptions {
    public:
      PlotOptions();
      ~PlotOptions();

      void Reset();
      void Set(const std::vector<std::string>& opt);

      void SetPad(TPad* p);
      void MakeLabels(const TH1* h, const HistoData* hd);

      void MakeLabelText(const TH1* h);

      void AutoScale(TH1F* h);
      void AutoScale(TH2F* h);

    private:
      void ParseXscale(const char* opt);
      void ParseYscale(const char* opt);
      void ParseZscale(const char* opt);

    public:
      ///
      /// The list of custom plotting options
      ///
      bool fZoomHour;      ///< A special zoom option for plots vs. UTC hour
      bool fAutoZoomX;     ///< Auto zoom the horizontal scale
      bool fAutoZoomY;     ///< Auto zoom the vertical scale
      bool fAutoZoomZ;     ///< Auto zoom the z scale
      bool fAlert;         ///< Draw histo in "alert" mode

      ///
      /// Pad plotting options
      ///
      bool   fLogx;       ///< Log X axis
      bool   fLogy;       ///< Log Y axis
      bool   fLogz;       ///< Log Z axis
      bool   fGridx;      ///< Grid x
      bool   fGridy;      ///< Grid y
      bool   fHaveXscale; ///< User specified x-scale?
      double fXlo;        ///< Low end of user-specified x-scale
      double fXhi;        ///< High end of user-specified x-scale
      bool   fHaveYscale; ///< User specified y-scale?
      double fYlo;        ///< Low end of user-specified y-scale
      double fYhi;        ///< High end of user-specified y-scale
      bool   fHaveZscale; ///< User specified z-scale?
      double fZlo;        ///< Low end of user-specified z-scale
      double fZhi;        ///< High end of user-specified z-scale

      ///
      /// Histogram drawing options accumulate here
      ///
      std::string fDrawOpt;
      ///
      /// Some label objects
      ///
      TPaveText* fLabelText;
      TPaveText* fSLText;
      TLine* fLineTmin;
      TLine* fLineTmax;
      ///
      /// Label Maker for UTC plots
      ///
      UTCLabel fUTCLabel;
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
