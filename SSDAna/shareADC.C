#include"TFile.h"
#include"TTree.h"
#include"TRandom.h"
#include"TF1.h"
#include"TH1.h"
#include<iostream>
#include"TH2.h"
#include"TH3.h"
#include "/exp/emph/app/users/abhattar/digitizationWork/emphaticsoft/StandardRecord/StandardRecord.h"

bool goodMap(int istation, int iplane)
{
  vector <pair<int, int>>map = {{0,2}, {1,2}, {4,2}, {7,2}};
  pair<int, int>check = {istation, iplane};

  for (int i = 0; i < map.size(); ++i) {
    if (check == map[i]) {
      return false;
    }
  }
  return true;
}

void useADC(double adc, double &hit, double &rms, TH3D* hist3D)
{
  double adcBin = hist3D->GetYaxis()->FindBin(adc); //Bin that corresponds to ADC
  hist3D->GetYaxis()->SetRange(adcBin, adcBin); //Range for only this bin in Y-axis 
  TH2D *h2slice = (TH2D*)hist3D->Project3D("zx");  
  
  if (h2slice->GetEntries() == 0) {
    std::cout << "h2slice is empty for adcBin: " << adcBin << std::endl;
    return;
   }
  
  h2slice->GetRandom2(hit, rms);
  hist3D->GetYaxis()->SetRange(1, hist3D->GetNbinsY()); //resetting the y-axis range back to default
}

void shareADC()
{
  gRandom->SetSeed(0);

  //Loading 3D hist
  TFile *file1 = TFile::Open("singleRMSplot.root", "READ");
  TH3D *hist3D = (TH3D*)file1->Get("hist3D");
 
  //Loading DE vs ADC Histogram
  TFile *adcDEfile = TFile::Open("devsADCFile.root", "READ");
  TH1D *adcVsDEHist = (TH1D*)adcDEfile->Get("adcVsDEHist");

  //Opening a Simulation file (SSDHits) and getting a tree
  TFile *file2 = new TFile("emphmc_r2098_s9.dig.caf.root", "read");
  TTree *tree = (TTree*)file2->Get("recTree");

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  const int stationCount = 8; //0 to 7
  const int planeCount = 3; //0, 1 or 2
  const double stripWidth = 0.06; //units in mm

  std::vector<float> allowedValues = {41, 58, 73, 88, 103, 118, 133, 140};
  
  //Looping over tree entries
  for (int i = 0; i < tree->GetEntries(); ++i) { 
    tree->GetEntry(i); 
    int clusterCount[stationCount][planeCount] = {{0}}; 
    int clusterSize = rec->cluster.clust.size(); 

    //Looping over number of clusters 
    for(int idx = 0; idx < clusterSize; ++idx) {
      int station = rec->cluster.clust[idx].station;
      int plane = rec->cluster.clust[idx].plane;
      clusterCount[station][plane]++;
    }

    //Good event selection
    bool allGoodClusters = true;
    for (int iStation = 0; iStation < stationCount; ++iStation) {
      for (int iPlane = 0; iPlane < planeCount ; ++iPlane) {
        if (goodMap(iStation, iPlane) == false) {  
          continue;
        }
       
        if (clusterCount[iStation][iPlane] != 1) {
          allGoodClusters = false;
          break;
        }
      }
   //   if (!allGoodClusters) break;
    }

    if (allGoodClusters) {
      for(int icluster = 0; icluster < clusterSize; ++icluster) {
        float dEnergy = rec->truth.truehits.truehits[icluster].GetDE;
     
   	     //Selecting threshold
         if (dEnergy > 0.000000005) {
					
					//Mapping DE to ADC using the DE vs ADC histogram	
					double deBin = adcVsDEHist->GetXaxis()->FindBin(dEnergy);
					double adc = adcVsDEHist->GetBinContent(deBin);
		
					double hit= 0;
					double rms = 0;
		
					useADC(adc, hit, rms, hist3D);
          int hits = std::lround(hit);
				  
         	float xtrue = rec->truth.truehits.truehits[icluster].GetX;
         	float ytrue = rec->truth.truehits.truehits[icluster].GetY;
         	float ztrue = rec->truth.truehits.truehits[icluster].GetZ;
					float x0 = rec->lineseg.lineseg[icluster].x0.x;
					float y0 = rec->lineseg.lineseg[icluster].x0.y;
					float z0 = rec->lineseg.lineseg[icluster].x0.z;
		    	float x1 = rec->lineseg.lineseg[icluster].x1.x;
		    	float y1 = rec->lineseg.lineseg[icluster].x1.y;
					float z1 = rec->lineseg.lineseg[icluster].x1.z;
          
          float utrue = (sqrt(2)/2) * (xtrue + ytrue);
          float wtrue = (sqrt(2)/2) * (-xtrue + ytrue);
         
           float dtrue, dstrip;

          if (x0 == x1) { //xplane; horizontal
            float dtrue = xtrue;
            float dstrip = x0;
          }
          else if (y0 == y1) { //yplane; vertical
            float dtrue = ytrue;
            float dstrip = y0;
          }
          else { //wplane; diagonal -- NOTE: phase1x only has w plane; no u plane
            float dtrue = wtrue;
          //  float dstrip = ?			   
	        }

         	float position = (dtrue - dstrip) / stripWidth; 
					float mean = position;
				  float stddev = rms;
         
          cout << "------------------" << endl;
          cout << "xtrue (true hit): " << xtrue << endl;
          cout << "ytrue (true hit): " << ytrue << endl;
          cout << "ztrue (true hit): " << ztrue << endl;
          cout << "x0: " << x0 << endl;
          cout << "y0: " << y0 << endl;
          cout << "x1: " << x1 << endl;
          cout << "y1: " << y1 << endl;
          cout << "z0: " << z0 << endl;
          cout << "z1: " << z1 << endl;
          cout << "dtrue: " << dtrue << endl;
          cout << "dstrip: " << dstrip << endl;
         
          cout << "adc (data): "<< adc << endl;
          cout << "hits: " << hits << endl;
          cout << "mean: " << mean << endl;
          cout << "stddev (rms): " << rms << endl;

          float loLimit = mean - (2 * hits);
          float hiLimit = mean + (2 * hits);
		
    			TF1 *func = new TF1 ("func", "gaus", loLimit, hiLimit);
					func->SetParameters(1, mean, stddev);
			
      		float totADC  = func->Integral(loLimit, hiLimit);
         // cout << "Total ADC: " << adc << endl;
          std::vector<std::pair<float, float>> intervals;
          for (float interval = (-hits/2.0); interval < hits/2.0; interval +=1) {
            intervals.push_back({mean + interval,mean + interval + 1});
          }
          
          for (auto&i : intervals) {
          //  std::cout << "intevals: (" << i.first << ", " << i.second << ")" << endl;
          }

          std::vector<float> intervalIntegrals;

          for (const auto&interval : intervals) {
            float integral = func->Integral(interval.first, interval.second);
            intervalIntegrals.push_back(integral);
          }

          float sumIntervalIntegrals = std::accumulate(intervalIntegrals.begin(), intervalIntegrals.end(), 0.0);
            float difference = adc - sumIntervalIntegrals;
            float adjustment = difference / intervalIntegrals.size();

            for (auto &integral : intervalIntegrals)
            {
                integral += adjustment;
            }

            // Output adjusted interval integrals
            for (const auto &integral : intervalIntegrals)
            {
           //     std::cout << "Adjusted interval integral: " << integral << std::endl;
            }

            //Need a loop to go over integrals and round to get a allowed ADC values     
        
		    }	
      }
    }
  }
}

