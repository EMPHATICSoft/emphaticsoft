/////////////////////////////////////////////////////////////////////////////
// Description: This macro  identifies good events (events with exactly    //
// one cluster per plane) for a chain of data files, identifies good       //
// events (events with exactly one truehit per plane) for a chain of MC    //
// files with the goal of mapping charge deposition (GetDE) into avgADC    //
// and eventually width as well. Using phase 1c geometry.                  //
//                                                                         //
//Date: November 01, 2023                                                  //
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

void histograms()
{
  //Create a TChains 
  TChain *datachain = new TChain("recTree");
  TChain *MCchain = new TChain("recTree");
  
  //add files to datachain
  datachain->Add("emphdata_v03_02a_r2098_s1.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s2.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s3.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s4.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s5.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s6.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s7.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s8.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s9.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s10.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s11.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s12.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s13.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s14.caf.root");
  datachain->Add("emphdata_v03_02a_r2098_s15.caf.root");

  datachain->Add("emphdata_v03_02a_r2099_s1.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s2.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s3.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s4.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s5.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s6.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s7.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s8.caf.root");
  datachain->Add("emphdata_v03_02a_r2099_s9.caf.root");
  
  //add files to MCchain
  MCchain->Add("emphmc_r2098_s1_dig.caf.root");
  MCchain->Add("emphmc_r2098_s2_dig.caf.root");
  MCchain->Add("emphmc_r2098_s3_dig.caf.root");
  MCchain->Add("emphmc_r2098_s4_dig.caf.root");
  MCchain->Add("emphmc_r2098_s5_dig.caf.root");
  MCchain->Add("emphmc_r2098_s6_dig.caf.root");
  MCchain->Add("emphmc_r2098_s7_dig.caf.root");
  MCchain->Add("emphmc_r2098_s8_dig.caf.root");
  MCchain->Add("emphmc_r2098_s9_dig.caf.root");
  MCchain->Add("emphmc_r2098_s10_dig.caf.root");
  MCchain->Add("emphmc_r2098_s11_dig.caf.root");
  MCchain->Add("emphmc_r2098_s12_dig.caf.root");
  MCchain->Add("emphmc_r2098_s13_dig.caf.root");
  MCchain->Add("emphmc_r2098_s14_dig.caf.root");
  MCchain->Add("emphmc_r2098_s15_dig.caf.root");


  MCchain->Add("emphmc_r2099_s1_dig.caf.root");
  MCchain->Add("emphmc_r2099_s2_dig.caf.root");
  MCchain->Add("emphmc_r2099_s3_dig.caf.root");
  MCchain->Add("emphmc_r2099_s4_dig.caf.root");
  MCchain->Add("emphmc_r2099_s5_dig.caf.root");  
  MCchain->Add("emphmc_r2099_s6_dig.caf.root");
  MCchain->Add("emphmc_r2099_s7_dig.caf.root");
  MCchain->Add("emphmc_r2099_s8_dig.caf.root");
  MCchain->Add("emphmc_r2099_s9_dig.caf.root");

  // Create StandardRecord objects and set the branch address using our tree
  caf::StandardRecord* recData = 0;
  datachain->SetBranchAddress("rec", &recData);
  caf::StandardRecord* recMC = 0;
  MCchain->SetBranchAddress("rec", &recMC);

  // Initalize histograms
  TH1D *getde = new TH1D ("getde", "GetDE; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1D *totADC = new TH1D ("totADC", "Total ADC; totADC; Number of Clusters", 809, 41, 850);

  TH1D *ngetde = new TH1D ("ngetde", "Normalized GetDE; GetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, 0.0010001);
  TH1D *ntotADC = new TH1D ("ntotADC", "Normalized Total ADC; totADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  TH1D *ntotadc = new TH1D("ntotadc", "Normalized Total ADC (ignoring bins with fraction < 6.5e-6) ; totADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  // Initalize the number of good data events to 0
  int n_good_data_events = 0;
  
  /// Counter for total number of ADC clusters
  int TotalADCClustersData = 0;

  int ndigits;
  double TotADC;

  // Loop over our tree and each time we call GetEntries, the data in the tree is copied to recData
  for (int i = 0; i < datachain->GetEntries(); ++i)
    {
      datachain->GetEntry(i); //i is the entries/events

      // Number of Clusters per Event
      int nclusts = recData->cluster.clust.size(); 
      int nclusts_plane[18] = {0}; //array that holds the number of clusters in each plane for that event
      
      //loop over nclusts and if they are in a specific station and sensor, add one to the corresponding plane
      //if there is at least one cluster per event in that plane
      for (int idx = 0; idx < nclusts; ++idx)
        {
          int plane = recData->cluster.clust[idx].plane;
          nclusts_plane[plane]++;
	  //the array nclusts_plane is now filled with the number of clusters per plane for the specific event
        }
      bool good_data_event = true;

      for (int j = 0; j < 18; j++)
        {
	  if (nclusts_plane[j] != 1) good_data_event = false;
	  //conditional to check if each plane has exactly one cluster
	}

      for (int idx = 0; idx < nclusts; ++idx)
	{
	  ndigits = recData->cluster.clust[idx].ndigits;
	  int width = recData->cluster.clust[idx].width;
	  if (ndigits != width) good_data_event = false;
	  //conditional to check that ndigits and width match
	}

      if (good_data_event)
        {
          TotalADCClustersData += nclusts;
          n_good_data_events++;
  
          for (int idx = 0; idx < nclusts; ++idx)
            {
              double AvgADC = recData->cluster.clust[idx].avgadc;
	      TotADC = ndigits*AvgADC;
	      totADC->Fill(TotADC);
	    }
        } 
    } // end loop over data entries   
  
  // Initalize the number of good MC events
  int n_good_MC_events = 0;

  /// Counter for total number of DE clusters  
  int TotalDEClustersMC = 0;

  double GetDE;
  
  // Loop over our tree and each time we call GetEntries, the MCdata in the tree is copied to recMC.
  for (int a = 0; a < MCchain->GetEntries(); ++a)
    {
      MCchain->GetEntry(a); //a is the entries/events
      
      // Number of truehits per Event   
      int ntruehits = recMC->truth.truehits.truehits.size();
      int ntruehits_plane[18] = {0}; //array that holds the number of truehits in each plane per event
      
      //loop over ntruehits and if they are in a specific plane, add one to the corresponding plane
      //if there is at least one truehit per event in that plane                                
      for (int idx = 0; idx < ntruehits; ++idx)
        {
          int plane = recMC->truth.truehits.truehits[idx].GetPlane;
          ntruehits_plane[plane]++;
          //the array ntruehits_plane is now filled with the number of clusters per plane for the specific event
        }
      bool good_MC_event = true;
      for (int b = 0; b < 18; b++)
        {
          if (ntruehits_plane[b] != 1) good_MC_event = false;
          //conditional to check if each plane has exactly one hit                   
        }
      if (good_MC_event)
        {
          TotalDEClustersMC += ntruehits;
          n_good_MC_events++;
          for (int idx = 0; idx < ntruehits; ++idx)
            {
              GetDE = recMC->truth.truehits.truehits[idx].GetDE;
              getde->Fill(GetDE);
            }
        }
    } // end loop over MC entries

  int totADCclusts = 0;

  for (int ADCbin = 1; ADCbin <= totADC->GetNbinsX(); ++ADCbin)
    {
      totADCclusts += totADC->GetBinContent(ADCbin);
    }

  // Create a vector to store ADC bin contents fractions and values 
  std::vector<double> ADCBinContents;
  std::vector<double> ADCBinFractions;
  std::vector<int> ADCBinValues;
  std::vector<std::pair<int, double>> ADC2DVector;

  // Print the number of clusters in each bin of totADC
  for (int ADCbin = 1; ADCbin <= totADC->GetNbinsX(); ++ADCbin)
    {
      double ADCBinContent = totADC->GetBinContent(ADCbin);
      ADCBinContents.push_back(ADCBinContent); // Store bin content in ADCBinContents
      
      // Calculate the fraction and store it in ADCBinFractions
      long double ADCFraction = static_cast<double>(ADCBinContent) / totADCclusts;
      ADCBinFractions.push_back(ADCFraction);

      int ADCBinValue = totADC->GetXaxis()->GetBinCenter(ADCbin);
      ADCBinValues.push_back(ADCBinValue);

      // Create a pair and store it in the 2D vector
      ADC2DVector.push_back(std::make_pair(ADCBinValue, ADCFraction));
    }
  for (int i = 0; i < ADC2DVector.size(); ++i)  
  {
      if (ntotADC)
	{
	  // Check if i is a valid index for ADCBinFractions
	  if (i >= 0 && i < ADC2DVector.size()) 
	    {
	      ntotADC->SetBinContent(i+1, ADC2DVector[i].second);
	    }
	  else 
	    {
	      std::cerr << "Error: Invalid index i for ADCBinFractions." << std::endl;
	    }
	}
      else 
	{
	  std::cerr << "Error: ntotADC is not a valid pointer." << std::endl;
	}
    }
  
  // Create vectors to store the content and fractions                                                    
  //  std::vector<int> adcBinContents;
  std::vector<double> adcBinFractions;
  std::vector<int> adcBinValues;                                                                          
  std::vector<std::pair<int, double>> adc2DVector;

  int ntotadcIndex = 1;
  double threshold = 6.5e-6;

  // Loop over ADC bins
  for (int nADCbin = 1; nADCbin <= ntotADC->GetNbinsX(); ++nADCbin)                                     
    {
      //      double nADCBinContent = ntotADC->GetBinContent(nADCbin);
      double nADCFraction = ntotADC->GetBinContent(nADCbin); 

      if (nADCFraction > threshold)
        {
	  //  adcBinContents.push_back(nADCBinContent);
	  adcBinFractions.push_back(nADCFraction);

          // Store the corresponding x-axis value
          int adcBinValue = ntotADC->GetXaxis()->GetBinCenter(nADCbin);
          adcBinValues.push_back(adcBinValue);

          // Create a pair and store it in the 2D vector
          adc2DVector.push_back(std::make_pair(adcBinValue, nADCFraction));
          ntotadcIndex++;
        }
    }


  /*
  // Iterate through the 2D vector and print each element                                               
  std::cout << "ADC2DVector: ";                                                                         
  for (int i = 0; i < ADC2DVector.size(); ++i)                                                          
    {                                                                                                   
      std::cout << "(" << ADC2DVector[i].first << ", " << ADC2DVector[i].second << ") ";                
    }                                                                                                   
  std::cout << std::endl;

  // Iterate through the 2D vector and print each element                                               
  std::cout << "adc2DVector: ";                                                                         
  for (int i = 0; i < adc2DVector.size(); ++i)                                                          
    {                                                                                                   
      std::cout << "(" << adc2DVector[i].first << ", " << adc2DVector[i].second << ") ";                
    }                                                                                                   
  std::cout << std::endl;

  */

  /*
  // Create vectors to store the content and fractions (old cutoff method)
  std::vector<int> adcBinContents;
  std::vector<double> adcBinFractions;
  std::vector<int> adcBinValues;  
  std::vector<std::pair<int, double>> adc2DVector;
  
  int ntotadcIndex = 1;
    
  // Loop over ADC bins
  for (int ADCbin = 1; ADCbin <= totADC->GetNbinsX(); ++ADCbin) 
    {
      double ADCBinContent = totADC->GetBinContent(ADCbin);
      double ADCFraction = static_cast<double>(ADCBinContent) / totADCclusts;
      
      // If the content of the bin is more than zero, add it to ntotadc
      if (ADCBinContent > 5)
	{
	  adcBinContents.push_back(ADCBinContent);
	  adcBinFractions.push_back(ADCFraction);

	  // Store the corresponding x-axis value
	  int adcBinValue = totADC->GetXaxis()->GetBinCenter(ADCbin);
	  adcBinValues.push_back(adcBinValue);
	  
	  // Create a pair and store it in the 2D vector           
	  adc2DVector.push_back(std::make_pair(adcBinValue, ADCFraction));
	  ntotadcIndex++;
	}
    }
  */



  // Iterate through the 2D vector and plot ntotadc
  for (int i = 0; i < adc2DVector.size(); ++i) 
    {
      ntotadc->SetBinContent(ntotadc->FindBin(adc2DVector[i].first), adc2DVector[i].second);
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
  
  for (int i = 0; i < DE2DVector.size(); ++i) 
    {
      ngetde->SetBinContent(i+1, DE2DVector[i].second);
    }
  /*
  // Iterate through the 2D vector and print each element                                                              
  std::cout << "DE2DVector: ";                                                                                         
  for (int i = 0; i < DE2DVector.size(); ++i)                                                                          
    {
      std::cout << "(" << DE2DVector[i].first << ", " << DE2DVector[i].second << ") ";      
    }                                                                        
  std::cout << std::endl;
  */ 




  // Initialize a vector to store the number of DE bins needed for each ADC bin
  std::vector<int> DEBinsNeeded;
  double currentADCThreshold =  adc2DVector[0].second; //here
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
	  /*	  std::cout << " currentADCFraction: "<< currentADCFraction
                    << ", binIndex: " << binIndex << std::endl; */
	  accumulatedADCFractions.push_back(currentADCFraction);
	  DEBinsNeeded.push_back(DEBinsCount);
	  DEBinsCount = 0;
	  currentADCFraction = 0.0;
	  
	  if (DEBinsNeeded.size() == adc2DVector.size()) 
	    {
	      break; // All ADC bins processed
	    }
	  currentADCThreshold = adcBinFractions[DEBinsNeeded.size()]; //here 
	  //currentADCThreshold = adcBinContents[DEBinsNeeded.size()]; 
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
  /*
  // Iterate through the 2D vector and print each element                                          
  std::cout << "combined2DVector: ";
  for (int i = 0; i < combined2DVector.size(); ++i)
    {
      std::cout << "(" << combined2DVector[i].first << ", " << combined2DVector[i].second << ") ";
    }
  std::cout << std::endl;
  */



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
  int numXBins = mappedHistogram->GetNbinsX();

  // Create a 2D vector to store (bin number, X-axis value)
  std::vector<std::pair<int, double>> binNumberToDEValue;

  for (int i = 1; i <= numXBins; ++i) {
    double xValue = mappedHistogram->GetXaxis()->GetBinCenter(i);
    binNumberToDEValue.push_back(std::make_pair(i, xValue));
  }

  // Print the bin number and X-axis value mapping
  for (const auto& pair : binNumberToDEValue) {
    int binNumber = pair.first;
    double xValue = pair.second;
    std::cout << "Bin Number: " << binNumber << ", X-Axis Value: " << xValue << std::endl;
  }

  int numBins = barGraph->GetN();

  // Create a 2D vector to store (bin number, ADC value)
  std::vector<std::pair<int, double>> binNumberToXValue;

  for (int i = 0; i < numBins; ++i)
    {
      double xValue, yValue ;
      barGraph->GetPoint(i, xValue, yValue);
      binNumberToXValue.push_back(std::make_pair(i + 1, xValue));
    }

  // Print the bin number and x-axis value mapping
  for (const auto& pair : binNumberToXValue) {
    std::cout << "Bin Number: " << pair.first << ", X-Axis Value: " << pair.second << std::endl;
  }

  std::vector<std::pair<double, double>> combinedVector;

  for (size_t i = 0; i < binNumberToDEValue.size() && i < binNumberToXValue.size(); ++i) {
    double firstValue = binNumberToDEValue[i].second;
    double secondValue = binNumberToXValue[i].second;
    combinedVector.push_back(std::make_pair(firstValue, secondValue));
  }

  // Print the combined vector
  for (const auto& pair : combinedVector) {
    std::cout << "First Value: " << pair.first << ", Second Value: " << pair.second << std::endl;
  }

  int decimalPlaces = 5; 
  int Bins = combinedVector.size();
  
  TH1D* DEvsADC = new TH1D("DEvsADC", "DE Values vs ADC Values", Bins, 0, Bins);

  // Fill the histogram with data from combinedVector
  for (int i = 0; i < Bins; ++i) {
    DEvsADC->SetBinContent(i + 1, combinedVector[i].second); // Set y-values
    
    if (i % 10 == 0)
      {
      DEvsADC->GetXaxis()->SetBinLabel(i + 1, Form("%g", combinedVector[i].first));
      }
  }
   
  // Create a canvas to visualize the histogram
  TCanvas* canvas = new TCanvas("myCanvas", "My Canvas", 1000, 500);
  DEvsADC->SetMarkerStyle(20);
  DEvsADC->SetMarkerSize(.5);
  DEvsADC->Draw("P");
  */ 
 
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

  TCanvas *c6 = new TCanvas("c6", "c6", 1000, 500);
  mappedHistogram->SetFillColor(0);
  mappedHistogram->SetLineColor(kBlue);
  mappedHistogram->Draw("HIST");
  //  mappedHistogram->GetYaxis()->SetRangeUser(0, .2);  

  TCanvas* c7 = new TCanvas("c7", "c7", 1000, 500);
  barGraph->SetTitle("ADC Value vs. DE Fraction");
  barGraph->SetMarkerStyle(21);
  barGraph->SetMarkerSize(1.0);
  barGraph->SetFillColor(kRed);
  barGraph->Draw("AB1");
  barGraph->GetXaxis()->SetTitle("ADC Value");
  barGraph->GetYaxis()->SetTitle("Fraction");
  // barGraph->GetYaxis()->SetRangeUser(0, .0002);
  //c7->SaveAs("barGraph.root");

  TCanvas *c8 = new TCanvas("c8", "c8", 1000, 500);
  barGraph->Draw("AB1");
  barGraph->GetXaxis()->SetRangeUser(xMin, xMax);
  ntotadc->SetMarkerStyle(2);
  ntotadc->SetMarkerSize(0.5);
  ntotadc->Draw("P SAME");
  ntotadc->GetXaxis()->SetRangeUser(xMin, xMax);
  //  ntotadc->GetYaxis()->SetRangeUser(0, .0002);
  /*
  // Create separate output files for each histogram 
  TFile getdeFile("getde.root", "RECREATE");
  getde->Write();
  getdeFile.Close();

  TFile totADCFile("totADC.root", "RECREATE");
  totADC->Write();
  totADCFile.Close();

  TFile ngetdeFile("ngetde.root", "RECREATE");
  ngetde->Write();
  ngetdeFile.Close();

  TFile ntotADCFile("ntotADC.root", "RECREATE");
  ntotADC->Write();
  ntotADCFile.Close();

  TFile ntotadcFile("ntotadc.root", "RECREATE");
  ntotadc->Write();
  ntotadcFile.Close();
  */
  TFile mappedHistogramFile(" mappedHistogram.root", "RECREATE");
   mappedHistogram->Write();
   mappedHistogramFile.Close();

  TFile barGraphFile("barGraph.root", "RECREATE");
  barGraph->Write();
  barGraphFile.Close();
  /*
  TFile ADCValueVSDEFractionFile("ADCValueVSDEFraction.root", "RECREATE");
  c8->Write();
  ADCValueVSDEFractionFile.Close();
  */
}
