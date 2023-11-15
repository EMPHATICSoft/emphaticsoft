/////////////////////////////////////////////////////////////////////////////                                             
// Description: This macro holds a function that takes in a single DE      //                                             
// value and one root file holding a histogram with  DE vs ADC values and  //                                             
// returns a vector that holds (ADC, width, RMS).                          //                                             
// Using phase 1c geometry.                                                //                                             
//                                                                         //                                             
//Date: November 08, 2023                                                  //                                             
//Author: D.A.H.                                                           //                                             
/////////////////////////////////////////////////////////////////////////////                                             

#include "StandardRecord/StandardRecord.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <vector>
#include <TChain.h>
#include <TMath.h>
#include "TGraph.h"
#include <cstdlib>
#include <tuple>

/////////////////////////////////////////////////////////////////////////////                                             
// Description: This macro holds a function that takes in a single DE      //                                             
// value and one root file holding a histogram with  DE vs ADC values and  //                                             
// returns a vector that holds (ADC, width, RMS).                          //                                             
// Using phase 1c geometry.                                                //                                             
//                                                                         //                                             
//Date: November 08, 2023                                                  //                                             
//Author: D.A.H.                                     
