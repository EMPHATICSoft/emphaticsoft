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
  std::vector<int> DEBinContents;
  std::vector<double> DEBinFractions;
  std::vector<double> DEBinValues;
  std::vector<std::pair<double, double>> DE2DVector;

  int totDEclusts = 0;

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

  TH1F *st0s0 = new TH1F ("st0s0", "station 0, sensor 0; totADC; Number of Clusters", 809, 41, 850);
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

  TH1F *getdest0s0 = new TH1F ("getdest0s0", "station 0, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
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
  
  for (int i = 0; i < DE2DVector.size(); ++i) 
    {
      ngetde->SetBinContent(i+1, DE2DVector[i].second);
    }

  //ProcessDEAndNDE(getde, ngetde);
  ProcessDEAndNDE(getdest0s0, ngetdest0s0);






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
 
  // Create separate output files for needed histogram 
  TFile mappedHistogramFile(" mappedHistogram.root", "RECREATE");
   mappedHistogram->Write();
   mappedHistogramFile.Close();

  TFile barGraphFile("barGraph.root", "RECREATE");
  barGraph->Write();
  barGraphFile.Close();
  */
  /*
  //totADC histograms
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);
  c3->Divide(3,3);

  c3->cd(1);
  st0s0->SetFillColor(kBlue);
  st0s0->Draw("colz");

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

  /*  
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
  */

  TCanvas *c3 = new TCanvas("c3", "c3", 1000, 500);                                                                                                                 
  ngetde->Draw();                                                                                                                                                   
  ngetde->SetFillColor(kRed);    

  TCanvas *c4 = new TCanvas("c4", "4", 1000, 500);                                                                                                                 
  ngetdest0s0->Draw();                                                                                                                           
  ngetdest0s0->SetFillColor(kRed);    
}
