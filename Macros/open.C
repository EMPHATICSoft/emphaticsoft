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
#include "DEVSADC2.C"


//std::vector<std::tuple<double, int, double>> DEVSADC2();

void open()
{

  /*
  double ADC;
  int width;
  double RMS;
  */

  // Call DEVSADC function
  std::vector<std::tuple<double, int, double>> ADCWidthRMS = Funct();

  std::cout << "DEVSADC vector elements: ";

  for (const auto& element : ADCWidthRMS)
    {
      double ADC = std::get<0>(element);
      int width = std::get<1>(element);
      double RMS = std::get<2>(element);
      std::cout << "(" << ADC << ", " << width << ", " << RMS << ") ";
    }

  std::cout << std::endl;
  //  std::cout << "(" << ADC << ", " << width << ", " << RMS << ") ";
  //  Funct(GetDE, file1.c_str(), file2.c_str());
  //  Funct(GetDE, "sensor0_DEvsADC.root", " Sensor0.root");

}
