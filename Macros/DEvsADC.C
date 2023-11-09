/////////////////////////////////////////////////////////////////////////////
// Description: This macro holds a function that takes in a single DE      //
// value and one root file holding a histogram with  DE vs ADC values and  //
// returns a single ADC value. Eventually width will be returned as well   //
// Using phase 1c geometry.                                                //
//                                                                         //
//Date: October 31, 2023                                                   //
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

double Funct(double DEvalue, const char* DEvsADCFile)
{
  // Open the input ROOT files
  TFile* DEvsADCInput = new TFile(DEvsADCFile, "READ");

  // Check if the input files are open 
  if (!DEvsADCInput || DEvsADCInput->IsZombie()) 
    {
      cerr << "Error opening input files." << endl;
      return -1.0;
    }
  
  //get histogram from the input files
  TH1D* DEvsADC = (TH1D*)DEvsADCInput->Get("NewHistogram");
 
  // Check if histograms are retrieved successfully
  if (!DEvsADC)
    {
      cerr << "Error retrieving histograms from input files." << endl;
      return -1.0;
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
    
  // Close input files
  DEvsADCInput->Close();

  return ADCvalue;
}

void DEvsADC()
{
  Funct(0.000104103, "DEvsADC.root");
}
