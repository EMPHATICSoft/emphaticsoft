/////////////////////////////////////////////////////////////////////////////
// Description: This macro  identifies good events (events with exactly    //
// one cluster per plane) for a chain of data files, identifies good       //
// events (events with exactly one truehit per plane) for a chain of MC    //
// files with the goal of mapping charge deposition (GetDE) into avgADC    //
// and eventually width as well. Using phase 1c geometry.                  //
//                                                                         //
//Date: November 09, 2023                                                  //
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

void ProcessADC(TH1* adcHist, TH1* nADCHist)
{
  int totADCclusts = 0;

  for (int ADCbin = 1; ADCbin <= adcHist->GetNbinsX(); ++ADCbin)
    {
      totADCclusts += adcHist->GetBinContent(ADCbin);
    }

  // Create a vector to store ADC bin contents fractions and values
  std::vector<double> ADCBinContents;
  std::vector<double> ADCBinFractions;
  std::vector<int> ADCBinValues;
  std::vector<std::pair<int, double>> ADC2DVector;

  // Print the number of clusters in each bin of the ADC histogram
  for (int ADCbin = 1; ADCbin <= adcHist->GetNbinsX(); ++ADCbin)
    {
      double ADCBinContent = adcHist->GetBinContent(ADCbin);
      ADCBinContents.push_back(ADCBinContent);

      // Calculate the fraction and store it in ADCBinFractions
      long double ADCFraction = static_cast<double>(ADCBinContent) / totADCclusts;
      ADCBinFractions.push_back(ADCFraction);

      int ADCBinValue = adcHist->GetXaxis()->GetBinCenter(ADCbin);
      ADCBinValues.push_back(ADCBinValue);

      // Create a pair and store it in the 2D vector
      ADC2DVector.push_back(std::make_pair(ADCBinValue, ADCFraction));
    }

  for (int i = 0; i < ADC2DVector.size(); ++i)
    {
      if (nADCHist)
	{
	  // Check if i is a valid index for ADCBinFractions
	  if (i >= 0 && i < ADC2DVector.size())
	    {
	      nADCHist->SetBinContent(i + 1, ADC2DVector[i].second);
	    }
	  else
	    {
	      std::cerr << "Error: Invalid index i for ADCBinFractions." << std::endl;
	    }
	}
      else
	{
	  std::cerr << "Error: nADCHist is not a valid pointer." << std::endl;
	}
    }
}

// Function to process an ADC histogram and update the corresponding nADC histogram
void ProcessADCAndNADC(TH1* ntotADC, TH1* ntotadc)
{
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
	  int adcBinValue = ntotADC->GetXaxis()->GetBinCenter(nADCbin);
	  adcBinValues.push_back(adcBinValue);

	  adc2DVector.push_back(std::make_pair(adcBinValue, nADCFraction));
	  ntotadcIndex++;
	}
    }

  // Iterate through the 2D vector and update ntotadc
  for (int i = 0; i < adc2DVector.size(); ++i)
    {
      ntotadc->SetBinContent(ntotadc->FindBin(adc2DVector[i].first), adc2DVector[i].second);
    }



}

// Function to process a DE histogram and update the corresponding nDE histogram
void ProcessDEAndNDE(TH1* getde, TH1* ngetde)
{

  if (!getde || !ngetde) {
    std::cerr << "Error: Invalid input histograms." << std::endl;
    return;
  }

  int totDEclusts = 0;

  for (int DEbin = 1; DEbin <= getde->GetNbinsX(); ++DEbin) {
    int DEBinContent = getde->GetBinContent(DEbin);
    totDEclusts += DEBinContent; // Accumulate the total

    if (std::isnan(DEBinContent) || std::isinf(DEBinContent)) {
      std::cerr << "Error: Input histogram contains NaN or Inf values." << std::endl;
      return;
    }
  }

  if (totDEclusts == 0) {
    std::cerr << "Error: Total number of clusters is zero." << std::endl;
    return;
  }

  std::vector<int> DEBinContents;
  std::vector<double> DEBinFractions;
  std::vector<double> DEBinValues;
  std::vector<std::pair<double, double>> DE2DVector;

  for (int DEbin = 1; DEbin <= getde->GetNbinsX(); ++DEbin)
    {
      int DEBinContent = getde->GetBinContent(DEbin);
      DEBinContents.push_back(DEBinContent);

      long double DEFraction = static_cast<double>(DEBinContent) / totDEclusts;
      DEBinFractions.push_back(static_cast<double>(DEFraction));

      double DEBinValue = getde->GetXaxis()->GetBinCenter(DEbin);
      DEBinValues.push_back(DEBinValue);

      DE2DVector.push_back(std::make_pair(DEBinValue, DEFraction));
    }

  for (int i = 0; i < DE2DVector.size(); ++i)
    {
      if (ngetde)
	{
	  ngetde->SetBinContent(i + 1, DE2DVector[i].second);
	}
      else
	{
	  std::cerr << "Error: ngetde is not a valid pointer." << std::endl;
	}
    }
}

void hist()
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

  TH1F *st0s0 = new TH1F ("totADC", "station 0, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st0s1 = new TH1F ("st0s1", "station 0, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st1s0 = new TH1F ("st1s0", "station 1, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st1s1 = new TH1F ("st1s1", "station 1, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st2s0 = new TH1F ("st2s0", "station 2, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st2s1 = new TH1F ("st2s1", "station 2, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st2s2 = new TH1F ("st2s2", "station 2, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st3s0 = new TH1F ("st3s0", "station 3, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st3s1 = new TH1F ("st3s1", "station 3, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st3s2 = new TH1F ("st3s2", "station 3, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st4s0 = new TH1F ("st4s0", "station 4, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st4s1 = new TH1F ("st4s1", "station 4, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st5s0 = new TH1F ("st5s0", "station 5, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st5s1 = new TH1F ("st5s1", "station 5, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st5s2 = new TH1F ("st5s2", "station 5, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st5s3 = new TH1F ("st5s3", "station 5, sensor 3; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st5s4 = new TH1F ("st5s4", "station 5, sensor 4; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st5s5 = new TH1F ("st5s5", "station 5, sensor 5; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st6s0 = new TH1F ("st6s0", "station 6, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st6s1 = new TH1F ("st6s1", "station 6, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st6s2 = new TH1F ("st6s2", "station 6, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st6s3 = new TH1F ("st6s3", "station 6, sensor 3; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st6s4 = new TH1F ("st6s4", "station 6, sensor 4; totADC; Number of Clusters", 809, 41, 850);
  TH1F *st6s5 = new TH1F ("st6s5", "station 6, sensor 5; totADC; Number of Clusters", 809, 41, 850);

  TH1F *getdest0s0 = new TH1F ("getde", "station 0, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest0s1 = new TH1F ("getdest0s1", "station 0, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest1s0 = new TH1F ("getdest1s0", "station 1, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001); 
  TH1F *getdest1s1 = new TH1F ("getdest1s1", "station 1, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest2s0 = new TH1F ("getdest2s0", "station 2, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest2s1 = new TH1F ("getdest2s1", "station 2, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest2s2 = new TH1F ("getdest2s2", "station 2, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest3s0 = new TH1F ("getdest3s0", "station 3, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest3s1 = new TH1F ("getdest3s1", "station 3, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest3s2 = new TH1F ("getdest3s2", "station 3, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest4s0 = new TH1F ("getdest4s0", "station 4, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest4s1 = new TH1F ("getdest4s1", "station 4, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001); 
  TH1F *getdest5s0 = new TH1F ("getdest5s0", "station 5, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest5s1 = new TH1F ("getdest5s1", "station 5, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest5s2 = new TH1F ("getdest5s2", "station 5, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest5s3 = new TH1F ("getdest5s3", "station 5, sensor 3; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest5s4 = new TH1F ("getdest5s4", "station 5, sensor 4; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest5s5 = new TH1F ("getdest5s5", "station 5, sensor 5; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest6s0 = new TH1F ("getdest6s0", "station 6, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest6s1 = new TH1F ("getdest6s1", "station 6, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest6s2 = new TH1F ("getdest6s2", "station 6, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest6s3 = new TH1F ("getdest6s3", "station 6, sensor 3; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest6s4 = new TH1F ("getdest6s4", "station 6, sensor 4; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *getdest6s5 = new TH1F ("getdest6s5", "station 6, sensor 5; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);

  TH1F *nst0s0 = new TH1F ("nst0s0", "station 0, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst0s1 = new TH1F ("nst0s1", "station 0, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst1s0 = new TH1F ("nst1s0", "station 1, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst1s1 = new TH1F ("nst1s1", "station 1, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst2s0 = new TH1F ("nst2s0", "station 2, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst2s1 = new TH1F ("nst2s1", "station 2, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst2s2 = new TH1F ("nst2s2", "station 2, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst3s0 = new TH1F ("nst3s0", "station 3, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst3s1 = new TH1F ("nst3s1", "station 3, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst3s2 = new TH1F ("nst3s2", "station 3, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst4s0 = new TH1F ("nst4s0", "station 4, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst4s1 = new TH1F ("nst4s1", "station 4, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst5s0 = new TH1F ("nst5s0", "station 5, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst5s1 = new TH1F ("nst5s1", "station 5, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst5s2 = new TH1F ("nst5s2", "station 5, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst5s3 = new TH1F ("nst5s3", "station 5, sensor 3; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst5s4 = new TH1F ("nst5s4", "station 5, sensor 4; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst5s5 = new TH1F ("nst5s5", "station 5, sensor 5; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst6s0 = new TH1F ("nst6s0", "station 6, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst6s1 = new TH1F ("nst6s1", "station 6, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst6s2 = new TH1F ("nst6s2", "station 6, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst6s3 = new TH1F ("nst6s3", "station 6, sensor 3; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst6s4 = new TH1F ("nst6s4", "station 6, sensor 4; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *nst6s5 = new TH1F ("nst6s5", "station 6, sensor 5; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  TH1F *Nst0s0 = new TH1F ("Nst0s0", "station 0, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst0s1 = new TH1F ("Nst0s1", "station 0, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst1s0 = new TH1F ("Nst1s0", "station 1, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst1s1 = new TH1F ("Nst1s1", "station 1, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst2s0 = new TH1F ("Nst2s0", "station 2, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst2s1 = new TH1F ("Nst2s1", "station 2, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst2s2 = new TH1F ("Nst2s2", "station 2, sensor 2; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst3s0 = new TH1F ("Nst3s0", "station 3, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst3s1 = new TH1F ("Nst3s1", "station 3, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst3s2 = new TH1F ("Nst3s2", "station 3, sensor 2; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst4s0 = new TH1F ("Nst4s0", "station 4, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst4s1 = new TH1F ("Nst4s1", "station 4, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s0 = new TH1F ("Nst5s0", "station 5, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s1 = new TH1F ("Nst5s1", "station 5, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s2 = new TH1F ("Nst5s2", "station 5, sensor 2; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s3 = new TH1F ("Nst5s3", "station 5, sensor 3; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s4 = new TH1F ("Nst5s4", "station 5, sensor 4; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s5 = new TH1F ("Nst5s5", "station 5, sensor 5; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s0 = new TH1F ("Nst6s0", "station 6, sensor 0; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s1 = new TH1F ("Nst6s1", "station 6, sensor 1; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s2 = new TH1F ("Nst6s2", "station 6, sensor 2; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s3 = new TH1F ("Nst6s3", "station 6, sensor 3; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s4 = new TH1F ("Nst6s4", "station 6, sensor 4; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s5 = new TH1F ("Nst6s5", "station 6, sensor 5; ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  TH1F *ngetdest0s0 = new TH1F ("ngetdest0s0", "station 0, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest0s1 = new TH1F ("ngetdest0s1", "station 0, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest1s0 = new TH1F ("ngetdest1s0", "station 1, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest1s1 = new TH1F ("ngetdest1s1", "station 1, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest2s0 = new TH1F ("ngetdest2s0", "station 2, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest2s1 = new TH1F ("ngetdest2s1", "station 2, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest2s2 = new TH1F ("ngetdest2s2", "station 2, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest3s0 = new TH1F ("ngetdest3s0", "station 3, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest3s1 = new TH1F ("ngetdest3s1", "station 3, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest3s2 = new TH1F ("ngetdest3s2", "station 3, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest4s0 = new TH1F ("ngetdest4s0", "station 4, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest4s1 = new TH1F ("ngetdest4s1", "station 4, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest5s0 = new TH1F ("ngetdest5s0", "station 5, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest5s1 = new TH1F ("ngetdest5s1", "station 5, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest5s2 = new TH1F ("ngetdest5s2", "station 5, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest5s3 = new TH1F ("ngetdest5s3", "station 5, sensor 3; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest5s4 = new TH1F ("ngetdest5s4", "station 5, sensor 4; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest5s5 = new TH1F ("ngetdest5s5", "station 5, sensor 5; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest6s0 = new TH1F ("ngetdest6s0", "station 6, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest6s1 = new TH1F ("ngetdest6s1", "station 6, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest6s2 = new TH1F ("ngetdest6s2", "station 6, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest6s3 = new TH1F ("ngetdest6s3", "station 6, sensor 3; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest6s4 = new TH1F ("ngetdest6s4", "station 6, sensor 4; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *ngetdest6s5 = new TH1F ("ngetdest6s5", "station 6, sensor 5; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);

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
	      int sensor = recData->cluster.clust[idx].sens;
              int station = recData->cluster.clust[idx].station;

              double AvgADC = recData->cluster.clust[idx].avgadc;
	      TotADC = ndigits*AvgADC;
	      totADC->Fill(TotADC);
	      
	      if (station == 0 && sensor == 0)
		{
		  // plane 0, only has one sensor    
		  st0s0->Fill(TotADC);
		}
              if (station == 0 && sensor == 1)
                {
                  //plane 1
                  st0s1->Fill(TotADC);
                }
              if (station == 1 && sensor == 0)
                {
                  //plane 2                                                  
                  st1s0->Fill(TotADC);
                }
	      if (sensor == 3)
                {
                  //plane 3                
                  st1s1->Fill(TotADC);
                }
              if (station == 2 && sensor == 0)
                {
                  //plane 4
                  st2s0->Fill(TotADC);
                }
              if (station == 2 && sensor == 1)
                {
                  //plane 5
                  st2s1->Fill(TotADC);
                }
              if (station == 2 && sensor == 2)
                {
                  //plane 6
                  st2s2->Fill(TotADC);
                }
              if (station == 3 && sensor == 0)
                {
                  //plane 7
                  st3s0->Fill(TotADC);
                }
              if (station == 3 && sensor == 1)
                {
                  //plane 8
                  st3s1->Fill(TotADC);
                }
              if (station == 3 && sensor == 2)
                {
                  //plane 9
                  st3s2->Fill(TotADC);
                }
	      if (station == 4 && sensor == 0)
                {
                  //plane 10
                  st4s0->Fill(TotADC);
                }
              if (station == 4 && sensor == 1)
                {
                  //plane 11
                  st4s1->Fill(TotADC);
                }
              if (station == 5 && sensor == 0)
                {
                  //plane 12
                  st5s0->Fill(TotADC);
                }
              if (station == 5 && sensor == 1)
                {
                  //plane 12
                  st5s1->Fill(TotADC);
                }
              if (station == 5 && sensor == 2)
                {
                  //plane 13           
                  st5s2->Fill(TotADC);
                }
              if (station == 5 && sensor == 3)
                {
                  //plane 13
                  st5s3->Fill(TotADC);
                }
              if (station == 5 && sensor == 4)
                {
                  //plane 14
                  st5s4->Fill(TotADC);
                }
	      if (station == 5 && sensor == 5)
                {
                  //plane 14 
                  st5s5->Fill(TotADC);
                }
	      if (station == 6 && sensor == 0)
                {
                  //plane 15
                  st6s0->Fill(TotADC);
                }
              if (station == 6 && sensor == 1)
                {
                  //plane 15
                  st6s1->Fill(TotADC);
                }
              if (station == 6 && sensor == 2)
                {
                  //plane 16
                  st6s2->Fill(TotADC);
                }
              if (station == 6 && sensor == 3)
                {
                  //plane 16
                  st6s3->Fill(TotADC);
                }
              if (station == 6 && sensor == 4)
                {
                  //plane 17                                                                                               
                  st6s4->Fill(TotADC);
                }
	      if (station == 6 && sensor == 5)
                {
                  //plane 17           
                  st6s5->Fill(TotADC);
                }
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

	      int sensor = recMC->truth.truehits.truehits[idx].GetSensor;
              int station = recMC->truth.truehits.truehits[idx].GetStation;

	      if (sensor == 0)
                {
                  // plane 0, only has one sensor
                  getdest0s0->Fill(GetDE);
                }
              if (sensor == 1)
                {
                  //plane 1      
                  getdest0s1->Fill(GetDE);
                }
              if (sensor == 2)
                {
                  //plane 2               
                  getdest1s0->Fill(GetDE);
                }
              if (sensor == 3)
                {
                  //plane 3
                  getdest1s1->Fill(GetDE);
                }
	      if (sensor == 4)
                {
                  //plane 4
                   getdest2s0->Fill(GetDE);
                }
              if (sensor == 5)
                {
                  //plane 5
                   getdest2s1->Fill(GetDE);
                }
              if (sensor == 6)
                {
                  //plane 6
                   getdest2s2->Fill(GetDE);
                }
              if (sensor == 7)
                {
                  //plane 7
                   getdest3s0->Fill(GetDE);
                }
              if (sensor == 8)
                {
                  //plane 8
                   getdest3s1->Fill(GetDE);
                }
              if (sensor == 9)
                {
                  //plane 9
                   getdest3s2->Fill(GetDE);
                }
              if (sensor == 10)
                {
                  //plane 10
                   getdest4s0->Fill(GetDE);
                }
              if (sensor == 11)
                {
                  //plane 11
                   getdest4s1->Fill(GetDE);
                }
	      if (sensor == 12)
                {
                  //plane 12
                   getdest5s0->Fill(GetDE);
                }
              if (sensor == 13)
                {
                  //plane 12
                   getdest5s1->Fill(GetDE);
                }
              if (sensor == 14)
                {
                  //plane 13
                   getdest5s2->Fill(GetDE);
                }
              if (sensor == 15)
                {
                  //plane 13
                   getdest5s3->Fill(GetDE);
                }
              if (sensor == 16)
                {
                  //plane 14
                   getdest5s4->Fill(GetDE);
                }
	      if (sensor == 17)
                {
                  //plane 14
		  getdest5s5->Fill(GetDE);
                }
              if (sensor == 18)
                {
                  //plane 15
                   getdest6s0->Fill(GetDE);
                }
              if (sensor == 19)
                {
                  //plane 15
                   getdest6s1->Fill(GetDE);
                }
	      if (sensor == 20)
                {
                  //plane 16
                   getdest6s2->Fill(GetDE);
                }
              if (sensor == 21)
                {
                  //plane 16
                   getdest6s3->Fill(GetDE);
                }
              if (sensor ==22)
                {
                  //plane 17
                   getdest6s4->Fill(GetDE);
                }
              if (sensor == 23)
                {
                  //plane 17
                   getdest6s5->Fill(GetDE);
                }
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

  ProcessADC(st0s0, nst0s0);
  ProcessADC(st0s1, nst0s1);
  ProcessADC(st1s0, nst1s0);
  ProcessADC(st1s1, nst1s1);
  ProcessADC(st2s0, nst2s0);
  ProcessADC(st2s1, nst2s1);
  ProcessADC(st2s2, nst2s2);
  ProcessADC(st3s0, nst3s0);
  ProcessADC(st3s1, nst3s1);
  ProcessADC(st3s2, nst3s2);
  ProcessADC(st4s0, nst4s0);
  ProcessADC(st4s1, nst4s1);
  ProcessADC(st5s0, nst5s0);
  ProcessADC(st5s1, nst5s1);
  ProcessADC(st5s2, nst5s2);
  ProcessADC(st5s3, nst5s3);
  ProcessADC(st5s4, nst5s4);
  ProcessADC(st5s5, nst5s5);
  ProcessADC(st6s0, nst6s0);
  ProcessADC(st6s1, nst6s1);
  ProcessADC(st6s2, nst6s2);
  ProcessADC(st6s3, nst6s3);
  ProcessADC(st6s4, nst6s4);
  ProcessADC(st6s5, nst6s5);

  // Create vectors to store the content and fractions
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

  //  ProcessADCAndNADC(ntotADC, ntotadc);
  ProcessADCAndNADC(nst0s0, Nst0s0);
  ProcessADCAndNADC(nst0s1, Nst0s1);
  ProcessADCAndNADC(nst1s0, Nst1s0);
  ProcessADCAndNADC(nst1s1, Nst1s1);
  ProcessADCAndNADC(nst2s0, Nst2s0);
  ProcessADCAndNADC(nst2s1, Nst2s1);
  ProcessADCAndNADC(nst2s2, Nst2s2);
  ProcessADCAndNADC(nst3s0, Nst3s0);
  ProcessADCAndNADC(nst3s1, Nst3s1);
  ProcessADCAndNADC(nst3s2, Nst3s2);
  ProcessADCAndNADC(nst4s0, Nst4s0);
  ProcessADCAndNADC(nst4s1, Nst4s1);
  ProcessADCAndNADC(nst5s0, Nst5s0);
  ProcessADCAndNADC(nst5s1, Nst5s1);
  ProcessADCAndNADC(nst5s2, Nst5s2);
  ProcessADCAndNADC(nst5s3, Nst5s3);
  ProcessADCAndNADC(nst5s4, Nst5s4);
  ProcessADCAndNADC(nst5s5, Nst5s5);
  ProcessADCAndNADC(nst6s0, Nst6s0);
  ProcessADCAndNADC(nst6s1, Nst6s1);
  ProcessADCAndNADC(nst6s2, Nst6s2);
  ProcessADCAndNADC(nst6s3, Nst6s3);
  ProcessADCAndNADC(nst6s4, Nst6s4);
  ProcessADCAndNADC(nst6s5, Nst6s5);

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

  ProcessDEAndNDE(getde, ngetde);
  ProcessDEAndNDE(getdest0s0, ngetdest0s0);
  ProcessDEAndNDE(getdest0s1, ngetdest0s1);
  ProcessDEAndNDE(getdest1s0, ngetdest1s0);
  ProcessDEAndNDE(getdest1s1, ngetdest1s1);
  ProcessDEAndNDE(getdest2s0, ngetdest2s0);
  ProcessDEAndNDE(getdest2s1, ngetdest2s1);
  ProcessDEAndNDE(getdest2s2, ngetdest2s2);
  ProcessDEAndNDE(getdest3s0, ngetdest3s0);
  ProcessDEAndNDE(getdest3s1, ngetdest3s1);
  ProcessDEAndNDE(getdest3s2, ngetdest3s0);
  ProcessDEAndNDE(getdest4s0, ngetdest4s0);
  ProcessDEAndNDE(getdest4s1, ngetdest4s1);
  ProcessDEAndNDE(getdest5s0, ngetdest5s0);
  ProcessDEAndNDE(getdest5s1, ngetdest5s1);
  ProcessDEAndNDE(getdest5s2, ngetdest5s2);
  ProcessDEAndNDE(getdest5s3, ngetdest5s3);
  ProcessDEAndNDE(getdest5s4, ngetdest5s4);
  ProcessDEAndNDE(getdest5s5, ngetdest5s5);
  ProcessDEAndNDE(getdest6s0, ngetdest6s0);
  ProcessDEAndNDE(getdest6s1, ngetdest6s1);
  ProcessDEAndNDE(getdest6s2, ngetdest6s2);
  ProcessDEAndNDE(getdest6s3, ngetdest6s3);
  ProcessDEAndNDE(getdest6s4, ngetdest6s4);
  ProcessDEAndNDE(getdest6s5, ngetdest6s5);
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
 

  // Create separate output files for needed histogram
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
 
  //totADC histograms
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);
  c3->Divide(3,3);

  c3->cd(1);
  st0s0->SetFillColor(kBlue);
  st0s0->Draw("colz");
  /*
  c3->cd(2);
  st0s1->SetFillColor(kBlue);
  st0s1->Draw("colz");

  c3->cd(3);
  st1s0->SetFillColor(kBlue);
  st1s0->Draw("colz");

  c3->cd(4);
  st1s1->SetFillColor(kBlue);
  st1s1->Draw("colz");
  
  c3->cd(5);
  st2s0->SetFillColor(kBlue);
  st2s0->Draw("colz");
  
  c3->cd(6);
  st2s1->SetFillColor(kBlue);
  st2s1->Draw("colz");
  
  c3->cd(7);
  st2s2->SetFillColor(kBlue);
  st2s2->Draw("colz");
  
  c3->cd(8);
  st3s0->SetFillColor(kBlue);
  st3s0->Draw("colz");

  c3->cd(9);
  st3s1->SetFillColor(kBlue);
  st3s1->Draw("colz");
  
  TCanvas *c4 = new TCanvas("c4","c4",2000,1500);
  c4->Divide(3,3);
  
  c4->cd(1);
 st3s2->SetFillColor(kBlue);
 st3s2->Draw("colz");
 
 c4->cd(2);
 st4s0->SetFillColor(kBlue);
 st4s0->Draw("colz");
 
 c4->cd(3);
 st4s1->SetFillColor(kBlue);
 st4s1->Draw("colz");
 
 c4->cd(4);
 st5s0->SetFillColor(kBlue);
 st5s0->Draw("colz");
 
 c4->cd(5);
 st5s1->SetFillColor(kBlue);
 st5s1->Draw("colz");
 
 c4->cd(6);
 st5s2->SetFillColor(kBlue);
 st5s2->Draw("colz");
 
 c4->cd(7);
st5s3->SetFillColor(kBlue);
 st5s3->Draw("colz");
 
 c4->cd(8);
 st5s4->SetFillColor(kBlue);
 st5s4->Draw("colz");
 
 c4->cd(9);
st5s5->SetFillColor(kBlue);
 st5s5->Draw("colz");
 
 TCanvas *c5 = new TCanvas("c5","c5",2000,1500);
 c5->Divide(3,3);
 
 c5->cd(1);
 st6s0->SetFillColor(kBlue);
 st6s0->Draw("colz");
 
 c5->cd(2);
 st6s1->SetFillColor(kBlue);
 st6s1->Draw("colz");

 c5->cd(3);
 st6s2->SetFillColor(kBlue);
 st6s2->Draw("colz");
 
 c5->cd(4);
 st6s3->SetFillColor(kBlue);
 st6s3->Draw("colz");
 
 c5->cd(5);
 st6s4->SetFillColor(kBlue);
 st6s4->Draw("colz");
 
 c5->cd(6);
 st6s5->SetFillColor(kBlue);
 st6s5->Draw("colz");
 */
  
 // Create separate output files for needed histograms
 TFile st0s0File("st0s0.root", "RECREATE");  
 st0s0->Write();
 st0s0File.Close();
 /*
 TFile st0s1File("st0s1.root", "RECREATE");
 st0s1->Write();
 st0s1File.Close();

 TFile st1s0File("st1s0.root", "RECREATE");
 st1s0->Write();
 st1s0File.Close();

 TFile st1s1File("st1s1.root", "RECREATE");
 st1s1->Write();
 st1s1File.Close();

 TFile st2s0File("st2s0.root", "RECREATE");
 st2s0->Write();
 st2s0File.Close();

 TFile st2s1File("st2s1.root", "RECREATE");
 st2s1->Write();
 st2s1File.Close();

 TFile st2s2File("st2s2.root", "RECREATE");
 st2s2->Write();
 st2s2File.Close();

 TFile st3s0File("st3s0.root", "RECREATE");
 st3s0->Write();
 st3s0File.Close();

 TFile st3s1File("st3s1.root", "RECREATE");
 st3s1->Write();
 st3s1File.Close();

 TFile st3s2File("st3s2.root", "RECREATE");
 st3s2->Write();
 st3s2File.Close();

 TFile st4s0File("st4s0.root", "RECREATE");
 st4s0->Write();
 st4s0File.Close();

 TFile st4s1File("st4s1.root", "RECREATE");
 st4s1->Write();
 st4s1File.Close();

 TFile st5s0File("st5s0.root", "RECREATE");
 st5s0->Write();
 st5s0File.Close();

 TFile st5s1File("st5s1.root", "RECREATE");
 st5s1->Write();
 st5s1File.Close();

 TFile st5s2File("st5s2.root", "RECREATE");
 st5s2->Write();
 st5s2File.Close();

 TFile st5s3File("st5s3.root", "RECREATE");
 st5s3->Write();
 st5s3File.Close();

 TFile st5s4File("st5s4.root", "RECREATE");
 st5s4->Write();
 st5s4File.Close();

 TFile st5s5File("st5s5.root", "RECREATE");
 st5s5->Write();
 st5s5File.Close();

 TFile st6s0File("st6s0.root", "RECREATE");
 st6s0->Write();
 st6s0File.Close();

 TFile st6s1File("st6s1.root", "RECREATE");
 st6s1->Write();
 st6s1File.Close();

 TFile st6s2File("st6s2.root", "RECREATE");
 st6s2->Write();
 st6s2File.Close();

 TFile st6s3File("st6s3.root", "RECREATE");
 st6s3->Write();
 st6s3File.Close();

 TFile st6s4File("st6s4.root", "RECREATE");
 st6s4->Write();
 st6s4File.Close();

 TFile st6s5File("st6s5.root", "RECREATE");
 st6s5->Write();
 st6s5File.Close();
  */

  /*
  //getde histograms
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);
  c3->Divide(3,3);
  c3->cd(1);
  getdest0s0->SetFillColor(kBlue);
  getdest0s0->Draw("colz");
                  
  c3->cd(2); 
  getdest0s1->SetFillColor(kBlue);
  getdest0s1->Draw("colz");
                       
  c3->cd(3);        
  getdest1s0->SetFillColor(kBlue);
  getdest1s0->Draw("colz");

  c3->cd(4);
  getdest1s1->SetFillColor(kBlue);
  getdest1s1->Draw("colz");            
  
  c3->cd(5);
  getdest2s0->SetFillColor(kBlue);
  getdest2s0->Draw("colz");
                       
  c3->cd(6);             
  getdest2s1->SetFillColor(kBlue); 
  getdest2s1->Draw("colz");

  c3->cd(7);
  getdest2s2->SetFillColor(kBlue);
  getdest2s2->Draw("colz");                           

  c3->cd(8);                                                                       
  getdest3s0->SetFillColor(kBlue);
  getdest3s0->Draw("colz");

  c3->cd(9); 
  getdest3s1->SetFillColor(kBlue);
  getdest3s1->Draw("colz");

  TCanvas *c4 = new TCanvas("c4","c4",2000,1500); 
  c4->Divide(3,3);
   
  c4->cd(1);
  getdest3s2->SetFillColor(kBlue);
  getdest3s2->Draw("colz");
           
  c4->cd(2);           
  getdest4s0->SetFillColor(kBlue);
  getdest4s0->Draw("colz");            
                                                        
  c4->cd(3);
  getdest4s1->SetFillColor(kBlue);
  getdest4s1->Draw("colz");

  c4->cd(4);                                          
  getdest5s0->SetFillColor(kBlue);                       
  getdest5s0->Draw("colz");
                                                    
  c4->cd(5);                      
  getdest5s1->SetFillColor(kBlue);
  getdest5s1->Draw("colz");
              
  c4->cd(6);                      
  getdest5s2->SetFillColor(kBlue);
  getdest5s2->Draw("colz");
                        
  c4->cd(7);         
  getdest5s3->SetFillColor(kBlue);            
  getdest5s3->Draw("colz");

  c4->cd(8);                    
  getdest5s4->SetFillColor(kBlue);          
  getdest5s4->Draw("colz");
       
  c4->cd(9);                  
  getdest5s5->SetFillColor(kBlue);      
  getdest5s5->Draw("colz");
                     
  TCanvas *c5 = new TCanvas("c5","c5",2000,1500);                                  
  c5->Divide(3,3);
                                                               
  c5->cd(1);                                                      
  getdest6s0->SetFillColor(kBlue);              
  getdest6s0->Draw("colz");

  c5->cd(2);              
  getdest6s1->SetFillColor(kBlue);
  getdest6s1->Draw("colz");
  
  c5->cd(3);
  getdest6s2->SetFillColor(kBlue);
  getdest6s2->Draw("colz");
           
  c5->cd(4);  
  getdest6s3->SetFillColor(kBlue);
  getdest6s3->Draw("colz");
                       
  c5->cd(5);  
  getdest6s4->SetFillColor(kBlue);
  getdest6s4->Draw("colz");                           

  c5->cd(6);
  getdest6s5->SetFillColor(kBlue);
  getdest6s5->Draw("colz");
  
  // Create separate output files for needed histograms 
  TFile getdest0s0File("getdest0s0.root", "RECREATE");           
  getdest0s0->Write();                                                          
  getdest0s0File.Close();

  TFile getdest0s1File("getdest0s1.root", "RECREATE");
  getdest0s1->Write();                                                          
  getdest0s1File.Close();

  TFile getdest1s0File("getdest1s0.root", "RECREATE");
  getdest1s0->Write();                                                         
  getdest1s0File.Close();

  TFile getdest1s1File("getdest1s1.root", "RECREATE");
  getdest1s1->Write();                                                        
  getdest1s1File.Close();

  TFile getdest2s0File("getdest2s0.root", "RECREATE");
  getdest2s0->Write();                        
  getdest2s0File.Close();
                                                            
  TFile getdest2s1File("getdest2s1.root", "RECREATE");
  getdest2s1->Write();  
  getdest2s1File.Close();
                                                           
  TFile getdest2s2File("getdest2s2.root", "RECREATE");
  getdest2s2->Write();     
  getdest2s2File.Close();

  TFile getdest3s0File("getdest3s0.root", "RECREATE");
  getdest3s0->Write();                           
  getdest3s0File.Close();

  TFile getdest3s1File("getdest3s1.root", "RECREATE");
  getdest3s1->Write();                            
  getdest3s1File.Close();

  TFile getdest3s2File("getdest3s2.root", "RECREATE");                      
  getdest3s2->Write();
  getdest3s2File.Close();
  
  TFile getdest4s0File("getdest4s0.root", "RECREATE");
  getdest4s0->Write();                            
  getdest4s0File.Close();

  TFile getdest4s1File("getdest4s1.root", "RECREATE");
  getdest4s1->Write();                                                          
  getdest4s1File.Close();

  TFile getdest5s0File("getdest5s0.root", "RECREATE");
  getdest5s0->Write();                            
  getdest5s0File.Close();           

  TFile getdest5s1File("getdest5s1.root", "RECREATE");
  getdest5s1->Write();
  getdest5s1File.Close();

  TFile getdest5s2File("getdest5s2.root", "RECREATE");
  getdest5s2->Write();                         
  getdest5s2File.Close();

  TFile getdest5s3File("getdest5s3.root", "RECREATE");
  getdest5s3->Write();                          
  getdest5s3File.Close();
                                             
  TFile getdest5s4File("getdest5s4.root", "RECREATE");
  getdest5s4->Write();                         
  getdest5s4File.Close();
   
  TFile getdest5s5File("getdest5s5.root", "RECREATE");
  getdest5s5->Write();                       
  getdest5s5File.Close();

  TFile getdest6s0File("getdest6s0.root", "RECREATE");
  getdest6s0->Write();                           
  getdest6s0File.Close();

  TFile getdest6s1File("getdest6s1.root", "RECREATE");
  getdest6s1->Write();                 
  getdest6s1File.Close();
                                                                                    
  TFile getdest6s2File("getdest6s2.root", "RECREATE");
  getdest6s2->Write();                          
  getdest6s2File.Close();
                                                                                  
  TFile getdest6s3File("getdest6s3.root", "RECREATE");
  getdest6s3->Write();                       
  getdest6s3File.Close();
 
  TFile getdest6s4File("getdest6s4.root", "RECREATE");
  getdest6s4->Write();                          
  getdest6s4File.Close();

  TFile getdest6s5File("getdest6s5.root", "RECREATE");                          
  getdest6s5->Write();                                                     
  getdest6s5File.Close();     
  */

  /*
  //ntotADC histograms
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);                       
  c3->Divide(3,3);
                                
  c3->cd(1);                                                  
  nst0s0->SetFillColor(kBlue);                               
  nst0s0->Draw("colz");
                                  
  c3->cd(2);                                                  
  nst0s1->SetFillColor(kBlue);                                       
  nst0s1->Draw("colz");
                      
  c3->cd(3);                                                 
  nst1s0->SetFillColor(kBlue);                                       
  nst1s0->Draw("colz");
                           
  c3->cd(4);                                             
  nst1s1->SetFillColor(kBlue);                                      
  nst1s1->Draw("colz");
                          
  c3->cd(5);          
  nst2s0->SetFillColor(kBlue);                                                  
  nst2s0->Draw("colz");                            
  
  c3->cd(6);                            
  nst2s1->SetFillColor(kBlue);                                                 
  nst2s1->Draw("colz");
                            
  c3->cd(7);                                                   
  nst2s2->SetFillColor(kBlue);                                    
  nst2s2->Draw("colz");             

  c3->cd(8); 
  nst3s0->SetFillColor(kBlue);
  nst3s0->Draw("colz");
                  
  c3->cd(9);                                           
  nst3s1->SetFillColor(kBlue);                                 
  nst3s1->Draw("colz");
                    
  TCanvas *c4 = new TCanvas("c4","c4",2000,1500);
  c4->Divide(3,3);
                          
  c4->cd(1);                                        
  nst3s2->SetFillColor(kBlue);                                
  nst3s2->Draw("colz");
                     
  c4->cd(2);                                            
  nst4s0->SetFillColor(kBlue);                                 
  nst4s0->Draw("colz");
                   
  c4->cd(3);                                             
  nst4s1->SetFillColor(kBlue);                                
  nst4s1->Draw("colz");
                    
  c4->cd(4);                                            
  nst5s0->SetFillColor(kBlue);                                
  nst5s0->Draw("colz");
                  
  c4->cd(5);                                            
  nst5s1->SetFillColor(kBlue);                                
  nst5s1->Draw("colz");
                       
  c4->cd(6);                                              
  nst5s2->SetFillColor(kBlue);                                     
  nst5s2->Draw("colz");

  c4->cd(7);
  nst5s3->SetFillColor(kBlue);                                             
  nst5s3->Draw("colz");
                    
  c4->cd(8);                                         
  nst5s4->SetFillColor(kBlue);                               
  nst5s4->Draw("colz");
                       
  c4->cd(9);                                               
  nst5s5->SetFillColor(kBlue);                                    
  nst5s5->Draw("colz");
                    
  TCanvas *c5 = new TCanvas("c5","c5",2000,1500);
  c5->Divide(3,3);
                          
  c5->cd(1);                                           
  nst6s0->SetFillColor(kBlue);                                
  nst6s0->Draw("colz");
                               
  c5->cd(2);                      
  nst6s1->SetFillColor(kBlue);                                             
  nst6s1->Draw("colz");
                                 
  c5->cd(3);    
  nst6s2->SetFillColor(kBlue);                                
  nst6s2->Draw("colz");
                     
  c5->cd(4);                                              
  nst6s3->SetFillColor(kBlue);                                                                                                      
  nst6s3->Draw("colz");
                                                                                                                              
  c5->cd(5);                                                                                                                   
  nst6s4->SetFillColor(kBlue);                                                                                                
  nst6s4->Draw("colz");                                                                                                          
                                                                                                                                 
  c5->cd(6);                                                                                                                 
  nst6s5->SetFillColor(kBlue);
  nst6s5->Draw("colz"); 
 
  // Create separate output files for needed histograms
  TFile nst0s0File("nst0s0.root", "RECREATE");                                                                              
  nst0s0->Write();                                                                                                            
  nst0s0File.Close();                                                                                                  
                                                                                                                       
  TFile nst0s1File("nst0s1.root", "RECREATE");                                                                              
  nst0s1->Write();                                                                                                  
  nst0s1File.Close();                                                                                                        
                                                                                                                          
  TFile nst1s0File("nst1s0.root", "RECREATE");                                                                            
  nst1s0->Write();                                                                                              
  nst1s0File.Close();                                                                                                   
                                                                                                                            
  TFile nst1s1File("nst1s1.root", "RECREATE");                                                                               
  nst1s1->Write();                                                                                                           
  nst1s1File.Close();                                                                                                         
                                                                                                                              
  TFile nst2s0File("nst2s0.root", "RECREATE");                                                                                
  nst2s0->Write();                                                                                                     
  nst2s0File.Close();                                                                                                       
                                                                                                                        
  TFile nst2s1File("nst2s1.root", "RECREATE");                                                                            
  nst2s1->Write();                                                                                                     
  nst2s1File.Close();                                                                                                         
                                                                                                                              
  TFile nst2s2File("nst2s2.root", "RECREATE");                                                                                
  nst2s2->Write();                                                                                                  
  nst2s2File.Close();                                                                                              
                                                                                                                   
  TFile nst3s0File("nst3s0.root", "RECREATE");                                                                         
  nst3s0->Write();                                                                                                           
  nst3s0File.Close();                                                                                                      
                                                                                                                              
  TFile nst3s1File("nst3s1.root", "RECREATE");                                                                              
  nst3s1->Write();                                                                                                              
  nst3s1File.Close();                                                                                                           
                                                                                                                           
  TFile nst3s2File("nst3s2.root", "RECREATE");                                                                               
  nst3s2->Write();                                                                                                     
  nst3s2File.Close();                                                                                                    
                                                                                                                            
  TFile nst4s0File("nst4s0.root", "RECREATE");                                                                            
  nst4s0->Write();                                                                                                           
  nst4s0File.Close();                                                                                                         
                                                                                                                                
  TFile nst4s1File("nst4s1.root", "RECREATE");                                                                                 
  nst4s1->Write();                                                                                                          
  nst4s1File.Close();                                                                                                        
                                                                                                                          
  TFile nst5s0File("nst5s0.root", "RECREATE");                                                                              
  nst5s0->Write();                                                                                                       
  nst5s0File.Close();

  TFile nst5s1File("nst5s1.root", "RECREATE");
  nst5s1->Write();                                                                                                    
  nst5s1File.Close();                                                                                                        
                                                                                                                        
  TFile nst5s2File("nst5s2.root", "RECREATE");                                                                              
  nst5s2->Write();                                                                                                              
  nst5s2File.Close();                                                                                                
                                                                                                                             
  TFile nst5s3File("nst5s3.root", "RECREATE");                                                                              
  nst5s3->Write();                                                                                                   
  nst5s3File.Close();                                                                                                
                                                                                                                            
  TFile nst5s4File("nst5s4.root", "RECREATE");                                                                         
  nst5s4->Write();                                                                                                             
  nst5s4File.Close();                                                                                                      
                                                                                                                                   
  TFile nst5s5File("nst5s5.root", "RECREATE");                                                                                
  nst5s5->Write();                                                                                                           
  nst5s5File.Close();                                                                                                         
                                                                                                                               
  TFile nst6s0File("nst6s0.root", "RECREATE");                                                                               
  nst6s0->Write();                                                                                                           
  nst6s0File.Close();                                                                                                          
                                                                                                                            
  TFile nst6s1File("nst6s1.root", "RECREATE");                                                                              
  nst6s1->Write();                                                                                                           
  nst6s1File.Close();                                                                                                        
                                                                                                                           
  TFile nst6s2File("nst6s2.root", "RECREATE");                                                                              
  nst6s2->Write();                                                                                                        
  nst6s2File.Close();                                                                                                       
                                                                                                                            
  TFile nst6s3File("nst6s3.root", "RECREATE");                                                                             
  nst6s3->Write();                                                                                                         
  nst6s3File.Close();                                                                                                     
                                                                                                                         
  TFile nst6s4File("nst6s4.root", "RECREATE");                                                                          
  nst6s4->Write();                                                                                                         
  nst6s4File.Close();                                                                                                  
                                                                                                                              
  TFile nst6s5File("nst6s5.root", "RECREATE");                                                                          
  nst6s5->Write();                                                                                                           
  nst6s5File.Close();  
  */

  /*
    //ntotadc histograms                                                                                       
    TCanvas *c3 = new TCanvas("c3","c3",2000,1500);
    c3->Divide(3,3);

    c3->cd(1);
    Nst0s0->SetFillColor(kBlue);
    Nst0s0->Draw("colz");

    c3->cd(2);
    Nst0s1->SetFillColor(kBlue);
    Nst0s1->Draw("colz");

    c3->cd(3);
    Nst1s0->SetFillColor(kBlue);
    Nst1s0->Draw("colz");

    c3->cd(4);
    Nst1s1->SetFillColor(kBlue);
    Nst1s1->Draw("colz");

    c3->cd(5);
    Nst2s0->SetFillColor(kBlue);
    Nst2s0->Draw("colz");

    c3->cd(6);
    Nst2s1->SetFillColor(kBlue);
    Nst2s1->Draw("colz");

    c3->cd(7);
    Nst2s2->SetFillColor(kBlue);
    Nst2s2->Draw("colz");

    c3->cd(8);
    Nst3s0->SetFillColor(kBlue);
    Nst3s0->Draw("colz");

    c3->cd(9);
    Nst3s1->SetFillColor(kBlue);
    Nst3s1->Draw("colz");

    TCanvas *c4 = new TCanvas("c4","c4",2000,1500);
    c4->Divide(3,3);                                                                                                             
                                                                                                                                
    c4->cd(1);                                                                                                                   
    Nst3s2->SetFillColor(kBlue);                                                                                               
    Nst3s2->Draw("colz");                                                                                                       
                                                                                                                                
    c4->cd(2);                                                                                                                     
    Nst4s0->SetFillColor(kBlue);                                                                                                   
    Nst4s0->Draw("colz");                                                                                                         
                                                                                                                                 
    c4->cd(3);                                                                                                                    
    Nst4s1->SetFillColor(kBlue);                                                                                                   
    Nst4s1->Draw("colz");                                                                                                         
                                                                                                                                
    c4->cd(4);                                                                                                                   
    Nst5s0->SetFillColor(kBlue);                                                                                                   
    Nst5s0->Draw("colz");                                                                                                        
                                                                                                                                 
    c4->cd(5);                                                                                                                   
    Nst5s1->SetFillColor(kBlue);                                                                                                   
    Nst5s1->Draw("colz");                                                                                                         
                                                                                                                                      
    c4->cd(6);                                                                                                                    
    Nst5s2->SetFillColor(kBlue);                                                                                                   
    Nst5s2->Draw("colz");                                                                                                             
                                                                                                                                  
    c4->cd(7);                                                                                                                     
    Nst5s3->SetFillColor(kBlue);                                                                                                       
    Nst5s3->Draw("colz");                                                                                                           
                                                                                                                                 
    c4->cd(8);                                                                                                                   
    Nst5s4->SetFillColor(kBlue);                                                                                                
    Nst5s4->Draw("colz");                                                                                                        
                                                                                                                                 
    c4->cd(9);                                                                                                                    
    Nst5s5->SetFillColor(kBlue);                                                                                                  
    Nst5s5->Draw("colz"); 

    TCanvas *c5 = new TCanvas("c5","c5",2000,1500);                                                                              
    c5->Divide(3,3);                                                                                                              
                                                                                                                                   
    c5->cd(1);                                                                                                                     
    Nst6s0->SetFillColor(kBlue);                                                                                                 
    Nst6s0->Draw("colz");                                                                                                       
                                                                                                                                
    c5->cd(2);                                                                                                                    
    Nst6s1->SetFillColor(kBlue);                                                                                                  
    Nst6s1->Draw("colz");                                                                                                        
                                                                                                                                
    c5->cd(3);                                                                                                                   
    Nst6s2->SetFillColor(kBlue);                                                                                                 
    Nst6s2->Draw("colz");                                                                                                        
                                                                                                                                  
    c5->cd(4);                                                                                                                    
    Nst6s3->SetFillColor(kBlue);                                                                                               
    Nst6s3->Draw("colz");                                                                                                     
                                                                                                       
    c5->cd(5);                                                                                                          
    Nst6s4->SetFillColor(kBlue);                                                                               
    Nst6s4->Draw("colz");                                              
                                                                                   
    c5->cd(6);                                                                                                                      
    Nst6s5->SetFillColor(kBlue);                                                                                                     
    Nst6s5->Draw("colz");

    // Create separate output files for needed histograms                                                                              
    TFile Nst0s0File("Nst0s0.root", "RECREATE");                                                                           
    if (Nst0s0File.IsOpen()) {
    Nst0s0->Write();                                                                                                       
    Nst0s0File.Close();
    std::cout << "Histogram written to Nst0s0.root" << std::endl;
    } else {
      std::cerr << "Error: Unable to open Nst0s0.root for writing." << std::endl;
    }                                                                                         
                                                                                                                             
    TFile Nst0s1File("Nst0s1.root", "RECREATE");                                                                             
    Nst0s1->Write();                                                                                                         
    Nst0s1File.Close();                                                                                                 
                                                                                                                         
    TFile Nst1s0File("Nst1s0.root", "RECREATE");                                                                          
    Nst1s0->Write();                                                                                            
    Nst1s0File.Close();                                                                                                      
                                                                                                                                    
    TFile Nst1s1File("Nst1s1.root", "RECREATE");                                                                         
    Nst1s1->Write();                                                                                                         
    Nst1s1File.Close();                                                                                                         
                                                                                                                            
    TFile Nst2s0File("Nst2s0.root", "RECREATE");                                                                             
    Nst2s0->Write();                                                                                                           
    Nst2s0File.Close();                                                                                                        
                                                                                                                               
    TFile Nst2s1File("Nst2s1.root", "RECREATE");                                                                               
    Nst2s1->Write();                                                                                                          
    Nst2s1File.Close();                                                                                                     
                                                                                                                              
    TFile Nst2s2File("Nst2s2.root", "RECREATE");                                                                               
    Nst2s2->Write();                                                                                                            
    Nst2s2File.Close();                                                                                                         
                                                                                                                            
    TFile Nst3s0File("Nst3s0.root", "RECREATE");                                                                               
    Nst3s0->Write();                                                                                                         
    Nst3s0File.Close();                                                                                                      
                                                                                                                            
    TFile Nst3s1File("Nst3s1.root", "RECREATE");                                                                    
    Nst3s1->Write();                                                                                                     
    Nst3s1File.Close();                                                                                               
                                                                                                                             
    TFile Nst3s2File("Nst3s2.root", "RECREATE");                                                                      
    Nst3s2->Write();                                                                                                     
    Nst3s2File.Close();                                                                                                     
                                                                                                                           
    TFile Nst4s0File("Nst4s0.root", "RECREATE");                                                                           
    Nst4s0->Write();                                                                                                        
    Nst4s0File.Close();                                                                                                      
                                                                                                                     
    TFile Nst4s1File("Nst4s1.root", "RECREATE");                                                                     
    Nst4s1->Write();                                                                                                        
    Nst4s1File.Close();

    TFile Nst5s0File("Nst5s0.root", "RECREATE");
    Nst5s0->Write();                                                                                                        
    Nst5s0File.Close();                                                                                                        
                                                                                                                                
    TFile Nst5s1File("Nst5s1.root", "RECREATE");                                                                               
    Nst5s1->Write();                                                                                                             
    Nst5s1File.Close();                                                                                                         
                                                                                                                              
    TFile Nst5s2File("Nst5s2.root", "RECREATE");                                                                                 
    Nst5s2->Write();                                                                                                        
    Nst5s2File.Close();                                                                                                         
                                                                                                                                  
    TFile Nst5s3File("Nst5s3.root", "RECREATE");                                                                                  
    Nst5s3->Write();                                                                                                             
    Nst5s3File.Close();                                                                                                       
                                                                                                                          
    TFile Nst5s4File("Nst5s4.root", "RECREATE");                                                                           
    Nst5s4->Write();                                                                                                       
    Nst5s4File.Close();                                                                                                    
                                                                                                                      
    TFile Nst5s5File("Nst5s5.root", "RECREATE");                                                                 
    Nst5s5->Write();                                                                                                        
    Nst5s5File.Close();                                                                                                       
                                                                                                                              
    TFile Nst6s0File("Nst6s0.root", "RECREATE");                                                                           
    Nst6s0->Write();                                                                                                           
    Nst6s0File.Close();                                                                                                         
                                                                                                                                
    TFile Nst6s1File("Nst6s1.root", "RECREATE");                                                                             
    Nst6s1->Write();                                                                                                          
    Nst6s1File.Close();                                                                                                         
                                                                                                                               
    TFile Nst6s2File("Nst6s2.root", "RECREATE");                                                                               
    Nst6s2->Write();                                                                                                           
    Nst6s2File.Close();                                                                                                       
                                                                                                                             
    TFile Nst6s3File("Nst6s3.root", "RECREATE");                                                                              
    Nst6s3->Write();                                                                                                      
    Nst6s3File.Close();                                                                                                      
                                                                                                                         
    TFile Nst6s4File("Nst6s4.root", "RECREATE");                                                                            
    Nst6s4->Write();                                                                                                          
    Nst6s4File.Close();                                                                                                      
                                                                                                                                
    TFile Nst6s5File("Nst6s5.root", "RECREATE");                                                                               
    Nst6s5->Write();                                                                                                     
    Nst6s5File.Close(); 
  */

 /*
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);                                                              
  c3->Divide(3,3);                                                                                                             
  c3->cd(1);                                                                                                                
  ngetdest0s0->SetFillColor(kBlue);                                                                                          
  ngetdest0s0->Draw("colz");                                                                                                  
                                                                                                                            
  c3->cd(2);                                                                                                               
  ngetdest0s1->SetFillColor(kBlue);                                                                                          
  ngetdest0s1->Draw("colz");                                                                                                 
                                                                                                                               
  c3->cd(3);                                                                                                                  
  ngetdest1s0->SetFillColor(kBlue);                                                                                           
  ngetdest1s0->Draw("colz");                                                                                                  
                                                                                                                              
  c3->cd(4);                                                                                                                  
  ngetdest1s1->SetFillColor(kBlue);                                                                                        
  ngetdest1s1->Draw("colz");                                                                                                
                                                                                                                               
  c3->cd(5);                                                                                                             
  ngetdest2s0->SetFillColor(kBlue);                                                                                             
  ngetdest2s0->Draw("colz");                                                                                                  
                                                                                                                                 
  c3->cd(6);                                                                                                                   
  ngetdest2s1->SetFillColor(kBlue);                                                                                           
  ngetdest2s1->Draw("colz");                                                                                                  
                                                                                                                             
  c3->cd(7);                                                                                                                            
  ngetdest2s2->SetFillColor(kBlue);                                                                                             
  ngetdest2s2->Draw("colz");  

  c3->cd(8);                                                                                                                       
  ngetdest3s0->SetFillColor(kBlue);                                                                                                    
  ngetdest3s0->Draw("colz");                                                                                                           
                                                                                                                                     
  c3->cd(9);                                                                                                                        
  ngetdest3s1->SetFillColor(kBlue);                                                                                                    
  ngetdest3s1->Draw("colz");                                                                                                          
                                                                                                                                       
  TCanvas *c4 = new TCanvas("c4","c4",2000,1500);                                                                                     
  c4->Divide(3,3);                                                                                                                      
                                                                                                                                        
  c4->cd(1);                                                                                                                          
  ngetdest3s2->SetFillColor(kBlue);                                                                                                   
  ngetdest3s2->Draw("colz");                                                                                                 
                                                                                                                          
  c4->cd(2);                                                                                                                           
  ngetdest4s0->SetFillColor(kBlue);                                                                                                    
  ngetdest4s0->Draw("colz");                                                                                                            
                                                                                                                                        
  c4->cd(3);                                                                                                                          
  ngetdest4s1->SetFillColor(kBlue);                                                                                                  
  ngetdest4s1->Draw("colz");                                                                                                            
                                                                                                                                       
  c4->cd(4);                                                                                                                            
  ngetdest5s0->SetFillColor(kBlue);                                                                                                
  ngetdest5s0->Draw("colz");                                                                                                       
                                                                                                                                        
  c4->cd(5);                                                                                                             
  ngetdest5s1->SetFillColor(kBlue);                                                                                       
  ngetdest5s1->Draw("colz");                                                                                                    
                                                                                                                                      
  c4->cd(6);                                                                                                                           
  ngetdest5s2->SetFillColor(kBlue);                                                                                                     
  ngetdest5s2->Draw("colz");                                                                                                           
                                                                                                                         
  c4->cd(7);                                                                                                             
  ngetdest5s3->SetFillColor(kBlue);                                                                                                  
  ngetdest5s3->Draw("colz");  

  c4->cd(8);                                                                                                                     
  ngetdest5s4->SetFillColor(kBlue);                                                                                                 
  ngetdest5s4->Draw("colz");
                                      
  c4->cd(9);              
  ngetdest5s5->SetFillColor(kBlue);                                    
  ngetdest5s5->Draw("colz");
  
  TCanvas *c5 = new TCanvas("c5","c5",2000,1500);
  c5->Divide(3,3);
                                       
  c5->cd(1);              
  ngetdest6s0->SetFillColor(kBlue);
  ngetdest6s0->Draw("colz");
                                       
  c5->cd(2);                 
  ngetdest6s1->SetFillColor(kBlue);
  ngetdest6s1->Draw("colz");
                                       
  c5->cd(3);                
  ngetdest6s2->SetFillColor(kBlue);
  ngetdest6s2->Draw("colz");
                                        
  c5->cd(4);               
  ngetdest6s3->SetFillColor(kBlue);                                       
  ngetdest6s3->Draw("colz");
                                      
  c5->cd(5);               
  ngetdest6s4->SetFillColor(kBlue);
  ngetdest6s4->Draw("colz");
                                       
  c5->cd(6);             
  ngetdest6s5->SetFillColor(kBlue);
  ngetdest6s5->Draw("colz"); 

  // Create separate output files for needed histograms
  TFile ngetdest0s0File("ngetdest0s0.root", "RECREATE");                        
  ngetdest0s0->Write();                                      
  ngetdest0s0File.Close();

  TFile ngetdest0s1File("ngetdest0s1.root", "RECREATE");               
  ngetdest0s1->Write();                                      
  ngetdest0s1File.Close();

  TFile ngetdest1s0File("ngetdest1s0.root", "RECREATE");                    
  ngetdest1s0->Write();                                   
  ngetdest1s0File.Close();

  TFile ngetdest1s1File("ngetdest1s1.root", "RECREATE");
  ngetdest1s1->Write();                    
  ngetdest1s1File.Close();

  TFile ngetdest2s0File("ngetdest2s0.root", "RECREATE");               
  ngetdest2s0->Write();                                      
  ngetdest2s0File.Close();

  TFile ngetdest2s1File("ngetdest2s1.root", "RECREATE");              
  ngetdest2s1->Write();                                      
  ngetdest2s1File.Close();

  TFile ngetdest2s2File("ngetdest2s2.root", "RECREATE");                     
  ngetdest2s2->Write();                                       
  ngetdest2s2File.Close();

  TFile ngetdest3s0File("ngetdest3s0.root", "RECREATE");                 
  ngetdest3s0->Write();                                     
  ngetdest3s0File.Close();

  TFile ngetdest3s1File("ngetdest3s1.root", "RECREATE");
  ngetdest3s1->Write();                                         
  ngetdest3s1File.Close();

  TFile ngetdest3s2File("ngetdest3s2.root", "RECREATE");             
  ngetdest3s2->Write();                                     
  ngetdest3s2File.Close();

  TFile ngetdest4s0File("ngetdest4s0.root", "RECREATE");            
  ngetdest4s0->Write();                               
  ngetdest4s0File.Close();

  TFile ngetdest4s1File("ngetdest4s1.root", "RECREATE");                   
  ngetdest4s1->Write();                                    
  ngetdest4s1File.Close();

  TFile ngetdest5s0File("ngetdest5s0.root", "RECREATE");               
  ngetdest5s0->Write();                                        
  ngetdest5s0File.Close();

  TFile ngetdest5s1File("ngetdest5s1.root", "RECREATE");               
  ngetdest5s1->Write();                                   
  ngetdest5s1File.Close();

  TFile ngetdest5s2File("ngetdest5s2.root", "RECREATE");                
  ngetdest5s2->Write();                                            
  ngetdest5s2File.Close();

  TFile ngetdest5s3File("ngetdest5s3.root", "RECREATE");                  
  ngetdest5s3->Write();                                         
  ngetdest5s3File.Close();
                                                                       
  TFile ngetdest5s4File("ngetdest5s4.root", "RECREATE");                
  ngetdest5s4->Write();                                        
  ngetdest5s4File.Close();

  TFile ngetdest5s5File("ngetdest5s5.root", "RECREATE");
  ngetdest5s5->Write();
  ngetdest5s5File.Close();

  TFile ngetdest6s0File("ngetdest6s0.root", "RECREATE");                  
  ngetdest6s0->Write();                                         
  ngetdest6s0File.Close();

  TFile ngetdest6s1File("ngetdest6s1.root", "RECREATE");               
  ngetdest6s1->Write();                                        
  ngetdest6s1File.Close();

  TFile ngetdest6s2File("ngetdest6s2.root", "RECREATE");              
  ngetdest6s2->Write();                                          
  ngetdest6s2File.Close();

  TFile ngetdest6s3File("ngetdest6s3.root", "RECREATE");                 
  ngetdest6s3->Write();                                     
  ngetdest6s3File.Close();

  TFile ngetdest6s4File("ngetdest6s4.root", "RECREATE");                   
  ngetdest6s4->Write();                                           
  ngetdest6s4File.Close();

  TFile ngetdest6s5File("ngetdest6s5.root", "RECREATE");
  ngetdest6s5->Write();               
  ngetdest6s5File.Close(); 
 */

}
