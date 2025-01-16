#include "ARICHRecoUtils/HoughFitter.h"

using namespace std;

namespace arichreco{

HoughFitter::HoughFitter(TH2Poly* event){
}

HoughFitter::~HoughFitter(){
    
}

std::vector<double> HoughFitter::linspace(double minRadius, double maxRadius, int nPoints) {
    std::vector<double> radii(nPoints);
    double step = (maxRadius - minRadius) / (nPoints - 1);  // Calculate step size

    for (int i = 0; i < nPoints; i++) {
        radii[i] = minRadius + i * step;  // Generate each radius
    }

    return radii;
}


std::vector<std::pair<double, double>> HoughFitter::EdgePoints(TH2Poly* event){
    
    std::vector<std::pair<double, double>> edgePoints;
    TList* listBins = event->GetBins();
    
    for (int i = 0; i <= event->GetNumberOfBins(); i++) {
            double content = event->GetBinContent(i);
        
            if (content > 0) { // Consider bins with hits (content = 1)
                
                TH2PolyBin* th2Bins = (TH2PolyBin*)listBins->At(i);
                double xmin = th2Bins->GetXMin();
                double xmax = th2Bins->GetXMax();
                double ymin = th2Bins->GetYMin();
                double ymax = th2Bins->GetYMax();
                edgePoints.push_back({(xmin+xmax)/2 - 6, (ymin+ymax)/2}); // Store bin center as an edge point
            }
        }
    return edgePoints;
}

void HoughFitter::HoughCircleTransform(std::vector<std::pair<double, double>>& edgePoints, TH2Poly* event,std::vector<double> radii, int steps,
                                       std::vector<std::vector<int>> & accumulator){
    accumulator.resize(event->GetNumberOfBins(), std::vector<int>(steps + 1, 0));
    // Iterate over each edge point
    for (const auto& point : edgePoints) {
        double x = point.first;
        double y = point.second;

        // For each radius
        for (int idr = 0; idr <= steps; idr++) {
            // For each angle from 0 to 360 degrees
            for (int theta = 0; theta < 360; theta++) {
                double rad = theta * M_PI / 180.0;  // Convert degrees to radians
                int a = static_cast<int>(x - radii[idr] * std::cos(rad));  // Center x (a)
                int b = static_cast<int>(y - radii[idr] * std::sin(rad));  // Center y (b)
                int n_bin =  event->FindBin(a,b);
                
                if(n_bin < 0 || n_bin >= event->GetNumberOfBins())continue;
                 accumulator[n_bin][idr]++;  // Increment accumulator
                }
            }
        }
    }

void HoughFitter::HoughCircleTransform(std::vector<std::pair<double, double>>& edgePoints, TH2Poly* event,std::vector<double> radii, int steps,
                                       std::vector<std::vector<int>> & accumulator, int SSD_bin){
    accumulator.resize(event->GetNumberOfBins(), std::vector<int>(steps + 1, 0));
    // Iterate over each edge point
    for (const auto& point : edgePoints) {
        double x = point.first;
        double y = point.second;

        // For each radius
        for (int idr = 0; idr <= steps; idr++) {
            // For each angle from 0 to 360 degrees
            for (int theta = 0; theta < 360; theta++) {
                double rad = theta * M_PI / 180.0;  // Convert degrees to radians
                int a = static_cast<int>(x - radii[idr] * std::cos(rad));  // Center x (a)
                int b = static_cast<int>(y - radii[idr] * std::sin(rad));  // Center y (b)
                int n_bin =  event->FindBin(a,b);
                
                if(n_bin < 0 || n_bin >= event->GetNumberOfBins())continue;
                if(SSD_bin == n_bin)accumulator[n_bin][idr] +=2;
                 accumulator[n_bin][idr]++;  // Increment accumulator
                }
            }
        }
    }



void HoughFitter::HoughCircleTransform(const std::vector<std::pair<double, double>>& edgePoints,std::vector<double> radii, int steps, int cols,
                               int rows, std::vector<std::vector<std::vector<int>>>& accumulator){
    // Initialize the 3D accumulator: (center_y, center_x, radius)
    accumulator.resize(rows, std::vector<std::vector<int>>(cols, std::vector<int>(steps + 1, 0)));

    // Iterate over each edge point
    for (const auto& point : edgePoints) {
        double x = point.first;
        double y = point.second;

        // For each radius
        for (int idr = 0; idr <= steps; idr++) {
            // For each angle from 0 to 360 degrees
            for (int theta = 0; theta < 360; theta++) {
                double rad = theta * M_PI / 180.0;  // Convert degrees to radians
                int a = static_cast<int>(x - radii[idr] * std::cos(rad)) + 80;  // Center x (a)
                int b = static_cast<int>(y - radii[idr] * std::sin(rad)) + 80;  // Center y (b)
                
               
                // Ensure (a, b) is within bounds
                if (a >= 0 && a < cols && b >= 0 && b < rows) {
                    accumulator[b][a][idr]++;  // Increment accumulator
                }
            }
        }
    }
}



std::vector<std::tuple<int, double>> HoughFitter::findBestCircle(const std::vector<std::vector<int>>& accumulator, int Nbins, std::vector<double> Radii, int to_find, int SSD_center_bin ) {
    
    std::vector<std::tuple<int, double>> circles;
    int best_score = 0;

       // Iterate through the accumulator to find the max number of votes
       for (int bin = 0; bin < Nbins; bin++) {
           for (int idr = 0; idr < Radii.size(); idr++) {
               if (accumulator[bin][idr] > best_score) {
                   if(bin != SSD_center_bin)continue;
                   best_score = accumulator[bin][idr];
                   circles.insert(circles.begin(), std::make_tuple(bin, Radii[idr]));
               }
               else{
                   circles.insert(circles.end(),std::make_tuple(bin, Radii[idr]));
               }
               if(circles.size() > to_find){circles.pop_back();}
           }
       }

    // Return the best-fitting circle (center_x, center_y, radius)
    return circles;
}
std::vector<std::tuple<int, int, double>> HoughFitter::findBestCircle(const std::vector<std::vector<std::vector<int>>>& accumulator, std::vector<double> Radii, int to_find, int SSD_center_bin) {
  
   int best_score = 0;
   std::vector<std::tuple<int, int, double>> circles;
   int rows = accumulator.size();
   int cols = accumulator[0].size();
   int radiusRange = Radii.size();

   // Iterate through the accumulator to find the max number of votes
   for (int y = 0; y < rows; y++) {
       for (int x = 0; x < cols; x++) {
           for (int idr = 0; idr < radiusRange; idr++) {
               if (accumulator[y][x][idr] > best_score) {
                   best_score = accumulator[y][x][idr];
                   circles.insert(circles.begin(), std::make_tuple(x-80, y-80, Radii[idr]));
               }
               else{
                   circles.insert(circles.end(),std::make_tuple(x-80, y-80, Radii[idr]));
               }

               if(circles.size() > to_find){circles.pop_back();}
           }
       }
   }
   // Return the best-fitting circle (center_x, center_y, radius)
    return circles;
}

std::vector<TGraph*> HoughFitter::createCircleGraph(std::vector<std::tuple<int, double>> circles, TH2Poly* hist, int nPoints) {
    
    std::vector<TGraph*> circleGraphs;
    
    for(auto circle : circles){
        TGraph* graph_circle = new TGraph(nPoints);
        int bin= std::get<0>(circle);
        double radius = std::get<1>(circle);
        //cout << "bin " << bin << " radius " << radius << endl;
        TH2PolyBin* thisBin = (TH2PolyBin*)hist->GetBins()->At(bin);

        double centerX = (thisBin->GetXMax() + thisBin->GetXMin())/2;
        double centerY = (thisBin->GetYMax() + thisBin->GetYMin())/2;
        // Generate points along the circle
        for (int i = 0; i < nPoints; i++) {
            double theta = 2 * M_PI * i / nPoints;  // Angle in radians
            double x = centerX + radius * std::cos(theta);  // X-coordinate of the point
            double y = centerY + radius * std::sin(theta);  // Y-coordinate of the point
            graph_circle->SetPoint(i, x, y);
        }

    // Optionally set circle style (line color, width, etc.)
        graph_circle->SetLineColor(kRed);
        graph_circle->SetLineWidth(1);
        circleGraphs.push_back(graph_circle);
    }
    
    return circleGraphs;
}

std::vector<TGraph*> HoughFitter::createCircleGraph(std::vector<std::tuple<int, int, double>> circles, int nPoints) {
    
    std::vector<TGraph*> circleGraphs;
    
    for(auto circle : circles){
    
        TGraph* graph_circle = new TGraph(nPoints);
        double centerX = std::get<0>(circle);
        double centerY = std::get<1>(circle);
        double radius = std::get<2>(circle);
        //cout << "X " << centerX <<  " Y " << centerY << " radius " << radius << endl;
    // Generate points along the circle
    for (int i = 0; i < nPoints; i++) {
        double theta = 2 * M_PI * i / nPoints;  // Angle in radians
        double x = centerX + radius * std::cos(theta);  // X-coordinate of the point
        double y = centerY + radius * std::sin(theta);  // Y-coordinate of the point
        graph_circle->SetPoint(i, x, y);
    }

    // Optionally set circle style (line color, width, etc.)
        graph_circle->SetLineColor(kGreen);
        graph_circle->SetLineWidth(1);
        circleGraphs.push_back(graph_circle);
    }
    
    return circleGraphs;
}


TH2Poly* HoughFitter::RecoveredHits(TGraph* circle, TH2Poly* event, int points, char* title){
    
    TH2Poly* recovered = (TH2Poly*)event->Clone();
    recovered->SetTitle(title);recovered->SetName(title);
    
    for(int point =0; point < points; point++){
        
        double point_x = circle->GetPointX(point);
        double point_y = circle->GetPointY(point);
        
        
        int n_bin =  event->FindBin(point_x,point_y);
        if (event->GetBinContent(n_bin) == 0){
            recovered->SetBinContent(n_bin,0.5);
        }
    }
    
    return recovered;
}


TH2Poly* HoughFitter::PlotAccumulator(std::vector<std::vector<int>> & accumulator,std::vector<double> Radii ){
    
    TH2Poly* accumulator_graph  = new TH2Poly();
    int Nbins = accumulator.size();
    int dr = Radii.size();
    cout << Nbins << " "  << Radii.size() << endl;
    
    for (int bin = 1; bin < Nbins; bin++) {
        for (int idr = 0; idr < dr; idr++) {
            
            accumulator_graph->AddBin(bin,Radii[idr], bin+1, Radii[idr+1]);
            int bb = accumulator_graph->FindBin(bin, Radii[idr]);
            accumulator_graph->SetBinContent(bb, accumulator[bin][idr]*100);
            
        }
    }
    cout << "Filled" << endl;
    accumulator_graph->SetTitle("Accumulator 2D");
    return accumulator_graph;
}



}//end namespace arichreco
