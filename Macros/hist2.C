/////////////////////////////////////////////////////////////////////////////
// Description: This macro  identifies good events (events with exactly    //
// one cluster per plane) for a chain of data files, identifies good       //
// events (events with exactly one truehit per plane) for a chain of MC    //
// files and saves 5 histograms (getde, totADC, ngetde, ntotADC, and       // 
// ntotADC) in a root file named after the sensor they correspond to.      //
// This root file can then be used to find the mapped histogram and bar    // 
// graph for each sensor (and total) in a different macro.                 //
//  Using phase 1c geometry.                                               //
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

//intitiate function that takes two histograms, adcHist: totADC for each sensor
//(which was filled with the number of clusters per bin) and nADCHist: ntotADC
//for each sensor (which is currently empty), fills vectors with the content 
//from adcHist, and fills nADCHist based on the fractions per bin

void ProcessADC(TH1* adcHist, TH1* nADCHist)
{
  int totADCclusts = 0;
  //loop through ADC bins and find the tot number of clusters in the histogram
  for (int ADCbin = 1; ADCbin <= adcHist->GetNbinsX(); ++ADCbin)
    {
      totADCclusts += adcHist->GetBinContent(ADCbin);
    }

  // Create vectors to store ADC bin contents fractions and values
  std::vector<double> ADCBinContents;
  std::vector<double> ADCBinFractions;
  std::vector<int> ADCBinValues;
  std::vector<std::pair<int, double>> ADC2DVector;

  // Loop through ADC histogram and fill vectors
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

  // Loop through the 2D vector and set the bin content of nADCHist
  for (int i = 0; i < ADC2DVector.size(); ++i)
    {
      if (nADCHist)
	{
	  // Check if i is a valid index
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

//intitiate function that takes two histograms, ntotADC: ntotADC for each sensor 
//(which is filled with the fractions per bin and fills vectors with the content
//from it to fill ntotadc with the fractions per bin if the fraction is higher 
//than the energy fraction threshold

void ProcessADCAndNADC(TH1* ntotADC, TH1* ntotadc)
{
  std::vector<double> adcBinFractions;
  std::vector<int> adcBinValues;
  std::vector<std::pair<int, double>> adc2DVector;

  int ntotadcIndex = 1;
  double threshold = 6.5e-6;
  //double threshold = 1e-4; //may need to change threshold depending on sensor
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

//intitiate function that takes two histograms, getde: for each sensor (which was 
//filled with the number of clusters per bin) and ngetde: for each sensor (which 
//is currently empty), fills vectors with the content from getde, and fills ngetde
// based on the fractions per bin

void ProcessDEAndNDE(TH1* getde, TH1* ngetde)
{
  
  if (!getde || !ngetde) 
    {
      std::cerr << "Error: Invalid input histograms." << std::endl;
      return;
    }
  
  int totDEclusts = 0;
  
  for (int DEbin = 1; DEbin <= getde->GetNbinsX(); ++DEbin) 
    {
      int DEBinContent = getde->GetBinContent(DEbin);
      totDEclusts += DEBinContent; // Accumulate the total
      
      if (std::isnan(DEBinContent) || std::isinf(DEBinContent))
	{
	  std::cerr << "Error: Input histogram contains NaN or Inf values." << std::endl;
	  return;
	}
    }
  
  if (totDEclusts == 0) 
    {
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

void hist2()
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

  // Create directories
  TDirectory* dir = gDirectory->mkdir("Total");
  TDirectory* dir0 = gDirectory->mkdir("Sensor0");
  TDirectory* dir1 = gDirectory->mkdir("Sensor1");
  TDirectory* dir2 = gDirectory->mkdir("Sensor2");
  TDirectory* dir3 = gDirectory->mkdir("Sensor3");
  TDirectory* dir4 = gDirectory->mkdir("Sensor4");
  TDirectory* dir5 = gDirectory->mkdir("Sensor5");
  TDirectory* dir6 = gDirectory->mkdir("Sensor6");
  TDirectory* dir7 = gDirectory->mkdir("Sensor7");
  TDirectory* dir8 = gDirectory->mkdir("Sensor8");
  TDirectory* dir9 = gDirectory->mkdir("Sensor9");
  TDirectory* dir10 = gDirectory->mkdir("Sensor10");
  TDirectory* dir11 = gDirectory->mkdir("Sensor11");
  TDirectory* dir12 = gDirectory->mkdir("Sensor12");
  TDirectory* dir13 = gDirectory->mkdir("Sensor13");
  TDirectory* dir14 = gDirectory->mkdir("Sensor14");
  TDirectory* dir15 = gDirectory->mkdir("Sensor15");
  TDirectory* dir16 = gDirectory->mkdir("Sensor16");
  TDirectory* dir17 = gDirectory->mkdir("Sensor17");
  TDirectory* dir18 = gDirectory->mkdir("Sensor18");
  TDirectory* dir19 = gDirectory->mkdir("Sensor19");
  TDirectory* dir20 = gDirectory->mkdir("Sensor20");
  TDirectory* dir21 = gDirectory->mkdir("Sensor21");
  TDirectory* dir22 = gDirectory->mkdir("Sensor22");
  TDirectory* dir23 = gDirectory->mkdir("Sensor23");

  // Initalize histograms, organized by sensor
  dir->cd();
  TFile* totalFile = new TFile("Total.root", "RECREATE");
  TH1D *getde = new TH1D ("getde", "GetDE; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1D *totADC = new TH1D ("totADC", "Total ADC; totADC; Number of Clusters", 809, 41, 850); // bin number chosen so there is exactly one bin per ADC value **DO NOT CHANGE**
  TH1D *ngetde = new TH1D ("ngetde", "Normalized GetDE; GetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, 0.0010001);
  TH1D *ntotADC = new TH1D ("ntotADC", "Normalized Total ADC; totADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1D *ntotadc = new TH1D("ntotadc", "Normalized Total ADC (ignoring bins with fraction < 6.5e-6) ; totADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir0->cd();
  TFile* sensor0File = new TFile("sensor0.root", "RECREATE");
  TH1F *getdest0s0 = new TH1F ("getde", "station 0, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st0s0 = new TH1F ("totADC", "station 0, sensor 0; totADC; Number of Clusters", 809, 41, 850); 
  TH1F *ngetdest0s0 = new TH1F ("ngetde", "station 0, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst0s0 = new TH1F ("ntotADC", "station 0, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst0s0 = new TH1F ("ntotadc", "station 0, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir1->cd();
  TFile* sensor1File = new TFile("sensor1.root", "RECREATE");
  TH1F *getdest0s1 = new TH1F ("getde", "station 0, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001); 
  TH1F *st0s1 = new TH1F ("totADC", "station 0, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest0s1 = new TH1F ("ngetde", "station 0, sensor1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst0s1 = new TH1F ("ntotADC", "station 0, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst0s1 = new TH1F ("ntotadc", "station 0, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir2->cd();
  TFile* sensor2File = new TFile("sensor2.root", "RECREATE");
  TH1F *getdest1s0 = new TH1F ("getde", "station 1, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st1s0 = new TH1F ("totADC", "station 1, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest1s0 = new TH1F ("ngetde", "station 1, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst1s0 = new TH1F ("ntotADC", "station 1, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst1s0 = new TH1F ("ntotadc", "station 1, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir3->cd();
  TFile* sensor3File = new TFile("sensor3.root", "RECREATE");
  TH1F *getdest1s1 = new TH1F ("getde", "station 1, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st1s1 = new TH1F ("totADC", "station 1, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest1s1 = new TH1F ("ngetde", "station 1, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst1s1 = new TH1F ("ntotADC", "station 1, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst1s1 = new TH1F ("ntotadc", "station 1, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir4->cd();
  TFile* sensor4File = new TFile("sensor4.root", "RECREATE");
  TH1F *getdest2s0 = new TH1F ("getde", "station 2, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st2s0 = new TH1F ("totADC", "station 2, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest2s0 = new TH1F ("ngetde", "station 2, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst2s0 = new TH1F ("ntotADC", "station 2, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst2s0 = new TH1F ("ntotadc", "station 2, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir5->cd();
  TFile* sensor5File = new TFile("sensor5.root", "RECREATE");
  TH1F *getdest2s1 = new TH1F ("getde", "station 2, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st2s1 = new TH1F ("totADC", "station 2, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest2s1 = new TH1F ("ngetde", "station 2, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst2s1 = new TH1F ("ntotADC", "station 2, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst2s1 = new TH1F ("ntotadc", "station 2, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir6->cd();
  TFile* sensor6File = new TFile("sensor6.root", "RECREATE");
  TH1F *getdest2s2 = new TH1F ("getde", "station 2, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st2s2 = new TH1F ("totADC", "station 2, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest2s2 = new TH1F ("ngetde", "station 2, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst2s2 = new TH1F ("ntotADC", "station 2, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst2s2 = new TH1F ("ntotadc", "station 2, sensor 2 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir7->cd();
  TFile* sensor7File = new TFile("sensor7.root", "RECREATE");
  TH1F *getdest3s0 = new TH1F ("getde", "station 3, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st3s0 = new TH1F ("totADC", "station 3, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest3s0 = new TH1F ("ngetde", "station 3, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst3s0 = new TH1F ("ntotADC", "station 3, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst3s0 = new TH1F ("ntotadc", "station 3, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir8->cd();
  TFile* sensor8File = new TFile("sensor8.root", "RECREATE");
  TH1F *getdest3s1 = new TH1F ("getde", "station 3, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st3s1 = new TH1F ("totADC", "station 3, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest3s1 = new TH1F ("ngetde", "station 3, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst3s1 = new TH1F ("ntotADC", "station 3, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst3s1 = new TH1F ("ntotadc", "station 3, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  
  dir9->cd();
  TFile* sensor9File = new TFile("sensor9.root", "RECREATE");
  TH1F *getdest3s2 = new TH1F ("getde", "station 3, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st3s2 = new TH1F ("totADC", "station 3, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest3s2 = new TH1F ("ngetde", "station 3, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst3s2 = new TH1F ("ntotADC", "station 3, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst3s2 = new TH1F ("ntotadc", "station 3, sensor 2 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir10->cd();
  TFile* sensor10File = new TFile("sensor10.root", "RECREATE");
  TH1F *getdest4s0 = new TH1F ("getde", "station 4, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st4s0 = new TH1F ("totADC", "station 4, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest4s0 = new TH1F ("ngetde", "station 4, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst4s0 = new TH1F ("ntotADC", "station 4, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst4s0 = new TH1F ("ntotadc", "station 4, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  
  dir11->cd();
  TFile* sensor11File = new TFile("sensor11.root", "RECREATE");
  TH1F *getdest4s1 = new TH1F ("getde", "station 4, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);  
  TH1F *st4s1 = new TH1F ("totADC", "station 4, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest4s1 = new TH1F ("ngetde", "station 4, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst4s1 = new TH1F ("ntotADC", "station 4, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst4s1 = new TH1F ("ntotadc", "station 4, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  
  dir12->cd();
  TFile* sensor12File = new TFile("sensor12.root", "RECREATE");
  TH1F *getdest5s0 = new TH1F ("getde", "station 5, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st5s0 = new TH1F ("totADC", "station 5, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest5s0 = new TH1F ("ngetde", "station 5, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst5s0 = new TH1F ("ntotADC", "station 5, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s0 = new TH1F ("ntotadc", "station 5, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir13->cd();
  TFile* sensor13File = new TFile("sensor13.root", "RECREATE");
  TH1F *getdest5s1 = new TH1F ("getde", "station 5, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st5s1 = new TH1F ("totADC", "station 5, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest5s1 = new TH1F ("ngetde", "station 5, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst5s1 = new TH1F ("ntotADC", "station 5, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s1 = new TH1F ("ntotadc", "station 5, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir14->cd();
  TFile* sensor14File = new TFile("sensor14.root", "RECREATE");
  TH1F *getdest5s2 = new TH1F ("getde", "station 5, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st5s2 = new TH1F ("totADC", "station 5, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest5s2 = new TH1F ("ngetde", "station 5, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst5s2 = new TH1F ("ntotADC", "station 5, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s2 = new TH1F ("ntotadc", "station 5, sensor 2 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir15->cd();
  TFile* sensor15File = new TFile("sensor15.root", "RECREATE");
  TH1F *getdest5s3 = new TH1F ("getde", "station 5, sensor 3; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st5s3 = new TH1F ("totADC", "station 5, sensor 3; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest5s3 = new TH1F ("ngetde", "station 5, sensor 3; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst5s3 = new TH1F ("ntotADC", "station 5, sensor 3; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s3 = new TH1F ("ntotadc", "station 5, sensor 3 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir16->cd();
  TFile* sensor16File = new TFile("sensor16.root", "RECREATE");
  TH1F *getdest5s4 = new TH1F ("getde", "station 5, sensor 4; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st5s4 = new TH1F ("totADC", "station 5, sensor 4; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest5s4 = new TH1F ("ngetde", "station 5, sensor 4; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst5s4 = new TH1F ("ntotADC", "station 5, sensor 4; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s4 = new TH1F ("ntotadc", "station 5, sensor 4 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir17->cd();
  TFile* sensor17File = new TFile("sensor17.root", "RECREATE");
  TH1F *getdest5s5 = new TH1F ("getde", "station 5, sensor 5; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st5s5 = new TH1F ("totADC", "station 5, sensor 5; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest5s5 = new TH1F ("ngetde", "station 5, sensor 5; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst5s5 = new TH1F ("ntotADC", "station 5, sensor 5; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst5s5 = new TH1F ("ntotadc", "station 5, sensor 5 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir18->cd();
  TFile* sensor18File = new TFile("sensor18.root", "RECREATE");
  TH1F *getdest6s0 = new TH1F ("getde", "station 6, sensor 0; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st6s0 = new TH1F ("totADC", "station 6, sensor 0; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest6s0 = new TH1F ("ngetde", "station 6, sensor 0; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst6s0 = new TH1F ("ntotADC", "station 6, sensor 0; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s0 = new TH1F ("ntotadc", "station 6, sensor 0 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir19->cd();
  TFile* sensor19File = new TFile("sensor19.root", "RECREATE");
  TH1F *getdest6s1 = new TH1F ("getde", "station 6, sensor 1; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st6s1 = new TH1F ("totADC", "station 6, sensor 1; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest6s1 = new TH1F ("ngetde", "station 6, sensor 1; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst6s1 = new TH1F ("ntotADC", "station 6, sensor 1; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s1 = new TH1F ("ntotadc", "station 6, sensor 1 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir20->cd();
  TFile* sensor20File = new TFile("sensor20.root", "RECREATE");
  TH1F *getdest6s2 = new TH1F ("getde", "station 6, sensor 2; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st6s2 = new TH1F ("totADC", "station 6, sensor 2; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest6s2 = new TH1F ("ngetde", "station 6, sensor 2; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst6s2 = new TH1F ("ntotADC", "station 6, sensor 2; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s2 = new TH1F ("ntotadc", "station 6, sensor 2 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir21->cd();
  TFile* sensor21File = new TFile("sensor21.root", "RECREATE");
  TH1F *getdest6s3 = new TH1F ("getde", "station 6, sensor 3; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);  
  TH1F *st6s3 = new TH1F ("totADC", "station 6, sensor 3; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest6s3 = new TH1F ("ngetde", "station 6, sensor 3; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst6s3 = new TH1F ("ntotADC", "station 6, sensor 3; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s3 = new TH1F ("ntotadc", "station 6, sensor 3 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir22->cd();
  TFile* sensor22File = new TFile("sensor22.root", "RECREATE");
  TH1F *getdest6s4 = new TH1F ("getde", "station 6, sensor 4; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st6s4 = new TH1F ("totADC", "station 6, sensor 4; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest6s4 = new TH1F ("ngetde", "station 6, sensor 4; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst6s4 = new TH1F ("ntotADC", "station 6, sensor 4; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s4 = new TH1F ("ntotadc", "station 6, sensor 4 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  dir23->cd();
  TFile* sensor23File = new TFile("sensor23.root", "RECREATE");
  TH1F *getdest6s5 = new TH1F ("getde", "station 6, sensor 5; GetDE; Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *st6s5 = new TH1F ("totADC", "station 6, sensor 5; totADC; Number of Clusters", 809, 41, 850);
  TH1F *ngetdest6s5 = new TH1F ("ngetde", "station 6, sensor 5; nGetDE; Number of Hits/Total Number of Hits", 10000000, 0.0000001, .0010001);
  TH1F *nst6s5 = new TH1F ("ntotADC", "station 6, sensor 5; ntotADC; Number of Clusters/Total Number of Clusters", 809, 41, 850);
  TH1F *Nst6s5 = new TH1F ("ntotadc", "station 6, sensor 5 (ignoring bins with fraction < 6.5e-6); ntotadc; Number of Clusters/Total Number of Clusters", 809, 41, 850);

  //change back to the global directory
  gDirectory->cd();

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
	  // if good event, update counter and loop over clusts
          TotalADCClustersData += nclusts;
          n_good_data_events++;
  
          for (int idx = 0; idx < nclusts; ++idx)
            {
	      int sensor = recData->cluster.clust[idx].sens;
              int station = recData->cluster.clust[idx].station;
              double AvgADC = recData->cluster.clust[idx].avgadc;

	      // calculate totADC for all clusters
	      TotADC = ndigits*AvgADC;
	      totADC->Fill(TotADC);
	      
	      // calculate totADC for the clusters in each sensor
	      if (station == 0 && sensor == 0)
		{
		  // plane 0
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
  
  // Loop over our tree and each time we call GetEntries, the MCdata in the tree is copied to recMC
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
	  // if good event,update counter and loop over clusts
          TotalDEClustersMC += ntruehits;
          n_good_MC_events++;

          for (int idx = 0; idx < ntruehits; ++idx)
            {
	      //fill GetDE for all sensors 
              GetDE = recMC->truth.truehits.truehits[idx].GetDE;
              getde->Fill(GetDE);

	      int sensor = recMC->truth.truehits.truehits[idx].GetSensor;
              int station = recMC->truth.truehits.truehits[idx].GetStation;

	      // fill GetDE for each sensor 
	      if (sensor == 0)
                {
                  // plane 0
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
  //loop through ADC bins and find the tot number of clusters in the histogram
  for (int ADCbin = 1; ADCbin <= totADC->GetNbinsX(); ++ADCbin)
    {
      totADCclusts += totADC->GetBinContent(ADCbin);
    }

  // Create a vector to store ADC bin contents fractions and values 
  std::vector<double> ADCBinContents;
  std::vector<double> ADCBinFractions;
  std::vector<int> ADCBinValues;
  std::vector<std::pair<int, double>> ADC2DVector;

  // Loop through ADC histogram and fill vectors
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
  
  // Loop through the 2D vector and set the bin content of ntotADC
  for (int i = 0; i < ADC2DVector.size(); ++i)  
  {
      if (ntotADC)
	{
	  // Check if i is a valid index
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

  //repeat for individual sensors using ProcessADC function
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
  
 // Create vectors to store the fractions and bin values
  std::vector<double> adcBinFractions;
  std::vector<int> adcBinValues;             
  std::vector<std::pair<int, double>> adc2DVector;

  int ntotadcIndex = 1;
  double threshold = 6.5e-6;

  // Loop through ntotADC and fill vectors if the fraction is higher than the 
  // energy fraction threshold
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

  //repeat for individual sensors using ProcessADCAndNADC function 
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
  //loop through DE bins and find the tot number of clusters in the histogram  
  for (int DEbin = 1; DEbin <= getde->GetNbinsX(); ++DEbin)
    {
      totDEclusts += getde->GetBinContent(DEbin);
    }

  // Create a vector to store DE bin contents fractions and values
  std::vector<int> DEBinContents;
  std::vector<double> DEBinFractions;
  std::vector<double> DEBinValues;
  std::vector<std::pair<double, double>> DE2DVector;

  // Loop through getDE histogram and fill vectors 
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

  //repeat for individual sensors using ProcessDEAndNDE function and fill histograms
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
  ProcessDEAndNDE(getdest3s2, ngetdest3s2);
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
 
  //write out the histograms for everything and the individual sensors in 
  //their corresponding root files, then save and close them 
  totalFile->Write();
  totalFile->Close();
  
  sensor0File->Write();
  sensor0File->Close();

  sensor1File->Write();
  sensor1File->Close();
 
  sensor2File->Write();
  sensor2File->Close();

  sensor3File->Write();
  sensor3File->Close();

  sensor4File->Write();
  sensor4File->Close();

  sensor5File->Write();
  sensor5File->Close();

  sensor6File->Write();
  sensor6File->Close();

  sensor7File->Write();
  sensor7File->Close();

  sensor8File->Write();
  sensor8File->Close();

  sensor9File->Write();
  sensor9File->Close();
  
  sensor10File->Write();
  sensor10File->Close();
  
  sensor11File->Write();
  sensor11File->Close();
  
  sensor12File->Write();
  sensor12File->Close();

  sensor13File->Write();
  sensor13File->Close();

  sensor14File->Write();
  sensor14File->Close();

  sensor15File->Write();
  sensor15File->Close();

  sensor16File->Write();
  sensor16File->Close();

  sensor17File->Write();
  sensor17File->Close();

  sensor18File->Write();
  sensor18File->Close();

  sensor19File->Write();
  sensor19File->Close();

  sensor20File->Write();
  sensor20File->Close();

  sensor21File->Write();
  sensor21File->Close();

  sensor22File->Write();
  sensor22File->Close();

  sensor23File->Write();
  sensor23File->Close();


  /*
  //example of how to print histograms directly when running root instead of opening the file
  TCanvas *c1 = new TCanvas("c1","c1",1000,500);
  getdest0s0->SetFillColor(kBlue);
  getdest0s0->Draw("colz");

  TCanvas *c2 = new TCanvas("c2","c4",1000,500);
  ngetdest0s0->SetFillColor(kBlue);
  ngetdest0s0->Draw("colz");  

  TCanvas *c3 = new TCanvas("c3","c3",1000,500);
  st0s0->SetFillColor(kBlue);
  st0s0->Draw("colz");

  TCanvas *c4 = new TCanvas("c4","c4",1000,500);
  nst0s0->SetFillColor(kBlue);
  nst0s0->Draw("colz"); 

  TCanvas *c5 = new TCanvas("c5","c5",1000,500);
  Nst0s0->SetFillColor(kBlue);   
  */
  }
