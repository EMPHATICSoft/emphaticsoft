///////////////////////////////////////////////////////////////////////////// 
// Description: This macro identifies good DATA events (events w/ exactly  //
// one cluster per plane) for a chain of data files, then creates regular  //
// and normalized width and RMS histograms, and a 3D WidthVsTADCVsRMS      //
// histogram as well. Using phase 1c geometry.                             //
//                                                                         //
//Date: December 05, 2023                                                  //
//Author: D.A.H.                                                           //
/////////////////////////////////////////////////////////////////////////////

#include "StandardRecord/StandardRecord.h"
#include "TCanvas.h"
#include "TH3.h"
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <vector>
#include <TChain.h>
#include <TMath.h>
#include "TGraph.h"

void WidthVsTADCVsRMS()
{
  //Create a TChain                                      
  TChain *datachain = new TChain("recTree");

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

  // Create StandardRecord objects and set the branch address using our tree 
  caf::StandardRecord* recData = 0;
  datachain->SetBranchAddress("rec", &recData);

  // Create directories                
  TDirectory* Dir = gDirectory->mkdir("total");
  TDirectory* Dir0 = gDirectory->mkdir("sensor0");
  TDirectory* Dir1 = gDirectory->mkdir("sensor1");
  TDirectory* Dir2 = gDirectory->mkdir("sensor2");
  TDirectory* Dir3 = gDirectory->mkdir("sensor3");
  TDirectory* Dir4 = gDirectory->mkdir("sensor4");
  TDirectory* Dir5 = gDirectory->mkdir("sensor5");
  TDirectory* Dir6 = gDirectory->mkdir("sensor6");
  TDirectory* Dir7 = gDirectory->mkdir("sensor7");
  TDirectory* Dir8 = gDirectory->mkdir("sensor8");
  TDirectory* Dir9 = gDirectory->mkdir("sensor9");
  TDirectory* Dir10 = gDirectory->mkdir("sensor10");
  TDirectory* Dir11 = gDirectory->mkdir("sensor11");
  TDirectory* Dir12 = gDirectory->mkdir("sensor12");
  TDirectory* Dir13 = gDirectory->mkdir("sensor13");
  TDirectory* Dir14 = gDirectory->mkdir("sensor14");
  TDirectory* Dir15 = gDirectory->mkdir("sensor15");
  TDirectory* Dir16 = gDirectory->mkdir("sensor16");
  TDirectory* Dir17 = gDirectory->mkdir("sensor17");
  TDirectory* Dir18 = gDirectory->mkdir("sensor18");
  TDirectory* Dir19 = gDirectory->mkdir("sensor19");
  TDirectory* Dir20 = gDirectory->mkdir("sensor20");
  TDirectory* Dir21 = gDirectory->mkdir("sensor21");
  TDirectory* Dir22 = gDirectory->mkdir("sensor22");
  TDirectory* Dir23 = gDirectory->mkdir("sensor23");

  // Initalize histograms 
  Dir->cd();
  TFile* TotalFile = new TFile("total.root", "RECREATE");
  TH1D *clustRMS = new TH1D ("RMS", "RMS; RMS; clusters per bin", 100, -0.5, 5);
  TH1D *width = new TH1D ("width", "Width; width; clusters per bin", 36, 0.5, 18.5);
  TH3D *WidthVsTADCVsRMS = new TH3D("WidthVsTADCVsRMS", "Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1D *nclustRMS = new TH1D ("nRMS", "RMS; RMS; clusters per bin", 100, -0.5, 5);
  TH1D *nwidth = new TH1D ("nwidth", "Width; width; clusters per bin", 36, 0.5, 18.5);

  Dir0->cd();
  TFile* sensor0File = new TFile("Sensor0.root", "RECREATE");
  TH1F *RMSst0s0 = new TH1F ("RMS", "station 0, sensor 0; RMS; clusters per bin", 100, -0.5, 5); 
  TH1F *Wst0s0 = new TH1F ("width", "station 0, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st0s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 0, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst0s0 = new TH1F ("nRMS", "station 0, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst0s0 = new TH1F ("nwidth", "station 0, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir1->cd();
  TFile* sensor1File = new TFile("Sensor1.root", "RECREATE");
  TH1F *RMSst0s1 = new TH1F ("RMS", "station 0, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst0s1 = new TH1F ("width", "station 0, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st0s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 0, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst0s1 = new TH1F ("nRMS", "station 0, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst0s1 = new TH1F ("nwidth", "station 0, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir2->cd();
  TFile* sensor2File = new TFile("Sensor2.root", "RECREATE");
  TH1F *RMSst1s0 = new TH1F ("RMS", "station 1, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst1s0 = new TH1F ("width", "station 1, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st1s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 1, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst1s0 = new TH1F ("nRMS", "station 1, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst1s0 = new TH1F ("nwidth", "station 1, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir3->cd();
  TFile* sensor3File = new TFile("Sensor3.root", "RECREATE");
  TH1F *RMSst1s1 = new TH1F ("RMS", "station 1, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst1s1 = new TH1F ("width", "station 1, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st1s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 1, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst1s1 = new TH1F ("nRMS", "station 1, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst1s1 = new TH1F ("nwidth", "station 1, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir4->cd();
  TFile* sensor4File = new TFile("Sensor4.root", "RECREATE");
  TH1F *RMSst2s0 = new TH1F ("RMS", "station 2, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst2s0 = new TH1F ("width", "station 2, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st2s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 2, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst2s0 = new TH1F ("nRMS", "station 2, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst2s0 = new TH1F ("nwidth", "station 2, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir5->cd();
  TFile* sensor5File = new TFile("Sensor5.root", "RECREATE");
  TH1F *RMSst2s1 = new TH1F ("RMS", "station 2, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst2s1 = new TH1F ("width", "station 2, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st2s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 2, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst2s1 = new TH1F ("nRMS", "station 2, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst2s1 = new TH1F ("nwidth", "station 2, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir6->cd();
  TFile* sensor6File = new TFile("Sensor6.root", "RECREATE");
  TH1F *RMSst2s2 = new TH1F ("RMS", "station 2, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst2s2 = new TH1F ("width", "station 2, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st2s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 2, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst2s2 = new TH1F ("nRMS", "station 2, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst2s2 = new TH1F ("nwidth", "station 2, sensor 2; width; clusters per bin", 36, 0.5, 18.5);

  Dir7->cd();
  TFile* sensor7File = new TFile("Sensor7.root", "RECREATE");
  TH1F *RMSst3s0 = new TH1F ("RMS", "station 3, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst3s0 = new TH1F ("width", "station 3, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st3s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 3, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst3s0 = new TH1F ("nRMS", "station 3, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst3s0 = new TH1F ("nwidth", "station 3, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir8->cd();
  TFile* sensor8File = new TFile("Sensor8.root", "RECREATE");
  TH1F *RMSst3s1 = new TH1F ("RMS", "station 3, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst3s1 = new TH1F ("width", "station 3, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st3s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 3, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst3s1 = new TH1F ("nRMS", "station 3, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst3s1 = new TH1F ("nwidth", "station 3, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir9->cd();
  TFile* sensor9File = new TFile("Sensor9.root", "RECREATE");
  TH1F *RMSst3s2 = new TH1F ("RMS", "station 3, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst3s2 = new TH1F ("width", "station 3, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st3s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 3, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst3s2 = new TH1F ("nRMS", "station 3, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst3s2 = new TH1F ("nwidth", "station 3, sensor 2; width; clusters per bin", 36, 0.5, 18.5);

  Dir10->cd();
  TFile* sensor10File = new TFile("Sensor10.root", "RECREATE");
  TH1F *RMSst4s0 = new TH1F ("RMS", "station 4, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst4s0 = new TH1F ("width", "station 4, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st4s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 4, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst4s0 = new TH1F ("nRMS", "station 4, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst4s0 = new TH1F ("nwidth", "station 4, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir11->cd();
  TFile* sensor11File = new TFile("Sensor11.root", "RECREATE");
  TH1F *RMSst4s1 = new TH1F ("RMS", "station 4, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst4s1 = new TH1F ("width", "station 4, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st4s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 4, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst4s1 = new TH1F ("nRMS", "station 4, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst4s1 = new TH1F ("nwidth", "station 4, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir12->cd();
  TFile* sensor12File = new TFile("Sensor12.root", "RECREATE");
  TH1F *RMSst5s0 = new TH1F ("RMS", "station 5, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s0 = new TH1F ("width", "station 5, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst5s0 = new TH1F ("nRMS", "station 5, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst5s0 = new TH1F ("nwidth", "station 5, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir13->cd();
  TFile* sensor13File = new TFile("Sensor13.root", "RECREATE");
  TH1F *RMSst5s1 = new TH1F ("RMS", "station 5, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s1 = new TH1F ("width", "station 5, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst5s1 = new TH1F ("nRMS", "station 5, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst5s1 = new TH1F ("nwidth", "station 5, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir14->cd();
  TFile* sensor14File = new TFile("Sensor14.root", "RECREATE");
  TH1F *RMSst5s2 = new TH1F ("RMS", "station 5, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s2 = new TH1F ("width", "station 5, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst5s2 = new TH1F ("nRMS", "station 5, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst5s2 = new TH1F ("nwidth", "station 5, sensor 2; width; clusters per bin", 36, 0.5, 18.5);

  Dir15->cd();
  TFile* sensor15File = new TFile("Sensor15.root", "RECREATE");
  TH1F *RMSst5s3 = new TH1F ("RMS", "station 5, sensor 3; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s3 = new TH1F ("width", "station 5, sensor 3; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s3_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 3; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst5s3 = new TH1F ("nRMS", "station 5, sensor 3; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst5s3 = new TH1F ("nwidth", "station 5, sensor 3; width; clusters per bin", 36, 0.5, 18.5);

  Dir16->cd();
  TFile* sensor16File = new TFile("Sensor16.root", "RECREATE");
  TH1F *RMSst5s4 = new TH1F ("RMS", "station 5, sensor 4; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s4 = new TH1F ("width", "station 5, sensor 4; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s4_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 4; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst5s4 = new TH1F ("nRMS", "station 5, sensor 4; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst5s4 = new TH1F ("nwidth", "station 5, sensor 4; width; clusters per bin", 36, 0.5, 18.5);

  Dir17->cd();
  TFile* sensor17File = new TFile("Sensor17.root", "RECREATE");
  TH1F *RMSst5s5 = new TH1F ("RMS", "station 5, sensor 5; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s5 = new TH1F ("width", "station 5, sensor 5; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s5_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 5; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst5s5 = new TH1F ("nRMS", "station 5, sensor 5; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst5s5 = new TH1F ("nwidth", "station 5, sensor 5; width; clusters per bin", 36, 0.5, 18.5);

  Dir18->cd();
  TFile* sensor18File = new TFile("Sensor18.root", "RECREATE");
  TH1F *RMSst6s0 = new TH1F ("RMS", "station 6, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s0 = new TH1F ("width", "station 6, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst6s0 = new TH1F ("nRMS", "station 6, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst6s0 = new TH1F ("nwidth", "station 6, sensor 0; width; clusters per bin", 36, 0.5, 18.5);

  Dir19->cd();
  TFile* sensor19File = new TFile("Sensor19.root", "RECREATE");
  TH1F *RMSst6s1 = new TH1F ("RMS", "station 6, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s1 = new TH1F ("width", "station 6, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst6s1 = new TH1F ("nRMS", "station 6, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst6s1 = new TH1F ("nwidth", "station 6, sensor 1; width; clusters per bin", 36, 0.5, 18.5);

  Dir20->cd();
  TFile* sensor20File = new TFile("Sensor20.root", "RECREATE");
  TH1F *RMSst6s2 = new TH1F ("RMS", "station 6, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s2 = new TH1F ("width", "station 6, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst6s2 = new TH1F ("nRMS", "station 6, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst6s2 = new TH1F ("nwidth", "station 6, sensor 2; width; clusters per bin", 36, 0.5, 18.5);

  Dir21->cd();
  TFile* sensor21File = new TFile("Sensor21.root", "RECREATE");
  TH1F *RMSst6s3 = new TH1F ("RMS", "station 6, sensor 3; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s3 = new TH1F ("width", "station 6, sensor 3; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s3_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 3; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst6s3 = new TH1F ("nRMS", "station 6, sensor 3; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst6s3 = new TH1F ("nwidth", "station 6, sensor 3; width; clusters per bin", 36, 0.5, 18.5);

  Dir22->cd();
  TFile* sensor22File = new TFile("Sensor22.root", "RECREATE");
  TH1F *RMSst6s4 = new TH1F ("RMS", "station 6, sensor 4; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s4 = new TH1F ("width", "station 6, sensor 4; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s4_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 4; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst6s4 = new TH1F ("nRMS", "station 6, sensor 4; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst6s4 = new TH1F ("nwidth", "station 6, sensor 4; width; clusters per bin", 36, 0.5, 18.5);

  Dir23->cd();
  TFile* sensor23File = new TFile("Sensor23.root", "RECREATE");
  TH1F *RMSst6s5 = new TH1F ("RMS", "station 6, sensor 5; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s5 = new TH1F ("width", "station 6, sensor 5; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s5_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 5; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);
  TH1F *nRMSst6s5 = new TH1F ("nRMS", "station 6, sensor 5; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *nWst6s5 = new TH1F ("nwidth", "station 6, sensor 5; width; clusters per bin", 36, 0.5, 18.5);

  //return to global directory
  gDirectory->cd();

  // Initalize the number of good data events to 0 
  int n_good_data_events = 0;

  // Counter for total number of ADC clusters                                            
  int TotalADCClustersData = 0;

  int ndigits;
  double TotADC;
  double RMS;

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
	  //if good event, update counter and loop over clusts
          TotalADCClustersData += nclusts;
          n_good_data_events++;
	  
          for (int idx = 0; idx < nclusts; ++idx)
            {
	      int sensor = recData->cluster.clust[idx].sens;
              int station = recData->cluster.clust[idx].station;
              double AvgADC = recData->cluster.clust[idx].avgadc;

	      //calculate totADC, find RMS and width and fill 1D and 3D hist for all clusters
              TotADC = ndigits*AvgADC;
	      RMS = recData->cluster.clust[idx].wgtrmsstrip;
	      WidthVsTADCVsRMS->Fill(ndigits, TotADC, RMS);
	      clustRMS->Fill(RMS);
	      width->Fill(ndigits);

	      //repeat for each sensor 
	      if (station == 0 && sensor == 0)
                {
                  // plane 0
		  st0s0_3D->Fill(ndigits, TotADC, RMS);
		  RMSst0s0->Fill(RMS);
		  Wst0s0->Fill(ndigits);
                }
              if (station == 0 && sensor == 1)
                {
                  //plane 1
		  st0s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst0s1->Fill(RMS);
                  Wst0s1->Fill(ndigits);
                }
              if (station == 1 && sensor == 0)
                {
                  //plane 2
		  st1s0_3D->Fill(ndigits, TotADC, RMS);
                  RMSst1s0->Fill(RMS);
                  Wst1s0->Fill(ndigits);
                }
              if (station == 1 && sensor == 1)
                {
                  //plane 3
		  st1s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst1s1->Fill(RMS);
                  Wst1s1->Fill(ndigits);
                }
              if (station == 2 && sensor == 0)
                {
                  //plane 4
		  st2s0_3D->Fill(ndigits, TotADC, RMS);
                  RMSst2s0->Fill(RMS);
                  Wst2s0->Fill(ndigits);
                }
              if (station == 2 && sensor == 1)
                {
                  //plane 5
		  st2s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst2s1->Fill(RMS);
                  Wst2s1->Fill(ndigits);
                }
              if (station == 2 && sensor == 2)
                {
                  //plane 6
		  st2s2_3D->Fill(ndigits, TotADC, RMS);
                  RMSst2s2->Fill(RMS);
                  Wst2s2->Fill(ndigits);
                }
              if (station == 3 && sensor == 0)
                {
                  //plane 7
		  st3s0_3D->Fill(ndigits, TotADC, RMS);
                  RMSst3s0->Fill(RMS);
                  Wst3s0->Fill(ndigits);
                }
	      if (station == 3 && sensor == 1)
                {
                  //plane 8
		  st3s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst3s1->Fill(RMS);
                  Wst3s1->Fill(ndigits);
                }
              if (station == 3 && sensor == 2)
                {
                  //plane 9
		  st3s2_3D->Fill(ndigits, TotADC, RMS);
                  RMSst3s2->Fill(RMS);
                  Wst3s2->Fill(ndigits);
                }
              if (station == 4 && sensor == 0)
                {
                  //plane 10
		  st4s0_3D->Fill(ndigits, TotADC, RMS);
                  RMSst4s0->Fill(RMS);
                  Wst4s0->Fill(ndigits);
                }
              if (station == 4 && sensor == 1)
                {
                  //plane 11
		  st4s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst4s1->Fill(RMS);
                  Wst4s1->Fill(ndigits);
                }
              if (station == 5 && sensor == 0)
                {
                  //plane 12
		  st5s0_3D->Fill(ndigits, TotADC, RMS);
                  RMSst5s0->Fill(RMS);
                  Wst5s0->Fill(ndigits);
                }
              if (station == 5 && sensor == 1)
                {
                  //plane 12
		  st5s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst5s1->Fill(RMS);
                  Wst5s1->Fill(ndigits);
                }
	      if (station == 5 && sensor == 2)
                {
                  //plane 13
		  st5s2_3D->Fill(ndigits, TotADC, RMS);
                  RMSst5s2->Fill(RMS);
                  Wst5s2->Fill(ndigits);
                }
              if (station == 5 && sensor == 3)
                {
                  //plane 13
		  st5s3_3D->Fill(ndigits, TotADC, RMS);
                  RMSst5s3->Fill(RMS);
                  Wst5s3->Fill(ndigits);
                }
              if (station == 5 && sensor == 4)
                {
                  //plane 14
		  st5s4_3D->Fill(ndigits, TotADC, RMS);
                  RMSst5s4->Fill(RMS);
                  Wst5s4->Fill(ndigits);
                }
              if (station == 5 && sensor == 5)
                {
                  //plane 14
		  st5s5_3D->Fill(ndigits, TotADC, RMS);
                  RMSst5s5->Fill(RMS);
                  Wst5s5->Fill(ndigits);
                }
              if (station == 6 && sensor == 0)
                {
                  //plane 15
		  st6s0_3D->Fill(ndigits, TotADC, RMS);
                  RMSst6s0->Fill(RMS);
                  Wst6s0->Fill(ndigits);
                }
              if (station == 6 && sensor == 1)
                {
                  //plane 15
		  st6s1_3D->Fill(ndigits, TotADC, RMS);
                  RMSst6s1->Fill(RMS);
                  Wst6s1->Fill(ndigits);
                }
              if (station == 6 && sensor == 2)
                {
                  //plane 16
		  st6s2_3D->Fill(ndigits, TotADC, RMS);
                  RMSst6s2->Fill(RMS);
                  Wst6s2->Fill(ndigits);
                }
              if (station == 6 && sensor == 3)
                {
                  //plane 16
		  st6s3_3D->Fill(ndigits, TotADC, RMS);
                  RMSst6s3->Fill(RMS);
                  Wst6s3->Fill(ndigits);
                }
	      if (station == 6 && sensor == 4)
                {
                  //plane 17
		  st6s4_3D->Fill(ndigits, TotADC, RMS);
                  RMSst6s4->Fill(RMS);
                  Wst6s4->Fill(ndigits);
                }
              if (station == 6 && sensor == 5)
                {
                  //plane 17
		  st6s5_3D->Fill(ndigits, TotADC, RMS);
                  RMSst6s5->Fill(RMS);
                  Wst6s5->Fill(ndigits);
                }
            }
	}
    } // end loop over data entries
  
  // Normalize RMS histograms (find fractions) 
  double totEntriesRMS = clustRMS->Integral();
  nclustRMS->Add(clustRMS, 1.0 / totEntriesRMS);

  double t0 = RMSst0s0->Integral();
  nRMSst0s0->Add(RMSst0s0, 1.0 / t0);

  double t1 = RMSst0s1->Integral();
  nRMSst0s1->Add(RMSst0s1, 1.0 / t1);

  double t2 = RMSst1s0->Integral();
  nRMSst1s0->Add(RMSst1s0, 1.0 / t2);

  double t3 = RMSst1s1->Integral();
  nRMSst1s1->Add(RMSst1s1, 1.0 / t3);

  double t4 = RMSst2s0->Integral();
  nRMSst2s0->Add(RMSst2s0, 1.0 / t4);

  double t5 = RMSst2s1->Integral();
  nRMSst2s1->Add(RMSst2s1, 1.0 / t5);

  double t6 = RMSst2s2->Integral();
  nRMSst2s2->Add(RMSst2s2, 1.0 / t6);

  double t7 = RMSst3s0->Integral();
  nRMSst3s0->Add(RMSst3s0, 1.0 / t7);

  double t8 = RMSst3s1->Integral();
  nRMSst3s1->Add(RMSst3s1, 1.0 / t8);

  double t9 = RMSst3s2->Integral();
  nRMSst3s2->Add(RMSst3s2, 1.0 / t9);

  double t10 = RMSst4s0->Integral();
  nRMSst4s0->Add(RMSst4s0, 1.0 / t10);

  double t11 = RMSst4s1->Integral();
  nRMSst4s1->Add(RMSst4s1, 1.0 / t11);

  double t12 = RMSst5s0->Integral();
  nRMSst5s0->Add(RMSst5s0, 1.0 / t12);

  double t13 = RMSst5s1->Integral();
  nRMSst5s1->Add(RMSst5s1, 1.0 / t13);

  double t14 = RMSst5s2->Integral();
  nRMSst5s2->Add(RMSst5s2, 1.0 / t14);

  double t15 = RMSst5s3->Integral();
  nRMSst5s3->Add(RMSst5s3, 1.0 / t15);

  double t16 = RMSst5s4->Integral();
  nRMSst5s4->Add(RMSst5s4, 1.0 / t16);

  double t17 = RMSst5s5->Integral();
  nRMSst5s5->Add(RMSst5s5, 1.0 / t17);

  double t18 = RMSst6s0->Integral();
  nRMSst6s0->Add(RMSst6s0, 1.0 / t18);

  double t19 = RMSst6s1->Integral();
  nRMSst6s1->Add(RMSst6s1, 1.0 / t19);

  double t20 = RMSst6s2->Integral();
  nRMSst6s2->Add(RMSst6s2, 1.0 / t20);

  double t21 = RMSst6s3->Integral();
  nRMSst6s3->Add(RMSst6s3, 1.0 / t21);

  double t22 = RMSst6s4->Integral();
  nRMSst6s4->Add(RMSst6s4, 1.0 / t22);

  double t23 = RMSst6s5->Integral();
  nRMSst6s5->Add(RMSst6s5, 1.0 / t23);

  // Normalize Width histograms 
  double totEntriesWidth = width->Integral();
  nwidth->Add(width, 1.0 / totEntriesWidth);

  double tot0 = Wst0s0->Integral();
  nWst0s0->Add(Wst0s0, 1.0 / tot0);

  double tot1 = Wst0s1->Integral();
  nWst0s1->Add(Wst0s1, 1.0 / tot1);

  double tot2 = Wst1s0->Integral();
  nWst1s0->Add(Wst1s0, 1.0 / tot2);

  double tot3 = Wst1s1->Integral();
  nWst1s1->Add(Wst1s1, 1.0 / tot3);

  double tot4 = Wst2s0->Integral();
  nWst2s0->Add(Wst2s0, 1.0 / tot4);

  double tot5 = Wst2s1->Integral();
  nWst2s1->Add(Wst2s1, 1.0 / tot5);
 
  double tot6 = Wst2s2->Integral();
  nWst2s2->Add(Wst2s2, 1.0 / tot6);

  double tot7 = Wst3s0->Integral();
  nWst3s0->Add(Wst3s0, 1.0 / tot7);

  double tot8 = Wst3s1->Integral();
  nWst3s1->Add(Wst3s1, 1.0 / tot8);

  double tot9 = Wst3s2->Integral();
  nWst3s2->Add(Wst3s2, 1.0 / tot9);

  double tot10 = Wst4s0->Integral();
  nWst4s0->Add(Wst4s0, 1.0 / tot10);

  double tot11 = Wst4s1->Integral();
  nWst4s1->Add(Wst4s1, 1.0 / tot11);

  double tot12 = Wst5s0->Integral();
  nWst5s0->Add(Wst5s0, 1.0 / tot12);

  double tot13 = Wst5s1->Integral();
  nWst5s1->Add(Wst5s1, 1.0 / tot13);

  double tot14 = Wst5s2->Integral();
  nWst5s2->Add(Wst5s2, 1.0 / tot14);

  double tot15 = Wst5s3->Integral();
  nWst5s3->Add(Wst5s3, 1.0 / tot15);

  double tot16 = Wst5s4->Integral();
  nWst5s4->Add(Wst5s4, 1.0 / tot16);

  double tot17 = Wst5s5->Integral();
  nWst5s5->Add(Wst5s5, 1.0 / tot17);

  double tot18 = Wst6s0->Integral();
  nWst6s0->Add(Wst6s0, 1.0 / tot18);

  double tot19 = Wst5s1->Integral();
  nWst6s1->Add(Wst6s1, 1.0 / tot19);

  double tot20 = Wst5s2->Integral();
  nWst6s2->Add(Wst6s2, 1.0 / tot20);

  double tot21 = Wst5s3->Integral();
  nWst6s3->Add(Wst6s3, 1.0 / tot21);

  double tot22 = Wst6s4->Integral();
  nWst6s4->Add(Wst6s4, 1.0 / tot22);

  double tot23 = Wst5s5->Integral();
  nWst6s5->Add(Wst6s5, 1.0 / tot23);

  //write out the histograms for everything and the individual sensors in
  //their corresponding root files, then save and close them
  TotalFile->Write();
  TotalFile->Close();

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
}
