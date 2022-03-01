////////////////////////////////////////////////////////////////////////
/// \file  ColorScale.h
/// \brief Define a color scale for displaying numeric data
///
/// \version $Id: ColorScale.cxx,v 1.3 2011-04-19 22:50:49 brebel Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#include "EventDisplayBase/ColorScale.h"

#include <iostream>
#include <cmath>

#include "TStyle.h"
#include "TColor.h"

namespace evdb {
  int ColorScale::Palette(const std::string& nm)
  {
    static const char* cs[] = {
      "Rainbow",     "InvRainbow",    "LinGray",     "ColdToHot",
      "BlueToRed",   "BlueToRedII",   "HeatedObject","Reds",
      "Greens",      "Blues",         "Geographic",  "BlueToGreen",
      "BlueToOrange","GreenToMagenta","Sequential",  "Focus",
      "Custom", 0
    };
    int i;
    for (i=0; cs[i]!=0; ++i) {
      if (nm==cs[i]) return i;
    }
    return 0;
  }
  int ColorScale::Scale(const std::string& nm)
  {
    static const char* cs[] = {
      "linear", "log","sqrt", 0
    };
    int i;
    for (i=0; cs[i]!=0; ++i) {
      if (nm==cs[i]) return i;
    }
    return 0;
  }

  ///
  /// Construct a color scale
  ///
  /// \param xlo - The value at the low end of the scale
  /// \param xhi - The value at the high end of the scale
  /// \param which - Which color map to use (see enum's in ColorScale.h)
  /// \param scale - How to scale range (see enum's in ColorScale.h)
  /// \param n     - How many colors to use
  /// \param h1    - First H value in range
  /// \param h2    - Last H value in range
  /// \param v1    - First V value in range
  /// \param v2    - Last V value in range
  ///
  ColorScale::ColorScale(double xlo, double xhi,
			 int which, int scale, int n,
			 double h1, double h2, double v1, double v2) :
    fXlo(xlo),
    fXhi(xhi),
    fScale(scale),
    fNcolor(n),
    fUnderFlowColor(-1),
    fOverFlowColor(-1)
  {
    if(fNcolor > 256) fNcolor = 256;

    switch(which) {
    case kSequential:     this->MakeSequential();                         break;
    case kFocus:          this->MakeFocus();                              break;
    case kInvRainbow:     this->MakeInvRainbow();                         break;
    case kGreenToMagenta: this->MakeGreenToMagenta();                     break;
    case kBlueToRed:      this->MakeBlueToRed();                          break;
    case kBlueToRedII:    this->MakeBlueToRedII();                        break;
    case kBlueToOrange:   this->MakeBlueToOrange();                       break;
    case kGeographic:     this->MakeBrownToBlue();                        break;
    case kLinGray:        this->MakeLinGray();                            break;
    case kHeatedObject:   this->MakeHeatedObject();                       break;
    case kColdToHot:      this->MakeHSVScale(n, 150.0, 0.0,   0.2, 0.5);  break;
    case kReds:           this->MakeHSVScale(n, 30.0,  0.0,   0.1, 0.9);  break;
    case kBlues:          this->MakeHSVScale(n, 180.0, 270.0, 0.1, 0.9);  break;
    case kGreens:         this->MakeHSVScale(n, 90.0,  120.0, 0.1, 0.9);  break;
    case kCustom:         this->MakeHSVScale(n, h1, h2, v1, v2);          break;
    case kRainbow:
      // Default to a rainbow.
    default:
      this->MakeRainbow();
      break;
    }
  }

  //......................................................................

  void ColorScale::SetUnderFlowColor(int c) { fUnderFlowColor = c; }

  //......................................................................

  void ColorScale::SetOverFlowColor(int c)  { fOverFlowColor  = c; }

  //......................................................................

  void ColorScale::Reverse()
  {
    std::swap(fUnderFlowColor, fOverFlowColor);
    for (int i=0; i<fNcolor/2; ++i) {
      std::swap(fColors[i], fColors[fNcolor-i-1]);
    }
  }

  //......................................................................

  bool ColorScale::InBounds(double x) const
  {
    if (x>=fXlo && x<=fXhi) return true;
    return false;
  }

  //......................................................................
  ///
  /// Assign a ROOT color index to the value x 
  ///
  /// \param   x - the value on the scale
  /// \returns A ROOT color number
  ///
  int ColorScale::GetColor(double x) const
  {
    if (x<fXlo && fUnderFlowColor!=-1) return fUnderFlowColor;
    if (x>fXhi && fOverFlowColor !=-1) return fOverFlowColor;
  
    double f=0.0;
    if (fScale == kLinear) {
      f = (x-fXlo)/(fXhi-fXlo);
    }
    else if (fScale == kLog) {
      f = (log(x)-log(fXlo))/(log(fXhi)-log(fXlo));
    }
    else if (fScale == kSqrt) {
      f = (sqrt(x)-sqrt(fXlo))/(sqrt(fXhi)-sqrt(fXlo));
    }

    int indx = (int)floor(f*(float)fNcolor);
    if (indx<0)        indx = 0;
    if (indx>=fNcolor) indx = fNcolor-1;

    return fColors[indx];
  }
  int ColorScale::operator()(double x) const { 
    return this->GetColor(x); 
  }

  //.....................................................................
  ///
  /// Convert hue, saturation and value numbers to red, green, blue
  ///
  /// \param h - hue (in range from 0 to 360)
  /// \param s - saturation
  /// \param v - value
  /// \param r - returned r value
  /// \param g - returned g value
  /// \param b - returned b value
  ///
  void ColorScale::HSVtoRGB(double h,  double s,  double v,
			    double* r, double* g, double* b) const
  {
    // A-chromatic, return grey scale values
    if (s==0.0) { *r = *g = *b = v; return; }

    int i;
    double f, p, q, t;
    double hh = h;
    while (hh<  0.0) hh += 360.0;
    while (hh>360.0) hh -= 360.0;
    hh /= 60;
    i = (int)floor(hh);
    f = hh - i;
    p = v * (1 - s);
    q = v * (1 - s*f);
    t = v * (1 - s*(1-f) );
    switch( i ) {
    case 0:  *r = v; *g = t; *b = p; break;
    case 1:  *r = q; *g = v; *b = p; break;
    case 2:  *r = p; *g = v; *b = t; break;
    case 3:  *r = p; *g = q; *b = v; break;
    case 4:  *r = t; *g = p; *b = v; break;
    default: *r = v; *g = p; *b = q; break;
    }
  }

  //......................................................................

  /// \brief Make a color scale of n colors ranging between two points
  /// in an HSV color space.
  ///
  /// Choose points so that the value of the colors changes
  /// uniformly. This ensures good viewing event in black and white.
  ///
  /// \param n   - number of colors in the scale
  /// \param h1  - first H value in color scale
  /// \param h2  - second H value in color scale
  /// \param vs1 - first V value in color scale
  /// \param vs2 - second V value in color scale
  ///
  void ColorScale::MakeHSVScale(int n,
				double h1, double h2,
				double vs1, double vs2) 
  {
    int i;
    double r, g, b;
    double h;
    double vs, v, s;
  
    if (n>128) n = 128;
    fNcolor = n;

    for (i=0; i<fNcolor; ++i) {
      h  = h1  + (h2-h1)*(float)i/(float)(fNcolor-1);
      vs = vs1 + (vs2-vs1)*(float)i/(float)(fNcolor-1);
      vs = -1.0 + 2.0*vs;
      if (vs<0.0) { v = 1.0; s = 1.0+vs; }
      else { v = 1.0-vs; s = 1.0; }
      this->HSVtoRGB(h, s, v, &r, &g, &b);
      r *= 255;
      g *= 255;
      b *= 255;
      fColors[i] = TColor::GetColor((int)r,(int)g,(int)b);
    }
  }

  //......................................................................
  ///
  /// Set the ROOT color palette to use this color scale
  ///
  void ColorScale::SetPalette()
  {
    gStyle->SetPalette(fNcolor, fColors);
  }

  //......................................................................
  ///
  /// Build the sequential color map
  ///
  void ColorScale::MakeSequential()
  {
    fNcolor = 25;
    static int rgb[25][3] = {
      {153,      15,      15},
      {178,      44,      44},
      {204,      81,      81},
      {229,     126,     126},
      {255,     178,     178},
      {153,      84,      15},
      {178,     111,      44},
      {204,     142,      81},
      {229,     177,     126},
      {255,     216,     178},
      {107,     153,      15},
      {133,     178,      44},
      {163,     204,      81},
      {195,     229,     126},
      {229,     255,     178},
      {15,      107,     153},
      {44,      133,     178},
      {81,      163,     204},
      {126,     195,     229},
      {178,     229,     255},
      {38,       15,     153},
      {66,       44,     178},
      {101,      81,     204},
      {143,     126,     229},
      {191,     178,     255}
    };
    for (int i=0; i<25; ++i) {
      fColors[i] = TColor::GetColor(rgb[i][0],rgb[i][1],rgb[i][2]);
    }
  }

  //......................................................................
  ///
  /// Build a sequential color map with red at the center
  ///
  void ColorScale::MakeFocus()
  {
    fNcolor = 25;
    static int rgb[25][3] = {
      {178,     229,     255}, // blue5
      {126,     195,     229}, // blue4
      {81,      163,     204}, // blue3
      {44,      133,     178}, // blue2
      {15,      107,     153}, // blue1
      {191,     178,     255}, // violet5
      {143,     126,     229}, // violet4
      {101,      81,     204}, // violet3
      {66,       44,     178}, // violet2
      {38,       15,     153}, // violet1
      {153,      15,      15}, // red1
      {178,      44,      44}, // red2
      {204,      81,      81}, // red3
      {229,     126,     126}, // red4
      {255,     178,     178}, // red5
      {153,      84,      15}, // brown1
      {178,     111,      44}, // brown2
      {204,     142,      81}, // brown3
      {229,     177,     126}, // brown4
      {255,     216,     178}, // brown5
      {107,     153,      15}, // green1
      {133,     178,      44}, // green2
      {163,     204,      81}, // green3
      {195,     229,     126}, // green4
      {229,     255,     178}  // green5
    };
    for (int i=0; i<25; ++i) {
      fColors[i] = TColor::GetColor(rgb[i][0],rgb[i][1],rgb[i][2]);
    }
  }

  //......................................................................

  void ColorScale::MakeInvRainbow()
  {
    int rgb[256];
    rgb[  0] = TColor::GetColor(  0,    0,  0  );
    rgb[  1] = TColor::GetColor(  45,   0,  36 );
    rgb[  2] = TColor::GetColor(  56,   0,  46 );
    rgb[  3] = TColor::GetColor(  60,   0,  49 );
    rgb[  4] = TColor::GetColor(  67,   0,  54 );
    rgb[  5] = TColor::GetColor(  70,   0,  59 );
    rgb[  6] = TColor::GetColor(  71,   0,  61 );
    rgb[  7] = TColor::GetColor(  75,   0,  68 );
    rgb[  8] = TColor::GetColor(  74,   0,  73 );
    rgb[  9] = TColor::GetColor(  74,   0,  77 );
    rgb[ 10] = TColor::GetColor(  73,   0,  81 );
    rgb[ 11] = TColor::GetColor(  71,   0,  87 );
    rgb[ 12] = TColor::GetColor(  69,   1,  90 );
    rgb[ 13] = TColor::GetColor(  68,   2,  94 );
    rgb[ 14] = TColor::GetColor(  66,   3,  97 );
    rgb[ 15] = TColor::GetColor(  63,   6, 102 );
    rgb[ 16] = TColor::GetColor(  61,   7, 106 );
    rgb[ 17] = TColor::GetColor(  58,  10, 109 );
    rgb[ 18] = TColor::GetColor(  56,  12, 113 );
    rgb[ 19] = TColor::GetColor(  53,  15, 116 );
    rgb[ 20] = TColor::GetColor(  48,  18, 119 );
    rgb[ 21] = TColor::GetColor(  47,  20, 121 );
    rgb[ 22] = TColor::GetColor(  44,  23, 124 );
    rgb[ 23] = TColor::GetColor(  41,  27, 128 );
    rgb[ 24] = TColor::GetColor(  40,  28, 129 );
    rgb[ 25] = TColor::GetColor(  37,  32, 132 );
    rgb[ 26] = TColor::GetColor(  34,  36, 134 );
    rgb[ 27] = TColor::GetColor(  29,  43, 137 );
    rgb[ 28] = TColor::GetColor(  25,  52, 138 );
    rgb[ 29] = TColor::GetColor(  24,  57, 139 );
    rgb[ 30] = TColor::GetColor(  24,  62, 141 );
    rgb[ 31] = TColor::GetColor(  24,  64, 142 );
    rgb[ 32] = TColor::GetColor(  23,  65, 142 );
    rgb[ 33] = TColor::GetColor(  23,  69, 143 );
    rgb[ 34] = TColor::GetColor(  23,  71, 142 );
    rgb[ 35] = TColor::GetColor(  23,  71, 142 );
    rgb[ 36] = TColor::GetColor(  23,  73, 142 );
    rgb[ 37] = TColor::GetColor(  23,  75, 142 );
    rgb[ 38] = TColor::GetColor(  23,  75, 142 );
    rgb[ 39] = TColor::GetColor(  23,  78, 142 );
    rgb[ 40] = TColor::GetColor(  23,  80, 142 );
    rgb[ 41] = TColor::GetColor(  23,  80, 142 );
    rgb[ 42] = TColor::GetColor(  23,  82, 141 );
    rgb[ 43] = TColor::GetColor(  23,  85, 141 );
    rgb[ 44] = TColor::GetColor(  23,  85, 141 );
    rgb[ 45] = TColor::GetColor(  23,  87, 140 );
    rgb[ 46] = TColor::GetColor(  23,  87, 140 );
    rgb[ 47] = TColor::GetColor(  24,  90, 140 );
    rgb[ 48] = TColor::GetColor(  24,  90, 140 );
    rgb[ 49] = TColor::GetColor(  24,  93, 139 );
    rgb[ 50] = TColor::GetColor(  24,  93, 139 );
    rgb[ 51] = TColor::GetColor(  24,  93, 139 );
    rgb[ 52] = TColor::GetColor(  24,  93, 139 );
    rgb[ 53] = TColor::GetColor(  24,  97, 139 );
    rgb[ 54] = TColor::GetColor(  24,  97, 139 );
    rgb[ 55] = TColor::GetColor(  25, 101, 138 );
    rgb[ 56] = TColor::GetColor(  25, 101, 138 );
    rgb[ 57] = TColor::GetColor(  25, 104, 137 );
    rgb[ 58] = TColor::GetColor(  25, 104, 137 );
    rgb[ 59] = TColor::GetColor(  25, 104, 137 );
    rgb[ 60] = TColor::GetColor(  26, 108, 137 );
    rgb[ 61] = TColor::GetColor(  26, 108, 137 );
    rgb[ 62] = TColor::GetColor(  27, 111, 136 );
    rgb[ 63] = TColor::GetColor(  27, 111, 136 );
    rgb[ 64] = TColor::GetColor(  27, 111, 136 );
    rgb[ 65] = TColor::GetColor(  27, 115, 135 );
    rgb[ 66] = TColor::GetColor(  27, 115, 135 );
    rgb[ 67] = TColor::GetColor(  28, 118, 134 );
    rgb[ 68] = TColor::GetColor(  28, 118, 134 );
    rgb[ 69] = TColor::GetColor(  29, 122, 133 );
    rgb[ 70] = TColor::GetColor(  29, 122, 133 );
    rgb[ 71] = TColor::GetColor(  29, 122, 133 );
    rgb[ 72] = TColor::GetColor(  29, 122, 133 );
    rgb[ 73] = TColor::GetColor(  29, 125, 132 );
    rgb[ 74] = TColor::GetColor(  29, 125, 132 );
    rgb[ 75] = TColor::GetColor(  30, 128, 131 );
    rgb[ 76] = TColor::GetColor(  30, 128, 131 );
    rgb[ 77] = TColor::GetColor(  31, 131, 130 );
    rgb[ 78] = TColor::GetColor(  31, 131, 130 );
    rgb[ 79] = TColor::GetColor(  31, 131, 130 );
    rgb[ 80] = TColor::GetColor(  32, 134, 128 );
    rgb[ 81] = TColor::GetColor(  32, 134, 128 );
    rgb[ 82] = TColor::GetColor(  33, 137, 127 );
    rgb[ 83] = TColor::GetColor(  33, 137, 127 );
    rgb[ 84] = TColor::GetColor(  33, 137, 127 );
    rgb[ 85] = TColor::GetColor(  34, 140, 125 );
    rgb[ 86] = TColor::GetColor(  34, 140, 125 );
    rgb[ 87] = TColor::GetColor(  35, 142, 123 );
    rgb[ 88] = TColor::GetColor(  35, 142, 123 );
    rgb[ 89] = TColor::GetColor(  36, 145, 121 );
    rgb[ 90] = TColor::GetColor(  36, 145, 121 );
    rgb[ 91] = TColor::GetColor(  36, 145, 121 );
    rgb[ 92] = TColor::GetColor(  37, 147, 118 );
    rgb[ 93] = TColor::GetColor(  37, 147, 118 );
    rgb[ 94] = TColor::GetColor(  38, 150, 116 );
    rgb[ 95] = TColor::GetColor(  38, 150, 116 );
    rgb[ 96] = TColor::GetColor(  40, 152, 113 );
    rgb[ 97] = TColor::GetColor(  40, 152, 113 );
    rgb[ 98] = TColor::GetColor(  41, 154, 111 );
    rgb[ 99] = TColor::GetColor(  41, 154, 111 );
    rgb[100] = TColor::GetColor(  42, 156, 108 );
    rgb[101] = TColor::GetColor(  42, 156, 108 );
    rgb[102] = TColor::GetColor(  43, 158, 106 );
    rgb[103] = TColor::GetColor(  43, 158, 106 );
    rgb[104] = TColor::GetColor(  43, 158, 106 );
    rgb[105] = TColor::GetColor(  45, 160, 104 );
    rgb[106] = TColor::GetColor(  45, 160, 104 );
    rgb[107] = TColor::GetColor(  46, 162, 101 );
    rgb[108] = TColor::GetColor(  46, 162, 101 );
    rgb[109] = TColor::GetColor(  48, 164,  99 );
    rgb[110] = TColor::GetColor(  48, 164,  99 );
    rgb[111] = TColor::GetColor(  50, 166,  97 );
    rgb[112] = TColor::GetColor(  50, 166,  97 );
    rgb[113] = TColor::GetColor(  51, 168,  95 );
    rgb[114] = TColor::GetColor(  53, 170,  93 );
    rgb[115] = TColor::GetColor(  53, 170,  93 );
    rgb[116] = TColor::GetColor(  53, 170,  93 );
    rgb[117] = TColor::GetColor(  55, 172,  91 );
    rgb[118] = TColor::GetColor(  55, 172,  91 );
    rgb[119] = TColor::GetColor(  57, 174,  88 );
    rgb[120] = TColor::GetColor(  57, 174,  88 );
    rgb[121] = TColor::GetColor(  59, 175,  86 );
    rgb[122] = TColor::GetColor(  62, 177,  84 );
    rgb[123] = TColor::GetColor(  64, 178,  82 );
    rgb[124] = TColor::GetColor(  64, 178,  82 );
    rgb[125] = TColor::GetColor(  67, 180,  80 );
    rgb[126] = TColor::GetColor(  67, 180,  80 );
    rgb[127] = TColor::GetColor(  69, 181,  79 );
    rgb[128] = TColor::GetColor(  72, 183,  77 );
    rgb[129] = TColor::GetColor(  72, 183,  77 );
    rgb[130] = TColor::GetColor(  72, 183,  77 );
    rgb[131] = TColor::GetColor(  75, 184,  76 );
    rgb[132] = TColor::GetColor(  77, 186,  74 );
    rgb[133] = TColor::GetColor(  80, 187,  73 );
    rgb[134] = TColor::GetColor(  83, 189,  72 );
    rgb[135] = TColor::GetColor(  87, 190,  72 );
    rgb[136] = TColor::GetColor(  91, 191,  71 );
    rgb[137] = TColor::GetColor(  95, 192,  70 );
    rgb[138] = TColor::GetColor(  99, 193,  70 );
    rgb[139] = TColor::GetColor( 103, 194,  70 );
    rgb[140] = TColor::GetColor( 107, 195,  70 );
    rgb[141] = TColor::GetColor( 111, 196,  70 );
    rgb[142] = TColor::GetColor( 111, 196,  70 );
    rgb[143] = TColor::GetColor( 115, 196,  70 );
    rgb[144] = TColor::GetColor( 119, 197,  70 );
    rgb[145] = TColor::GetColor( 123, 197,  70 );
    rgb[146] = TColor::GetColor( 130, 198,  71 );
    rgb[147] = TColor::GetColor( 133, 199,  71 );
    rgb[148] = TColor::GetColor( 137, 199,  72 );
    rgb[149] = TColor::GetColor( 140, 199,  72 );
    rgb[150] = TColor::GetColor( 143, 199,  73 );
    rgb[151] = TColor::GetColor( 143, 199,  73 );
    rgb[152] = TColor::GetColor( 147, 199,  73 );
    rgb[153] = TColor::GetColor( 150, 199,  74 );
    rgb[154] = TColor::GetColor( 153, 199,  74 );
    rgb[155] = TColor::GetColor( 156, 199,  75 );
    rgb[156] = TColor::GetColor( 160, 200,  76 );
    rgb[157] = TColor::GetColor( 167, 200,  78 );
    rgb[158] = TColor::GetColor( 170, 200,  79 );
    rgb[159] = TColor::GetColor( 173, 200,  79 );
    rgb[160] = TColor::GetColor( 173, 200,  79 );
    rgb[161] = TColor::GetColor( 177, 200,  80 );
    rgb[162] = TColor::GetColor( 180, 200,  81 );
    rgb[163] = TColor::GetColor( 183, 199,  82 );
    rgb[164] = TColor::GetColor( 186, 199,  82 );
    rgb[165] = TColor::GetColor( 190, 199,  83 );
    rgb[166] = TColor::GetColor( 196, 199,  85 );
    rgb[167] = TColor::GetColor( 199, 198,  85 );
    rgb[168] = TColor::GetColor( 199, 198,  85 );
    rgb[169] = TColor::GetColor( 203, 198,  86 );
    rgb[170] = TColor::GetColor( 206, 197,  87 );
    rgb[171] = TColor::GetColor( 212, 197,  89 );
    rgb[172] = TColor::GetColor( 215, 196,  90 );
    rgb[173] = TColor::GetColor( 218, 195,  91 );
    rgb[174] = TColor::GetColor( 224, 194,  94 );
    rgb[175] = TColor::GetColor( 224, 194,  94 );
    rgb[176] = TColor::GetColor( 230, 193,  96 );
    rgb[177] = TColor::GetColor( 233, 192,  98 );
    rgb[178] = TColor::GetColor( 236, 190, 100 );
    rgb[179] = TColor::GetColor( 238, 189, 104 );
    rgb[180] = TColor::GetColor( 240, 188, 106 );
    rgb[181] = TColor::GetColor( 240, 188, 106 );
    rgb[182] = TColor::GetColor( 242, 187, 110 );
    rgb[183] = TColor::GetColor( 244, 185, 114 );
    rgb[184] = TColor::GetColor( 245, 184, 116 );
    rgb[185] = TColor::GetColor( 247, 183, 120 );
    rgb[186] = TColor::GetColor( 248, 182, 123 );
    rgb[187] = TColor::GetColor( 248, 182, 123 );
    rgb[188] = TColor::GetColor( 250, 181, 125 );
    rgb[189] = TColor::GetColor( 251, 180, 128 );
    rgb[190] = TColor::GetColor( 252, 180, 130 );
    rgb[191] = TColor::GetColor( 253, 180, 133 );
    rgb[192] = TColor::GetColor( 253, 180, 133 );
    rgb[193] = TColor::GetColor( 254, 180, 134 );
    rgb[194] = TColor::GetColor( 254, 179, 138 );
    rgb[195] = TColor::GetColor( 255, 179, 142 );
    rgb[196] = TColor::GetColor( 255, 179, 145 );
    rgb[197] = TColor::GetColor( 255, 179, 145 );
    rgb[198] = TColor::GetColor( 255, 179, 152 );
    rgb[199] = TColor::GetColor( 255, 180, 161 );
    rgb[200] = TColor::GetColor( 255, 180, 164 );
    rgb[201] = TColor::GetColor( 255, 180, 167 );
    rgb[202] = TColor::GetColor( 255, 180, 167 );
    rgb[203] = TColor::GetColor( 255, 181, 169 );
    rgb[204] = TColor::GetColor( 255, 181, 170 );
    rgb[205] = TColor::GetColor( 255, 182, 173 );
    rgb[206] = TColor::GetColor( 255, 183, 176 );
    rgb[207] = TColor::GetColor( 255, 183, 176 );
    rgb[208] = TColor::GetColor( 255, 184, 179 );
    rgb[209] = TColor::GetColor( 255, 185, 179 );
    rgb[210] = TColor::GetColor( 255, 185, 182 );
    rgb[211] = TColor::GetColor( 255, 186, 182 );
    rgb[212] = TColor::GetColor( 255, 186, 182 );
    rgb[213] = TColor::GetColor( 255, 187, 185 );
    rgb[214] = TColor::GetColor( 255, 188, 185 );
    rgb[215] = TColor::GetColor( 255, 189, 188 );
    rgb[216] = TColor::GetColor( 255, 189, 188 );
    rgb[217] = TColor::GetColor( 255, 190, 188 );
    rgb[218] = TColor::GetColor( 255, 191, 191 );
    rgb[219] = TColor::GetColor( 255, 192, 191 );
    rgb[220] = TColor::GetColor( 255, 194, 194 );
    rgb[221] = TColor::GetColor( 255, 194, 194 );
    rgb[222] = TColor::GetColor( 255, 197, 197 );
    rgb[223] = TColor::GetColor( 255, 198, 198 );
    rgb[224] = TColor::GetColor( 255, 200, 200 );
    rgb[225] = TColor::GetColor( 255, 201, 201 );
    rgb[226] = TColor::GetColor( 255, 201, 201 );
    rgb[227] = TColor::GetColor( 255, 202, 202 );
    rgb[228] = TColor::GetColor( 255, 203, 203 );
    rgb[229] = TColor::GetColor( 255, 205, 205 );
    rgb[230] = TColor::GetColor( 255, 206, 206 );
    rgb[231] = TColor::GetColor( 255, 206, 206 );
    rgb[232] = TColor::GetColor( 255, 208, 208 );
    rgb[233] = TColor::GetColor( 255, 209, 209 );
    rgb[234] = TColor::GetColor( 255, 211, 211 );
    rgb[235] = TColor::GetColor( 255, 215, 215 );
    rgb[236] = TColor::GetColor( 255, 216, 216 );
    rgb[237] = TColor::GetColor( 255, 216, 216 );
    rgb[238] = TColor::GetColor( 255, 218, 218 );
    rgb[239] = TColor::GetColor( 255, 219, 219 );
    rgb[240] = TColor::GetColor( 255, 221, 221 );
    rgb[241] = TColor::GetColor( 255, 223, 223 );
    rgb[242] = TColor::GetColor( 255, 226, 226 );
    rgb[243] = TColor::GetColor( 255, 228, 228 );
    rgb[244] = TColor::GetColor( 255, 230, 230 );
    rgb[245] = TColor::GetColor( 255, 230, 230 );
    rgb[246] = TColor::GetColor( 255, 232, 232 );
    rgb[247] = TColor::GetColor( 255, 235, 235 );
    rgb[248] = TColor::GetColor( 255, 237, 237 );
    rgb[249] = TColor::GetColor( 255, 240, 240 );
    rgb[250] = TColor::GetColor( 255, 243, 243 );
    rgb[251] = TColor::GetColor( 255, 246, 246 );
    rgb[252] = TColor::GetColor( 255, 249, 249 );
    rgb[253] = TColor::GetColor( 255, 251, 251 );
    rgb[254] = TColor::GetColor( 255, 253, 253 );
    rgb[255] = TColor::GetColor( 255, 255, 255 );

    fNcolor = 256;
    for (int i=0; i<256; ++i) fColors[i] = rgb[255-i];
  }

  //......................................................................

  void ColorScale::MakeRainbow()
  {
    fNcolor = 256;
    fColors[  0] = TColor::GetColor(  45,   0,  36 );
    fColors[  1] = TColor::GetColor(  45,   0,  36 );
    fColors[  2] = TColor::GetColor(  56,   0,  46 );
    fColors[  3] = TColor::GetColor(  60,   0,  49 );
    fColors[  4] = TColor::GetColor(  67,   0,  54 );
    fColors[  5] = TColor::GetColor(  70,   0,  59 );
    fColors[  6] = TColor::GetColor(  71,   0,  61 );
    fColors[  7] = TColor::GetColor(  75,   0,  68 );
    fColors[  8] = TColor::GetColor(  74,   0,  73 );
    fColors[  9] = TColor::GetColor(  74,   0,  77 );
    fColors[ 10] = TColor::GetColor(  73,   0,  81 );
    fColors[ 11] = TColor::GetColor(  71,   0,  87 );
    fColors[ 12] = TColor::GetColor(  69,   1,  90 );
    fColors[ 13] = TColor::GetColor(  68,   2,  94 );
    fColors[ 14] = TColor::GetColor(  66,   3,  97 );
    fColors[ 15] = TColor::GetColor(  63,   6, 102 );
    fColors[ 16] = TColor::GetColor(  61,   7, 106 );
    fColors[ 17] = TColor::GetColor(  58,  10, 109 );
    fColors[ 18] = TColor::GetColor(  56,  12, 113 );
    fColors[ 19] = TColor::GetColor(  53,  15, 116 );
    fColors[ 20] = TColor::GetColor(  48,  18, 119 );
    fColors[ 21] = TColor::GetColor(  47,  20, 121 );
    fColors[ 22] = TColor::GetColor(  44,  23, 124 );
    fColors[ 23] = TColor::GetColor(  41,  27, 128 );
    fColors[ 24] = TColor::GetColor(  40,  28, 129 );
    fColors[ 25] = TColor::GetColor(  37,  32, 132 );
    fColors[ 26] = TColor::GetColor(  34,  36, 134 );
    fColors[ 27] = TColor::GetColor(  29,  43, 137 );
    fColors[ 28] = TColor::GetColor(  25,  52, 138 );
    fColors[ 29] = TColor::GetColor(  24,  57, 139 );
    fColors[ 30] = TColor::GetColor(  24,  62, 141 );
    fColors[ 31] = TColor::GetColor(  24,  64, 142 );
    fColors[ 32] = TColor::GetColor(  23,  65, 142 );
    fColors[ 33] = TColor::GetColor(  23,  69, 143 );
    fColors[ 34] = TColor::GetColor(  23,  71, 142 );
    fColors[ 35] = TColor::GetColor(  23,  71, 142 );
    fColors[ 36] = TColor::GetColor(  23,  73, 142 );
    fColors[ 37] = TColor::GetColor(  23,  75, 142 );
    fColors[ 38] = TColor::GetColor(  23,  75, 142 );
    fColors[ 39] = TColor::GetColor(  23,  78, 142 );
    fColors[ 40] = TColor::GetColor(  23,  80, 142 );
    fColors[ 41] = TColor::GetColor(  23,  80, 142 );
    fColors[ 42] = TColor::GetColor(  23,  82, 141 );
    fColors[ 43] = TColor::GetColor(  23,  85, 141 );
    fColors[ 44] = TColor::GetColor(  23,  85, 141 );
    fColors[ 45] = TColor::GetColor(  23,  87, 140 );
    fColors[ 46] = TColor::GetColor(  23,  87, 140 );
    fColors[ 47] = TColor::GetColor(  24,  90, 140 );
    fColors[ 48] = TColor::GetColor(  24,  90, 140 );
    fColors[ 49] = TColor::GetColor(  24,  93, 139 );
    fColors[ 50] = TColor::GetColor(  24,  93, 139 );
    fColors[ 51] = TColor::GetColor(  24,  93, 139 );
    fColors[ 52] = TColor::GetColor(  24,  93, 139 );
    fColors[ 53] = TColor::GetColor(  24,  97, 139 );
    fColors[ 54] = TColor::GetColor(  24,  97, 139 );
    fColors[ 55] = TColor::GetColor(  25, 101, 138 );
    fColors[ 56] = TColor::GetColor(  25, 101, 138 );
    fColors[ 57] = TColor::GetColor(  25, 104, 137 );
    fColors[ 58] = TColor::GetColor(  25, 104, 137 );
    fColors[ 59] = TColor::GetColor(  25, 104, 137 );
    fColors[ 60] = TColor::GetColor(  26, 108, 137 );
    fColors[ 61] = TColor::GetColor(  26, 108, 137 );
    fColors[ 62] = TColor::GetColor(  27, 111, 136 );
    fColors[ 63] = TColor::GetColor(  27, 111, 136 );
    fColors[ 64] = TColor::GetColor(  27, 111, 136 );
    fColors[ 65] = TColor::GetColor(  27, 115, 135 );
    fColors[ 66] = TColor::GetColor(  27, 115, 135 );
    fColors[ 67] = TColor::GetColor(  28, 118, 134 );
    fColors[ 68] = TColor::GetColor(  28, 118, 134 );
    fColors[ 69] = TColor::GetColor(  29, 122, 133 );
    fColors[ 70] = TColor::GetColor(  29, 122, 133 );
    fColors[ 71] = TColor::GetColor(  29, 122, 133 );
    fColors[ 72] = TColor::GetColor(  29, 122, 133 );
    fColors[ 73] = TColor::GetColor(  29, 125, 132 );
    fColors[ 74] = TColor::GetColor(  29, 125, 132 );
    fColors[ 75] = TColor::GetColor(  30, 128, 131 );
    fColors[ 76] = TColor::GetColor(  30, 128, 131 );
    fColors[ 77] = TColor::GetColor(  31, 131, 130 );
    fColors[ 78] = TColor::GetColor(  31, 131, 130 );
    fColors[ 79] = TColor::GetColor(  31, 131, 130 );
    fColors[ 80] = TColor::GetColor(  32, 134, 128 );
    fColors[ 81] = TColor::GetColor(  32, 134, 128 );
    fColors[ 82] = TColor::GetColor(  33, 137, 127 );
    fColors[ 83] = TColor::GetColor(  33, 137, 127 );
    fColors[ 84] = TColor::GetColor(  33, 137, 127 );
    fColors[ 85] = TColor::GetColor(  34, 140, 125 );
    fColors[ 86] = TColor::GetColor(  34, 140, 125 );
    fColors[ 87] = TColor::GetColor(  35, 142, 123 );
    fColors[ 88] = TColor::GetColor(  35, 142, 123 );
    fColors[ 89] = TColor::GetColor(  36, 145, 121 );
    fColors[ 90] = TColor::GetColor(  36, 145, 121 );
    fColors[ 91] = TColor::GetColor(  36, 145, 121 );
    fColors[ 92] = TColor::GetColor(  37, 147, 118 );
    fColors[ 93] = TColor::GetColor(  37, 147, 118 );
    fColors[ 94] = TColor::GetColor(  38, 150, 116 );
    fColors[ 95] = TColor::GetColor(  38, 150, 116 );
    fColors[ 96] = TColor::GetColor(  40, 152, 113 );
    fColors[ 97] = TColor::GetColor(  40, 152, 113 );
    fColors[ 98] = TColor::GetColor(  41, 154, 111 );
    fColors[ 99] = TColor::GetColor(  41, 154, 111 );
    fColors[100] = TColor::GetColor(  42, 156, 108 );
    fColors[101] = TColor::GetColor(  42, 156, 108 );
    fColors[102] = TColor::GetColor(  43, 158, 106 );
    fColors[103] = TColor::GetColor(  43, 158, 106 );
    fColors[104] = TColor::GetColor(  43, 158, 106 );
    fColors[105] = TColor::GetColor(  45, 160, 104 );
    fColors[106] = TColor::GetColor(  45, 160, 104 );
    fColors[107] = TColor::GetColor(  46, 162, 101 );
    fColors[108] = TColor::GetColor(  46, 162, 101 );
    fColors[109] = TColor::GetColor(  48, 164,  99 );
    fColors[110] = TColor::GetColor(  48, 164,  99 );
    fColors[111] = TColor::GetColor(  50, 166,  97 );
    fColors[112] = TColor::GetColor(  50, 166,  97 );
    fColors[113] = TColor::GetColor(  51, 168,  95 );
    fColors[114] = TColor::GetColor(  53, 170,  93 );
    fColors[115] = TColor::GetColor(  53, 170,  93 );
    fColors[116] = TColor::GetColor(  53, 170,  93 );
    fColors[117] = TColor::GetColor(  55, 172,  91 );
    fColors[118] = TColor::GetColor(  55, 172,  91 );
    fColors[119] = TColor::GetColor(  57, 174,  88 );
    fColors[120] = TColor::GetColor(  57, 174,  88 );
    fColors[121] = TColor::GetColor(  59, 175,  86 );
    fColors[122] = TColor::GetColor(  62, 177,  84 );
    fColors[123] = TColor::GetColor(  64, 178,  82 );
    fColors[124] = TColor::GetColor(  64, 178,  82 );
    fColors[125] = TColor::GetColor(  67, 180,  80 );
    fColors[126] = TColor::GetColor(  67, 180,  80 );
    fColors[127] = TColor::GetColor(  69, 181,  79 );
    fColors[128] = TColor::GetColor(  72, 183,  77 );
    fColors[129] = TColor::GetColor(  72, 183,  77 );
    fColors[130] = TColor::GetColor(  72, 183,  77 );
    fColors[131] = TColor::GetColor(  75, 184,  76 );
    fColors[132] = TColor::GetColor(  77, 186,  74 );
    fColors[133] = TColor::GetColor(  80, 187,  73 );
    fColors[134] = TColor::GetColor(  83, 189,  72 );
    fColors[135] = TColor::GetColor(  87, 190,  72 );
    fColors[136] = TColor::GetColor(  91, 191,  71 );
    fColors[137] = TColor::GetColor(  95, 192,  70 );
    fColors[138] = TColor::GetColor(  99, 193,  70 );
    fColors[139] = TColor::GetColor( 103, 194,  70 );
    fColors[140] = TColor::GetColor( 107, 195,  70 );
    fColors[141] = TColor::GetColor( 111, 196,  70 );
    fColors[142] = TColor::GetColor( 111, 196,  70 );
    fColors[143] = TColor::GetColor( 115, 196,  70 );
    fColors[144] = TColor::GetColor( 119, 197,  70 );
    fColors[145] = TColor::GetColor( 123, 197,  70 );
    fColors[146] = TColor::GetColor( 130, 198,  71 );
    fColors[147] = TColor::GetColor( 133, 199,  71 );
    fColors[148] = TColor::GetColor( 137, 199,  72 );
    fColors[149] = TColor::GetColor( 140, 199,  72 );
    fColors[150] = TColor::GetColor( 143, 199,  73 );
    fColors[151] = TColor::GetColor( 143, 199,  73 );
    fColors[152] = TColor::GetColor( 147, 199,  73 );
    fColors[153] = TColor::GetColor( 150, 199,  74 );
    fColors[154] = TColor::GetColor( 153, 199,  74 );
    fColors[155] = TColor::GetColor( 156, 199,  75 );
    fColors[156] = TColor::GetColor( 160, 200,  76 );
    fColors[157] = TColor::GetColor( 167, 200,  78 );
    fColors[158] = TColor::GetColor( 170, 200,  79 );
    fColors[159] = TColor::GetColor( 173, 200,  79 );
    fColors[160] = TColor::GetColor( 173, 200,  79 );
    fColors[161] = TColor::GetColor( 177, 200,  80 );
    fColors[162] = TColor::GetColor( 180, 200,  81 );
    fColors[163] = TColor::GetColor( 183, 199,  82 );
    fColors[164] = TColor::GetColor( 186, 199,  82 );
    fColors[165] = TColor::GetColor( 190, 199,  83 );
    fColors[166] = TColor::GetColor( 196, 199,  85 );
    fColors[167] = TColor::GetColor( 199, 198,  85 );
    fColors[168] = TColor::GetColor( 199, 198,  85 );
    fColors[169] = TColor::GetColor( 203, 198,  86 );
    fColors[170] = TColor::GetColor( 206, 197,  87 );
    fColors[171] = TColor::GetColor( 212, 197,  89 );
    fColors[172] = TColor::GetColor( 215, 196,  90 );
    fColors[173] = TColor::GetColor( 218, 195,  91 );
    fColors[174] = TColor::GetColor( 224, 194,  94 );
    fColors[175] = TColor::GetColor( 224, 194,  94 );
    fColors[176] = TColor::GetColor( 230, 193,  96 );
    fColors[177] = TColor::GetColor( 233, 192,  98 );
    fColors[178] = TColor::GetColor( 236, 190, 100 );
    fColors[179] = TColor::GetColor( 238, 189, 104 );
    fColors[180] = TColor::GetColor( 240, 188, 106 );
    fColors[181] = TColor::GetColor( 240, 188, 106 );
    fColors[182] = TColor::GetColor( 242, 187, 110 );
    fColors[183] = TColor::GetColor( 244, 185, 114 );
    fColors[184] = TColor::GetColor( 245, 184, 116 );
    fColors[185] = TColor::GetColor( 247, 183, 120 );
    fColors[186] = TColor::GetColor( 248, 182, 123 );
    fColors[187] = TColor::GetColor( 248, 182, 123 );
    fColors[188] = TColor::GetColor( 250, 181, 125 );
    fColors[189] = TColor::GetColor( 251, 180, 128 );
    fColors[190] = TColor::GetColor( 252, 180, 130 );
    fColors[191] = TColor::GetColor( 253, 180, 133 );
    fColors[192] = TColor::GetColor( 253, 180, 133 );
    fColors[193] = TColor::GetColor( 254, 180, 134 );
    fColors[194] = TColor::GetColor( 254, 179, 138 );
    fColors[195] = TColor::GetColor( 255, 179, 142 );
    fColors[196] = TColor::GetColor( 255, 179, 145 );
    fColors[197] = TColor::GetColor( 255, 179, 145 );
    fColors[198] = TColor::GetColor( 255, 179, 152 );
    fColors[199] = TColor::GetColor( 255, 180, 161 );
    fColors[200] = TColor::GetColor( 255, 180, 164 );
    fColors[201] = TColor::GetColor( 255, 180, 167 );
    fColors[202] = TColor::GetColor( 255, 180, 167 );
    fColors[203] = TColor::GetColor( 255, 181, 169 );
    fColors[204] = TColor::GetColor( 255, 181, 170 );
    fColors[205] = TColor::GetColor( 255, 182, 173 );
    fColors[206] = TColor::GetColor( 255, 183, 176 );
    fColors[207] = TColor::GetColor( 255, 183, 176 );
    fColors[208] = TColor::GetColor( 255, 184, 179 );
    fColors[209] = TColor::GetColor( 255, 185, 179 );
    fColors[210] = TColor::GetColor( 255, 185, 182 );
    fColors[211] = TColor::GetColor( 255, 186, 182 );
    fColors[212] = TColor::GetColor( 255, 186, 182 );
    fColors[213] = TColor::GetColor( 255, 187, 185 );
    fColors[214] = TColor::GetColor( 255, 188, 185 );
    fColors[215] = TColor::GetColor( 255, 189, 188 );
    fColors[216] = TColor::GetColor( 255, 189, 188 );
    fColors[217] = TColor::GetColor( 255, 190, 188 );
    fColors[218] = TColor::GetColor( 255, 191, 191 );
    fColors[219] = TColor::GetColor( 255, 192, 191 );
    fColors[220] = TColor::GetColor( 255, 194, 194 );
    fColors[221] = TColor::GetColor( 255, 194, 194 );
    fColors[222] = TColor::GetColor( 255, 197, 197 );
    fColors[223] = TColor::GetColor( 255, 198, 198 );
    fColors[224] = TColor::GetColor( 255, 200, 200 );
    fColors[225] = TColor::GetColor( 255, 201, 201 );
    fColors[226] = TColor::GetColor( 255, 201, 201 );
    fColors[227] = TColor::GetColor( 255, 202, 202 );
    fColors[228] = TColor::GetColor( 255, 203, 203 );
    fColors[229] = TColor::GetColor( 255, 205, 205 );
    fColors[230] = TColor::GetColor( 255, 206, 206 );
    fColors[231] = TColor::GetColor( 255, 206, 206 );
    fColors[232] = TColor::GetColor( 255, 208, 208 );
    fColors[233] = TColor::GetColor( 255, 209, 209 );
    fColors[234] = TColor::GetColor( 255, 211, 211 );
    fColors[235] = TColor::GetColor( 255, 215, 215 );
    fColors[236] = TColor::GetColor( 255, 216, 216 );
    fColors[237] = TColor::GetColor( 255, 216, 216 );
    fColors[238] = TColor::GetColor( 255, 218, 218 );
    fColors[239] = TColor::GetColor( 255, 219, 219 );
    fColors[240] = TColor::GetColor( 255, 221, 221 );
    fColors[241] = TColor::GetColor( 255, 223, 223 );
    fColors[242] = TColor::GetColor( 255, 226, 226 );
    fColors[243] = TColor::GetColor( 255, 228, 228 );
    fColors[244] = TColor::GetColor( 255, 230, 230 );
    fColors[245] = TColor::GetColor( 255, 230, 230 );
    fColors[246] = TColor::GetColor( 255, 232, 232 );
    fColors[247] = TColor::GetColor( 255, 235, 235 );
    fColors[248] = TColor::GetColor( 255, 237, 237 );
    fColors[249] = TColor::GetColor( 255, 240, 240 );
    fColors[250] = TColor::GetColor( 255, 243, 243 );
    fColors[251] = TColor::GetColor( 255, 246, 246 );
    fColors[252] = TColor::GetColor( 255, 249, 249 );
    fColors[253] = TColor::GetColor( 255, 251, 251 );
    fColors[254] = TColor::GetColor( 255, 253, 253 );
    fColors[255] = TColor::GetColor( 255, 255, 255 );
  }

  //......................................................................

  void ColorScale::MakeGreenToMagenta() 
  {
    static int rgb[18][3] = {
      {0,      80,       0},
      {0,     134,       0},
      {0,     187,       0},
      {0,     241,       0},
      {80,    255,      80},
      {134,   255,     134},
      {187,   255,     187},
      {255,   255,     255},
      {255,   241,     255},
      {255,   187,     255},
      {255,   134,     255},
      {255,    80,     255},
      {241,     0,     241},
      {187,     0,     187},
      {134,     0,     134},
      {80,      0,      80}
    };

    fNcolor = 18;
    for (int i=0; i<18; ++i) {
      fColors[i] = TColor::GetColor(rgb[i][0],rgb[i][1],rgb[i][2]);
    }
  }

  //......................................................................

  void ColorScale::MakeBlueToRed() 
  {
    static int rgb[18][3] = {
      { 36,       0,     216},
      { 24,      28,     247},
      { 40,      87,     255},
      { 61,     135,     255},
      { 86,     176,     255},
      {117,     211,     255},
      {153,     234,     255},
      {188,     249,     255},
      {234,     255,     255},
      {255,     255,     234},
      {255,     241,     188},
      {255,     214,     153},
      {255,     172,     117},
      {255,     120,      86},
      {255,      61,      61},
      {247,      39,      53},
      {216,      21,      47},
      {165,       0,      33}
    };

    fNcolor = 18;
    for (int i=0; i<18; ++i) {
      fColors[i] = TColor::GetColor(rgb[i][0],rgb[i][1],rgb[i][2]);
    }
  }

  //......................................................................

  void ColorScale::MakeBlueToRedII() 
  {
    const int NRGBs = 5;
    double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    double red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    double green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    double blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, fNcolor);
    gStyle->SetNumberContours(fNcolor);
  
    for (int i=0; i<fNcolor; ++i) 
      fColors[i] = gStyle->GetColorPalette(i);

  }

  //......................................................................

  void ColorScale::MakeBlueToGreen() 
  {
    static int rgb[14][3] = {
      {0,       0,     255},
      {51,      51,     255},
      {101,     101,     255},
      {153,     153,     255},
      {178,     178,     255},
      {203,     203,     255},
      {229,     229,     255},
      {229,     255,     229},
      {203,     255,     203},
      {178,     255,     178},
      {153,     255,     153},
      {101,     255,     101},
      {51,      255,      51},
      {0,       255,       0}
    };

    fNcolor = 14;
    for (int i=0; i<14; ++i) {
      fColors[i] = TColor::GetColor(rgb[i][0],rgb[i][1],rgb[i][2]);
    }
  }

  //......................................................................

  void ColorScale::MakeBlueToOrange() 
  {
    static int rgb[18][3] = {
      {0,     102,     102},
      {0,     153,     153},
      {0,     204,     204},
      {0,     255,     255},
      {51,    255,     255},
      {101,   255,     255},
      {153,   255,     255},
      {178,   255,     255},
      {203,   255,     255},
      {229,   255,     255},
      {255,   229,     203},
      {255,   202,     153},
      {255,   173,     101},
      {255,   142,      51},
      {255,   110,       0},
      {204,    85,       0},
      {153,    61,       0},
      {102,    39,       0}
    };

    fNcolor = 18;
    for (int i=0; i<18; ++i) {
      fColors[i] = TColor::GetColor(rgb[i][0],rgb[i][1],rgb[i][2]);
    }
  }

  //......................................................................

  void ColorScale::MakeBrownToBlue()
  {
    int rgb[12][3] = {
      {51,    25,      0},
      {102,   47,      0},
      {153,   96,      53},
      {204,   155,     122},
      {216,   175,     151},
      {242,   218,     205},
      {204,   253,     255},
      {153,   248,     255},
      {101,   239,     255},
      {50,    227,     255},
      {0,     169,     204},
      {0,     122,     153},
    };
  
    fNcolor = 12;
    for (int i=0; i<12; ++i) {
      fColors[i] = TColor::GetColor(rgb[11-i][0],rgb[11-i][1],rgb[11-i][2]);
    }
  }

  //......................................................................

  void ColorScale::MakeLinGray()
  {
    const int NRGBs = 3;
    double stops[NRGBs] = { 0.00, .50, 1.00 };
    double red[NRGBs]   = { 1.00, .75, 0.00 };
    double green[NRGBs] = { 1.00, .75, 0.00 };
    double blue[NRGBs]  = { 1.00, .75, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, fNcolor);
    gStyle->SetNumberContours(fNcolor);
  
    for (int i=0; i<fNcolor; ++i) 
      fColors[i] = gStyle->GetColorPalette(i);

  }

  //......................................................................

  void ColorScale::MakeHeatedObject()
  {
    int fRGB[256];
    fRGB[  0] = TColor::GetColor(   0,   0,   0 );
    fRGB[  1] = TColor::GetColor(  35,   0,   0 );
    fRGB[  2] = TColor::GetColor(  52,   0,   0 );
    fRGB[  3] = TColor::GetColor(  60,   0,   0 );
    fRGB[  4] = TColor::GetColor(  63,   1,   0 );
    fRGB[  5] = TColor::GetColor(  64,   2,   0 );
    fRGB[  6] = TColor::GetColor(  68,   5,   0 );
    fRGB[  7] = TColor::GetColor(  69,   6,   0 );
    fRGB[  8] = TColor::GetColor(  72,   8,   0 );
    fRGB[  9] = TColor::GetColor(  74,  10,   0 );
    fRGB[ 10] = TColor::GetColor(  77,  12,   0 );
    fRGB[ 11] = TColor::GetColor(  78,  14,   0 );
    fRGB[ 12] = TColor::GetColor(  81,  16,   0 );
    fRGB[ 13] = TColor::GetColor(  83,  17,   0 );
    fRGB[ 14] = TColor::GetColor(  85,  19,   0 );
    fRGB[ 15] = TColor::GetColor(  86,  20,   0 );
    fRGB[ 16] = TColor::GetColor(  89,  22,   0 );
    fRGB[ 17] = TColor::GetColor(  91,  24,   0 );
    fRGB[ 18] = TColor::GetColor(  92,  25,   0 );
    fRGB[ 19] = TColor::GetColor(  94,  26,   0 );
    fRGB[ 20] = TColor::GetColor(  95,  28,   0 );
    fRGB[ 21] = TColor::GetColor(  98,  30,   0 );
    fRGB[ 22] = TColor::GetColor( 100,  31,   0 );
    fRGB[ 23] = TColor::GetColor( 102,  33,   0 );
    fRGB[ 24] = TColor::GetColor( 103,  34,   0 );
    fRGB[ 25] = TColor::GetColor( 105,  35,   0 );
    fRGB[ 26] = TColor::GetColor( 106,  36,   0 );
    fRGB[ 27] = TColor::GetColor( 108,  38,   0 );
    fRGB[ 28] = TColor::GetColor( 109,  39,   0 );
    fRGB[ 29] = TColor::GetColor( 111,  40,   0 );
    fRGB[ 30] = TColor::GetColor( 112,  42,   0 );
    fRGB[ 31] = TColor::GetColor( 114,  43,   0 );
    fRGB[ 32] = TColor::GetColor( 115,  44,   0 );
    fRGB[ 33] = TColor::GetColor( 117,  45,   0 );
    fRGB[ 34] = TColor::GetColor( 119,  47,   0 );
    fRGB[ 35] = TColor::GetColor( 119,  47,   0 );
    fRGB[ 36] = TColor::GetColor( 120,  48,   0 );
    fRGB[ 37] = TColor::GetColor( 122,  49,   0 );
    fRGB[ 38] = TColor::GetColor( 123,  51,   0 );
    fRGB[ 39] = TColor::GetColor( 125,  52,   0 );
    fRGB[ 40] = TColor::GetColor( 125,  52,   0 );
    fRGB[ 41] = TColor::GetColor( 126,  53,   0 );
    fRGB[ 42] = TColor::GetColor( 128,  54,   0 );
    fRGB[ 43] = TColor::GetColor( 129,  56,   0 );
    fRGB[ 44] = TColor::GetColor( 129,  56,   0 );
    fRGB[ 45] = TColor::GetColor( 131,  57,   0 );
    fRGB[ 46] = TColor::GetColor( 132,  58,   0 );
    fRGB[ 47] = TColor::GetColor( 134,  59,   0 );
    fRGB[ 48] = TColor::GetColor( 134,  59,   0 );
    fRGB[ 49] = TColor::GetColor( 136,  61,   0 );
    fRGB[ 50] = TColor::GetColor( 137,  62,   0 );
    fRGB[ 51] = TColor::GetColor( 137,  62,   0 );
    fRGB[ 52] = TColor::GetColor( 139,  63,   0 );
    fRGB[ 53] = TColor::GetColor( 139,  63,   0 );
    fRGB[ 54] = TColor::GetColor( 140,  65,   0 );
    fRGB[ 55] = TColor::GetColor( 142,  66,   0 );
    fRGB[ 56] = TColor::GetColor( 142,  66,   0 );
    fRGB[ 57] = TColor::GetColor( 143,  67,   0 );
    fRGB[ 58] = TColor::GetColor( 143,  67,   0 );
    fRGB[ 59] = TColor::GetColor( 145,  68,   0 );
    fRGB[ 60] = TColor::GetColor( 145,  68,   0 );
    fRGB[ 61] = TColor::GetColor( 146,  70,   0 );
    fRGB[ 62] = TColor::GetColor( 146,  70,   0 );
    fRGB[ 63] = TColor::GetColor( 148,  71,   0 );
    fRGB[ 64] = TColor::GetColor( 148,  71,   0 );
    fRGB[ 65] = TColor::GetColor( 149,  72,   0 );
    fRGB[ 66] = TColor::GetColor( 149,  72,   0 );
    fRGB[ 67] = TColor::GetColor( 151,  73,   0 );
    fRGB[ 68] = TColor::GetColor( 151,  73,   0 );
    fRGB[ 69] = TColor::GetColor( 153,  75,   0 );
    fRGB[ 70] = TColor::GetColor( 153,  75,   0 );
    fRGB[ 71] = TColor::GetColor( 154,  76,   0 );
    fRGB[ 72] = TColor::GetColor( 154,  76,   0 );
    fRGB[ 73] = TColor::GetColor( 154,  76,   0 );
    fRGB[ 74] = TColor::GetColor( 156,  77,   0 );
    fRGB[ 75] = TColor::GetColor( 156,  77,   0 );
    fRGB[ 76] = TColor::GetColor( 157,  79,   0 );
    fRGB[ 77] = TColor::GetColor( 157,  79,   0 );
    fRGB[ 78] = TColor::GetColor( 159,  80,   0 );
    fRGB[ 79] = TColor::GetColor( 159,  80,   0 );
    fRGB[ 80] = TColor::GetColor( 159,  80,   0 );
    fRGB[ 81] = TColor::GetColor( 160,  81,   0 );
    fRGB[ 82] = TColor::GetColor( 160,  81,   0 );
    fRGB[ 83] = TColor::GetColor( 162,  82,   0 );
    fRGB[ 84] = TColor::GetColor( 162,  82,   0 );
    fRGB[ 85] = TColor::GetColor( 163,  84,   0 );
    fRGB[ 86] = TColor::GetColor( 163,  84,   0 );
    fRGB[ 87] = TColor::GetColor( 165,  85,   0 );
    fRGB[ 88] = TColor::GetColor( 165,  85,   0 );
    fRGB[ 89] = TColor::GetColor( 166,  86,   0 );
    fRGB[ 90] = TColor::GetColor( 166,  86,   0 );
    fRGB[ 91] = TColor::GetColor( 166,  86,   0 );
    fRGB[ 92] = TColor::GetColor( 168,  87,   0 );
    fRGB[ 93] = TColor::GetColor( 168,  87,   0 );
    fRGB[ 94] = TColor::GetColor( 170,  89,   0 );
    fRGB[ 95] = TColor::GetColor( 170,  89,   0 );
    fRGB[ 96] = TColor::GetColor( 171,  90,   0 );
    fRGB[ 97] = TColor::GetColor( 171,  90,   0 );
    fRGB[ 98] = TColor::GetColor( 173,  91,   0 );
    fRGB[ 99] = TColor::GetColor( 173,  91,   0 );
    fRGB[100] = TColor::GetColor( 174,  93,   0 );
    fRGB[101] = TColor::GetColor( 174,  93,   0 );
    fRGB[102] = TColor::GetColor( 176,  94,   0 );
    fRGB[103] = TColor::GetColor( 176,  94,   0 );
    fRGB[104] = TColor::GetColor( 177,  95,   0 );
    fRGB[105] = TColor::GetColor( 177,  95,   0 );
    fRGB[106] = TColor::GetColor( 179,  96,   0 );
    fRGB[107] = TColor::GetColor( 179,  96,   0 );
    fRGB[108] = TColor::GetColor( 180,  98,   0 );
    fRGB[109] = TColor::GetColor( 182,  99,   0 );
    fRGB[110] = TColor::GetColor( 182,  99,   0 );
    fRGB[111] = TColor::GetColor( 183, 100,   0 );
    fRGB[112] = TColor::GetColor( 183, 100,   0 );
    fRGB[113] = TColor::GetColor( 185, 102,   0 );
    fRGB[114] = TColor::GetColor( 185, 102,   0 );
    fRGB[115] = TColor::GetColor( 187, 103,   0 );
    fRGB[116] = TColor::GetColor( 187, 103,   0 );
    fRGB[117] = TColor::GetColor( 188, 104,   0 );
    fRGB[118] = TColor::GetColor( 188, 104,   0 );
    fRGB[119] = TColor::GetColor( 190, 105,   0 );
    fRGB[120] = TColor::GetColor( 191, 107,   0 );
    fRGB[121] = TColor::GetColor( 191, 107,   0 );
    fRGB[122] = TColor::GetColor( 193, 108,   0 );
    fRGB[123] = TColor::GetColor( 193, 108,   0 );
    fRGB[124] = TColor::GetColor( 194, 109,   0 );
    fRGB[125] = TColor::GetColor( 196, 110,   0 );
    fRGB[126] = TColor::GetColor( 196, 110,   0 );
    fRGB[127] = TColor::GetColor( 197, 112,   0 );
    fRGB[128] = TColor::GetColor( 197, 112,   0 );
    fRGB[129] = TColor::GetColor( 199, 113,   0 );
    fRGB[130] = TColor::GetColor( 200, 114,   0 );
    fRGB[131] = TColor::GetColor( 200, 114,   0 );
    fRGB[132] = TColor::GetColor( 202, 116,   0 );
    fRGB[133] = TColor::GetColor( 202, 116,   0 );
    fRGB[134] = TColor::GetColor( 204, 117,   0 );
    fRGB[135] = TColor::GetColor( 205, 118,   0 );
    fRGB[136] = TColor::GetColor( 205, 118,   0 );
    fRGB[137] = TColor::GetColor( 207, 119,   0 );
    fRGB[138] = TColor::GetColor( 208, 121,   0 );
    fRGB[139] = TColor::GetColor( 208, 121,   0 );
    fRGB[140] = TColor::GetColor( 210, 122,   0 );
    fRGB[141] = TColor::GetColor( 211, 123,   0 );
    fRGB[142] = TColor::GetColor( 211, 123,   0 );
    fRGB[143] = TColor::GetColor( 213, 124,   0 );
    fRGB[144] = TColor::GetColor( 214, 126,   0 );
    fRGB[145] = TColor::GetColor( 214, 126,   0 );
    fRGB[146] = TColor::GetColor( 216, 127,   0 );
    fRGB[147] = TColor::GetColor( 217, 128,   0 );
    fRGB[148] = TColor::GetColor( 217, 128,   0 );
    fRGB[149] = TColor::GetColor( 219, 130,   0 );
    fRGB[150] = TColor::GetColor( 221, 131,   0 );
    fRGB[151] = TColor::GetColor( 221, 131,   0 );
    fRGB[152] = TColor::GetColor( 222, 132,   0 );
    fRGB[153] = TColor::GetColor( 224, 133,   0 );
    fRGB[154] = TColor::GetColor( 224, 133,   0 );
    fRGB[155] = TColor::GetColor( 225, 135,   0 );
    fRGB[156] = TColor::GetColor( 227, 136,   0 );
    fRGB[157] = TColor::GetColor( 227, 136,   0 );
    fRGB[158] = TColor::GetColor( 228, 137,   0 );
    fRGB[159] = TColor::GetColor( 230, 138,   0 );
    fRGB[160] = TColor::GetColor( 230, 138,   0 );
    fRGB[161] = TColor::GetColor( 231, 140,   0 );
    fRGB[162] = TColor::GetColor( 233, 141,   0 );
    fRGB[163] = TColor::GetColor( 233, 141,   0 );
    fRGB[164] = TColor::GetColor( 234, 142,   0 );
    fRGB[165] = TColor::GetColor( 236, 144,   0 );
    fRGB[166] = TColor::GetColor( 236, 144,   0 );
    fRGB[167] = TColor::GetColor( 238, 145,   0 );
    fRGB[168] = TColor::GetColor( 239, 146,   0 );
    fRGB[169] = TColor::GetColor( 241, 147,   0 );
    fRGB[170] = TColor::GetColor( 241, 147,   0 );
    fRGB[171] = TColor::GetColor( 242, 149,   0 );
    fRGB[172] = TColor::GetColor( 244, 150,   0 );
    fRGB[173] = TColor::GetColor( 244, 150,   0 );
    fRGB[174] = TColor::GetColor( 245, 151,   0 );
    fRGB[175] = TColor::GetColor( 247, 153,   0 );
    fRGB[176] = TColor::GetColor( 247, 153,   0 );
    fRGB[177] = TColor::GetColor( 248, 154,   0 );
    fRGB[178] = TColor::GetColor( 250, 155,   0 );
    fRGB[179] = TColor::GetColor( 251, 156,   0 );
    fRGB[180] = TColor::GetColor( 251, 156,   0 );
    fRGB[181] = TColor::GetColor( 253, 158,   0 );
    fRGB[182] = TColor::GetColor( 255, 159,   0 );
    fRGB[183] = TColor::GetColor( 255, 159,   0 );
    fRGB[184] = TColor::GetColor( 255, 160,   0 );
    fRGB[185] = TColor::GetColor( 255, 161,   0 );
    fRGB[186] = TColor::GetColor( 255, 163,   0 );
    fRGB[187] = TColor::GetColor( 255, 163,   0 );
    fRGB[188] = TColor::GetColor( 255, 164,   0 );
    fRGB[189] = TColor::GetColor( 255, 165,   0 );
    fRGB[190] = TColor::GetColor( 255, 167,   0 );
    fRGB[191] = TColor::GetColor( 255, 167,   0 );
    fRGB[192] = TColor::GetColor( 255, 168,   0 );
    fRGB[193] = TColor::GetColor( 255, 169,   0 );
    fRGB[194] = TColor::GetColor( 255, 169,   0 );
    fRGB[195] = TColor::GetColor( 255, 170,   0 );
    fRGB[196] = TColor::GetColor( 255, 172,   0 );
    fRGB[197] = TColor::GetColor( 255, 173,   0 );
    fRGB[198] = TColor::GetColor( 255, 173,   0 );
    fRGB[199] = TColor::GetColor( 255, 174,   0 );
    fRGB[200] = TColor::GetColor( 255, 175,   0 );
    fRGB[201] = TColor::GetColor( 255, 177,   0 );
    fRGB[202] = TColor::GetColor( 255, 178,   0 );
    fRGB[203] = TColor::GetColor( 255, 179,   0 );
    fRGB[204] = TColor::GetColor( 255, 181,   0 );
    fRGB[205] = TColor::GetColor( 255, 181,   0 );
    fRGB[206] = TColor::GetColor( 255, 182,   0 );
    fRGB[207] = TColor::GetColor( 255, 183,   0 );
    fRGB[208] = TColor::GetColor( 255, 184,   0 );
    fRGB[209] = TColor::GetColor( 255, 187,   7 );
    fRGB[210] = TColor::GetColor( 255, 188,  10 );
    fRGB[211] = TColor::GetColor( 255, 189,  14 );
    fRGB[212] = TColor::GetColor( 255, 191,  18 );
    fRGB[213] = TColor::GetColor( 255, 192,  21 );
    fRGB[214] = TColor::GetColor( 255, 193,  25 );
    fRGB[215] = TColor::GetColor( 255, 195,  29 );
    fRGB[216] = TColor::GetColor( 255, 197,  36 );
    fRGB[217] = TColor::GetColor( 255, 198,  40 );
    fRGB[218] = TColor::GetColor( 255, 200,  43 );
    fRGB[219] = TColor::GetColor( 255, 202,  51 );
    fRGB[220] = TColor::GetColor( 255, 204,  54 );
    fRGB[221] = TColor::GetColor( 255, 206,  61 );
    fRGB[222] = TColor::GetColor( 255, 207,  65 );
    fRGB[223] = TColor::GetColor( 255, 210,  72 );
    fRGB[224] = TColor::GetColor( 255, 211,  76 );
    fRGB[225] = TColor::GetColor( 255, 214,  83 );
    fRGB[226] = TColor::GetColor( 255, 216,  91 );
    fRGB[227] = TColor::GetColor( 255, 219,  98 );
    fRGB[228] = TColor::GetColor( 255, 221, 105 );
    fRGB[229] = TColor::GetColor( 255, 223, 109 );
    fRGB[230] = TColor::GetColor( 255, 225, 116 );
    fRGB[231] = TColor::GetColor( 255, 228, 123 );
    fRGB[232] = TColor::GetColor( 255, 232, 134 );
    fRGB[233] = TColor::GetColor( 255, 234, 142 );
    fRGB[234] = TColor::GetColor( 255, 237, 149 );
    fRGB[235] = TColor::GetColor( 255, 239, 156 );
    fRGB[236] = TColor::GetColor( 255, 240, 160 );
    fRGB[237] = TColor::GetColor( 255, 243, 167 );
    fRGB[238] = TColor::GetColor( 255, 246, 174 );
    fRGB[239] = TColor::GetColor( 255, 248, 182 );
    fRGB[240] = TColor::GetColor( 255, 249, 185 );
    fRGB[241] = TColor::GetColor( 255, 252, 193 );
    fRGB[242] = TColor::GetColor( 255, 253, 196 );
    fRGB[243] = TColor::GetColor( 255, 255, 204 );
    fRGB[244] = TColor::GetColor( 255, 255, 207 );
    fRGB[245] = TColor::GetColor( 255, 255, 211 );
    fRGB[246] = TColor::GetColor( 255, 255, 218 );
    fRGB[247] = TColor::GetColor( 255, 255, 222 );
    fRGB[248] = TColor::GetColor( 255, 255, 225 );
    fRGB[249] = TColor::GetColor( 255, 255, 229 );
    fRGB[250] = TColor::GetColor( 255, 255, 233 );
    fRGB[251] = TColor::GetColor( 255, 255, 236 );
    fRGB[252] = TColor::GetColor( 255, 255, 240 );
    fRGB[253] = TColor::GetColor( 255, 255, 244 );
    fRGB[254] = TColor::GetColor( 255, 255, 247 );
    fRGB[255] = TColor::GetColor( 255, 255, 255 );

    fNcolor = 256;
    for (int i=0; i<256; ++i) fColors[i] = fRGB[255-i];
  }
}// namespace


////////////////////////////////////////////////////////////////////////
