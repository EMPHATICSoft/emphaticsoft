bool goodMap(int istation, int iplane)
{ 
  vector <pair<int, int>>map = {{0,2}, {1,2}, {4,2}, {7,2}};
  pair <int, int>check = {istation, iplane};
  
  for (int i = 0; i < map.size(); ++i) {
    if (check == map[i]) {
    return false;
    }
  }
 return true;
}

void rms()
{
  TFile *file = new TFile("emphdata_v04.00_r2252_s9.artdaq.caf.root", "READ"); 
  TTree *tree = (TTree*)file->Get("recTree");

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  const int stationCount = 8; // 0 to 6 for run before 2098
  const int planeCount = 3; //numbering 0, 1 or 2
  
  TH3F *hist3D = new TH3F("hist3D", "Hits vs totADC vs RMS; Number of hits in cluster; totADC; RMS", 20, 0, 10, 300, 0, 300, 100, 0, 5);
    
  for (int i = 0; i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    int clusterCount[stationCount][planeCount] = {{0}};
    int clusterSize = rec->cluster.clust.size();
    
    for(int idx = 0; idx < clusterSize; ++idx) {
      int station = rec->cluster.clust[idx].station;
      int plane = rec->cluster.clust[idx].plane;
      clusterCount[station][plane]++;
    }

    bool allGoodClusters = true;
    for (int iStation = 0; iStation < stationCount; ++iStation) {
			for (int iPlane = 0; iPlane < planeCount; ++iPlane) {
				if (goodMap(iStation, iPlane) == false) {
					continue;
				}

				if (clusterCount[iStation][iPlane] != 1) {
					allGoodClusters = false;
					break;
				}
			}
    }

    if (allGoodClusters) {
      for(int iCluster = 0; iCluster < clusterSize; ++iCluster) {
        float ndigits = rec->cluster.clust[iCluster].ndigits;
  	    float avgADC = rec->cluster.clust[iCluster].avgadc;
        float totADC = (ndigits * avgADC);
      	float RMS = rec->cluster.clust[iCluster].wgtrmsstrip;
      	hist3D->Fill(ndigits, totADC, RMS); 
      }
    }
  }
  
  TCanvas *c = new TCanvas();   
  hist3D->Scale(1.0 / hist3D->Integral());
  hist3D->Draw();
 
  TFile *outFile = new TFile("singleRMSplot.root", "RECREATE");
  hist3D->Write();
}
