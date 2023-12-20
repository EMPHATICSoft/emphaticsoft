/////////////////////////////////////////////////////////////////////////////
// Description: This macro holds a function that takes in a single DE      //
// value and one root file holding a histogram with  DE vs ADC values and  //
// returns a vector that holds (ADC, width, RMS).                          //
// Using phase 1c geometry.                                                //
//                                                                         //
//Date: November 17, 2023                                                  //
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
#include <algorithm>
#include "AllDES0.C"
#include "TRandom3.h"

std::vector<std::tuple<double, double, int>> AllHits();
std::vector<std::tuple<double, int, double>> ADCWidthRMS;
std::vector<std::pair<double, double>> GetDEADCValues;

std::vector<std::tuple<double, int, double>> Funct(const char* DEvsADCFile, const char* histFile)
{
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

  for (const auto& element : AllHits())
    {
  double GetDE = std::get<0>(element);
  double GetX = std::get<1>(element);
  int sensor = std::get<2>(element);
  double ADCvalue = -1.0;
  
      for (int bin = 1; bin <= DEvsADC->GetNbinsX(); bin++)
	{
	  double lowerEdge = DEvsADC->GetXaxis()->GetBinLowEdge(bin);
	  double upperEdge = DEvsADC->GetXaxis()->GetBinUpEdge(bin);
	  
	  if (GetDE >= lowerEdge && GetDE < upperEdge)
	    {
	      ADCvalue = DEvsADC->GetBinContent(bin);
	      break;
	    }
	}
        
      GetDEADCValues.push_back(std::make_pair(GetDE, ADCvalue));
    }

   // Print GetDEADCValues vector
  std::cout << "GetDEADCValues vector elements:" << std::endl;
  for (const auto& element : GetDEADCValues) {
    std::cout << "GetDE: " << std::get<0>(element)
              << ", ADCvalue: " << std::get<1>(element) <<std::endl;

    double ADCvalue = std::get<1>(element);

    // Calculate binY for the histogram
    int binY = hist3D->GetYaxis()->FindBin(ADCvalue);

    // Set the range in the Y-axis
    hist3D->GetYaxis()->SetRange(binY, binY);

    // Project to 2D histogram
    TH2D* slice2D = (TH2D*)hist3D->Project3D("zx");

    // Create a vector to hold the bin centers
    std::vector<std::pair<double, double>> binCenters;

    // Loop over the bins in the histogram and store the bin centers
    for (int binX = 1; binX <= slice2D->GetNbinsX(); ++binX) {
      for (int binY = 1; binY <= slice2D->GetNbinsY(); ++binY) {
	double centerX = slice2D->GetXaxis()->GetBinCenter(binX);
	double centerY = slice2D->GetYaxis()->GetBinCenter(binY);

	//	double probability = slice2D->GetBinContent(binX, binY) / slice2D->Integral();

	if (slice2D->GetBinContent(binX, binY) > 0) {
	  //	  int width = (gRandom->Rndm() < probability) ? TMath::Nint(centerX) : TMath::Nint(centerY);
	  //double rms = (gRandom->Rndm() < probability) ? centerX : centerY;

	  binCenters.push_back(std::make_pair(centerX, centerY));


	  /*auto it = std::find_if(binCenters.begin(), binCenters.end(),
				 [centerX, centerY](const std::pair<double, double>& binCenter) {
				   return binCenter.first == centerX && binCenter.second == centerY;
				 });
	  */



	  /*

	  if (it != binCenters.end()) {
	    // Valid option in binCenters, choose width and rms accordingly
	    int width = TMath::Nint(centerX);
	    double rms = (gRandom->Rndm() < probability) ? centerX : centerY;

	    std::cout << ", Width: " << width << ", RMS: " << rms << std::endl;

	    // Store the results in ADCWidthRMS vector
	    ADCWidthRMS.push_back(std::make_tuple(ADCvalue, width, rms));
	  } else {
	    // Not a valid option in binCenters
	    std::cout << " - Invalid bin center, skipping." << std::endl;
	  }
	  

	  */

	  /*
	  if (!binCenters.empty()) {
	    // Choose a random bin center (x, y) from binCenters
	    int randomIndex = gRandom->Integer(binCenters.size());
	    double centerX = binCenters[randomIndex].first;
	    double centerY = binCenters[randomIndex].second;

	    // Choose width and rms based on the selected (x, y)
	    int width = TMath::Nint(centerX);
	    double rms = (gRandom->Rndm() < probability) ? centerX : centerY;

	    std::cout << ", Width: " << width << ", RMS: " << rms << std::endl;

	    // Store the results in ADCWidthRMS vector
	    ADCWidthRMS.push_back(std::make_tuple(ADCvalue, width, rms));
	  } else {
	    // No valid options in binCenters
	    std::cout << " - No valid bin centers, skipping." << std::endl;
	  }

	  */


	  

	}
      }
    }
   
    // Print out the bin centers
    for (int i = 0; i < binCenters.size(); ++i) {
      std::cout << "Bin Center (X, Y): (" << binCenters[i].first << ", " << binCenters[i].second << ")" << std::endl;
    }
    
    
    /*
    // Print out the bin centers and associated probability
    std::cout << "Bin Center (X, Y): (" << binCenters.back().first << ", " << binCenters.back().second << ")"
    	      << ", Probability: " << probability << ", Width: " << width  << ", RMS: " << rms << std::endl;
    */


    TRandom3 randomGenerator;
    double Width, rms;

    // Get random values for Width and RMS
    slice2D->GetRandom2(Width, rms);

    // Convert Width to an integer
    int width = TMath::Nint(Width);
   

    // Print random width and RMS
    std::cout << "Random width (x): " << width << std::endl;
    std::cout << "Random RMS (z): " << rms << std::endl;

    // Store the results in ADCWidthRMS vector
     ADCWidthRMS.push_back(std::make_tuple(ADCvalue, width, rms));
     //	}
    
     //   }
     // }
      //  }
     /*
     // Print ADCWidthRMS vector
  for (int i = 0; i < ADCWidthRMS.size(); ++i) {
    std::cout << "ADCvalue: " << std::get<0>(ADCWidthRMS[i]) << ", Width: " << std::get<1>(ADCWidthRMS[i]) << ", RMS: " << std::get<2>(ADCWidthRMS[i]) << std::endl;
  }
  
     */

  // Close input files
  DEvsADCInput->Close();
  //  histInput->Close();


  // Print the number of elements in my vectors
  std::cout << "Number of elements in GetDEADCValues: " << GetDEADCValues.size() << std::endl;
  std::cout << "Number of elements in ADCWidthRMS: " << ADCWidthRMS.size() << std::endl;


  return ADCWidthRMS;
}

void DEVSADC()
{
  Funct("sensor0_DEvsADC.root", " Sensor0.root");
}
