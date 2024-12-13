#ifndef HOUGHFITTER_INCLUDE
#define HOUGHFITTER_INCLUDE
#include "stdlib.h"
#include <iostream>
#include <chrono>
#include <TROOT.h>
#include <TStyle.h>
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TMatrixT.h"
#include "TVector3.h"
#include "TEllipse.h"
#include "TCutG.h"
#include "TH2Poly.h"

namespace ARICHRECO{
 
 class HoughFitter{
    
  public:
    
    HoughFitter(TH2Poly* event);
    ~HoughFitter();
    
    std::vector<double> linspace(double minRadius, double maxRadius, int nPoints);
    
    std::vector<std::pair<double, double>> EdgePoints(TH2Poly* event);
    
    
    
    void HoughCircleTransform(std::vector<std::pair<double, double>>& edgePoints,         //use this to fit the best bin for the center
                              TH2Poly* event,std::vector<double> radii,
                              int steps,std::vector<std::vector<int>> & accumulator);
    
    void HoughCircleTransform(std::vector<std::pair<double, double>>& edgePoints,
                              TH2Poly* event,std::vector<double> radii,
                              int steps, std::vector<std::vector<int>> & accumulator,
                              int SSD_bin);
    
    void HoughCircleTransform(const std::vector<std::pair<double, double>>& edgePoints,     //use this for fitting best coordinates a,b of the center
                              std::vector<double> radii,
                              int steps, int cols, int rows,
                              std::vector<std::vector<std::vector<int>>>& accumulator);
    
    
    
    std::vector<std::tuple<int, double>> findBestCircle(const std::vector<std::vector<int>>& accumulator,  //for binned accumulator
                                                             int Nbins, std::vector<double> Radii, int to_find, int SSD_center_bin);
    
    std::vector<std::tuple<int, int, double>> findBestCircle(const std::vector<std::vector<std::vector<int>>>& accumulator, //for coordinate acumulator
                                                             std::vector<double> Radii, int to_find, int SSD_center_bin);
    
    std::vector<TGraph*> createCircleGraph(std::vector<std::tuple<int, double>> circles, TH2Poly* hist, int nPoints);  //for tuple (bin, radius)
    std::vector<TGraph*> createCircleGraph(std::vector<std::tuple<int, int, double>> circles, int nPoints);            //for tuple (a,b, radius)
    
    TH2Poly* RecoveredHits(TGraph* circle, TH2Poly* event, int points, char* title);
    TH2Poly* PlotAccumulator(std::vector<std::vector<int>> & accumulator,std::vector<double> Radii);
    
private:
    
    int nPoints = 360;
};

}









#endif
