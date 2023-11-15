///////////////////////////////////////////////////////////////////////////// 
// Description: This macro a root file that stores five histograms:        //
// (getde, totADC, ngetde, ntotADC, and ntotADC) for a specific sensor or  //
// in for all together (total). It then uses these histograms to create a  //
// mapped histogram and a bar graph for the given sensor. The new root     // 
// files can then be used to make a DE vs ADC histogram for the given      //
// sensor in a new macro.                                                  // 
//                                                                         //
//Date: November 13, 2023                                                  //
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

  void MapAndBar(const char* sensorFile)
  {
    TFile* sensorInput = new TFile(sensorFile, "READ");

    // Check if the input files are open
    if ( !sensorInput || sensorInput->IsZombie())
      {          
	cerr << "Error opening input file." << endl;
	return;   
      }
                                                
    //get histograms from the input file
    TH1D* getde = (TH1D*)sensorInput->Get("getde");   
    TH1D* totADC = (TH1D*)sensorInput->Get("totADC");
    TH1D* ngetde = (TH1D*)sensorInput->Get("ngetde");
    TH1D* ntotadc = (TH1D*)sensorInput->Get("ntotadc");
    TH1D* ntotADC = (TH1D*)sensorInput->Get("ntotADC");


  // Check if histograms are retrieved successfully
  if ( !getde || !totADC || !ntotADC || !ngetde || !ntotadc)
    {
      cerr << "Error retrieving histograms from input file" << endl;
      return;
    }

  int totADCclusts = 0;
  for (int ADCbin = 1; ADCbin <= totADC->GetNbinsX(); ++ADCbin)
    {
      totADCclusts += totADC->GetBinContent(ADCbin);
    }

  // Create vectors to store the content and fraction
  std::vector<double> adcBinFractions;
  std::vector<int> adcBinValues;
  std::vector<std::pair<int, double>> adc2DVector;

  int ntotadcIndex = 1;
  double threshold = 6.5e-6;

  // Loop over ADC bins
  for (int nADCbin = 1; nADCbin <= ntotADC->GetNbinsX(); ++nADCbin)
    { 
      double nADCFraction = ntotADC->GetBinContent(nADCbin);

      if (nADCFraction > threshold)
        {
          adcBinFractions.push_back(nADCFraction);

          // Store the corresponding x-axis value
          int adcBinValue = ntotADC->GetXaxis()->GetBinCenter(nADCbin);
          adcBinValues.push_back(adcBinValue);

          // Create a pair and store it in the 2D vector
          adc2DVector.push_back(std::make_pair(adcBinValue, nADCFraction));
          ntotadcIndex++;
        }
    }

  int totDEclusts = 0;
  for (int DEbin = 1; DEbin <= getde->GetNbinsX(); ++DEbin)
    {
      totDEclusts += getde->GetBinContent(DEbin);
    }

  // Create a vector to store DE bin contents
  std::vector<int> DEBinContents;
  std::vector<double> DEBinFractions;
  std::vector<double> DEBinValues;
  std::vector<std::pair<double, double>> DE2DVector;

  // Print the number of truehits in each bin of getDE
  for (int DEbin = 1; DEbin <= getde->GetNbinsX(); ++DEbin)
    {
      int DEBinContent = getde->GetBinContent(DEbin);
      DEBinContents.push_back(DEBinContent);

      // Calculate the fraction and store it in DEBinFractions
      double DEFraction = static_cast<long double>(DEBinContent) / totDEclusts;
      DEBinFractions.push_back(static_cast<double>(DEFraction));

      // Store the corresponding x-axis value
      long double DEBinValue = getde->GetXaxis()->GetBinCenter(DEbin);
      DEBinValues.push_back(DEBinValue);

      // Create a pair and store it in the 2D vector
      DE2DVector.push_back(std::make_pair(DEBinValue, DEFraction));
    }

  // Initialize a vector to store the number of DE bins needed for each ADC bin
  std::vector<int> DEBinsNeeded;
  double currentADCThreshold =  adc2DVector[0].second;                                           
  double currentADCFraction = 0.0;
  int DEBinsCount = 0;
  double totalADCFraction = 0.0;
  std::vector<double> accumulatedADCFractions;

  for (int DEbin = 0; DEbin < DE2DVector.size(); ++DEbin)
    {
      currentADCFraction += DE2DVector[DEbin].second;
      DEBinsCount++;

      if (currentADCFraction == 0.0)
        {
          DEBinsCount = 0;
        }
      else if (currentADCFraction >= currentADCThreshold)
        {
          //make sure the current bin count exceeds the ADC fraction
          totalADCFraction += currentADCFraction;
          int binIndex = 1 + DEBinsNeeded.size();
          accumulatedADCFractions.push_back(currentADCFraction);
          DEBinsNeeded.push_back(DEBinsCount);
          DEBinsCount = 0;
          currentADCFraction = 0.0;
	  if (DEBinsNeeded.size() == adc2DVector.size())
            {
              break; // All ADC bins processed
            }
          currentADCThreshold = adcBinFractions[DEBinsNeeded.size()];    
	}
    }

  int totbins = 0;
  double binWidth = ngetde->GetBinWidth(1);

  // Calculate the new bin edges
  std::vector<double> newBinEdges;
  long double currentEdge =  0.0000001;

  for (int i = 0; i < DEBinsNeeded.size(); i++)
    {
      int n = DEBinsNeeded[i];
      newBinEdges.push_back(currentEdge);
      currentEdge += n * binWidth;
    }
  newBinEdges.push_back(currentEdge);

  TH1* mappedHistogram = ngetde->Rebin(newBinEdges.size() -1, "mappedHistogram", newBinEdges.data());

  // Access and print the content of the last bin (the extra bin)                 
  int lastBin = mappedHistogram->GetNbinsX() + 1;
  double lastBinContent = mappedHistogram->GetBinContent(lastBin);
  double lastBinEdge = mappedHistogram->GetXaxis()->GetBinUpEdge(lastBin);
  accumulatedADCFractions.push_back(lastBinContent);
  newBinEdges.push_back(lastBinEdge);

  // Create a vector to store the bin centers and content
  std::vector<std::pair<double, double>> mapped2DVector;

  for (int i = 1; i <= mappedHistogram->GetNbinsX(); ++i)
    {
      double binContent = mappedHistogram->GetBinContent(i);
      double binCenter = mappedHistogram->GetXaxis()->GetBinCenter(i);
      mapped2DVector.push_back(std::make_pair(binCenter, binContent));
    }

  std::vector<std::pair<int, double>> combined2DVector;
  size_t size2D = adc2DVector.size();
  size_t size1D = accumulatedADCFractions.size();
  for (size_t i = 0; i < size2D; ++i)
    {
      int adcValue = adc2DVector[i].first;
      double fraction = (i < size1D) ? accumulatedADCFractions[i] : 0.0;
      if (fraction != 0.0)
        {
          combined2DVector.push_back(std::make_pair(adcValue, fraction));
        }
    }

  double xMin = ntotADC->GetXaxis()->GetXmin();
  double xMax = ntotADC->GetXaxis()->GetXmax();

  // Create a TGraph for the bar graph
  TGraph* barGraph = new TGraph(combined2DVector.size());
  barGraph->GetXaxis()->SetRangeUser(xMin, xMax);
  for (size_t i = 0; i < combined2DVector.size(); ++i)
    {
      barGraph->SetPoint(i, combined2DVector[i].first, combined2DVector[i].second);
    }
  /*  
  //print histograms
  TCanvas *c1 = new TCanvas("c1","c1",1000,500);
  getde->SetFillColor(kBlue);
  getde->Draw("colz");
  getde->GetYaxis()->SetRangeUser(0, 1.1*(getde->GetMaximum()));
  
  TCanvas *c2 = new TCanvas("c2","c2",1000,500);
  totADC->SetFillColor(kBlue);
  totADC->Draw("colz");
  totADC->GetYaxis()->SetRangeUser(0, 1.1*(totADC->GetMaximum()));

  TCanvas *c3 = new TCanvas("c3", "c3", 1000, 500);
  ngetde->Draw();
  ngetde->SetFillColor(kRed);
  ngetde->GetYaxis()->SetRangeUser(0, 1.1*(ngetde->GetMaximum()));

  TCanvas *c4 = new TCanvas("c4", "c4", 1000, 500);
  ntotADC->SetFillColor(kRed);
  ntotADC->Draw();
  ntotADC->GetYaxis()->SetRangeUser(0, 1.1*(ntotADC->GetMaximum()));

  TCanvas *c5 = new TCanvas("c5", "c5", 1000, 500);
  ntotadc->SetFillColor(kRed);
  ntotadc->Draw();
  ntotadc->GetYaxis()->SetRangeUser(0, 1.1*(ntotADC->GetMaximum()));
  */

  TCanvas *c6 = new TCanvas("c6", "c6", 1000, 500);
  mappedHistogram->SetFillColor(0);
  mappedHistogram->SetLineColor(kBlue);
  mappedHistogram->Draw("HIST");
  mappedHistogram->GetYaxis()->SetRangeUser(0, .2);

  TCanvas* c7 = new TCanvas("c7", "c7", 1000, 500);
  barGraph->SetTitle("ADC Value vs. DE Fraction");
  barGraph->SetMarkerStyle(21);
  barGraph->SetMarkerSize(1.0);
  barGraph->SetFillColor(kRed);
  barGraph->Draw("AB1");
  barGraph->GetXaxis()->SetTitle("ADC Value");
  barGraph->GetYaxis()->SetTitle("Fraction");

  TCanvas *c8 = new TCanvas("c8", "c8", 1000, 500);
  barGraph->Draw("AB1");
  barGraph->GetXaxis()->SetRangeUser(xMin, xMax);
  ntotadc->SetMarkerStyle(2);
  ntotadc->SetMarkerSize(0.5);
  ntotadc->Draw("P SAME");
  ntotadc->GetXaxis()->SetRangeUser(xMin, xMax);

  TString sensorName(sensorFile); // Convert const char* to TString
  sensorName.ReplaceAll(".root", ""); // Remove the ".root" extension

  // Save mappedHistogram with the original file name
  TString mappedHistogramFileName = sensorName + "_mappedHistogram.root";
  TFile mappedHistogramFile(mappedHistogramFileName, "RECREATE");
  mappedHistogram->Write();
  mappedHistogramFile.Close();

  // Save barGraph with the original file name
  TString barGraphFileName = sensorName + "_barGraph.root";
  TFile barGraphFile(barGraphFileName, "RECREATE");
  barGraph->Write();
  barGraphFile.Close();

  sensorInput->Close();
  delete sensorInput;

}

void Hists2()
{
  MapAndBar("sensor1.root");
}
