#include"TFile.h"
#include"TTree.h"
#include"TRandom.h"
#include"TF1.h"
#include"TH1.h"
#include<iostream>
#include"TH2.h"
#include"TH3.h"
#include "/exp/emph/app/users/abhattar/digitizationWork/emphaticsoft/StandardRecord/StandardRecord.h"
/*
float adcRange(float value, const std::vector<int>& allowedADC) {
  float nearestValue = allowedADC[0];
  float minDifference = std::abs(value - nearestValue);
   
  for (const auto& adc : allowedADC) {
    float difference = std::abs(value - adc);
    if (difference < minDifference) {
      nearestValue = adc;
      minDifference = difference;
    }
  }
    return nearestValue;
}
*/

float adcRange(float value) {
  if (value >= 1 && value <= 41.5) return 41;
  else if (value > 41.5 && value <= 58.5) return 58;
  else if (value > 58.5 && value <= 73.5) return 73;
  else if (value > 73.5 && value <= 88.5) return 88;
  else if (value > 88.5 && value <= 103.5) return 103;
  else if (value > 103.5 && value <= 118.5) return 118;
  else if (value > 118.5 && value <= 133.5) return 133;
  else if (value > 133.5 && value <= 140.5) return 140;
  else return value; 
}

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
  
  TH1F *newHist = new TH1F("newHist", "newHist", 500, 0, 500);

  //Loading 3D hist
  TFile *file1 = TFile::Open("singleRMSplot.root", "READ");
  TH3D *hist3D = (TH3D*)file1->Get("hist3D");
 
  //Loading DE vs ADC Histogram
  TFile *adcDEfile = TFile::Open("devsADCFile.root", "READ");
  TH1D *adcVsDEHist = (TH1D*)adcDEfile->Get("adcVsDEHist");

  //Opening a Simulation file (SSDHits) and getting a tree
  TFile *file2 = new TFile("emphmc_r2252_s9.dig.caf.root", "read");
  TTree *tree = (TTree*)file2->Get("recTree");

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  const int stationCount = 8; //0 to 7
  const int planeCount = 3; //0, 1 or 2
  const double stripWidth = 0.06; //units in mm

  std::vector<int> allowedADC = {41, 58, 73, 88, 103, 118, 133, 140};
  
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

           cout << "Total adc (adc vs DE): " << adc << endl;

           double hit= 0;
           double rms = 0;

           float xtrue = 0;
           float ytrue = 0;
           float ztrue = 0;
           float wtrue = 0;
           float dtrue = 0;
           float dstrip = 0;

           useADC(adc, hit, rms, hist3D);
           int hits = std::lround(hit);
           
           cout << "Number of hit(s): " << hits << endl;
           if (hits == 1) {
             float calADC = adcRange(adc);
          //   if (fabs(calADC -  58) < 0.5 ){cout << "we have 58.5 adc"<< endl;}
          //   newHist->Fill(calADC);
             cout << "Calculated ADC :" << calADC << endl;
             cout << "------------" << endl;
             continue;
           }

           xtrue = rec->truth.truehits.truehits[icluster].GetX;
           ytrue = rec->truth.truehits.truehits[icluster].GetY;
           ztrue = rec->truth.truehits.truehits[icluster].GetZ;
           wtrue = (sqrt(2)/2) * (-xtrue + ytrue);
          
           float x0 = rec->lineseg.lineseg[icluster].x0.x;
           float y0 = rec->lineseg.lineseg[icluster].x0.y;
           float z0 = rec->lineseg.lineseg[icluster].x0.z;
           float x1 = rec->lineseg.lineseg[icluster].x1.x;
           float y1 = rec->lineseg.lineseg[icluster].x1.y;
           float z1 = rec->lineseg.lineseg[icluster].x1.z;
         
           float w0 = (sqrt(2) / 2) * (-x0 + y0);
           float w1 = (sqrt(2) / 2) * (-x1 + y1);

           cout << "x0: " << x0 << endl; 
           cout << "y0: " << y0 << endl; 
           cout << "x1: " << x1 << endl; 
           cout << "y1: " << y1 << endl; 
           cout << "w0: " << w0 << endl;
           cout << "w1: " << w1 << endl;
           cout << "xtrue: " << xtrue << endl;
           cout << "ytrue: " << ytrue << endl;
           cout << "wtrue: " << wtrue << endl;

          // float utrue = (sqrt(2)/2) * (xtrue + ytrue);

           if (x0 == x1) { //xplane; vertical
             dtrue = xtrue;
             dstrip = x0;
           }
           else if (y0 == y1) { //yplane; horizontal 
             dtrue = ytrue;
             dstrip = y0;
           }
           else { //wplane; diagonal -- NOTE: phase1c only has w plane; no u plane
             dtrue = wtrue;
             dstrip = (w0 + w1) / 2; //same result with just w0 or w1			   
	         }

           cout << "dtrue: " << dtrue << endl;
           cout << "dstrip: " << dstrip << endl;

         	 float position = (dtrue - dstrip) / stripWidth; 
					 float mean = position;
           if (mean <  -0.5 || mean > 0.5) {
             continue;
           }

           float stddev = sqrt(abs((rms * rms) - (mean * mean))); 
/*
 	          if (abs(dtrue - dstrip) > 2 ) {
             cout << "total ADC: (DE vs ADC) " << adc << endl;
             cout << "Number of hit(s): " << hits << endl;
             cout << "(x0, y0) =  (" << x0 << ",  " << y0 << ") || (x1, y1) = (" << x1 << ",  " << y1 << ")" << endl; 
             cout << "dstrip: " << dstrip << endl;
             cout << "xtrue (dtrue): " << dtrue << endl;
             cout << "mean: "<<  mean  << endl;
             cout << "--------------" << endl;
           }
*/
	
    		   cout << "mean: " << mean << endl;  
           float stddev = rms;
           cout << "std dev = rms " << stddev << endl;
         
  //         cout << "Standard Deviation:" << stddev << endl;

           float loLimit = mean - (5 * stddev);
           float hiLimit = mean + (5 * stddev);
           float amplitude = adc / (stddev * sqrt(2 * M_PI));
		
    	    TF1 *func = new TF1 ("func", "gaus", loLimit, hiLimit);
					func->SetParameters(amplitude, mean, stddev);
			
       		float totADC  = func->Integral(loLimit, hiLimit);
          cout << "Total ADC: " << adc << endl;
	
					std::vector<std::pair<float, float>> intervals;
          
					bool wasEven = (hits % 2 == 0); //Checking if originally even

					if (wasEven) {
							hits += 1; // Pretending it's odd by adding one
					}

					//Generating intervals symmetrically around the mean
					for (float interval = (-hits / 2.0); interval < hits / 2.0; interval += 1) {
						intervals.push_back({interval, interval + 1});
					}   

					if (wasEven) { //Originally was even
						if (mean >= 0) {
							intervals.erase(intervals.begin()); //Removing the most left-hand side interval for positive mean
						} else {
						intervals.pop_back(); //Removing the most right-hand side interval for negative mean
						 }
					}

           for (auto&i : intervals) {
            std::cout << "intevals: (" << i.first << ", " << i.second << ")" << endl;
           }

           std::vector<float> intervalIntegrals;

           for (const auto&interval : intervals) {
             float integral = func->Integral(interval.first, interval.second);
             intervalIntegrals.push_back(integral);
           }

           for (const auto&integral : intervalIntegrals) {
             std::cout << "inteval integral: " << integral << endl;
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
    //          float afterADC = accumulate(intervalIntegrals.begin(), intervalIntegrals.end(), 0.0); 
      //        newHist->Fill(afterADC);
              std::cout << "Adjusted interval integral: " << integral << std::endl;
        //      std::cout << "ADC (Cal and adding them back:) " << afterADC << std::endl;
          //    break;
            }
   
	     			for (auto& integral : intervalIntegrals) {
								integral = adcRange(integral);
              //  newHist->Fill(integral);
            }

					  for (const auto& integral : intervalIntegrals) {
							std::cout << "Rounded interval integral: " << integral << std::endl;
            //    newHist->Fill(integral);
						}

           for (auto& integral : intervalIntegrals) {
             float finalADC = accumulate(intervalIntegrals.begin(), intervalIntegrals.end(), 0.0);
             newHist->Fill(finalADC);
             cout << "final ADC: " << finalADC << endl;
             break;
           }

	cout << "------------" << endl;
  	    }	
      }
    }
  }

  TFile outFile("outputHist.root", "RECREATE");
  newHist->Scale(1.0 / newHist->Integral());
  newHist->Draw("HIST");
  newHist->Write();
  outFile.Close();

}
