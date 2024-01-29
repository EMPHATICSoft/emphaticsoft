//First attempt for ADC hist of each sensor

void adc()
{
    // Opening ROOT File and Getting a TTree
    TFile *fInput = new TFile("r2257_combined.caf.root", "read");
    TTree *tree = (TTree*)fInput->Get("recTree");

    // Create a StandardRecord object and setting the branch address
    caf::StandardRecord* rec = 0;
    tree->SetBranchAddress("rec", &rec);

    int stationCount[] = {2, 2, 3, 3, 2, 6, 6, 4};  // Number of sensors per station

    // Looping over stations
    for (int station = 0; station < 8; ++station) {
        // Looping over sensors
        for (int sensor = 0; sensor < stationCount[station]; ++sensor) {
            // Create histograms for each station and sensor combination
            TH1F *hist = new TH1F("hist","ADC vs Entries; ADC; Entries", 30, 0, 30);

            // Looping over the TTree
            for (int i = 0; i < tree->GetEntries(); ++i) {
                tree->GetEntry(i);

                // Number of Cluster per Events
                int nclusts = rec->cluster.clust.size();

                // Looping over cluster
                for (int idx = 0; idx < nclusts; ++idx) {
                    if (rec->cluster.clust[idx].station == station && rec->cluster.clust[idx].sens == sensor) {
                        hist->Fill(rec->cluster.clust[idx].avgadc);
                    }
	//Setting up Canvas
            TCanvas *canvas = new TCanvas();
            hist->Draw();
            canvas->Print(Form("hist_%d%d.png", station, sensor));
            canvas->Write();                                                

                }

                }
            }

            //Setting up Canvas
            //TCanvas *canvas = new TCanvas();
            //hist->Draw();

            //canvas->Print(Form("hist_%d%d.png", run_number, station, sensor));
	    //canvas->Write();
        }
}
