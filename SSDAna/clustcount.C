//First attempt for number of cluster per plane vs entries for each events (good events: exactly one cluster per event)

void clustcount()
{
	//Reading ROOT File
	TFile *fInput = new TFile("r2257_combined.caf.root", "read");
	TTree *tree = (TTree*)fInput->Get("recTree");

	caf::StandardRecord* rec = 0;
	tree->SetBranchAddress("rec", &rec);

	// Plane configuration: station and sensor combinations for each plane
	int planes[][3] = {
	    {0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}, {2, 2},
	    {3, 0}, {3, 1}, {3, 2}, {4, 0}, {4, 1}, {5, 0, 1},
	    {5, 2, 3}, {5, 4, 5}, {6, 0, 1}, {6, 2, 3}, {6, 4, 5},
	    {7, 0, 1}, {7, 2, 3}
	};

        int numPlanes = sizeof(planes) / sizeof(planes[0]);

       	// Looping over planes
        for (int plane = 0; plane < numPlanes; ++plane) {
        int station = planes[plane][0];
        int sensor1 = planes[plane][1];
        int sensor2 = (plane >= 10) ? planes[plane][2] : -1;
	
	TH1F *hist = new TH1F("hist","Title; Number of Cluster per Plane per Events;Entries", 30, 0, 30);

        // Looping over the TTree
        for (int i = 0; i < tree->GetEntries(); ++i) {
        //  std::cout << i << std::endl; } //output: total number of entries;
        	float entry = tree->GetEntry(i);

        // Number of Cluster per Events
        int nclusts = rec->cluster.clust.size();
        int nclusters = 0;

        // Looping over cluster
        for (int idx = 0; idx < nclusts; ++idx) {
        	if (rec->cluster.clust[idx].station == station) {
               
           	// Plane with two sensors
		if (plane >= 12) {
        		if (rec->cluster.clust[idx].sens == sensor1 || rec->cluster.clust[idx]. sens == sensor2) {
        			nclusters++;
        		}
        	
		} 
			else {
      
			// Plane with one sensor
        		if (rec->cluster.clust[idx].sens == sensor1){
        			nclusters++;
        		}
        		}
       	        }
        	}

        //Good event selection; EXACTLY one cluster per plane
        if (nclusts == 1) {
        hist->Fill(nclusters);
        }
	}
	
	//Setting up Canvas
	TCanvas *canvas = new TCanvas();
	hist->Draw();
	canvas->Print();	
}
}
