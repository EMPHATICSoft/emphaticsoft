#include "EventDisplayBase/Colors.h"
#include <vector>
#include <string>
#include "TROOT.h"
#include "TStyle.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "EventDisplayBase/ColorScale.h"

namespace evdb {

  Colors::Colors(fhicl::ParameterSet const& p) : Reconfigurable{p}
  {
    this->reconfigure(p);
  }

  //......................................................................

  Colors::~Colors()
  {
    std::map<std::string,ColorScale*>::iterator itr(fColorScales.begin());
    std::map<std::string,ColorScale*>::iterator itrEnd(fColorScales.end());
    for (; itr!=itrEnd; ++itr) {
      if (itr->second) { delete itr->second; itr->second = 0; }
    }
  }

  //......................................................................

  void Colors::reconfigure(fhicl::ParameterSet const& p)
  {
    int black_on_white = p.get<int>("BlackOnWhite.val");
    if (black_on_white) this->BlackOnWhite();
    else                this->WhiteOnBlack();

    std::vector<std::string>
      cs = p.get<std::vector<std::string> >("ColorScales.val");
    for (unsigned int i=0; i<cs.size(); ++i) {
      this->UnpackColorScale(p,cs[i]);
    }
  }

  //......................................................................

  void Colors::UnpackColorScale(fhicl::ParameterSet const& p,
                                const std::string&         nm)
  {
    std::string palette_tag(nm); palette_tag += "_Palette.val";
    std::string n_tag(nm);       n_tag       += "_N.val";
    std::string r_tag(nm);       r_tag       += "_Range.val";
    std::string scale_tag(nm);   scale_tag   += "_Scale.val";
    std::string reverse_tag(nm); reverse_tag += "_Reverse.val";
    std::string ofufc_tag(nm);   ofufc_tag   += "_UnderOverflowColors.val";
    std::string hv_tag(nm);      hv_tag      += "_HVPairs.val";

    int n, reverse;
    std::string palette, scale;
    std::vector<float> r, hv;
    std::vector<int>   ofufc;
    palette = p.get<std::string>        (palette_tag);
    n       = p.get<int>                (n_tag);
    r       = p.get<std::vector<float> >(r_tag);
    scale   = p.get<std::string>        (scale_tag);
    reverse = p.get<int>                (reverse_tag);
    ofufc   = p.get<std::vector<int> >  (ofufc_tag);
    hv      = p.get<std::vector<float> >(hv_tag);

    ColorScale* cs = new ColorScale(r[0],
                                    r[1],
                                    ColorScale::Palette(palette),
                                    ColorScale::Scale(scale),
                                    n,
                                    hv[0],
                                    hv[1],
                                    hv[2],
                                    hv[3]);
    cs->SetUnderFlowColor(ofufc[0]);
    cs->SetOverFlowColor(ofufc[1]);
    if (reverse) cs->Reverse();
    ColorScale* old = fColorScales[nm];
    if (old) delete old;
    fColorScales[nm] = cs;
  }

  ///
  /// Look up a color scale by name
  ///
  ColorScale& Colors::Scale(const std::string& nm)
  {
    ColorScale* cs = fColorScales[nm];
    if (cs) return (*cs);

    static ColorScale gsDefaultCS(0,100);
    return gsDefaultCS;
  }

  //......................................................................

  void Colors::WhiteOnBlack()
  {
    fFG[0] = fBG[5] = kWhite;
    fFG[1] = fBG[4] = kGray;
    fFG[2] = fBG[3] = kGray+1;
    fFG[3] = fBG[2] = kGray+2;
    fFG[4] = fBG[1] = kGray+3;
    fFG[5] = fBG[0] = kBlack;
    this->SetStyle();
  }

  //......................................................................

  void Colors::BlackOnWhite()
  {
    fFG[5] = fBG[0] = kWhite;
    fFG[4] = fBG[1] = kGray;
    fFG[3] = fBG[2] = kGray+1;
    fFG[2] = fBG[3] = kGray+2;
    fFG[1] = fBG[4] = kGray+3;
    fFG[0] = fBG[5] = kBlack;
    this->SetStyle();
  }

  //......................................................................

  int Colors::Foreground(int i)
  {
    i = std::max(0,          i);
    i = std::min(kMAX_FGBG-1,i);
    return fFG[i];
  }

  //......................................................................

  int Colors::Background(int i)
  {
    i = std::max(0,          i);
    i = std::min(kMAX_FGBG-1,i);
    return fBG[i];
  }

  //......................................................................

  void Colors::SetStyle()
  {
    int bgcolor = this->Background(0);
    int fgcolor = this->Foreground(1);
    gStyle->SetAxisColor(fgcolor,"XYZ");
    gStyle->SetLabelColor(fgcolor,"XYZ");
    gStyle->SetTitleColor(fgcolor,"XYZ");
    gStyle->SetCanvasColor(bgcolor);
    gStyle->SetLegendFillColor(bgcolor);
    gStyle->SetPadColor(bgcolor);
    gStyle->SetFuncColor(kRed);
    gStyle->SetGridColor(fgcolor);
    gStyle->SetFrameFillColor(bgcolor);
    gStyle->SetFrameLineColor(bgcolor);
    // Leave histogram fill color clear
    // gStyle->SetHistFillColor(bgcolor);
    gStyle->SetHistLineColor(fgcolor);
    gStyle->SetStatColor(bgcolor);
    gStyle->SetStatTextColor(fgcolor);
    gStyle->SetTitleFillColor(bgcolor);
    gStyle->SetTitleTextColor(fgcolor);

    // Force this style on all histograms
    gROOT->ForceStyle();
  }

} // namespace evdb

////////////////////////////////////////////////////////////////////////
