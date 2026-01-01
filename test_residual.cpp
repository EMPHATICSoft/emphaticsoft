// Test script to verify residual functionality
#include "RecoUtils/RecoUtils.h"
#include <iostream>
#include <vector>

int main() {
    ru::RecoUtils recoFcn(0);
    
    // Create test track segments
    // Upstream segment (z=0 to z=281)
    std::vector<double> point1 = {10.0, 5.0, 100.0};
    std::vector<double> point2 = {12.0, 6.0, 200.0};
    
    // Downstream segment (z=505 to z=619)  
    std::vector<double> point3 = {14.5, 7.5, 550.0};
    std::vector<double> point4 = {15.5, 8.0, 600.0};
    
    rb::TrackSegment ts1, ts2;
    
    // Set up segments (this is simplified - normally would use SpacePoints)
    ts1.SetA(point1.data());
    ts1.SetB(point2.data());
    
    ts2.SetA(point3.data());
    ts2.SetB(point4.data());
    
    double intersection[3];
    double residual;
    
    std::cout << "Testing findTrackIntersectionNew with residual..." << std::endl;
    
    // Test the new function with residual
    recoFcn.findTrackIntersectionNew(ts1, ts2, intersection, &residual);
    
    std::cout << "Intersection point: (" << intersection[0] << ", " 
              << intersection[1] << ", " << intersection[2] << ")" << std::endl;
    std::cout << "Residual: " << residual << " mm" << std::endl;
    
    // Test without residual parameter
    recoFcn.findTrackIntersectionNew(ts1, ts2, intersection);
    
    std::cout << "Without residual - Intersection point: (" << intersection[0] << ", " 
              << intersection[1] << ", " << intersection[2] << ")" << std::endl;
    
    return 0;
}