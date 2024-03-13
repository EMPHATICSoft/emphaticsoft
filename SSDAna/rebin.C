//Draws energy and rebinned energy in a same plot 

void overlap()
{
  TFile *rebinFile = new TFile("outRebinHist.root", "READ");
  TFile *energyFile = new TFile("DEnorm_plots.root", "READ");

  TH1F *rebinHist = (TH1F*)rebinFile->Get("rebinHist");
  TH1F *energyHist = (TH1F*)energyFile->Get("hist_00");

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
  //Reading histogram files
  TFile *adcFile = TFile::Open("ADCnorm_plots.root", "READ"); //Data file
  TFile *energyFile = TFile::Open("DEnorm_plots.root", "READ"); //Simulation file

  //Selecting histogram from data and sim files
  TH1F *adcHist = (TH1F*)adcFile->Get("hist_00"); //for now just working with one histogram; need to loop around all later
  TH1F *energyHist = (TH1F*)energyFile->Get("hist_00"); 
  
  //Number of bins of ADC and energy histogram
  int nADCbin = adcHist->GetNbinsX(); //Out: 500
  int nEnergybin = energyHist->GetNbinsX(); //Out: 1e7
  
  //cout << "**************************" << endl;
  //cout << "The number of bins of ADC hist: " << nADCbin << endl;
  //cout << "**************************" << endl;

  std::vector<int> binNumber;
  std::vector<double> newBinEdges; //holding new bin edges for energy hist
  newBinEdges.push_back(energyHist->GetBinLowEdge(1));  
  binNumber.push_back(1);

  int usedBins = 0;
  const float threshold =  0.00098;

  //Looping over adc bins
  for (int adcBin = 1; adcBin <= nADCbin; ++adcBin) {
    float targetProb = adcHist->GetBinContent(adcBin); //adc probability 
   // cout << "target probability: " << targetProb << " and ADC bin: " << adcBin << endl;
   if (targetProb < threshold) { continue;
   }
  //  cout << "First bin content (target probability) of ADC: " << targetProb << endl;
    float accumulatedProb = 0; 
    int energyBin = usedBins + 1; 
    //Looping over energy bins
    for (; energyBin <= nEnergybin; ++energyBin) {
      float binContent = energyHist->GetBinContent(energyBin);
    //  cout << "Bin content from energy hist to match target prob: " << binContent << endl;
      accumulatedProb += binContent;
      
      if (accumulatedProb >= targetProb) {
        newBinEdges.push_back(energyHist->GetBinLowEdge(energyBin + 1));
        binNumber.push_back(energyBin);
        usedBins = energyBin;
    //    cout << "Target probability: " << targetProb << endl;
      //  cout << "pushing bin edges for " << adcBin << " many times" <<  endl;
        break;
      } 
    }
    
    //cout << "Number of bins that satisfied threshold:" << adcBin << endl; 
    if (energyBin > nEnergybin) { 
      cout << "NOT GOODD! *********" << endl;
      newBinEdges.push_back(energyHist->GetBinLowEdge(energyBin + 1));
      binNumber.push_back(energyBin);
      usedBins = energyBin;
      break;
    }
  }
  
  //newBinEdges.push_back(energyHist->GetBinLowEdge(nEnergybin));
 
  // cout << "Number of ADC bins that satisfied threshold" <<  << endl; 
   //cout << "Number of new bin edges " << newBinEdges.size() << endl;  
  //DEBUGGING
  std::cout << "new bin edges:" << std::endl;
  for (int i = 0; i < newBinEdges.size(); ++i) {
        std::cout << newBinEdges[i] << endl;
  }

  //Rebinned Histogram
  TH1F *rebinHist = new TH1F("rebinHist", "Rebinned histogram" , newBinEdges.size() - 1, newBinEdges.data());
  TH1F* newrebinHist = new TH1F("newrebinHist", "New Rebin Histogram", newBinEdges.size() - 1, 0, newBinEdges.size() - 1);
  for (int bin = 0; bin < newBinEdges.size() - 1; bin++) {
    rebinHist->SetBinContent(bin + 1, energyHist->Integral(binNumber[bin], binNumber[bin + 1]));
    newrebinHist->SetBinContent(bin + 1, energyHist->Integral(binNumber[bin], binNumber[bin + 1]));
  } 

  //cout << "**************************" << endl;
  //cout << "The number of nbins of new rebinned hist: " << newrebinHist->GetNbinsX() << endl; 
 // cout << "**************************" << endl;
  
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
  
  TFile *outputFile = new TFile("outRebinHist.root", "RECREATE");
  rebinHist->Write();
  //outputFile->Close();

  //adcFile->Close();
  //energyFile->Close();

  overlap();
  //newrebinHist->Print("all");
} 
