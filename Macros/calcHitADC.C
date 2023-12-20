/////////////////////////////////////////////////////////////////////////////
// Description: This macro holds a function that takes in a single DE      //
// value and one root file holding a histogram with  DE vs ADC values and  //
// returns a single ADC value. Eventually width will be returned as well   //
// Using phase 1c geometry.                                                //
//                                                                         //
//Date: November 07, 2023                                                  //
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
#include <cmath>

double gaussian(double x, double mu, double sigma) 
{
  return (1.0 / (sigma * std::sqrt(2 * M_PI))) * std::exp(-((x - mu) * (x - mu)) / (2 * sigma * sigma));
}

double integral(double x, double mu, double sigma) 
{
  return 0.5 * (1.0 + std::erf( (x - mu) / (sigma * std::sqrt(2.0) ) ) );
}

void calcHitADC()
{
  double ADC = 280;
  int width = 2;
  double rms =  0.293035;
  double GetX = 0.000236161;
  double position = 0;
  double xtrue = 50.0;
  double xstrip = 30.0; // is this the center of the strip in micrometers?
  //double hitADC = 0;

  std::cout << "ADC: " << ADC << ", width: " << width << std::endl;

  // Create a vector of pairs (hit, ADC) with size equal to 'width'
  std::vector<std::pair<int, double>> hitADCVector(width);
  
  if (width == 1) 
    {
      hitADCVector[0] = std::make_pair(1, ADC);
    }
  else if (width == 2) 
    {
      hitADCVector[0] = std::make_pair(1, ADC / 2);
      hitADCVector[1] = std::make_pair(2, ADC / 2);
    }
  else if (width > 2)
    {
      position = (xtrue - xstrip)/xstrip; 
      //      std::cout << "position:" << position << std::endl;
    
      double gauss = gaussian(width, position, rms);
      std::cout << "Gaussian(" << width << ") = " << gauss << std::endl;

      double gaussIntegral = integral(width, position, rms);
      std::cout << "Integral of Gaussian from -infinity to " << width << " = " << gaussIntegral << std::endl;

    }

  // Print the contents of the vector
  for (int i = 0; i < width; i++) 
    {
      std::cout << "Hit: " << hitADCVector[i].first << ", ADC: " << hitADCVector[i].second << std::endl;
  }

}
