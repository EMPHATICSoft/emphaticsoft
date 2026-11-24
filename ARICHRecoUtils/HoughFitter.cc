#include "ARICHRecoUtils/HoughFitter.h"

using namespace std;

namespace arichreco{

HoughFitter::HoughFitter(TH2D* event_histo){
	fevent_histo = event_histo;
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


std::vector<std::pair<double, double>> HoughFitter::EdgePoints(){
    

    std::vector<std::pair<double, double>> edgePoints;

    
    for (int i = 0; i <= fevent_histo->GetNbinsX(); i++){
	 for (int j = 0; j <= fevent_histo->GetNbinsY(); j++){

            double content = fevent_histo->GetBinContent(i,j);
        
	    
            if (content > 0) { // Consider bins with hits (content = 1)
               
		float center_bin_x = fevent_histo->GetXaxis()->GetBinCenter(i);
		float center_bin_y = fevent_histo->GetYaxis()->GetBinCenter(j); 
                edgePoints.push_back({center_bin_x,center_bin_y}); // Store bin center as an edge point
            }
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

    std::vector<std::vector<int>> coarseAcc(rows, std::vector<int>(cols, 0));
    const int coarseStride = std::max(1, (int)edgePoints.size() / 200); // cap cost regardless of cluster size
    const int coarseAngleStep = 10; // degrees
    for (size_t ip = 0; ip < edgePoints.size(); ip += coarseStride) {
        double x = edgePoints[ip].first, y = edgePoints[ip].second;
        for (int idr = 0; idr <= steps; idr += 2) {
            for (int theta = 0; theta < 360; theta += coarseAngleStep) {
                double rad = theta * M_PI / 180.0;
                int a = std::lround(x - radii[idr] * std::cos(rad)) + 80;
                int b = std::lround(y - radii[idr] * std::sin(rad)) + 80;
                if (a >= 0 && a < cols && b >= 0 && b < rows) coarseAcc[b][a]++;
            }
        }
    }
    int cx_est = cols / 2, cy_est = rows / 2, coarseBest = -1;
    for (int b = 0; b < rows; ++b) {
        for (int a = 0; a < cols; ++a) {
            if (coarseAcc[b][a] > coarseBest) { coarseBest = coarseAcc[b][a]; cy_est = b; cx_est = a; }
        }
    }

    // ---- Pass 2: full-resolution refinement around the coarse estimate ----
    int roi = 10;
    int xmin = std::max(0, cx_est - roi);
    int xmax = std::min(cols - 1, cx_est + roi);

    int ymin = std::max(0, cy_est - roi);
    int ymax = std::min(rows - 1, cy_est + roi);

    //std::cout << "boundaries x:" << xmin  << "; " << xmax << " y:"<< ymin << " " << ymax << std::endl;

    // Iterate over each edge point
    for (const auto& point : edgePoints) {
        double x = point.first;
        double y = point.second;

        // For each radius
        for (int idr = 0; idr <= steps; idr++) {
            // For each angle from 0 to 360 degrees
            for (int theta = 0; theta < 360; theta++) {
                double rad = theta * M_PI / 180.0;  // Convert degrees to radians
                int a = std::lround(x - radii[idr] * std::cos(rad)) + 80;  // Center x (a)
                int b = std::lround(y - radii[idr] * std::sin(rad)) + 80;  // Center y (b)
                
               
                // Ensure (a, b) is within bounds
                if (a >= xmin && a <= xmax && b >= ymin && b <= ymax) {
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
           for (int idr = 0; idr < (int)Radii.size(); idr++) {
               if (accumulator[bin][idr] > best_score) {
                   if(bin != SSD_center_bin)continue;
                   best_score = accumulator[bin][idr];
                   circles.insert(circles.begin(), std::make_tuple(bin, Radii[idr]));
               }
               else{
                   circles.insert(circles.end(),std::make_tuple(bin, Radii[idr]));
               }
               if((int)circles.size() > to_find){circles.pop_back();}
           }
       }

    // Return the best-fitting circle (center_x, center_y, radius)
    return circles;
}
std::vector<std::tuple<int, int, double>> HoughFitter::findBestCircle(const std::vector<std::vector<std::vector<int>>>& accumulator, std::vector<double> Radii, int to_find) {

   int best_score = 0;
   std::vector<std::tuple<int, int, double>> circles;
   int rows = accumulator.size();
   int cols = accumulator[0].size();
   int radiusRange = Radii.size();

   for (int y = 0; y < rows; y++) {
       for (int x = 0; x < cols; x++) {
           for (int idr = 0; idr < radiusRange; idr++) {
               if (accumulator[y][x][idr] >= best_score) {
                   best_score = accumulator[y][x][idr];
                   circles.insert(circles.begin(), std::make_tuple(x-80, y-80, Radii[idr]));
               }
               else{
                   circles.insert(circles.end(),std::make_tuple(x-80, y-80, Radii[idr]));
               }

               if((int)circles.size() > to_find){circles.pop_back();}
           }
       }
   }
   lastBestScore = best_score;
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

std::vector<std::vector<std::vector<int>>> HoughFitter::SmoothAccumulator(
    const std::vector<std::vector<std::vector<int>>>& accumulator, int dCell, int dRad)
{
    int nrows = (int)accumulator.size();
    int ncols = nrows ? (int)accumulator[0].size() : 0;
    int nrad  = ncols ? (int)accumulator[0][0].size() : 0;
    std::vector<std::vector<std::vector<int>>> smoothed(
        nrows, std::vector<std::vector<int>>(ncols, std::vector<int>(nrad, 0)));

    for (int y = 0; y < nrows; ++y) {
        for (int x = 0; x < ncols; ++x) {
            for (int r = 0; r < nrad; ++r) {
                int sum = 0;
                for (int dy = -dCell; dy <= dCell; ++dy) {
                    int yy = y + dy;
                    if (yy < 0 || yy >= nrows) continue;
                    for (int dx = -dCell; dx <= dCell; ++dx) {
                        int xx = x + dx;
                        if (xx < 0 || xx >= ncols) continue;
                        for (int dr = -dRad; dr <= dRad; ++dr) {
                            int rr = r + dr;
                            if (rr < 0 || rr >= nrad) continue;
                            sum += accumulator[yy][xx][rr];
                        }
                    }
                }
                smoothed[y][x][r] = sum;
            }
        }
    }
    return smoothed;
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


std::vector<std::vector<std::vector<int>>> HoughFitter::GetAccumulator(){
     
    return this->accumulator;
}

std::vector<std::tuple<int,int, double>> HoughFitter::GetCirclesCenters(int to_find)
{

	edges =  this->EdgePoints();
	radii = this->linspace(minRadius,maxRadius,nRadii);
	this->HoughCircleTransform(edges, radii, nRadii, cols, rows, accumulator);

	std::vector<std::vector<std::vector<int>>> smoothedAcc =
	    (fSmoothDCell > 0 || fSmoothDRad > 0)
	    ? this->SmoothAccumulator(accumulator, fSmoothDCell, fSmoothDRad)
	    : accumulator;
	std::vector<std::tuple<int,int,double>> circles = this->findBestCircle(smoothedAcc, radii,to_find);
	if (minVoteFraction > 0. && !edges.empty() &&
	    lastBestScore < minVoteFraction * (double)edges.size()) {
		return {};
	}
	return circles;

}//end GetCirclesCenters

	
}//end namespace arichreco
