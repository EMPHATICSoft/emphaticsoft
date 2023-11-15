///////////////////////////////////////////////////////////////////////////// 
// Description: This macro  identifies good events (events with exactly    //
// one cluster per plane) for a chain of data files                        //
// and eventually width as well. Using phase 1c geometry.                  //
//                                                                         //
//Date: November 10, 2023                                                  //
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
  TFile* TotalFile = new TFile("Total.root", "RECREATE");
  TH1D *clustRMS = new TH1D ("RMS", "RMS; RMS; clusters per bin", 100, -0.5, 5);
  TH1D *width = new TH1D ("width", "Width; width; clusters per bin", 36, 0.5, 18.5);
  TH3D *WidthVsTADCVsRMS = new TH3D("WidthVsTADCVsRMS", "Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir0->cd();
  TFile* sensor0File = new TFile("Sensor0.root", "RECREATE");
  TH1F *RMSst0s0 = new TH1F ("RMS", "station 0, sensor 0; RMS; clusters per bin", 100, -0.5, 5); 
  TH1F *Wst0s0 = new TH1F ("width", "station 0, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st0s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 0, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir1->cd();
  TFile* sensor1File = new TFile("Sensor1.root", "RECREATE");
  TH1F *RMSst0s1 = new TH1F ("RMS", "station 0, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst0s1 = new TH1F ("width", "station 0, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st0s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 0, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir2->cd();
  TFile* sensor2File = new TFile("Sensor2.root", "RECREATE");
  TH1F *RMSst1s0 = new TH1F ("RMS", "station 1, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst1s0 = new TH1F ("width", "station 1, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st1s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 1, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir3->cd();
  TFile* sensor3File = new TFile("Sensor3.root", "RECREATE");
  TH1F *RMSst1s1 = new TH1F ("RMS", "station 1, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst1s1 = new TH1F ("width", "station 1, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st1s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 1, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir4->cd();
  TFile* sensor4File = new TFile("Sensor4.root", "RECREATE");
  TH1F *RMSst2s0 = new TH1F ("RMS", "station 2, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst2s0 = new TH1F ("width", "station 2, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st2s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 2, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir5->cd();
  TFile* sensor5File = new TFile("Sensor5.root", "RECREATE");
  TH1F *RMSst2s1 = new TH1F ("RMS", "station 2, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst2s1 = new TH1F ("width", "station 2, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st2s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 2, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir6->cd();
  TFile* sensor6File = new TFile("Sensor6.root", "RECREATE");
  TH1F *RMSst2s2 = new TH1F ("RMS", "station 2, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst2s2 = new TH1F ("width", "station 2, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st2s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 2, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir7->cd();
  TFile* sensor7File = new TFile("Sensor7.root", "RECREATE");
  TH1F *RMSst3s0 = new TH1F ("RMS", "station 3, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst3s0 = new TH1F ("width", "station 3, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st3s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 3, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir8->cd();
  TFile* sensor8File = new TFile("Sensor8.root", "RECREATE");
  TH1F *RMSst3s1 = new TH1F ("RMS", "station 3, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst3s1 = new TH1F ("width", "station 3, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st3s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 3, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir9->cd();
  TFile* sensor9File = new TFile("Sensor9.root", "RECREATE");
  TH1F *RMSst3s2 = new TH1F ("RMS", "station 3, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst3s2 = new TH1F ("width", "station 3, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st3s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 3, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir10->cd();
  TFile* sensor10File = new TFile("Sensor10.root", "RECREATE");
  TH1F *RMSst4s0 = new TH1F ("RMS", "station 4, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst4s0 = new TH1F ("width", "station 4, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st4s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 4, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir11->cd();
  TFile* sensor11File = new TFile("Sensor11.root", "RECREATE");
  TH1F *RMSst4s1 = new TH1F ("RMS", "station 4, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst4s1 = new TH1F ("width", "station 4, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st4s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 4, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir12->cd();
  TFile* sensor12File = new TFile("Sensor12.root", "RECREATE");
  TH1F *RMSst5s0 = new TH1F ("RMS", "station 5, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s0 = new TH1F ("width", "station 5, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir13->cd();
  TFile* sensor13File = new TFile("Sensor13.root", "RECREATE");
  TH1F *RMSst5s1 = new TH1F ("RMS", "station 5, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s1 = new TH1F ("width", "station 5, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir14->cd();
  TFile* sensor14File = new TFile("Sensor14.root", "RECREATE");
  TH1F *RMSst5s2 = new TH1F ("RMS", "station 5, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s2 = new TH1F ("width", "station 5, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir15->cd();
  TFile* sensor15File = new TFile("Sensor15.root", "RECREATE");
  TH1F *RMSst5s3 = new TH1F ("RMS", "station 5, sensor 3; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s3 = new TH1F ("width", "station 5, sensor 3; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s3_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 3; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir16->cd();
  TFile* sensor16File = new TFile("Sensor16.root", "RECREATE");
  TH1F *RMSst5s4 = new TH1F ("RMS", "station 5, sensor 4; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s4 = new TH1F ("width", "station 5, sensor 4; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s4_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 4; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir17->cd();
  TFile* sensor17File = new TFile("Sensor17.root", "RECREATE");
  TH1F *RMSst5s5 = new TH1F ("RMS", "station 5, sensor 5; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst5s5 = new TH1F ("width", "station 5, sensor 5; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st5s5_3D = new TH3F ("WidthVsTADCVsRMS", "station 5, sensor 5; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir18->cd();
  TFile* sensor18File = new TFile("Sensor18.root", "RECREATE");
  TH1F *RMSst6s0 = new TH1F ("RMS", "station 6, sensor 0; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s0 = new TH1F ("width", "station 6, sensor 0; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s0_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 0; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir19->cd();
  TFile* sensor19File = new TFile("Sensor19.root", "RECREATE");
  TH1F *RMSst6s1 = new TH1F ("RMS", "station 6, sensor 1; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s1 = new TH1F ("width", "station 6, sensor 1; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s1_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 1; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir20->cd();
  TFile* sensor20File = new TFile("Sensor20.root", "RECREATE");
  TH1F *RMSst6s2 = new TH1F ("RMS", "station 6, sensor 2; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s2 = new TH1F ("width", "station 6, sensor 2; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s2_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 2; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir21->cd();
  TFile* sensor21File = new TFile("Sensor21.root", "RECREATE");
  TH1F *RMSst6s3 = new TH1F ("RMS", "station 6, sensor 3; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s3 = new TH1F ("width", "station 6, sensor 3; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s3_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 3; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir22->cd();
  TFile* sensor22File = new TFile("Sensor22.root", "RECREATE");
  TH1F *RMSst6s4 = new TH1F ("RMS", "station 6, sensor 4; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s4 = new TH1F ("width", "station 6, sensor 4; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s4_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 4; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  Dir23->cd();
  TFile* sensor23File = new TFile("Sensor23.root", "RECREATE");
  TH1F *RMSst6s5 = new TH1F ("RMS", "station 6, sensor 5; RMS; clusters per bin", 100, -0.5, 5);
  TH1F *Wst6s5 = new TH1F ("width", "station 6, sensor 5; width; clusters per bin", 36, 0.5, 18.5);
  TH3F *st6s5_3D = new TH3F ("WidthVsTADCVsRMS", "station 6, sensor 5; Width Vs TotADC Vs RMS", 18, 0.5, 18.5, 809, 41, 850, 50, 0, 5);

  gDirectory->cd();

  // Initalize the number of good data events to 0 
  int n_good_data_events = 0;

  /// Counter for total number of ADC clusters                                            
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
          TotalADCClustersData += nclusts;
          n_good_data_events++;

          for (int idx = 0; idx < nclusts; ++idx)
            {
	      int sensor = recData->cluster.clust[idx].sens;
              int station = recData->cluster.clust[idx].station;

              double AvgADC = recData->cluster.clust[idx].avgadc;
              TotADC = ndigits*AvgADC;
	      RMS = recData->cluster.clust[idx].wgtrmsstrip;
	      WidthVsTADCVsRMS->Fill(ndigits, TotADC, RMS);
	      clustRMS->Fill(RMS);
	      width->Fill(ndigits);

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
  /*
  TCanvas *c1 = new TCanvas("c1","c1",1000,500);
  WidthVsTADCVsRMS->GetXaxis()->SetTitle("Width");
  WidthVsTADCVsRMS->GetYaxis()->SetTitle("Total ADC");
  WidthVsTADCVsRMS->GetZaxis()->SetTitle("Cluster RMS");
  WidthVsTADCVsRMS->SetMarkerSize(1.0);

  WidthVsTADCVsRMS->Draw("LEGO2");

  TCanvas *c2 = new TCanvas("c2","c2",1000,500);
  clustRMS->SetFillColor(kBlue);       
  clustRMS->Draw("colz");
  //  gPad->SetLogy();

  TCanvas *c3 = new TCanvas("c3","c3",1000,500);
  width->SetFillColor(kBlue);
  width->Draw("colz");
  //  gPad->SetLogy();

  TFile  WidthVsTADCVsRMSFile("3Dhist.root", "RECREATE");
  WidthVsTADCVsRMS->Write();
  WidthVsTADCVsRMSFile.Close();
  */
  /*
  //RMS histograms
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);                                               
  c3->Divide(3,3);                                                                             
                                                                                               
  c3->cd(1);                                                                                    
  RMSst0s0->SetFillColor(kBlue);                                                                   
  RMSst0s0->Draw("colz");                                                                            
                                                                                                 
  c3->cd(2);                                                                                  
  RMSst0s1->SetFillColor(kBlue);                                                                    
  RMSst0s1->Draw("colz");                                                                          
                                                                                                 
  c3->cd(3);                                                                                     
  RMSst1s0->SetFillColor(kBlue);                                                                 
  RMSst1s0->Draw("colz");                                                                         
                                                                                                  
  c3->cd(4);                                                                                   
  RMSst1s1->SetFillColor(kBlue);                                                                
  RMSst1s1->Draw("colz");                                                                             
      
  c3->cd(5);                                                                                     
  RMSst2s0->SetFillColor(kBlue);                                                                    
  RMSst2s0->Draw("colz");                                                                          
                                                                                                
  c3->cd(6);                                                                                    
  RMSst2s1->SetFillColor(kBlue);                                                                 
  RMSst2s1->Draw("colz");                                                                         
                                                                                                 
  c3->cd(7);                                                                                   
  RMSst2s2->SetFillColor(kBlue);                                                                    
  RMSst2s2->Draw("colz");                                                                          
                                                                                                 
  c3->cd(8);                                                                                  
  RMSst3s0->SetFillColor(kBlue);                                                                  
  RMSst3s0->Draw("colz");                                                                           
                                                                                                   
  c3->cd(9);                                                                                    
  RMSst3s1->SetFillColor(kBlue);                                                                   
  RMSst3s1->Draw("colz");                                                                       
                                                                                                  
  TCanvas *c4 = new TCanvas("c4","c4",2000,1500);                                               
  c4->Divide(3,3);                                                                                 
                                                                                                  
  c4->cd(1);                                                                                     
  RMSst3s2->SetFillColor(kBlue);                                                                     
  RMSst3s2->Draw("colz");                                                                          
                                                                                                 
  c4->cd(2);                                                                                     
  RMSst4s0->SetFillColor(kBlue);                                                                
  RMSst4s0->Draw("colz");                                                                          
  
  c4->cd(3);                                                                                    
  RMSst4s1->SetFillColor(kBlue); 
  RMSst4s1->Draw("colz");                                                                       
                                                                                                  
  c4->cd(4);                                                                                       
  RMSst5s0->SetFillColor(kBlue);                                                               
  RMSst5s0->Draw("colz");        

  c4->cd(5);                                                                                       
  RMSst5s1->SetFillColor(kBlue);                                                                      
  RMSst5s1->Draw("colz");                                                                            
                                                                                                  
  c4->cd(6);                                                                                       
  RMSst5s2->SetFillColor(kBlue);                                                                     
  RMSst5s2->Draw("colz");                                                                           
                                                                                                   
  c4->cd(7);                                                                                     
  RMSst5s3->SetFillColor(kBlue);                                                                     
  RMSst5s3->Draw("colz");                                                                           
                                                                                                   
  c4->cd(8);                                                                                       
  RMSst5s4->SetFillColor(kBlue);                                                                      
  RMSst5s4->Draw("colz");                                                                             
                                                                                                  
  c4->cd(9);                                                                                       
  RMSst5s5->SetFillColor(kBlue);                                                                     
  RMSst5s5->Draw("colz");                                                                             
                                                                                                  
  TCanvas *c5 = new TCanvas("c5","c5",2000,1500);                                                
  c5->Divide(3,3);                                                                                
                                                                                                 
  c5->cd(1);                                                                                    
  RMSst6s0->SetFillColor(kBlue);                                                                   
  RMSst6s0->Draw("colz");                                            
                       
  c5->cd(2);                                                                                     
  RMSst6s1->SetFillColor(kBlue);                                                                  
  RMSst6s1->Draw("colz");        

  c5->cd(3);                                                                                 
  RMSst6s2->SetFillColor(kBlue);                                                                   
  RMSst6s2->Draw("colz");                                        
                                                                                           
  c5->cd(4);
  RMSst6s3->SetFillColor(kBlue);
  RMSst6s3->Draw("colz");
                                                                                                          
  c5->cd(5);
  RMSst6s4->SetFillColor(kBlue);
  RMSst6s4->Draw("colz");
                                                                                                   
  c5->cd(6);                                                                                      
  RMSst6s5->SetFillColor(kBlue);                                                                     
  RMSst6s5->Draw("colz");
  */

  /*
  //Width histograms 
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);
  c3->Divide(3,3);

  c3->cd(1);
  Wst0s0->SetFillColor(kBlue);
  Wst0s0->Draw("colz");

  c3->cd(2);
  Wst0s1->SetFillColor(kBlue);
  Wst0s1->Draw("colz");

  c3->cd(3);
  Wst1s0->SetFillColor(kBlue);
  Wst1s0->Draw("colz");

  c3->cd(4);
  Wst1s1->SetFillColor(kBlue);
  Wst1s1->Draw("colz");

  c3->cd(5);
  Wst2s0->SetFillColor(kBlue);
  Wst2s0->Draw("colz");

  c3->cd(6);
  Wst2s1->SetFillColor(kBlue);
  Wst2s1->Draw("colz");

  c3->cd(7);
  Wst2s2->SetFillColor(kBlue);
  Wst2s2->Draw("colz");

  c3->cd(8);
  Wst3s0->SetFillColor(kBlue);
  Wst3s0->Draw("colz");

  c3->cd(9);
  Wst3s1->SetFillColor(kBlue);
  Wst3s1->Draw("colz");

  TCanvas *c4 = new TCanvas("c4","c4",2000,1500);
  c4->Divide(3,3);

  c4->cd(1);
  Wst3s2->SetFillColor(kBlue);
  Wst3s2->Draw("colz");

  c4->cd(2);
  Wst4s0->SetFillColor(kBlue);
  Wst4s0->Draw("colz");

  c4->cd(3);
  Wst4s1->SetFillColor(kBlue);
  Wst4s1->Draw("colz");

  c4->cd(4);
  Wst5s0->SetFillColor(kBlue);
  Wst5s0->Draw("colz");

  c4->cd(5);
  Wst5s1->SetFillColor(kBlue);
  Wst5s1->Draw("colz");

  c4->cd(6);
  Wst5s2->SetFillColor(kBlue);
  Wst5s2->Draw("colz");

  c4->cd(7);
  Wst5s3->SetFillColor(kBlue);
  Wst5s3->Draw("colz");

  c4->cd(8);
  Wst5s4->SetFillColor(kBlue);
  Wst5s4->Draw("colz");

  c4->cd(9);
  Wst5s5->SetFillColor(kBlue);
  Wst5s5->Draw("colz");

  TCanvas *c5 = new TCanvas("c5","c5",2000,1500);
  c5->Divide(3,3);

  c5->cd(1);
  Wst6s0->SetFillColor(kBlue);
  Wst6s0->Draw("colz");

  c5->cd(2);
  Wst6s1->SetFillColor(kBlue);
  Wst6s1->Draw("colz");

  c5->cd(3);
  Wst6s2->SetFillColor(kBlue);
  Wst6s2->Draw("colz");

  c5->cd(4);
  Wst6s3->SetFillColor(kBlue);
  Wst6s3->Draw("colz");

  c5->cd(5);
  Wst6s4->SetFillColor(kBlue);
  Wst6s4->Draw("colz");

  c5->cd(6);
  Wst6s5->SetFillColor(kBlue);
  Wst6s5->Draw("colz");

  //3D hists
  TCanvas *c3 = new TCanvas("c3","c3",2000,1500);
  c3->Divide(3,3);                                                                               
                                                                                                   
  c3->cd(1);                                                                                      
  st0s0_3D->SetFillColor(kBlue);                                                                     
  st0s0_3D->Draw("colz");                                                                            
                                                                                               
  c3->cd(2);                                                                                 
  st0s1_3D->SetFillColor(kBlue);                                                                       
  st0s1_3D->Draw("colz");                                                                         
                                                                                                 
  c3->cd(3);                                                                                    
  st1s0_3D->SetFillColor(kBlue);                                                                    
  st1s0_3D->Draw("colz");                                                                            
                                                                                                           
  c3->cd(4);                                                                                     
  st1s1_3D->SetFillColor(kBlue);                                                                      
  st1s1_3D->Draw("colz");                                                                         
                                                                                                 
  c3->cd(5);                                                                                    
  st2s0_3D->SetFillColor(kBlue);                                                                   
  st2s0_3D->Draw("colz");                                                                         
                                                                                                 
  c3->cd(6);                                                                                           
  st2s1_3D->SetFillColor(kBlue);                                                                    
  st2s1_3D->Draw("colz");                                                                             
                                                                                                    
  c3->cd(7);                                                                                      
  st2s2_3D->SetFillColor(kBlue);                                                                       
  st2s2_3D->Draw("colz");                                                                              
                                                                                                    
  c3->cd(8);                                                                                     
  st3s0_3D->SetFillColor(kBlue);                                                                         
  st3s0_3D->Draw("colz");                                                                             
                                                                                                  
  c3->cd(9);                                                                                     
  st3s1_3D->SetFillColor(kBlue);                                                                      
  st3s1_3D->Draw("colz");                                                                             
                                                                                                   
  TCanvas *c4 = new TCanvas("c4","c4",2000,1500);                                                 
  c4->Divide(3,3);                                                                               
                                                                                                   
  c4->cd(1);                                                                                     
  st3s2_3D->SetFillColor(kBlue);                                                                    
  st3s2_3D->Draw("colz");                                                                           
                                                                                                  
  c4->cd(2);                                                                                 
  st4s0_3D->SetFillColor(kBlue);                                                                    
  st4s0_3D->Draw("colz");                                                                       
                                                                                                     
  c4->cd(3);                                                                                       
  st4s1_3D->SetFillColor(kBlue);                                                                   
  st4s1_3D->Draw("colz");                                                                          
                                                                                                 
  c4->cd(4);                                                                                     
  st5s0_3D->SetFillColor(kBlue);                                                                  
  st5s0_3D->Draw("colz");                                                                            
                                                                                                  
  c4->cd(5);                                                                                     
  st5s1_3D->SetFillColor(kBlue);                                                                  
  st5s1_3D->Draw("colz");                                                                         
                                                                                                
  c4->cd(6);                                                                                      
  st5s2_3D->SetFillColor(kBlue);                                                              
  st5s2_3D->Draw("colz"); 

  c4->cd(7);
  st5s3_3D->SetFillColor(kBlue);
  st5s3_3D->Draw("colz");

  c4->cd(8);
  st5s4_3D->SetFillColor(kBlue);
  st5s4_3D->Draw("colz");

  c4->cd(9);
  st5s5_3D->SetFillColor(kBlue);
  st5s5_3D->Draw("colz");

  TCanvas *c5 = new TCanvas("c5","c5",2000,1500);
  c5->Divide(3,3);

  c5->cd(1);
  st6s0_3D->SetFillColor(kBlue);
  st6s0_3D->Draw("colz");

  c5->cd(2);
  st6s1_3D->SetFillColor(kBlue);
  st6s1_3D->Draw("colz");

  c5->cd(3);
  st6s2_3D->SetFillColor(kBlue);
  st6s2_3D->Draw("colz");

  c5->cd(4);
  st6s3_3D->SetFillColor(kBlue);
  st6s3_3D->Draw("colz");

  c5->cd(5);
  st6s4_3D->SetFillColor(kBlue);
  st6s4_3D->Draw("colz");

  c5->cd(6);
  st6s5_3D->SetFillColor(kBlue);
  st6s5_3D->Draw("colz");

  // Create separate output files for needed histograms
  TFile st0s0_3DFile("st0s0_3D.root", "RECREATE");
  st0s0_3D->Write();
  st0s0_3DFile.Close();

  TFile st0s1_3DFile("st0s1_3D.root", "RECREATE");
  st0s1_3D->Write();
  st0s1_3DFile.Close();

  TFile st1s0_3DFile("st1s0_3D.root", "RECREATE");
  st1s0_3D->Write();
  st1s0_3DFile.Close();

  TFile st1s1_3DFile("st1s1_3D.root", "RECREATE");
  st1s1_3D->Write();
  st1s1_3DFile.Close();

  TFile st2s0_3DFile("st2s0_3D.root", "RECREATE");
  st2s0_3D->Write();
  st2s0_3DFile.Close();

  TFile st2s1_3DFile("st2s1_3D.root", "RECREATE");
  st2s1_3D->Write();
  st2s1_3DFile.Close();

  TFile st2s2_3DFile("st2s2_3D.root", "RECREATE");
  st2s2_3D->Write();
  st2s2_3DFile.Close();

  TFile st3s0_3DFile("st3s0_3D.root", "RECREATE");
  st3s0_3D->Write();
  st3s0_3DFile.Close();

  TFile st3s1_3DFile("st3s1_3D.root", "RECREATE");
  st3s1_3D->Write();
  st3s1_3DFile.Close();

  TFile st3s2_3DFile("st3s2_3D.root", "RECREATE");
  st3s2_3D->Write();
  st3s2_3DFile.Close();

  TFile st4s0_3DFile("st4s0_3D.root", "RECREATE");
  st4s0_3D->Write();
  st4s0_3DFile.Close();

  TFile st4s1_3DFile("st4s1_3D.root", "RECREATE");
  st4s1_3D->Write();
  st4s1_3DFile.Close();

  TFile st5s0_3DFile("st5s0_3D.root", "RECREATE");
  st5s0_3D->Write();
  st5s0_3DFile.Close();

  TFile st5s1_3DFile("st5s1_3D.root", "RECREATE");
  st5s1_3D->Write();
  st5s1_3DFile.Close();

  TFile st5s2_3DFile("st5s2_3D.root", "RECREATE");
  st5s2_3D->Write();
  st5s2_3DFile.Close();

  TFile st5s3_3DFile("st5s3_3D.root", "RECREATE");
  st5s3_3D->Write();
  st5s3_3DFile.Close();

  TFile st5s4_3DFile("st5s4_3D.root", "RECREATE");
  st5s4_3D->Write();
  st5s4_3DFile.Close();

  TFile st5s5_3DFile("st5s5_3D.root", "RECREATE");
  st5s5_3D->Write();
  st5s5_3DFile.Close();

  TFile st6s0_3DFile("st6s0_3D.root", "RECREATE");
  st6s0_3D->Write();
  st6s0_3DFile.Close();

  TFile st6s1_3DFile("st6s1_3D.root", "RECREATE");
  st6s1_3D->Write();
  st6s1_3DFile.Close();

  TFile st6s2_3DFile("st6s2_3D.root", "RECREATE");
  st6s2_3D->Write();
  st6s2_3DFile.Close();

  TFile st6s3_3DFile("st6s3_3D.root", "RECREATE");
  st6s3_3D->Write();
  st6s3_3DFile.Close();

  TFile st6s4_3DFile("st6s4_3D.root", "RECREATE");
  st6s4_3D->Write();
  st6s4_3DFile.Close();

  TFile st6s5_3DFile("st6s5_3D.root", "RECREATE");
  st6s5_3D->Write();
  st6s5_3DFile.Close();
*/

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
