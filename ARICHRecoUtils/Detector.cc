////////////////////////////////////////////////////////////////////////
/////// \brief   DETECTOR class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////



#include <iostream>
#include "ARICHRecoUtils/Detector.h"

namespace arichreco{



        Detector::Detector(bool mirror, double drate, double dwin, double ffactor, double zposition, TString file){
          fdet = new TFile(file, "read");
	  TGraph *qe = (TGraph*)fdet->Get("gDet");
	  hDet = (TH2D*)fdet->Get("hDet");
          this->darkrate = drate * 1e-9;
          this->window = dwin;
          this->fillFactor = ffactor;
          this->zPos = zposition;
          this->quantumEff = qe;
          this->randomGenerate = std::make_shared<TRandom3>();
          this->randomGenerate->SetSeed(0);
          this->mirror = mirror;
	
        }

        Detector::~Detector(){
            delete quantumEff;
            delete hDet;
            delete fdet;
        }

        TH2D* Detector::makeDetectorHist(char* name, char* title){
          //std::cout << "HERE" << std::endl;
          //TH2Poly *htemp = (TH2Poly*)hDet->Clone();
          TH2D* htemp = new TH2D("","",26,-75.7,75.7,26,-75.7,75.7);
	  htemp->SetName(name);
          htemp->SetTitle(title);
          return htemp;
	  delete htemp;
        }

        double Detector::getDarkRate(){ return darkrate; }
        double Detector::getWin(){ return window; }
        double Detector::getFillFactor(){ return fillFactor; }

        double Detector::evalQEff(double wav){
          wav = wav*1E9;
          return quantumEff->Eval(wav);
        }

void Detector::projectPhotons(TH2D* photonHist, std::vector<arichreco::Photon*> photons, TH1D* rHist){
          /*
          Project photons onto histogram representing detector plane
          Optional mirror on edges of detector plane
          Optional 1-D histogram representing the distance of each photon to the center of the plane
          */
          for(int j = 0; (unsigned) j < photons.size(); j++){
              arichreco::Photon* ph = photons[j];
            // Get distance to each wall in direction of travel
            double xDist = (TMath::Sign(xmax, ph->dir[0]) - ph->pos[0]) / ph->dir[0];
            double yDist = (TMath::Sign(ymax, ph->dir[1]) - ph->pos[1]) / ph->dir[1];
            double zDist = (zPos - ph->pos[2]) / ph->dir[2];
            // Pick the x y or z dimension with the closest wall and travel that way
            double dists[3] = {xDist, yDist, zDist};
            int minDistDimension = TMath::LocMin(3, dists);
            ph->travelDist(dists[minDistDimension]-1e-8);
            if(mirror){
              // Continuously reflect until photon hits detector plane
              TVector3 incidentPlane;
              int counter = 0; // Cap it at 10 reflections
              while(minDistDimension != 2 && counter < 10){
            // Get the normal vector of that wall
            incidentPlane = TVector3(0.,0.,0.);
                incidentPlane[minDistDimension] = TMath::Sign(1, -ph->dir[minDistDimension]);
                // Reflect the light
            ph->dir = reflectedDirection(ph->dir, incidentPlane);
                // Pick the x y or z dimension with the closest wall and travel that way
            dists[0] = (TMath::Sign(xmax, ph->dir[0]) - ph->pos[0]) / ph->dir[0];
                dists[1] = (TMath::Sign(ymax, ph->dir[1]) - ph->pos[1]) / ph->dir[1];
                dists[2] = (zPos - ph->pos[2]) / ph->dir[2];
                minDistDimension = TMath::LocMin(3, dists);
                ph->travelDist(dists[minDistDimension]);
                counter++;
              }
            }
            if(minDistDimension == 2){
            	photonHist->Fill(ph->pos[0]*10, ph->pos[1]*10);
              if(rHist) rHist->Fill(ph->dir[2]*10);
              //if(rHist) rHist->Fill(sqrt((ph->pos[0]-5.4)*(ph->pos[0]-5.4) + (ph->pos[1]-5.4)*(ph->pos[1]-5.4)));
            }
          }
        } 
 }
