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
#include <cstdlib>
#include <ctime> 
#include "TGraph.h"
#include <tuple>

std::vector<std::tuple<double, double, int>> GetDEGetXsensor;

std::vector<std::tuple<double, double, int>> RandomHit()
{
  //Create a TChains  
  TChain *MCchain = new TChain("recTree");

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
  caf::StandardRecord* recMC = 0;
  MCchain->SetBranchAddress("rec", &recMC);

  // Initalize the number of good MC events 
  int n_good_MC_events = 0;

  /// Counter for total number of DE clusters
  int TotalDEClustersMC = 0;

  double GetDE;
  double GetX;
  int sensor;

  // Seed the random number generator
  std::srand(std::time(0));

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

      // Check if the event is in sensors 0-23
      if (good_MC_event)
	{
	  TotalDEClustersMC += ntruehits;
	  n_good_MC_events++;
	  
	  // Choose a random truehit index from the good events
	  int randomIndex = std::rand() % ntruehits;
	  
	  // Access the random truehit
	  GetX = recMC->truth.truehits.truehits[randomIndex].GetX;
	  GetDE = recMC->truth.truehits.truehits[randomIndex].GetDE;
	  sensor = recMC->truth.truehits.truehits[randomIndex].GetSensor;
	  
	  while (!(sensor >= 0 && sensor <= 23))
	    {
	      //if the random hit is not in one of the sensors it can be in, choose another
	      randomIndex = std::rand() % ntruehits;
	      GetX = recMC->truth.truehits.truehits[randomIndex].GetX;
	      GetDE = recMC->truth.truehits.truehits[randomIndex].GetDE;
	      sensor = recMC->truth.truehits.truehits[randomIndex].GetSensor;
            }
	  //store the first valid hit info
	  GetDEGetXsensor.push_back(std::make_tuple(GetDE, GetX, sensor));
	}
    } // end loop over MC entries
  
  std::cout << "Random TrueHit:" << std::endl;
  std::cout << "GetX: " << GetX << std::endl;
  std::cout << "GetDE: " << GetDE << std::endl;
  std::cout << "Sensor: " << sensor << std::endl;
  
  return GetDEGetXsensor;
}
void Sim()
{
  GetDEGetXsensor = RandomHit();
}
