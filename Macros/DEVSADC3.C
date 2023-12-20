
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

  TH1D* ADCHist = new TH1D("ADCHist", "ADCvalue Histogram", 809, 41, 850);

  // Fill the histogram with ADCvalue frequencies using an index-based loop               
  for (size_t i = 0; i < GetDEADCValues.size(); ++i)
    {
      double ADCvalue = GetDEADCValues[i].second;
      ADCHist->Fill(ADCvalue);
    }
  
  ADCHist->Scale(1.0 / ADCHist->GetEntries());
  
  // Create a canvas to draw the histogram     
  TCanvas* canvas = new TCanvas("canvas", "ADCvalue Histogram", 1000, 500);
  ADCHist->SetFillColor(kRed);
  ADCHist->Draw("BAR");
  
  // Create a 2D histogram for GetDE vs ADCvalue               
  TH2D* DEvsADCHist = new TH2D("DEvsADCHist", "GetDE vs ADCvalue Histogram",10000, 0.0000001, .0010001, 859, 41, 900);
  
  for (size_t i = 0; i < GetDEADCValues.size(); ++i)
    {
      double GetDE = GetDEADCValues[i].first;
      double ADCvalue = GetDEADCValues[i].second;
      DEvsADCHist->Fill(GetDE, ADCvalue);
    }
  
  // Create a canvas to draw the histogram
  TCanvas* c10 = new TCanvas("c10", "GetDE vs ADCvalue Canvas", 1000, 500);
  DEvsADCHist->Draw("BAR");
  
  
  TH2D* slice2D = (TH2D*)hist3D->Project3D("zx");
  TCanvas* c1 = new TCanvas("c1", "2D Slice", 1000, 500);
  slice2D->Draw("COLZ");
  
  
  // Print GetDEADCValues vector  
  std::cout << "GetDEADCValues vector elements:" << std::endl;
  for (const auto& element : GetDEADCValues) 
    {
      std::cout << "GetDE: " << std::get<0>(element) << ", ADCvalue: " << std::get<1>(element) <<std::endl;
      
      double ADCvalue = std::get<1>(element);
      
      // Calculate binY for the histogram
      int binY = hist3D->GetYaxis()->FindBin(ADCvalue);

      // Set the range in the Y-axis
      hist3D->GetYaxis()->SetRange(binY, binY);
      
      // Project to 2D histogram
      TH2D* slice2D = (TH2D*)hist3D->Project3D("zx");
      
      // Create a vector to hold the bin centers
      std::vector<std::pair<double, double>> binCenters;  
      
      double probability = 0.0; 
      
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
      TRandom3 randomGenerator;
      double Width, rms;
      
      // Get random values for Width and RMS 
      slice2D->GetRandom2(Width, rms);
      
      // Convert Width to an integer   
      int width = TMath::Nint(Width);
      
      // Print random width and RMS
      std::cout << "Random Bin Center (X, Y): (" << width << ", " << rms << ")" << std::endl; 
      
      // Store the results in ADCWidthRMS vector
      ADCWidthRMS.push_back(std::make_tuple(ADCvalue, width, rms));
    }
    




  // Print ADCWidthRMS vector                                                  
  for (int i = 0; i < ADCWidthRMS.size(); ++i) 
    {
      std::cout << "ADCvalue: " << std::get<0>(ADCWidthRMS[i]) << ", Width: " << std::get<1>(ADCWidthRMS[i]) << ", RMS: " << std::get<2>(ADCWidthRMS[i]) << std::endl;
    }
  
  // Print the number of elements in my vectors   
  std::cout << "Number of elements in GetDEADCValues: " << GetDEADCValues.size() << std::endl;
  std::cout << "Number of elements in ADCWidthRMS: " << ADCWidthRMS.size() << std::endl;

  
  // Create histograms
  TH1D* clustRMS = new TH1D("RMS", "RMS; RMS; clusters per bin", 100, -0.5, 5);
  TH1D* Width = new TH1D("Width", "Width; width; clusters per bin", 36, 0.5, 18.5);
  TH1D* nclustRMS = new TH1D("nRMS", "Normalized RMS; RMS; clusters per bin", 100, -0.5, 5);
  TH1D* nWidth = new TH1D("nWidth", "Normalized Width; width; clusters per bin", 36, 0.5, 18.5);
  
  // Fill histograms with data
  for (const auto& entry : ADCWidthRMS)
    {
      double RMS = std::get<2>(entry);
      double width = std::get<1>(entry);

      clustRMS->Fill(RMS);
      Width->Fill(width);
    }

  // Normalize histograms
  double totEntriesRMS = clustRMS->Integral();
  if (totEntriesRMS > 0) {
    nclustRMS->Add(clustRMS, 1.0 / totEntriesRMS);
  }

  double totEntriesWidth = Width->Integral();
  if (totEntriesWidth > 0) {
    nWidth->Add(Width, 1.0 / totEntriesWidth);
  }  
  
  // Create canvases
  TCanvas* canvas1 = new TCanvas("canvas1", "RMS Histograms", 1200, 600);
  canvas1->Divide(2, 1);

  TCanvas* canvas2 = new TCanvas("canvas2", "Width Histograms", 1200, 600);
  canvas2->Divide(2, 1);

  // Draw histograms on canvas1
  canvas1->cd(1);
  clustRMS->Draw();
  canvas1->cd(2);
  nclustRMS->Draw();

  // Draw histograms on canvas2
  canvas2->cd(1);
  Width->Draw();
  canvas2->cd(2);
  nWidth->Draw();

  // Update and display canvases
  canvas1->Update();
  canvas1->Draw();
  canvas2->Update();
  canvas2->Draw();
  
  // Close input files
  DEvsADCInput->Close();
  //  histInput->Close();
  
  return ADCWidthRMS;
}

void DEVSADC3()
{
  Funct("sensor0_DEvsADC.root", " Sensor0.root");
}
