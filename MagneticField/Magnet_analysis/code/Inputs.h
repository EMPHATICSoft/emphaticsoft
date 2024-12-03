#include <string>
using namespace std;
#include "Funcs.h"




enum whichMagnetType {CMSM,SABR};
whichMagnetType whichMagnet = CMSM;




string dirName = "/nashome/b/bellanto/MapMaking/Magnet_analysis/code/";
int const Nfiles = 3;
string SABRfiles[Nfiles] = {"Data_in/CylinderGrid_Section2_231026-085444.txt",      // central
                            "Data_in/CylinderGrid_Section1_231025-112209.txt",      // upstream
                            "Data_in/CylinderGrid_Section3_231026-121846.txt",      // downstream
};
string CMSMfiles[Nfiles] = {"Data_in/EMPHATIC_body_scan_5280148.csv",
                            "Data_in/EMPHATIC_smallApertureFringe_cleaned.csv",     // One odd point fixed for this file
                            "Data_in/EMPHATIC_largeApertureFringe_cleaned.csv",     // Four odd points fixed for this file
};





bool const   showXYscan         = false;

bool const   showDivHisto       = false;
bool const   logPlots           = false;
int const    nBinsHistoCenter   =   120;
double const loHistoEdgeCenter  =  -4.0;
double const hiHistoEdgeCenter  =  +4.0;
int const    nBinsHistoFringe   =   120;
double const loHistoEdgeFringe  =  -0.2;
double const hiHistoEdgeFringe  =  +0.2;

// These plots appear as a new plot for each z in the map
// You probably only want to look at one map at a time.
WhichMap const showThis         = central;
bool const     showBxyzVsZ      = false;
bool const     showSliceMag     = false;
bool const     showSliceDiv     = false;
bool const     showSliceVec     = false;
bool const     showSliceBz      = false;
bool const     showIntegrals    = false;

// Just checks that the quadratic interpolation code isn't broken.
bool checkUp                    = false;
bool checkCn                    = false;
bool checkDn                    = false;

// Options for finding the offsets needed to stitch
bool const   fitUpstream        = false;
bool const   fitDownstream      = false;
bool const   plotDiffUp         = false;
bool const   plotDiffDown       = false;
double const CMSMprobeErr       = 0.000190;     // Tesla
double const CMSMmechanicalErr  = 0.000051;     // meters
double const SABRprobeErr       = 0.000330;
double const SABRmechanicalErr  = 0.000051;
int  const   fitVerbosity       = 2;
bool const   trySIMPLEX         = true;         // Gradient minimization needs reliable
                                                // uncertainties in chi-sq & doesn't work yet

// Options for stitching together a single map.  Distances will be rounded to nearest half mm
bool const  makeSingleMap       = false;
bool const  copySpacing         = false;        // If true, grid taken from central map's pattern - rarely used.
// Use these settings to make a map for the reconstruction code
double const lowestZout         = -0.140;
double const highestZout        = +0.310;
double const halfSpanX          = 0.017;        // Distance axis to furthest point in map
double const halfSpanY          = 0.017;
double const stepX = 0.002;   double const stepY = 0.002;   double const stepZ = 0.002;
// For Helmholtz decomposition, you want the full span in Z on 5mm steps and not go to 17mm.
// double const lowestZout         = -0.310;
// double const highestZout        = +0.480;
// double const halfSpanX          = 0.015;        // Distance axis to furthest point in map
// double const halfSpanY          = 0.015;
// double const stepX = 0.005;   double const stepY = 0.005;   double const stepZ = 0.005;



bool const  plotIntegral        = false;
bool const  plotStitchedBvsZ    = false;
bool const  checkFull           = false;

string CMSMoutMapOnDesktop = "mfCMSMMapStitched.txt";
string SABRoutMapOnDesktop = "mfSABRMapStitched.txt";
string headerFileName      = dirName + "CodeGuts/OutputHeader.txt";






// Convention is position in the field map (csv file, as found with a
// FieldMapItr) minus an offset, saved as the ra<double>* offset, is
// a position in physical space, with origin at the center of the hole
// in the upstream side of the magnet.  The offset for the central maps
// is from Mark Thompson's metrology on the stand.
double SABRcentralOff[3]  = {0.0, 0.0, 0.0};
double SABRupInitOff[3]   = {0.0, 0.0, 0.0};
double SABRdownInitOff[3] = {0.0, 0.0, 0.0};
// Here are the fit results, should you want to restart with them:
//double SABRupInitOff[3]   = { 1.1669e-05, -3.55751e-05, -0.000125817}; // 70.6 for 837 DoF (version in note)
//double SABRdownInitOff[3] = {-1.0076e-05, -2.25818e-05,  1.07872e-05}; // 111.3 for 837 DoF (version in note)



double CMSMcentralOff[3]  = {0.0, 0.0, 2.362*(25.4/1000)};
// Mike Tartaglia's eyeball fit had upstream offset of 12.362 inches == 310.399 mm
// (in Z only).  From fits done so far:
//double CMSMupInitOff[3]   = {-0.000115, -0.000422, 0.312704}; // 7.06 (minimized rel err), 3x105 -3 = DoF
//double CMSMupInitOff[3]   = {-0.000159, -0.000417, 0.312519}; // 155.2 (minimized abs err), 312 DoF
  double CMSMupInitOff[3]   = {-0.000144, -0.000411, 0.312486}; // 77.5 for 447 DoF (version in note)

// Mike Tartaglia's eyeball fit had upstream offset of -7.8 inches == 198.120 mm
// (in Z only).  From fits done so far:
//double CMSMdownInitOff[3] = {0.000347,  0.000071, -0.199957};  // 8.21 (minimized rel err), 3x84 -3 = 249 DoF
//double CMSMdownInitOff[3] = {0.000371,  0.000073, -0.200046};  // 155.7 (minimized abs err), 249 DoF
  double CMSMdownInitOff[3] = {0.000375,  0.000081, -0.200079};  // 86.4 for 297 DoF (version in note)
