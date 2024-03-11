void rebin() 
{
  //Reading histogram files
  TFile *adcFile = TFile::Open("ADCnorm_plots.root", "READ"); //Data file
  TFile *energyFile = TFile::Open("normDE_plots.root", "READ"); //Simulation file

  //Selecting histogram from data and sim files
  TH1F *adcHist = (TH1F*)adcFile->Get("hist_00"); //for now just working with one histogram; need to loop around all later
  TH1F *energyHist = (TH1F*)energyFile->Get("hist_00"); 
  
  //Number of bins of ADC and energy histogram
  int nADCbin = adcHist->GetNbinsX(); //500
  int nEnergybin = energyHist->GetNbinsX(); //1000

  std::vector<int> binNumber;
  std::vector<double> newBinEdges; //holding new bin edges for energy hist
  newBinEdges.push_back(energyHist->GetBinLowEdge(1));  

  int usedBins = 0;

  //Looping over adc bins
  for (int adcBin = 1; adcBin <= nADCbin; ++adcBin) {
    float targetProb = adcHist->GetBinContent(adcBin); //adc probability 
    float accumulatedProb = 0.0; 
  
    //Looping over energy bins
    for (int energyBin = usedBins + 1; energyBin <= nEnergybin; ++energyBin) {
      float binContent = energyHist->GetBinContent(energyBin);
      accumulatedProb += binContent;
      
      if (accumulatedProb >= targetProb) {
        newBinEdges.push_back(energyHist->GetBinLowEdge(energyBin + 1));
        binNumber.push_back(energyBin);
        usedBins = energyBin;
        break;
      }
    }
  }

 //DEBUGGING

  std::cout << "Bin numbers and new bin edges:" << std::endl;
  for (int i = 0; i < binNumber.size(); ++i) {
    std::cout << "Bin number: " << binNumber[i] << ", New bin edge: ";
    if (i < newBinEdges.size()) {
        std::cout << newBinEdges[i];
    }
    std::cout << std::endl;
  }



  //Rebinned Histogram
  TH1F *rebinHist = new TH1F("rebinHist", "Rebinned histogram" , newBinEdges.size() - 1, newBinEdges.data());
  for ( int bin = 0; bin < newBinEdges.size() - 1; bin++) {
    rebinHist->SetBinContent(bin, energyHist->Integral(binNumber[bin] + 1, binNumber[bin + 1]));
  } 
  
  TFile *outputFile = new TFile("outRebinHist.root", "RECREATE");
  rebinHist->Write();
  outputFile->Close();

  adcFile->Close();
  energyFile->Close();
} 
