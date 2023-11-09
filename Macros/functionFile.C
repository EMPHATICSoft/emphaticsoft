/////////////////////////////////////////////////////////////////////////////
// Description: This macro holds a function that takes in four root        //
// filesholding the regular and normalized DE and ADC data, and returns    //
// four additional root files, with the goal of mapping charge             //
// deposition (GetDE) into avgADC and eventually width as well.            //
// Using phase 1c geometry.                                                //
//                                                                         //
//Date: October 30, 2023                                                   //
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

void Function(const char* mappedHistogramFile, const char* barGraphFile)
{
  // Open the input ROOT files
  TFile* mappedHistogramInput = new TFile(mappedHistogramFile, "READ");
  TFile* barGraphInput = new TFile(barGraphFile, "READ");

  // Check if the input files are open 
  if (!mappedHistogramInput || mappedHistogramInput->IsZombie() || !barGraphInput || barGraphInput->IsZombie()) 
    {
      cerr << "Error opening input files." << endl;
      return;
    }
  
  //get histograms from the input files
  TH1D* mappedHistogram = (TH1D*)mappedHistogramInput->Get("mappedHistogram");
  TGraph* barGraph = (TGraph*)barGraphInput->Get("Graph");
  
  // Check if histograms are retrieved successfully
  if (!mappedHistogram || !barGraph)
    {
      cerr << "Error retrieving histograms from input files." << endl;
      return;
    }
   
  // Create a vector to store bin edges
  std::vector<double> binEdges;

  int bins = mappedHistogram->GetNbinsX();

  for (int i = 1; i <= bins; ++i) {
    double xLow = mappedHistogram->GetXaxis()->GetBinLowEdge(i);
    double xHigh = mappedHistogram->GetXaxis()->GetBinUpEdge(i);
    
    binEdges.push_back(xLow);
    binEdges.push_back(xHigh);
  }

  // Include the overflow bin edge
  binEdges.push_back(mappedHistogram->GetXaxis()->GetBinUpEdge(bins + 1));

  int numXBins = mappedHistogram->GetNbinsX();
                                                             
  // Create a 2D vector to store (bin number, DE value)
  std::vector<std::pair<int, double>> binNumberToDEValue;

  for (int i = 1; i <= numXBins; ++i)
 {                                                            
    double xValue = mappedHistogram->GetXaxis()->GetBinCenter(i);
    binNumberToDEValue.push_back(std::make_pair(i, xValue));
  }
  /*
  // Iterate through the 2D vector and print each element                                                                        
  std::cout << "binNumberToDEValue: ";
  for (int i = 0; i < binNumberToDEValue.size(); ++i)
    {
      std::cout << "(" << binNumberToDEValue[i].first << ", " << binNumberToDEValue[i].second << ") ";
    }
  std::cout << std::endl;
  */


  int numBins = barGraph->GetN();
                                        
  // Create a 2D vector to store (bin number, ADC value)
  std::vector<std::pair<int, double>> binNumberToXValue;

  for (int i = 0; i < numBins; ++i)                                        
    {
      double xValue, yValue ;
      barGraph->GetPoint(i, xValue, yValue);
      binNumberToXValue.push_back(std::make_pair(i + 1, xValue)); 
    }

  std::vector<std::pair<double, double>> combinedVector;
     
  for (size_t i = 0; i < binNumberToDEValue.size() && i < binNumberToXValue.size(); ++i) 
    {                             
      double firstValue = binNumberToDEValue[i].second;
    double secondValue = binNumberToXValue[i].second;
    combinedVector.push_back(std::make_pair(firstValue, secondValue));
  } 

  /*
  // Iterate through the 2D vector and print each element
  std::cout << "binNumberToXValue: ";
  for (int i = 0; i < binNumberToXValue.size(); ++i)
    {
      std::cout << "(" << binNumberToXValue[i].first << ", " << binNumberToXValue[i].second << ") ";
    }
  std::cout << std::endl;
  */
  
  // Iterate through the 2D vector and print each element
  std::cout << "combinedVector: ";
  for (int i = 0; i < combinedVector.size(); ++i)
    {
      std::cout << "(" << combinedVector[i].first << ", " << combinedVector[i].second << ") ";
    }
  std::cout << std::endl;


  // Create a new histogram with the same bin edges as mappedHistogram
    TH1D* newHistogram = new TH1D("NewHistogram", "New Histogram", binEdges.size() - 1, binEdges.data());

        
    for (int i = 0; i < combinedVector.size(); ++i) 
      {
	double binCenter = combinedVector[i].first;
	double adcValue = combinedVector[i].second; 
	int bin = newHistogram->FindBin(binCenter);
	newHistogram->SetBinContent(bin, adcValue);
      }
    
    /*
  // Fill the new histogram with the second values from binNumberToXValue
    for (int i = 0; i < binNumberToXValue.size(); ++i) 
      {
	int bin = newHistogram->FindBin(binNumberToDEValue[i].second);
	newHistogram->SetBinContent(bin, binNumberToXValue[i].second);
      }
    */
    /*
    // Print each bin and its content
    for (int bin = 1; bin <= newHistogram->GetNbinsX(); ++bin) {
      double content = newHistogram->GetBinContent(bin);
      double binLowEdge = newHistogram->GetXaxis()->GetBinLowEdge(bin);
      double binUpEdge = newHistogram->GetXaxis()->GetBinUpEdge(bin);

      std::cout << "Bin " << bin << ": (" << binLowEdge << ", " << binUpEdge << ") - Content: " << content << std::endl;
    }
    */ 


  // Create a canvas for the plot
  TCanvas* c1 = new TCanvas("c1", "New Histogram Plot", 1000, 500);
  newHistogram->Draw();
  newHistogram->SetTitle("DE vs ADC");
  c1->Draw();
  c1->WaitPrimitive();
  /*
  // Close input files
  mappedHistogramInput->Close();
  barGraphInput->Close();
  */ 
  TFile DEvsADCFile("DEvsADC.root", "RECREATE");
  /* 
  if (!DEvsADCFile || DEvsADCFile.IsZombie()) {
    cerr << "Error opening the output file." << endl;
    return;
  }
  */ 
  newHistogram->Write();
  DEvsADCFile.Close();
}

void functionFile()
{
  Function("mappedHistogram.root", "barGraph.root");
}
