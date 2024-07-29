void overlap()
{
  TFile *rebinFile = new TFile("outRebinHist.root", "READ");
  TFile *energyFile = new TFile("DEnorm_plots.root", "READ");
 
  TH1F *rebinHist = (TH1F*)rebinFile->Get("rebinHist");
  TH1F *energyHist = (TH1F*)energyFile->Get("deHist");
 
  rebinHist->SetLineColor(kBlue);
  energyHist->SetLineColor(kRed);
 
  cout << "Rebin histogram: " << rebinHist->Integral() << endl;
  cout << "Energy histogram: " << energyHist->Integral() << endl;
 
  TCanvas *c1 = new TCanvas();
  rebinHist->SetTitle("Comparing energy hist");
  rebinHist->Draw();
  energyHist->Draw("HIST SAME");
 
  TLegend *legend = new TLegend(0.7, 0.7, 0.5, 0.5);
  legend->AddEntry(rebinHist, "Rebinned", "l");
  legend->AddEntry(energyHist, "Without rebin", "l");
  legend->Draw();
}

void rebin()
{
  TFile *adcFile = TFile::Open("ADCnorm_plots.root", "READ"); 
  TFile *energyFile = TFile::Open("DEnorm_plots.root", "READ"); 
 
  TH1F *adcHist = (TH1F*)adcFile->Get("adcHist"); 
  TH1F *energyHist = (TH1F*)energyFile->Get("deHist");

   // Number of bins of ADC and energy histogram
   int nADCBin = adcHist->GetNbinsX(); // Out: 500
   int nEnergyBin = energyHist->GetNbinsX(); // Out: 1e7

   std::vector<int> binNumber;
   std::vector<double> newBinEdges; // Holding new bin edges for energy hist
   newBinEdges.push_back(energyHist->GetBinLowEdge(1));
   binNumber.push_back(1);

   int usedBins = 0;
   const float threshold = 0.00098;

   std::vector<double> totalADC; // ***ADC vs DE***

   // Looping over adc bins
   for (int adcBin = 1; adcBin <= nADCBin; ++adcBin) {
     float targetProb = adcHist->GetBinContent(adcBin); // adc probability
     if (targetProb < threshold) continue;
       totalADC.push_back(adcHist->GetBinCenter(adcBin)); // ***ADC vs DE***
       float accumulatedProb = 0;
       int energyBin = usedBins + 1;

       // Looping over energy bins
       while (energyBin <= nEnergyBin) {
         float binContent = energyHist->GetBinContent(energyBin);
         accumulatedProb += binContent;

	       if (accumulatedProb > targetProb) {
	       float excessProb = accumulatedProb - targetProb;
				 accumulatedProb -= binContent; // Removing  the last added binContent to avoid exceeding

				 newBinEdges.push_back(energyHist->GetBinLowEdge(energyBin));
				 binNumber.push_back(energyBin - 1);
				 usedBins = energyBin - 1;

         // Starting a new bin with the excess probability
				 accumulatedProb = excessProb;
				 break;
				 }
           else if (accumulatedProb == targetProb) {
				     newBinEdges.push_back(energyHist->GetBinLowEdge(energyBin + 1));
				     binNumber.push_back(energyBin);
				     usedBins = energyBin;
				     break;
				   }
				   energyBin++;
			 }
				   if (energyBin > nEnergyBin) {
			     cout << "NOT GOOD! *********" << endl;
				 	 newBinEdges.push_back(energyHist->GetBinLowEdge(energyBin + 1));
					 binNumber.push_back(energyBin);
				 	 usedBins = energyBin;
					 break;
				   }
			}
/* 
   // Including overflow bin in new bin edges
    if (usedBins < nEnergyBin) {
      newBinEdges.push_back(energyHist->GetBinLowEdge(nEnergyBin + 1));
      binNumber.push_back(nEnergyBin);
    }
*/
		 // Rebinded Histogram
		 TH1F *rebinHist = new TH1F("rebinHist", "Rebinded histogram", newBinEdges.size() - 1, newBinEdges.data());
		 TH1F *newrebinHist = new TH1F("newrebinHist", "New Rebin Histogram", newBinEdges.size() - 1, 0, newBinEdges.size() - 1);

	   for (int bin = 0; bin < newBinEdges.size() - 1; bin++) {
		 rebinHist->SetBinContent(bin + 1, energyHist->Integral(binNumber[bin], binNumber[bin + 1]));
		 newrebinHist->SetBinContent(bin + 1, energyHist->Integral(binNumber[bin], binNumber[bin + 1]));
	   }
		 
     TCanvas *c2 = new TCanvas();
		 adcHist->SetLineColor(kBlue);
		 adcHist->SetLineWidth(2);
		 newrebinHist->SetLineColor(kRed);
		 adcHist->Draw("HIST");
		 newrebinHist->Draw("HIST SAME");

		 TLegend *legend2 = new TLegend(0.7, 0.7, 0.5, 0.5);
		 legend2->AddEntry(adcHist, "adcHist", "l");
		 legend2->AddEntry(newrebinHist, "Rebinning", "l");
		 legend2->Draw();

		 // ***ADC vs DE***
		 TH1F* adcVsDEHist = new TH1F("adcVsDEHist", "ADC vs DE; Rebinned DE; totADC", newBinEdges.size() - 1, &newBinEdges[0]);

     for (int i = 0; i < totalADC.size(); ++i) {
       cout << "Bin (DE): " << i+1 << " --- totalADC: " << totalADC[i] << endl;
       adcVsDEHist->SetBinContent(i + 1, totalADC[i]);
     }

     TCanvas *c3 = new TCanvas();
		 adcVsDEHist->Draw("HIST");
		 TFile *devsADCFile = new TFile("devsADCFile.root", "RECREATE");
		 adcVsDEHist->Write();

		 TFile *outputFile = new TFile("outRebinHist.root", "RECREATE");
		 rebinHist->Write();

		 overlap();
}
												
