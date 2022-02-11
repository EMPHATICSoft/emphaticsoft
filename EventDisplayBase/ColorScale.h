///
/// \file ColorScale.h 
/// \brief Build an association between a numerical range and a ROOT
/// color index
///
/// \version $Id: ColorScale.h,v 1.5 2011-07-21 21:04:34 brebel Exp $
/// \author messier@indiana.edu
///
#ifndef EVDB_COLORSCALE_H
#define EVDB_COLORSCALE_H
#include <string>

namespace evdb {
  /// \brief The list of default color options
  enum _color_scales {
    kRainbow,       /// ROY G. BIV (default)
    kInvRainbow,    /// Rainbow with high and low flipped
    kLinGray,       /// Linearized gray scale
    kColdToHot,     /// A color scale primarily in reds
    kBlueToRed,     /// Blue = cold, red = hot
    kBlueToRedII,   /// A color scale from dark blue (very cold) to red (hot) passing through yellow
    kHeatedObject,  /// A color scale from light to "burned"
    kReds,          /// A color scale primarily in reds
    kGreens,        /// A color scale primarily in greens
    kBlues,         /// A color scale primarily in blues
    kGeographic,    /// Light earth tones through green to blues
    kBlueToGreen,   /// Blue to green transition
    kBlueToOrange,  /// Blue to orange transition
    kGreenToMagenta,/// Green to magenta transition
    kSequential,    /// Useful for sequential data
    kFocus,         /// Sequential data with focus in center of scale
    kCustom         /// User specfied 
  };
  
  /// \brief How to scale between low and high ranges
  enum _scale_options {
    kLinear, /// Linearly (default)
    kLog,    /// According to log(x)
    kSqrt    /// According to sqrt(x)
  };
  
  /// \brief Build an association between a numerical range and a ROOT
  /// color index for use in, eg., the event display
  class ColorScale {
  public:
    ColorScale(double xlo, double xhi, 
	       int which=kRainbow,
	       int scale=kLinear,
	       int n=40, 
	       double h1=0, double h2=0, 
	       double v1=0, double v2=0);

    int  operator()(double x) const;
    int  GetColor(double x)   const;
    bool InBounds(double x)   const;
    void SetPalette();
    void SetBounds(double xlo, double xhi) { fXlo = xlo; fXhi = xhi; }
    void SetUnderFlowColor(int c);
    void SetOverFlowColor(int c);
    void Reverse();

    static int Palette(const std::string& nm);
    static int Scale(const std::string& nm);

  private:
    void HSVtoRGB(double  h, double  s, double v,
		  double* r, double* g, double* b) const;

    void MakeHSVScale(int n, double h1, double h2, double vs1, double vs2);
    void MakeSequential();
    void MakeFocus();
    void MakeInvRainbow();
    void MakeRainbow();
    void MakeGreenToMagenta();
    void MakeBlueToRed();
    void MakeBlueToRedII();
    void MakeBlueToGreen();
    void MakeBlueToOrange();
    void MakeBrownToBlue();
    void MakeLinGray();
    void MakeHeatedObject();
    
  private:
    double fXlo;            /// Numeric value at low end of scale
    double fXhi;            /// Numeric value at high end of scale
    int    fScale;          /// Linear? Log? Sqrt?
    int    fNcolor;         /// How many colors in scale?
    int    fColors[256];    /// List of ROOT color indicies
    int    fUnderFlowColor; /// Color to use for under flows
    int    fOverFlowColor;  /// Color to use for over flows
  };
}
#endif
////////////////////////////////////////////////////////////////////////
