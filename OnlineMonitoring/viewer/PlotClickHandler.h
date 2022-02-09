#ifndef OM_PLOTCLICKHANDLER_H
#define OM_PLOTCLICKHANDLER_H
#include <string>

class TH1;
class TH2;
namespace emph { namespace onmon { class PlotOptions; } }

namespace emph { 
  namespace onmon {
    ///
    /// Handle clicks on the plot view histograms
    ///
    class PlotClickHandler {
    public:
      PlotClickHandler();
      ~PlotClickHandler();
      void SingleClick(const PlotOptions& plotopt);
      void DoubleClick(const PlotOptions& plotopt);
    private:
      int DecodeClick();

      void DefaultHistogramInfo();    

      std::string fHistoName;  ///< Name of Current Histogram

      TH1*   fH1;   ///< 1D histogram clicked on
      TH2*   fH2;   ///< 2D histogram clicked on
      double fX;    ///< X position of click
      double fY;    ///< Y position of click
      int    fXbin; ///< X bin number of click
      int    fYbin; ///< Y bin number of click
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
