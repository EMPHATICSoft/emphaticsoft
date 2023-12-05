/////////////////////////////////////////////////////////////////////////////
// Description: This macro opens two root files that hold a mapped         //
// histogram and a bar graph for a specific sensor or all together and     //
// makes a DE vs ADC histogram for the given sensor.                       //
//                                                                         //
//Date: December 05, 2023                                                  //
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

  //find the bin edges for all the bins in mappedHistogram and store
  for (int i = 1; i <= bins; ++i) 
    {
      double xLow = mappedHistogram->GetXaxis()->GetBinLowEdge(i);
      double xHigh = mappedHistogram->GetXaxis()->GetBinUpEdge(i);
      
      binEdges.push_back(xLow);
      binEdges.push_back(xHigh);
    }
  
  // Include the overflow bin edge
  binEdges.push_back(mappedHistogram->GetXaxis()->GetBinUpEdge(bins + 1));

  //find the number of bins in mappedHistogram
  int numXBins = mappedHistogram->GetNbinsX();
                                                             
  // Create a 2D vector to store (bin number, DE value)
  std::vector<std::pair<int, double>> binNumberToDEValue;

  //loop over bin and store the bin number and its corresponding value in the vector 
  for (int i = 1; i <= numXBins; ++i)
 {                                                            
    double xValue = mappedHistogram->GetXaxis()->GetBinCenter(i);
    binNumberToDEValue.push_back(std::make_pair(i, xValue));
  }

  //find the number of bins in barGraph
  int numBins = barGraph->GetN();
                                        
  // Create a 2D vector to store (bin number, ADC value)
  std::vector<std::pair<int, double>> binNumberToXValue;

  //loop over the barGraph bins and store the bin number and its corresponding value in the vector
  for (int i = 0; i < numBins; ++i)                                        
    {
      double xValue, yValue;
      barGraph->GetPoint(i, xValue, yValue);
      binNumberToXValue.push_back(std::make_pair(i + 1, xValue)); 
    }

  //initiate a vector to store DE, ADC pairs 
  std::vector<std::pair<double, double>> combinedVector;

  for (size_t i = 0; i < binNumberToDEValue.size() && i < binNumberToXValue.size(); ++i) 
    {                             
      double firstValue = binNumberToDEValue[i].second;
      double secondValue = binNumberToXValue[i].second;
      combinedVector.push_back(std::make_pair(firstValue, secondValue));
    }
  
  // Create a new histogram with the same bin edges as mappedHistogram that stores DE vs ADC
    TH1D* newHistogram = new TH1D("NewHistogram", "New Histogram", binEdges.size() - 1, binEdges.data());
  
    for (int i = 0; i < combinedVector.size(); ++i) 
      {
	double binCenter = combinedVector[i].first;
	double adcValue = combinedVector[i].second; 
	int bin = newHistogram->FindBin(binCenter);
	newHistogram->SetBinContent(bin, adcValue);
      }
    
    // Create a canvas for the plot
    TCanvas *c6 = new TCanvas("c6", "c6", 1000, 500);
    mappedHistogram->SetFillColor(0);
    mappedHistogram->SetLineColor(kBlue);
    mappedHistogram->Draw("HIST");
    mappedHistogram->GetYaxis()->SetRangeUser(0, .2);
    c6->Update();
    //    c6->WaitPrimitive();

    TCanvas* c7 = new TCanvas("c7", "c7", 1000, 500);
    barGraph->SetTitle("ADC Value vs. DE Fraction");
    barGraph->SetMarkerStyle(21);
    barGraph->SetMarkerSize(1.0);
    barGraph->SetFillColor(kRed);
    barGraph->Draw("AB1");
    barGraph->GetXaxis()->SetRangeUser(41, 850);
    barGraph->GetXaxis()->SetTitle("ADC Value");
    barGraph->GetYaxis()->SetTitle("DE Fraction");
    c7->Update();
    c7->WaitPrimitive();

  TCanvas* c1 = new TCanvas("c1", "New Histogram Plot", 1000, 500);
  newHistogram->Draw();
  newHistogram->SetTitle("DE vs ADC");
  c1->Draw();
  //  c1->WaitPrimitive();

  TString sensorName = TString(mappedHistogramFile).ReplaceAll("_mappedHistogram.root", "");

  // Create a new file name for the output based on the sensor name
  TString outputFileName = sensorName + "_DEvsADC.root";

  // Create a new TFile for the output
  TFile DEvsADCFile(outputFileName, "RECREATE");

  newHistogram->Write();
  DEvsADCFile.Close();

  // Close input files                                
  mappedHistogramInput->Close();
  barGraphInput->Close();
}

void functionFile()
{
  Function("sensor0_mappedHistogram.root", "sensor0_barGraph.root");
}
