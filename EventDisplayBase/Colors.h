/// $Id:$
///
/// \brief  Manage all things related to colors for the event display
/// \author messier@indiana.edu
///
#ifndef EVDB_COLORS_H
#define EVDB_COLORS_H

#include <vector>
#include <string>

#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "EventDisplayBase/Reconfigurable.h"

namespace evdb
{
  class ColorScale;
  class Colors : public Reconfigurable
  {
  public:
    Colors(fhicl::ParameterSet const& p);
    ~Colors();
    void reconfigure(fhicl::ParameterSet const& p);

    ///
    /// Set foreground and background colors for white text on black
    /// background
    ///
    void WhiteOnBlack();

    ///
    /// Set foreground and background colors for black text on black
    /// background
    ///
    void BlackOnWhite();

    ///
    /// Return the foreground color
    /// \param i : 0 is highest contrast to background color, 5 is
    ///            least
    ///
    int Foreground(int i=0);

    ///
    /// Return the background color
    /// \param i : 0 is highest contrast to foreground color, 5 is
    /// least
    ///
    int Background(int i=0);

    ///
    /// Look up a color scale by name
    ///
    ColorScale& Scale(const std::string& nm);

  private:
    ///
    /// Unpack the parameters for a named color scale
    ///
    void UnpackColorScale(fhicl::ParameterSet const& p,
                          const std::string&         c);
    ///
    /// Push the colors off to the ROOT style
    ///
    void SetStyle();

 private:
    static const int kMAX_FGBG = 6;
    int fFG[kMAX_FGBG]; ///< Foreground colors
    int fBG[kMAX_FGBG]; ///< Background colors
    ///
    /// Collection of color scales managed by this class
    ///
    std::map<std::string,ColorScale*> fColorScales;
  };
}

DECLARE_ART_SERVICE(evdb::Colors, LEGACY)
#endif // EVDB_COLORS
////////////////////////////////////////////////////////////////////////
