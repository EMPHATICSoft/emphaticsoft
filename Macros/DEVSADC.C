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
/*
std::vector<std::pair<double, int>> Funct(double DEvalue, const char* DEvsADCFile, const char* histFile)
{
  std::vector<std::pair<double, int>> ADCWidth;
*/

std::vector<std::tuple<double, int, double>> Funct(double DEvalue, const char* DEvsADCFile, const char* histFile)
{
  std::vector<std::tuple<double, int, double>> ADCWidthRMS;


  // Open the input ROOT files
  TFile* DEvsADCInput = new TFile(DEvsADCFile, "READ");
  TFile* histInput = new TFile(histFile, "READ");

  // Check if the input files are open 
  if (!DEvsADCInput || DEvsADCInput->IsZombie() || !histInput || histInput->IsZombie()) 
    {
      cerr << "Error opening input files." << endl;
      return ADCWidthRMS;
    }
  
  //get histogram from the input files
  TH1D* DEvsADC = (TH1D*)DEvsADCInput->Get("NewHistogram");
  TH3D* hist3D = (TH3D*)histInput->Get("WidthVsTADCVsRMS"); 

  // Check if histograms are retrieved successfully
  if (!DEvsADC || !hist3D)
    {
      cerr << "Error retrieving histograms from input files." << endl;
      return ADCWidthRMS;
    }
 
  double ADCvalue = -1.0;

    for (int bin = 1; bin <= DEvsADC->GetNbinsX(); bin++)
      {
	double lowerEdge = DEvsADC->GetXaxis()->GetBinLowEdge(bin);
	double upperEdge = DEvsADC->GetXaxis()->GetBinUpEdge(bin);

	if (DEvalue >= lowerEdge && DEvalue < upperEdge)
	  {
	    ADCvalue = DEvsADC->GetBinContent(bin);
	    break;
	  }
      }
    
    {
      std::cout << "ADCvalue: " << ADCvalue << std::endl;
    }

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
	std::cout << "Bin Center (X, Y): (" << binCenters[i].first << ", " << binCenters[i].second << ")" << std::endl;
      }

    gRandom->SetSeed(0);

    // Create a random number generator                                            
    TRandom3 randomGenerator;

    double Width, rms;

    slice2D->GetRandom2(Width, rms);
   
    int width = TMath::Nint(Width);

    std::cout << "Random width (x): " << width << std::endl;
    std::cout << "Random RMS (z): " << rms << std::endl;

    //    ADCWidth.push_back(std::make_pair(ADCvalue, width));
    ADCWidthRMS.push_back(std::make_tuple(ADCvalue, width, rms));
    for (int i = 0; i < ADCWidthRMS.size(); ++i) 
      {
	//      std::cout << "ADCvalue: " << ADCWidthRMS[i].first << ", Width: " << ADCWidthRMS[i].second << std::endl;
	std::cout << "ADCvalue: " << std::get<0>(ADCWidthRMS[i]) << ", Width: " << std::get<1>(ADCWidthRMS[i]) << ", RMS: " << std::get<2>(ADCWidthRMS[i]) << std::endl;

      }




  // Close input files
  DEvsADCInput->Close();
  //  histInput->Close();

  return ADCWidthRMS;
}

void DEVSADC()
{
  Funct(0.0004, "DEvsADC.root", "3Dhist.root");
}
