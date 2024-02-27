//Macro6: Identifies good events (exactly one cluster per plane) in MC data and plot SSDHit DE 
//Date: Feb 27, 2024

void macro6()
{
  gROOT->SetBatch(); //stop hists from popping up 

  //Opening a ROOT file and getting a tree
  TFile *file = new TFile("emphmc_r2098_s9_dig.caf.root", "read");
  TTree *tree = (TTree*)file->Get("recTree");

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  const int planeCount = 18; //0 to 17  
  const int stationCount = 8; 
  int sensorCount[] = {2, 2, 3, 3, 2, 6, 6, 4}; //sensors per station

  //Histogram initialization
  std::vector<std::vector<TH1F*>> energyHists;
  for (int station = 0; station < stationCount; ++station) {
    std::vector<TH1F*> sensorHists;	
      for (int sensor = 0; sensor < sensorCount[station]; ++sensor) {
	sensorHists.push_back(new TH1F(Form("hist_%d%d", station, sensor), Form("Station %d: Sensor %d; Deposited Energy; Number of Entries", station, sensor), 100, 0, 0.001));
      }	
    energyHists.push_back(sensorHists);		
  }
  
  //Looping over tree entries
  for (int i = 0; i < tree->GetEntries(); ++i) { //tree->GetEntries() = total number of entries (events) in a tree
    tree->GetEntry(i); //loading the i-th event data for processing 
    int clusterCount[planeCount] = {0}; //number of cluster per plane for the selected event
    int clusterSize = rec->cluster.clust.size(); //number of cluster(s) for selected event from the loop

    //Looping over number of clusters -> Counting clusters of current plane for the current event  
    for(int idx = 0; idx < clusterSize; ++idx) {
      int plane = rec->cluster.clust[idx].plane;
      clusterCount[plane]++;
    }

    //Good event selection
    bool allGoodClusters = true;
    for (int iPlane = 0; iPlane < planeCount ; ++iPlane) { 
      if (clusterCount[iPlane] != 1) {
	allGoodClusters = false;
	break;
      }
    }
				      
    if (allGoodClusters) {
      for(int icluster = 0; icluster < clusterSize; ++icluster) {
	int station = rec->cluster.clust[icluster].station;
	int sensor = rec->cluster.clust[icluster].sens;
	float dEnergy = rec->truth.truehits.truehits[icluster].GetDE;
	energyHists[station][sensor]->Fill(dEnergy);
      }
    }
  }		
/*
  //Probability distribution normalization
  for (int station = 0; station < energyHists.size(); ++station) {
    for (int sensor = 0; sensor < energyHists[station].size(); ++sensor) {
      TH1F* hist = energyHists[station][sensor];
      int totalCluster = hist->GetEntries();
      if(totalCluster > 0) {
	hist->Scale(1.0 / totalCluster);		  
      }
    }	
  }
*/
  TFile *outFile = new TFile("hist_output.root", "RECREATE");

  //Drawing histograms
  for (int station = 0; station < energyHists.size(); ++station) {
    for (int sensor = 0; sensor < energyHists[station].size(); ++sensor) {
      energyHists[station][sensor]->Write();
      TCanvas *c = new TCanvas(Form("c%d%d", station, sensor), Form("c%d%d", station, sensor), 1600, 1400);
      energyHists[station][sensor]->Draw();
      c->Print(Form("c%d%d.png", station, sensor));
      delete energyHists[station][sensor]; //Memory clear up
      delete c;
    }
  }

  file->Close();
}
