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
#include <cstdlib>
#include <TRandom3.h>

//double Func(double ADCvalue, const char* histFile)
void  Slice()
{
  // Open the input ROOT files
  TFile* histInput = new TFile("3Dhist.root", "READ");

  // Check if the input files are open 
  if (!histInput || histInput->IsZombie()) 
    {
      cerr << "Error opening input files." << endl;
      return -1.0;
    }
  
  //get histogram from the input files
  TH3D* hist3D = (TH3D*)histInput->Get("WidthVsTADCVsRMS");
 
  // Check if histograms are retrieved successfully
  if (!hist3D)
    {
      cerr << "Error retrieving histograms from input files." << endl;
      return -1.0;
    }
  double ADCvalue = 205;

  int binY = hist3D->GetYaxis()->FindBin(ADCvalue);

  hist3D->GetYaxis()->SetRange(binY, binY);
  
  TH2D* slice2D = (TH2D*)hist3D->Project3D("zx");

  TCanvas* c1 = new TCanvas("c1", "2D Slice", 1000, 500);
  slice2D->Draw("COLZ");

  // Create a vector to hold the bin centers
  std::vector<std::pair<double, double>> binCenters;

  // Loop over the bins in the histogram and store the bin centers
  for (int binX = 1; binX <= slice2D->GetNbinsX(); ++binX)
    {
      for (int binY = 1; binY <= slice2D->GetNbinsY(); ++binY)
	{
	  double centerX = slice2D->GetXaxis()->GetBinCenter(binX);
	  double centerY = slice2D->GetYaxis()->GetBinCenter(binY);

	  if (slice2D->GetBinContent(binX, binY) > 0)
	    {
	      binCenters.push_back(std::make_pair(centerX, centerY));
	    }

	}
    }

  // Print out the bin centers
  for (int i = 0; i < binCenters.size(); ++i)
    {
      cout << "Bin Center (X, Y): (" << binCenters[i].first << ", " << binCenters[i].second << ")" << endl;
    }


    
  gRandom->SetSeed(0);
  
  // Create a random number generator
  TRandom3 randomGenerator;

  double randomX, randomZ;
  slice2D->GetRandom2(randomX, randomZ);

  int width = TMath::Nint(randomX);
 
  std::cout << "Random width (x): " << width << std::endl;
  std::cout << "Random RMS (z): " << randomZ << std::endl;
  
}
