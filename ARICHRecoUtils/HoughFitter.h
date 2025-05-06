#ifndef HOUGHFITTER_INCLUDE
#define HOUGHFITTER_INCLUDE
#include "stdlib.h"
#include <iostream>
#include <chrono>
#include <TROOT.h>
#include <TStyle.h>
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TH2Poly.h"
#include "TGraph.h"

namespace arichreco{
 
 class HoughFitter{
    
  public:
    
    HoughFitter(TH2D* event_histo);
    ~HoughFitter();
    
    std::vector<double> linspace(double minRadius, double maxRadius, int nPoints);
    
    std::vector<std::pair<double, double>> EdgePoints();
    
    
    
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
                                                             std::vector<double> Radii, int to_find);
    
    std::vector<TGraph*> createCircleGraph(std::vector<std::tuple<int, double>> circles, TH2Poly* hist, int nPoints);  //for tuple (bin, radius)
    std::vector<TGraph*> createCircleGraph(std::vector<std::tuple<int, int, double>> circles, int nPoints);            //for tuple (a,b, radius)
    
    TH2Poly* RecoveredHits(TGraph* circle, TH2Poly* event, int points, char* title);
    TH2Poly* PlotAccumulator(std::vector<std::vector<int>> & accumulator,std::vector<double> Radii);
   
    std::vector<std::tuple<int,int, double>> GetCirclesCenters(int to_find); //number of circles to find


 
private:

    int minRadius = 30;
    int maxRadius = 100;
    int nRadii = 700;
    int nPoints = 360;
    
    int rows = 350;
    int cols = 350;
    TH2D* fevent_histo;

    std::vector<std::pair<double, double>> edges;
    std::vector<double> radii;
    std::vector<std::vector<std::vector<int>>> accumulator;
};

}









#endif
