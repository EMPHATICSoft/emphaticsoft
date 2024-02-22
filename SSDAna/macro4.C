//Macro4: Identify good events (exactly one cluster per plane) and plot total cluster ADC separately for each sensor. 
//Date: Feb 22, 2024

void macro4()
{
  gROOT->SetBatch(); //stop hists from popping up 

  //Opening a ROOT file and getting a tree
  TFile *file = new TFile("emphdata_v03_02a_r2098_s9.caf.root", "read");
  TTree *tree = (TTree*)file->Get("recTree");

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  const int planeCount = 18; //0 to 17  
  const int stationCount = 8; 
  int sensorCount[] = {2, 2, 3, 3, 2, 6, 6, 4}; //sensors per station

  //Histogram initialization
  std::vector<std::vector<TH1F*>> adcHists;
  for (int station = 0; station < stationCount; ++station) {
    std::vector<TH1F*> sensorHists;	
      for (int sensor = 0; sensor < sensorCount[station]; ++sensor) {
	sensorHists.push_back(new TH1F(Form("hist%d,%d", sensor, station), Form("Station %d: Sensor %d; Total ADC; Number of cluster", station, sensor), 200, 0, 200));
      }	
    adcHists.push_back(sensorHists);		
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
	adcHists[station][sensor]->Fill(rec->cluster.clust[icluster].avgadc);
      }
    }
  }		

  for (int station = 0; station < adcHists.size(); ++station) {
    for (int sensor = 0; sensor < adcHists[station].size(); ++sensor) {
			      
      //Drawing and saving histogram 
      TCanvas *c = new TCanvas(Form("c%d%d", station, sensor), Form("c%d%d", station, sensor), 1600, 1400);
      adcHists[station][sensor]->Draw();
      c->Print(Form("c%d%d.png", station, sensor));
      delete adcHists[station][sensor]; //Memory clear up
      delete c;
    }
  }

  file->Close();
}
