////////////////////////////////////////////////////////////////////////
///// \brief   ARICH_UTILS class 
///// \author  mdallolio
///// \date
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <ostream> 
#include <iostream>
#include "TRandom3.h"
#include "TMath.h"
#include "CLHEP/Random/Randomize.h"

#include "Geometry/service/GeometryService.h"
#include "ARICHRecoUtils/ArichUtils.h"
#include "ARICHRecoUtils/Particle.h"
#include <map>

namespace arichreco{

const int NUMPARTICLES = 3;
const double MASSES[NUMPARTICLES] = {0.1395701, 0.493677, 0.938272};
const char* PNAMES[NUMPARTICLES] = {"Pion", "Kaon", "Proton"}; 

ARICH_UTILS::ARICH_UTILS()
   {
	std::cout << "CREATING ARICHUTILS" << std::endl;
   }

ARICH_UTILS::~ARICH_UTILS()
   {
	delete Detector;
	delete Arich;
   } // end deconstructor

void ARICH_UTILS::SetUpDet(double PDdarkrate, double PDwin, double PDfillfactor, double PDzpos,TString file)
   { 
	PDfile = file;
	Detector = new arichreco::Detector(true, PDdarkrate, PDwin, PDfillfactor, PDzpos, PDfile);
   } 

//.......................................................................

void ARICH_UTILS::SetUpArich(double up_n, double down_n, double up_pos, double down_pos, double up_thick, double down_thick)
  {
	Arich = new arichreco::Arich(Detector, up_n, down_n, up_pos, down_pos, up_thick, down_thick);
  }
//.......................................................................

TH2D* ARICH_UTILS::DigsToHist(std::vector<int>& blocks)
   {
//     TFile* fdet = new TFile(PDfile, "read");
     //TH2Poly* hDet = (TH2Poly*) fdet->Get("hDet");
//     TH2Poly* htemp = (TH2Poly*)fdet->Get("hDet")->Clone();
    TH2D* htemp = new TH2D("","",26,-75.7,75.7,26,-75.7,75.7); 
    //htemp->SetTitle(Form("event_%i",event_number));
     
    std::vector<float> pos;	
    for(size_t i =0; i < blocks.size(); i++)
    {
	emph::arich_util::PMT* mpmt = 0;
        pos = mpmt->GetPosfromBlockNumber(blocks[i]);
	delete mpmt;
	htemp->Fill(pos[0],pos[1]);
        pos.clear();
    }
	return htemp;
   }
//.......................................................................
	
double ARICH_UTILS::computeLogLikelihood(TH2D* event, TH2D* distribution)
{
	/*
  	Compare every bin of an event histogram to a bin in some probability distribution,
	to give the negative log-likelihood of that event under that distribution
	*/
    
	double darkrate = Detector->getDarkRate()*Detector->getWin();
	int nBins = event->GetNcells();
	int test = distribution->GetNcells();
	double logLikelihood = 0.;
	if(nBins != test){
		std::cerr << "ERROR: Bin Mismatch" << std::endl;
		throw "Bin Mismatch Error";
	}
	
	for(int i=1; i<=nBins; i++){
		double lambda = distribution->GetBinContent(i) + darkrate;
		bool pixelHit = event->GetBinContent(i) > 0;
		if(pixelHit) logLikelihood += log(1 - exp(-lambda));
		else logLikelihood += log(exp(-lambda));
	}
	return -2*logLikelihood;
 } // end computeLogLikelihood
//.......................................................................

double ARICH_UTILS::calcBeta(int particlei, double mom)
 {
   double M = MASSES[particlei];
   return sqrt( 1. / (1. + M*M/(mom*mom)) );
 } //end calcBeta
//.......................................................................

std::vector<double> ARICH_UTILS::IdentifyMultiParticle(TH2D* hist, int np, std::vector<double> mom, 
	 std::vector<TVector3> pos0s,std::vector<TVector3> dir0s) 
    {

	std::vector<std::vector<TH2D>> calculatedPdfs;
	std::vector<double> LogLike;
	TH2D calculatedPdf;

	for (int i = 0; i < np; i++) {
		std::vector<TH2D> particleiCalculatedPdfs;
		hypothesis.pos = pos0s[i];
		hypothesis.dir = dir0s[i];
		
		for (int p = 0; p < NUMPARTICLES; p++) { //NUMPARTICLES = number of possible particles for now = 3	
		  hypothesis.beta =  calcBeta(p, mom[i]);
		  hypothesis.name = PNAMES[p];
		
		  calculatedPdf = Arich->calculatePdf(hypothesis, Form("pdf_%i_%i", i, p));
		  particleiCalculatedPdfs.push_back(calculatedPdf);
		  }
		
		calculatedPdfs.push_back(particleiCalculatedPdfs);
		particleiCalculatedPdfs.clear();
	 }

	int numCombinations = TMath::Power(NUMPARTICLES, np);
	double minLoglikelihood = 1E10;
	int bestCombination[np];
	TH2D *hs;
        
	for (int i = 0; i < numCombinations; i++) {
		int index = i;
		if (i > 0) delete hs;
		char* stackedTitle = Form("PDF%i", i);
		int combination[np];
		for (int k=np-1; k>=0; k--) { 
			int p = index % NUMPARTICLES;
			index = index / NUMPARTICLES;
			combination[k] = p;
               		stackedTitle = Form("%s_%s", stackedTitle, PNAMES[p]);
			if(k==np-1) {hs=(TH2D*)calculatedPdfs[k][p].Clone();}
			
                	else for(int j=1;j<=calculatedPdfs[k][p].GetNcells();j++)
                	{hs->SetBinContent(j,hs->GetBinContent(j)+ calculatedPdfs[k][p].GetBinContent(j));}
		
		}
		//hs->SetTitle(stackedTitle);
		double logLikelihood = computeLogLikelihood(hist, hs);
		LogLike.push_back(logLikelihood);
	}
	delete hs;
	calculatedPdfs.clear();
	return LogLike;		
   
    } //end IdentifyMultiParticle
//.......................................................................
std::vector<std::vector<TH2D>> ARICH_UTILS::GetPDFs(int np, std::vector<double> mom,
         std::vector<TVector3> pos0s,std::vector<TVector3> dir0s)
    {
        std::vector<std::vector<TH2D>> calculatedPdfs;
        std::vector<double> LogLike;
        for (int i = 0; i < np; i++) {
                std::vector<TH2D> particleiCalculatedPdfs;
                arichreco::particleInfoStruct hypothesis;
                hypothesis.pos = pos0s[i];
                hypothesis.dir = dir0s[i];

	for (int p = 0; p < NUMPARTICLES; p++) { //NUMPARTICLES = number of possible particles for now = 3

                        hypothesis.beta =  calcBeta(p, mom[i]);
                        hypothesis.name = PNAMES[p];

                        TH2D calculatedPdf = Arich->calculatePdf(hypothesis, Form("pdf_%i_%i", i, p));
                        particleiCalculatedPdfs.push_back(calculatedPdf);
                        }
                calculatedPdfs.push_back(particleiCalculatedPdfs);
         }

	return calculatedPdfs;
}
//.......................................................................

std::vector<double> ARICH_UTILS::recoCherenkov(TH2Poly* eventHist, int nDetected, std::vector<TVector3> pos0s, std::vector<TVector3> dir0s)
  {
   
    TVector3 hiti, diri;
    std::vector<double> theta_bin[nDetected], thetasC;
    double thetai, thetaC = 0;

    TH1D* hChe[nDetected];
    TList *binlist=eventHist->GetBins();
    TH2PolyBin *thisBin;

    for(int np = 0; np < nDetected; np ++){
    for(int j=0;j<eventHist->GetNumberOfBins();j++){
	if(eventHist->GetBinContent(j+1) < 1e-6)continue;
	  thisBin=(TH2PolyBin*)binlist->At(j);
		hiti[0] = ((thisBin->GetXMax()+thisBin->GetXMin())/2 - pos0s[np][0])/10; // dividing by 2 and convert it to cm
		hiti[1] = ((thisBin->GetYMax()+thisBin->GetYMin())/2 - pos0s[np][1])/10; // dividing by 2 and convert it to cm
		hiti[2] =  Detector->getDist();   //already in cm
		diri = hiti.Unit();
		thetai = diri.Angle(dir0s[np]) * 180 / TMath::Pi();
		theta_bin[np].push_back(thetai);
	  }

	
     std::unordered_map<double, int> frequency;
     for (double num : theta_bin[np]) { 
 	num = static_cast<double>(static_cast<int>(num * 20.)) / 20.;  //precision to 0.05 degrees
	frequency[num]++;  } 
  
    double mode = 0; 
    int maxFrequency = 0; 
    for (const auto& pair : frequency) { 
      std::cout << pair.first << " freq " << pair.second << std::endl; 
      if (pair.second > maxFrequency) { 
	    maxFrequency = pair.second;     
            mode = pair.first; 
            } 
     } 
     thetasC.push_back(mode);  
/*
      int nav = 0;
      int nsize = theta_bin[np].size();
      double ratio = 0, thetatmp = 0;
	
      thetatmp = 0;
      for(int i = 0; i < nsize; i++){
	  if(theta_bin[np][i] > 45)continue;
	  thetatmp += theta_bin[np][i];
	  nav ++;
	}
	if(nav == 0) thetasC.push_back(0.);
	thetaC = thetatmp / nav;
	nav = 0;
	thetatmp = 0;
	for(int i = 0; i < nsize; i++){
	  ratio = theta_bin[np][i] / thetaC;
	  if(ratio > 1.2 || ratio < 0.8)continue;
	  thetatmp += theta_bin[np][i];
	  nav ++;
	}
	if(nav <= 1)thetasC.push_back(thetaC);
	thetaC = thetatmp / nav;
	thetasC.push_back(thetaC);

	theta_bin[nDetected].clear();
*/
     }
	return thetasC;
	thetasC.clear();

 } //end recoCherenkov
//.......................................................................

} //end ARICH::RECO
	
