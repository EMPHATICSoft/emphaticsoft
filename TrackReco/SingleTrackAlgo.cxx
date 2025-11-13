////////////////////////////////////////////////////////////////////////
/// \brief   Class for single track algorithm
/// \author  Robert Chirco
/// \date    5/30/24
/// \song    Dejavu by Lolina
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "TrackReco/SingleTrackAlgo.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoUtils/RecoUtils.h"

#include "Geometry/service/GeometryService.h"

#include <vector>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

using namespace ROOT::Math;

namespace emph {
  
  //----------------------------------------------------------------------
  
  SingleTrackAlgo::SingleTrackAlgo() :
    fEvtNum(-1),
    nStations(-1),
    nPlanes(-1)
  {
    for (int i=0; i<3; ++i) {
      beamtrkvtx[i] = -999999.;
      beamtrkp[i] = -999999.;
      sectrkvtx[i] = -999999.;
      sectrkp[i] = -999999.;
    }
  }

  //----------------------------------------------------------------------

  SingleTrackAlgo::SingleTrackAlgo(int num, size_t nstations, size_t nplanes) :
    fEvtNum(num),
    nStations(nstations),
    nPlanes(nplanes)
  {
    for (int i=0; i<3; ++i) {
      beamtrkvtx[i] = -999999.;
      beamtrkp[i] = -999999.;
      sectrkvtx[i] = -999999.;
      sectrkp[i] = -999999.;
    }
  }

  //----------------------------------------------------------------------
  // Define functions
  //------------------------------------------------------------
  std::vector<rb::SpacePoint> SingleTrackAlgo::MakeHitsOrig(std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group) 
  {
    rb::SpacePoint sp;

    for (size_t i=0; i<nStations; i++){
      int nssds = 0;
      for (size_t j=0; j<ls_group[i].size(); j++){
        nssds += ls_group[i][j].size();
      }
      for (size_t j=0; j<nPlanes; j++){
        if (nssds == 2){ //station 0,1,4,7
          for (size_t k=0; k<ls_group[i][j].size(); k++) {
            for (size_t l=0; l<ls_group[i][j+1].size(); l++) {
              XYZVector fA( ls_group[i][j][k]->X0().X(), ls_group[i][j][k]->X0().Y(), ls_group[i][j][k]->X0().Z() );
              XYZVector fB( ls_group[i][j][k]->X1().X(), ls_group[i][j][k]->X1().Y(), ls_group[i][j][k]->X1().Z() );
              XYZVector fC( ls_group[i][j+1][l]->X0().X(), ls_group[i][j+1][l]->X0().Y(), ls_group[i][j+1][l]->X0().Z() );
              XYZVector fD( ls_group[i][j+1][l]->X1().X(), ls_group[i][j+1][l]->X1().Y(), ls_group[i][j+1][l]->X1().Z() );

              double x[3];
              double l1[3]; double l2[3];
              recoFcn.ClosestApproach(fA,fB,fC,fD,x,l1,l2,"SSD",false);

              //set SpacePoint object
              sp.SetX(x);

              sp.SetStation(i);
              spv.push_back(sp);
            }
          }
        }
        if (nssds == 3) { //station 2,3,5,6
          for (size_t k=0; k<ls_group[i][j].size(); k++) {
            for (size_t l=0; l<ls_group[i][j+1].size(); l++){
              for (size_t m=0; m<ls_group[i][j+2].size(); m++){
                XYZVector fA01( ls_group[i][j][k]->X0().X(), ls_group[i][j][k]->X0().Y(), ls_group[i][j][k]->X0().Z() );
                XYZVector fB01( ls_group[i][j][k]->X1().X(), ls_group[i][j][k]->X1().Y(), ls_group[i][j][k]->X1().Z() );
                XYZVector fC01( ls_group[i][j+1][l]->X0().X(), ls_group[i][j+1][l]->X0().Y(), ls_group[i][j+1][l]->X0().Z() );
                XYZVector fD01( ls_group[i][j+1][l]->X1().X(), ls_group[i][j+1][l]->X1().Y(), ls_group[i][j+1][l]->X1().Z() );

                double x01[3];
                double l1_01[3]; double l2_01[3];
                recoFcn.ClosestApproach(fA01,fB01,fC01,fD01,x01,l1_01,l2_01,"SSD",false);

                XYZVector fA02( ls_group[i][j][k]->X0().X(), ls_group[i][j][k]->X0().Y(), ls_group[i][j][k]->X0().Z() );
                XYZVector fB02( ls_group[i][j][k]->X1().X(), ls_group[i][j][k]->X1().Y(), ls_group[i][j][k]->X1().Z() );
                XYZVector fC02( ls_group[i][j+2][m]->X0().X(), ls_group[i][j+2][m]->X0().Y(), ls_group[i][j+2][m]->X0().Z() );
                XYZVector fD02( ls_group[i][j+2][m]->X1().X(), ls_group[i][j+2][m]->X1().Y(), ls_group[i][j+2][m]->X1().Z() );

                double x02[3];
                double l1_02[3]; double l2_02[3];
                recoFcn.ClosestApproach(fA02,fB02,fC02,fD02,x02,l1_02,l2_02,"SSD",false);

                XYZVector fA12( ls_group[i][j+1][l]->X0().X(), ls_group[i][j+1][l]->X0().Y(), ls_group[i][j+1][l]->X0().Z() );
                XYZVector fB12( ls_group[i][j+1][l]->X1().X(), ls_group[i][j+1][l]->X1().Y(), ls_group[i][j+1][l]->X1().Z() );
                XYZVector fC12( ls_group[i][j+2][m]->X0().X(), ls_group[i][j+2][m]->X0().Y(), ls_group[i][j+2][m]->X0().Z() );
                XYZVector fD12( ls_group[i][j+2][m]->X1().X(), ls_group[i][j+2][m]->X1().Y(), ls_group[i][j+2][m]->X1().Z() );

                double x12[3];
                double l1_12[3]; double l2_12[3];
                recoFcn.ClosestApproach(fA12,fB12,fC12,fD12,x12,l1_12,l2_12,"SSD",false);

                //average of three points (center of mass)
                double x[3];
                for (int i=0; i<3; i++){
                    x[i] = (x01[i]+x02[i]+x12[i])/3.;
                }

                //set SpacePoint object
                sp.SetX(x);

                sp.SetStation(i);
                spv.push_back(sp);
              }
            }
          }
        }
      }
    }
    return spv;
  }

  //------------------------------------------------------------
  void SingleTrackAlgo::doTwoPlanes(const rb::LineSegment* ls1, const rb::LineSegment* ls2, double x[3]){

    XYZVector fA( ls1->X0().X(), ls1->X0().Y(), ls1->X0().Z() );
    XYZVector fB( ls1->X1().X(), ls1->X1().Y(), ls1->X1().Z() );
    XYZVector fC( ls2->X0().X(), ls2->X0().Y(), ls2->X0().Z() );
    XYZVector fD( ls2->X1().X(), ls2->X1().Y(), ls2->X1().Z() );

    double l1[3]; double l2[3];
    recoFcn.ClosestApproach(fA,fB,fC,fD,x,l1,l2,"SSD",false);   

  }

  //------------------------------------------------------------

  void SingleTrackAlgo::doThreePlanes(const rb::LineSegment* ls1, const rb::LineSegment* ls2, const rb::LineSegment* ls3, double x[3]) {

    XYZVector fA01( ls1->X0().X(), ls1->X0().Y(), ls1->X0().Z() );
    XYZVector fB01( ls1->X1().X(), ls1->X1().Y(), ls1->X1().Z() );
    XYZVector fC01( ls2->X0().X(), ls2->X0().Y(), ls2->X0().Z() );
    XYZVector fD01( ls2->X1().X(), ls2->X1().Y(), ls2->X1().Z() );

    double x01[3];
    double l1_01[3]; double l2_01[3];
    recoFcn.ClosestApproach(fA01,fB01,fC01,fD01,x01,l1_01,l2_01,"SSD",false);

    XYZVector fA02( ls1->X0().X(), ls1->X0().Y(), ls1->X0().Z() );
    XYZVector fB02( ls1->X1().X(), ls1->X1().Y(), ls1->X1().Z() );
    XYZVector fC02( ls3->X0().X(), ls3->X0().Y(), ls3->X0().Z() );
    XYZVector fD02( ls3->X1().X(), ls3->X1().Y(), ls3->X1().Z() );

    double x02[3];
    double l1_02[3]; double l2_02[3];
    recoFcn.ClosestApproach(fA02,fB02,fC02,fD02,x02,l1_02,l2_02,"SSD",false);

    XYZVector fA12( ls2->X0().X(), ls2->X0().Y(), ls2->X0().Z() );
    XYZVector fB12( ls2->X1().X(), ls2->X1().Y(), ls2->X1().Z() );
    XYZVector fC12( ls3->X0().X(), ls3->X0().Y(), ls3->X0().Z() );
    XYZVector fD12( ls3->X1().X(), ls3->X1().Y(), ls3->X1().Z() );

    double x12[3];
    double l1_12[3]; double l2_12[3];
    recoFcn.ClosestApproach(fA12,fB12,fC12,fD12,x12,l1_12,l2_12,"SSD",false);   

    for (int i=0; i<3; i++){
      x[i] = (x01[i]+x02[i]+x12[i])/3.;
    }

  }

  //------------------------------------------------------------

  std::vector<rb::SpacePoint> SingleTrackAlgo::MakeHits(std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group, std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group) 
  {
    rb::SpacePoint sp;

    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();

    for (size_t i=0; i<nStations; i++){
      int nUnique = 0;
      for (size_t j=0; j<ls_group[i].size(); j++){
        if (ls_group[i][j].size() != 0) nUnique++;
      }
      int nPlanesGeo = emgeo->GetSSDStation(i)->NPlanes();

      if (nUnique < 2) continue; // can't make a space point

      for (size_t j=0; j<ls_group[i].size(); j++){
        if (nPlanesGeo == 2){ //station 0,1,4,7
          for (size_t k=0; k<ls_group[i][j].size(); k++){
            for (size_t l=0; l<ls_group[i][j+1].size(); l++){
	            double x[3];
              doTwoPlanes(ls_group[i][j][k],ls_group[i][j+1][l],x);

	      //std::cout<<"cl_group[i][j][k] = "<<cl_group[i][j][k]->WgtRmsStrip()<<std::endl;
              //std::cout<<"cl_group[i][j+1][k] = "<<cl_group[i][j+1][k]->WgtRmsStrip()<<std::endl;
              sp.SetX(x);
              sp.SetStation(i);
              spv.push_back(sp);
	            spv.back().Add(*ls_group[i][j][k]);
	            spv.back().Add(*ls_group[i][j+1][l]);
	            spv.back().Add(*cl_group[i][j][k]);
	            spv.back().Add(*cl_group[i][j+1][l]);
            }
          }
        }
        else if (nPlanesGeo == 3){ //station 2,3,5,6
          for (size_t k=0; k<ls_group[i][j].size(); k++){
            for (size_t l=0; l<ls_group[i][j+1].size(); l++){
	            if (nUnique < nPlanesGeo){
                double x[3];
                doTwoPlanes(ls_group[i][j][k],ls_group[i][j+1][l],x); 

                //std::cout<<"cl_group[i][j][k] = "<<cl_group[i][j][k]->WgtRmsStrip()<<std::endl;
                //std::cout<<"cl_group[i][j+1][l] = "<<cl_group[i][j+1][l]->WgtRmsStrip()<<std::endl;		
                sp.SetX(x);
		            sp.SetStation(i);
                spv.push_back(sp);
                spv.back().Add(*ls_group[i][j][k]);
                spv.back().Add(*ls_group[i][j+1][l]);
                spv.back().Add(*cl_group[i][j][k]);
                spv.back().Add(*cl_group[i][j+1][l]);
              }
              else{
                for (size_t m=0; m<ls_group[i][j+2].size(); m++){
                  double x[3];
                  doThreePlanes(ls_group[i][j][k],ls_group[i][j+1][l],ls_group[i][j+2][m],x);

                  //std::cout<<"cl_group[i][j][k] = "<<cl_group[i][j][k]->WgtRmsStrip()<<std::endl;	
                  //std::cout<<"cl_group[i][j+1][l] = "<<cl_group[i][j+1][l]->WgtRmsStrip()<<std::endl;
                  //std::cout<<"cl_group[i][j+2][m] = "<<cl_group[i][j+2][m]->WgtRmsStrip()<<std::endl;
                  sp.SetX(x);
                  sp.SetStation(i);
                  spv.push_back(sp);
                  spv.back().Add(*ls_group[i][j][k]);
                  spv.back().Add(*ls_group[i][j+1][l]);
                  spv.back().Add(*ls_group[i][j+2][m]);
                  spv.back().Add(*cl_group[i][j][k]);
                  spv.back().Add(*cl_group[i][j+1][l]);
                  spv.back().Add(*cl_group[i][j+2][m]);
	              }
              }
            }
          }
        }
      }
    }

    return spv;

  }

  //------------------------------------------------------------

  std::vector<rb::TrackSegment> SingleTrackAlgo::MakeLines(std::vector<std::vector<double>> sp1, std::vector<std::vector<double>> sp2, std::vector<std::vector<double>> sp3) 
  {
    //segment 1 -> don't need to fit anything, just connect two points
    double lfirst1[3]; double llast1[3];
    lfirst1[0] = sp1[0][0];
    lfirst1[1] = sp1[0][1];
    lfirst1[2] = sp1[0][2];

    llast1[0] = sp1[1][0];
    llast1[1] = sp1[1][1];
    llast1[2] = sp1[1][2];

    //test
    double lfirst1t[3]; double llast1t[3];
    recoFcn.findLine(sp1,lfirst1t,llast1t);
    //std::cout<<"Before (lfirst): "<<lfirst1[0]<<", "<<lfirst1[1]<<", "<<lfirst1[2]<<std::endl;
    //std::cout<<"After  (lfirst): "<<lfirst1t[0]<<", "<<lfirst1t[1]<<", "<<lfirst1t[2]<<std::endl;
    //std::cout<<"Before  (llast): "<<llast1[0]<<", "<<llast1[1]<<", "<<llast1[2]<<std::endl;
    //std::cout<<"After   (llast): "<<llast1t[0]<<", "<<llast1t[1]<<", "<<llast1t[2]<<std::endl;
    //std::cout<<"....................."<<std::endl;

    for (int i=0; i<3; i++){
      if (std::fabs(lfirst1[i] - lfirst1t[i]) > 1E-3) std::cout<<"BAD LF @ "<<i<<std::endl;
      if (std::fabs(llast1[i] - llast1t[i]) > 1E-3) std::cout<<"BAD LL @ "<<i<<std::endl; 
    }

    //segment 2  
    double lfirst2[3]; double llast2[3];
    recoFcn.findLine(sp2,lfirst2,llast2);

    //segment 3
    double lfirst3[3]; double llast3[3];
    recoFcn.findLine(sp3,lfirst3,llast3);

    //assign to track vector
    rb::LineSegment track1 = rb::LineSegment(lfirst1,llast1);
    rb::LineSegment track2 = rb::LineSegment(lfirst2,llast2);
    rb::LineSegment track3 = rb::LineSegment(lfirst3,llast3);

    // create rb::TrackSegments and insert them into the vector
    rb::TrackSegment ts1 = rb::TrackSegment();
    for (auto p : spv)
      if (p.Station() == 0 || p.Station() == 1)
        ts1.Add(p);
    ts1.vtx.SetCoordinates(lfirst1);
    ts1.pointA.SetCoordinates(lfirst1);
    ts1.pointB.SetCoordinates(llast1);
    double p[3];
    double dx = llast1[0]-lfirst1[0];
    double dy = llast1[1]-lfirst1[1];
    double dz = llast1[2]-lfirst1[2];

    p[0] = dx/dz;
    p[1] = dy/dz;
    p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
    ts1.mom.SetCoordinates(p);

    rb::TrackSegment ts2 = rb::TrackSegment();
    for (auto p : spv)
      if (p.Station() == 2 || p.Station() == 3 || p.Station() == 4)
        ts2.Add(p);
    ts2.vtx.SetCoordinates(lfirst2);
    ts2.pointA.SetCoordinates(lfirst2);
    ts2.pointB.SetCoordinates(llast2);
    dx = llast2[0]-lfirst2[0];
    dy = llast2[1]-lfirst2[1];
    dz = llast2[2]-lfirst2[2];

    p[0] = dx/dz;
    p[1] = dy/dz;
    p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
    ts2.mom.SetCoordinates(p);

    rb::TrackSegment ts3 = rb::TrackSegment();
    for (auto p : spv)
      if (p.Station() == 5 || p.Station() == 6 || p.Station() == 7)
        ts3.Add(p);
    ts3.vtx.SetCoordinates(lfirst3);
    ts3.pointA.SetCoordinates(lfirst3);
    ts3.pointB.SetCoordinates(llast3);
    dx = llast3[0]-lfirst3[0];
    dy = llast3[1]-lfirst3[1];
    dz = llast3[2]-lfirst3[2];

    p[0] = dx/dz;
    p[1] = dy/dz;
    p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
    ts3.mom.SetCoordinates(p);

    double ts2_dot_ts3 = ts2.mom.Dot(ts3.mom);//P()[0]*ts3.P()[0]+ts2.P()[1]*ts3.P()[1]+ts2.P()[2]*ts3.P()[2];
    double ts2_mag = sqrt(ts2.mom.Mag2());//P()[0]*ts2.P()[0]+ts2.P()[1]*ts2.P()[1]+ts2.P()[2]*ts2.P()[2]);
    double ts3_mag = sqrt(ts3.mom.Mag2());//P()[0]*ts3.P()[0]+ts3.P()[1]*ts3.P()[1]+ts3.P()[2]*ts3.P()[2]);
    double recoBendAngle = TMath::ACos(ts2_dot_ts3/(ts2_mag*ts3_mag));
    double recop = recoFcn.getMomentum(recoBendAngle);

    //change track segments
    double realp1[3];
    realp1[2] = recop*ts1.mom.Z();
    realp1[0] = ts1.mom.X()*realp1[2];
    realp1[1] = ts1.mom.Y()*realp1[2];
    ts1.mom.SetCoordinates(realp1);
    tsv.push_back(ts1);

    double realp2[3];
    realp2[2] = recop*ts2.mom.Z();
    realp2[0] = ts2.mom.X()*realp2[2];
    realp2[1] = ts2.mom.Y()*realp2[2];
    ts2.mom.SetCoordinates(realp2);
    tsv.push_back(ts2);

    double realp3[3];
    realp3[2] = recop*ts3.mom.Z();
    realp3[0] = ts3.mom.X()*realp3[2];
    realp3[1] = ts3.mom.Y()*realp3[2];
    ts3.mom.SetCoordinates(realp3);
    tsv.push_back(ts3);

    double recopz = ts2.mom.Z();
    double recopx = ts2.mom.X();
    double recopy = ts2.mom.Y();

    sectrkp[0] = recopx;
    sectrkp[1] = recopy;
    sectrkp[2] = recopz;

    XYZVector a(lfirst1[0],lfirst1[1],lfirst1[2]);
    XYZVector b(llast1[0],llast1[1],llast1[2]);
    XYZVector c(lfirst2[0],lfirst2[1],lfirst2[2]);
    XYZVector d(llast2[0],llast2[1],llast2[2]);
    double l0t[3];
    double l1t[3];
    recoFcn.ClosestApproach(a,b,c,d,sectrkvtx,l0t,l1t,"TrackSegment",false);

    return tsv;

  }

  //------------------------------------------------------------

  void SingleTrackAlgo::getCombinations(std::vector<std::vector<rb::SpacePoint>> &matrix, int row, std::vector<rb::SpacePoint> &combination, std::vector<std::vector<rb::SpacePoint>> &result,int stop, int s) {

    if ((int)combination.size() == stop){
      result.push_back(combination);
      return;
    }
    if (row > s) {
      return;
    }

    for (int r = row; r <= s; ++r) {
      if (matrix[r].empty()) continue;
      for (auto val : matrix[r]) {
        combination.push_back(val);
        getCombinations(matrix, r + 1, combination, result, stop, s);
        combination.pop_back();
      }
    }

  }

  //------------------------------------------------------------

  std::vector<rb::TrackSegment> SingleTrackAlgo::MakeTrackSeg(std::vector<rb::SpacePoint> spacepoints)
  {

    // Reorganize input
    std::vector<std::vector<rb::SpacePoint>> spmatrix;
    spmatrix.resize(nStations);
    std::vector<rb::TrackSegment> alltrackcombos;

    for (size_t i=0; i<spacepoints.size(); i++){
      int station = spacepoints[i].Station();
      spmatrix[station].push_back(spacepoints[i]);
    }

    // Make all combinations of space points
    int combos = 1;
    for (size_t i=0; i<spmatrix.size(); i++){
      for (size_t j=0; j<spmatrix[i].size(); j++){ 
        if (spmatrix[i].size() == 0) combos *= 1;
        else combos *= (int)spmatrix[i].size();
      }
    }

    std::vector<std::vector<rb::SpacePoint>> sptmp;
    std::vector<rb::SpacePoint> combination;

    // Here, r is the starting point for getCombinations algorithm
    int r=0;
    for (auto row : spmatrix){
      if (row.size() != 0){ 
        r = row[0].Station();
        break;
      }
    }
    int s=0;
    for (auto row : spmatrix){
      if (row.size() != 0){
        s = row[0].Station();
      }
    }

    getCombinations(spmatrix, r, combination, sptmp,2, s);
    getCombinations(spmatrix, r, combination, sptmp,3, s);


    // If you care about requiring the first two stations
    // to be present in a TrackSegment

    /*
    // Now that check the first two stations are included
    std::vector<std::vector<rb::SpacePoint>> validcombos;

    // Define the organizer (what groups of stations do you want in a TrackSegment?)
    std::vector<std::vector<int>> organizer{ 
        {0, 1}, 
        {2, 3, 4},
        {5, 6, 7}
    };

    for (size_t a = 0; a < sptmp.size(); a++) {
      if (sptmp[a].size() > 2) {
        validcombos.push_back(sptmp[a]);
        continue; // Skip if the combination is not of size 2, if size 3 you can just add it to validcombos
      }

      int first = sptmp[a][0].Station();
      int second = sptmp[a][1].Station();

      for (const auto& row : organizer) {
        // The first element of the pair should match the first element of the row
        if (row.front() == first) {
          // Ensure that the next element exists in the row and matches the second element
          auto it = std::find(row.begin(), row.end(), first);
          size_t index = std::distance(row.begin(), it);
          // Check that the next element in the row matches the second element
          if (index + 1 < row.size() && row[index + 1] == second) {
            // If both elements are found next to each other, push the pair into validcombos
            validcombos.push_back(sptmp[a]);
            break; // We found a match, no need to check further rows
          }
        }
	//else return; // if TrackSegment does not include the first station of the organizer, we can still make a track in another region
	               // if we return, we do not
      }
    }
    // Below, you would need to replace sptmp with validcombos

    */

    //for (size_t a=0; a<validcombos.size(); a++){
    for (size_t a=0; a<sptmp.size(); a++){
      // Find line of best fit and return endpoints
      double lfirst[3]; double llast[3];

      std::vector<std::vector<double>> sppos;
      for (size_t k=0; k<sptmp[a].size(); k++){
        //std::vector<double> x = {validcombos[a][k].Pos()[0],validcombos[a][k].Pos()[1],validcombos[a][k].Pos()[2]};
        std::vector<double> x = {sptmp[a][k].Pos()[0],sptmp[a][k].Pos()[1],sptmp[a][k].Pos()[2]};
        sppos.push_back(x);
      }
      recoFcn.findLine(sppos,lfirst,llast);

      // Create rb::TrackSegments and insert them into the vector
      rb::TrackSegment ts = rb::TrackSegment();
      //for (auto p : validcombos[a]){
      for (auto p : sptmp[a]){
        ts.Add(p);
      }
      ts.vtx.SetCoordinates(lfirst);
      ts.pointA.SetCoordinates(lfirst);
      ts.pointB.SetCoordinates(llast);

      // Set null momentum
      double p0[3] = {0.,0.,0.};
      ts.mom.SetCoordinates(p0);

      float chi2tot = 0.;
      for (auto p : sptmp[a]){
	for (size_t i=0; i<p.NLineSegments(); i++){
 
 	  XYZVector x0(p.GetLineSegment(i)->X0()); //.X(),p.GetLineSegment(i)->X0().Y(),p.GetLineSegment(i)->X0().Z());
          XYZVector x1(p.GetLineSegment(i)->X1()); //.X(),p.GetLineSegment(i)->X1().Y(),p.GetLineSegment(i)->X1().Z());

          XYZVector a(ts.pointA); //()[0],ts.A()[1],ts.A()[2]);
          XYZVector b(ts.pointB); //()[0],ts.B()[1],ts.B()[2]);
          double f1[3]; double f2[3]; double f3[3];
          recoFcn.ClosestApproach(x0,x1,a,b,f1,f2,f3,"SSD",false);
          float pull = sqrt((f3[0]-f2[0])*(f3[0]-f2[0])+(f3[1]-f2[1])*(f3[1]-f2[1])+(f3[2]-f2[2])*(f3[2]-f2[2]));

          double sensorz = x0.Z(); //(2); //s[2];

          double t = ( sensorz - a.Z() )/( b.Z() - a.Z() );
          double tsx = a.X() + (b.X()-a.X())*t;
          double tsy = a.Y() + (b.Y()-a.Y())*t;

          double xz = a.X() + ts.mom.X()/ts.mom.Z()*sensorz;
          double yz = a.Y() + ts.mom.Y()/ts.mom.Z()*sensorz;
          // signed distance from point to a line
          double la = x1.Y() - x0.Y();
          double lb = x0.X() - x1.X();
          double lc = x0.Y()*(x1.X()-x0.X()) - (x1.Y()-x0.Y())*x0.X();
          float dsign = (la*tsx + lb*tsy + lc)/(sqrt(la*la + lb*lb));
	  
          double sigma = p.GetSSDCluster(i)->WgtRmsStrip()*0.06;
	  double rms = p.GetSSDCluster(i)->WgtAvgStrip()*0.06;
	  //if (sigma == 0) std::cout<<"sig0, avg: "<<p.GetSSDCluster(i)->WgtAvgStrip()<<" and rms = "<<p.GetSSDCluster(i)->WgtRmsStrip()<<std::endl;
	  if (sigma == 0) std::cout<<"ndigits = "<<p.GetSSDCluster(i)->NDigits()<<std::endl;
	  if (sigma == 0) std::cout<<"Station, Plane, Sensor = "<<p.GetSSDCluster(i)->Station()<<", "<<p.GetSSDCluster(i)->Plane()<<", "<<p.GetSSDCluster(i)->Sensor()<<std::endl;
	  if (sigma == 0) std::cout<<"width = "<<p.GetSSDCluster(i)->Width()<<std::endl;
	  double chi2 = dsign*dsign/sigma/sigma;

	  chi2tot += chi2;

	}
      }
      ts.chi2 = chi2tot;
      alltrackcombos.push_back(ts);
      
    }

    combination.clear();
    sptmp.clear();
    //validcombos.clear();

    return alltrackcombos;
  }

  //------------------------------------------------------------

  void SingleTrackAlgo::SetBeamTrk(rb::TrackSegment &ts1, double pbeam)
  {
    SetPtmp(ts1);  

    double p[3];
    p[2] = pbeam * ts1.mom.Z();
    p[0] = p[2] * ts1.mom.X();
    p[1] = p[2] * ts1.mom.Y();

    ts1.mom.SetCoordinates(p);  

    ts1.pointA.GetCoordinates(beamtrkvtx);

    for (int i=0; i<3; i++){
      beamtrkp[i] = p[i]; 
    }
  }

  //------------------------------------------------------------

  void SingleTrackAlgo::SetPtmp(rb::TrackSegment &ts)
  {
    auto dx = ts.pointB - ts.pointA;
    /*
    double dx = ts.B()[0]-ts.A()[0];
    double dy = ts.B()[1]-ts.A()[1];
    double dz = ts.B()[2]-ts.A()[2];
    */

    double pxpz = dx.X()/dx.Z();
    double pypz = dx.Y()/dx.Z();
    double pzpmag = 1./sqrt(1. + pxpz*pxpz + pypz*pypz);//(dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));

    double ptmp[3] = {pxpz,pypz,pzpmag};

    ts.mom.SetCoordinates(ptmp);
  }

  //------------------------------------------------------------

  void SingleTrackAlgo::SetRecoTrk(rb::TrackSegment &ts2, rb::TrackSegment &ts3, int pm)
  {
    SetPtmp(ts2);
    SetPtmp(ts3);

    double ts2_dot_ts3 = ts2.mom.Dot(ts3.mom);//P()[0]*ts3.P()[0]+ts2.P()[1]*ts3.P()[1]+ts2.P()[2]*ts3.P()[2];
    double ts2_mag = sqrt(ts2.mom.Mag2());//P()[0]*ts2.P()[0]+ts2.P()[1]*ts2.P()[1]+ts2.P()[2]*ts2.P()[2]);
    double ts3_mag = sqrt(ts3.mom.Mag2());//P()[0]*ts3.P()[0]+ts3.P()[1]*ts3.P()[1]+ts3.P()[2]*ts3.P()[2]);
    double recoBendAngle = TMath::ACos(ts2_dot_ts3/(ts2_mag*ts3_mag));
    double recop = recoFcn.getMomentum(recoBendAngle);

    // Change TrackSegments
    double realp2[3];
    realp2[2] = pm*recop*ts2.mom.Z();
    realp2[0] = ts2.mom.X()*realp2[2];
    realp2[1] = ts2.mom.Y()*realp2[2];
    ts2.mom.SetCoordinates(realp2);

    double realp3[3];
    realp3[2] = pm*recop*ts3.mom.Z();
    realp3[0] = ts3.mom.X()*realp3[2];
    realp3[1] = ts3.mom.Y()*realp3[2];
    ts3.mom.SetCoordinates(realp3);
  }

  //------------------------------------------------------------

  double* SingleTrackAlgo::SetTrackInfo(rb::TrackSegment &ts1, rb::TrackSegment &ts2)
  {
    XYZVector a(ts1.pointA);//()[0],ts1.A()[1],ts1.A()[2]);
    XYZVector b(ts1.pointB);//()[0],ts1.B()[1],ts1.B()[2]);
    XYZVector c(ts2.pointA);//()[0],ts2.A()[1],ts2.A()[2]);
    XYZVector d(ts2.pointB);//()[0],ts2.B()[1],ts2.B()[2]);
    double l0t[3];
    double l1t[3];
    recoFcn.ClosestApproach(a,b,c,d,sectrkvtx,l0t,l1t,"TrackSegment",false);

    return sectrkvtx;
  }

  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const SingleTrackAlgo& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Event Number = "       << std::setw(5) << std::right << h.GetEvtNum();
    o << " Number of Stations = " << std::setw(5) << std::right << h.NStations();
    o << " Number of Planes = "   << std::setw(5) << std::right << h.NPlanes();

    return o;
  }

  //------------------------------------------------------------

 
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
