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

#include <vector>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace emph {
  
  //----------------------------------------------------------------------
  
  SingleTrackAlgo::SingleTrackAlgo() :
    fEvtNum(-1),
    nStations(-1),
    nPlanes(-1)
  {
    for (int i=0; i<3; ++i) {
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
      sectrkvtx[i] = -999999.;
      sectrkp[i] = -999999.;
    }
  }

  //----------------------------------------------------------------------

  // Define functions

  //------------------------------------------------------------

  std::vector<rb::SpacePoint> SingleTrackAlgo::MakeHits(std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group) 
{
     rb::SpacePoint sp;

     //ru::RecoUtils recoFcn = ru::RecoUtils(fEvtNum);

     for (size_t i=0; i<nStations; i++){
         int nssds = 0;
         for (size_t j=0; j<ls_group[i].size(); j++){
           nssds += ls_group[i][j].size();
         }
         for (size_t j=0; j<nPlanes; j++){
             if (nssds == 2){ //station 0,1,4,7
                for (size_t k=0; k<ls_group[i][j].size(); k++){
                    for (size_t l=0; l<ls_group[i][j+1].size(); l++){
                        TVector3 fA( ls_group[i][j][k]->X0().X(), ls_group[i][j][k]->X0().Y(), ls_group[i][j][k]->X0().Z() );
                        TVector3 fB( ls_group[i][j][k]->X1().X(), ls_group[i][j][k]->X1().Y(), ls_group[i][j][k]->X1().Z() );
                        TVector3 fC( ls_group[i][j+1][l]->X0().X(), ls_group[i][j+1][l]->X0().Y(), ls_group[i][j+1][l]->X0().Z() );
                        TVector3 fD( ls_group[i][j+1][l]->X1().X(), ls_group[i][j+1][l]->X1().Y(), ls_group[i][j+1][l]->X1().Z() );

                        double x[3];
                        double l1[3]; double l2[3];
                        recoFcn.ClosestApproach(fA,fB,fC,fD,x,l1,l2,"SSD");

                        //set SpacePoint object
                        sp.SetX(x);

                        sp.SetStation(i);
                        spv.push_back(sp);
                    }
                }
             }
             if (nssds == 3){ //station 2,3,5,6
                for (size_t k=0; k<ls_group[i][j].size(); k++){
                    for (size_t l=0; l<ls_group[i][j+1].size(); l++){
                        for (size_t m=0; m<ls_group[i][j+2].size(); m++){
                            TVector3 fA01( ls_group[i][j][k]->X0().X(), ls_group[i][j][k]->X0().Y(), ls_group[i][j][k]->X0().Z() );
                            TVector3 fB01( ls_group[i][j][k]->X1().X(), ls_group[i][j][k]->X1().Y(), ls_group[i][j][k]->X1().Z() );
                            TVector3 fC01( ls_group[i][j+1][l]->X0().X(), ls_group[i][j+1][l]->X0().Y(), ls_group[i][j+1][l]->X0().Z() );
                            TVector3 fD01( ls_group[i][j+1][l]->X1().X(), ls_group[i][j+1][l]->X1().Y(), ls_group[i][j+1][l]->X1().Z() );

                            double x01[3];
                            double l1_01[3]; double l2_01[3];
                            recoFcn.ClosestApproach(fA01,fB01,fC01,fD01,x01,l1_01,l2_01,"SSD");

                            TVector3 fA02( ls_group[i][j][k]->X0().X(), ls_group[i][j][k]->X0().Y(), ls_group[i][j][k]->X0().Z() );
                            TVector3 fB02( ls_group[i][j][k]->X1().X(), ls_group[i][j][k]->X1().Y(), ls_group[i][j][k]->X1().Z() );
                            TVector3 fC02( ls_group[i][j+2][m]->X0().X(), ls_group[i][j+2][m]->X0().Y(), ls_group[i][j+2][m]->X0().Z() );
                            TVector3 fD02( ls_group[i][j+2][m]->X1().X(), ls_group[i][j+2][m]->X1().Y(), ls_group[i][j+2][m]->X1().Z() );

                            double x02[3];
                            double l1_02[3]; double l2_02[3];
                            recoFcn.ClosestApproach(fA02,fB02,fC02,fD02,x02,l1_02,l2_02,"SSD");

                            TVector3 fA12( ls_group[i][j+1][l]->X0().X(), ls_group[i][j+1][l]->X0().Y(), ls_group[i][j+1][l]->X0().Z() );
                            TVector3 fB12( ls_group[i][j+1][l]->X1().X(), ls_group[i][j+1][l]->X1().Y(), ls_group[i][j+1][l]->X1().Z() );
                            TVector3 fC12( ls_group[i][j+2][m]->X0().X(), ls_group[i][j+2][m]->X0().Y(), ls_group[i][j+2][m]->X0().Z() );
                            TVector3 fD12( ls_group[i][j+2][m]->X1().X(), ls_group[i][j+2][m]->X1().Y(), ls_group[i][j+2][m]->X1().Z() );

                            double x12[3];
                            double l1_12[3]; double l2_12[3];
                            recoFcn.ClosestApproach(fA12,fB12,fC12,fD12,x12,l1_12,l2_12,"SSD");

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

  std::vector<rb::TrackSegment> SingleTrackAlgo::MakeLines(std::vector<std::vector<double>> sp1, std::vector<std::vector<double>> sp2, std::vector<std::vector<double>> sp3) 
  {
    //ru::RecoUtils recoFcn2 = ru::RecoUtils(fEvtNum);

    //segment 1 -> don't need to fit anything, just connect two points
    double lfirst1[3]; double llast1[3];
    lfirst1[0] = sp1[0][0];
    lfirst1[1] = sp1[0][1];
    lfirst1[2] = sp1[0][2];

    llast1[0] = sp1[1][0];
    llast1[1] = sp1[1][1];
    llast1[2] = sp1[1][2];

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
    ts1.SetVtx(lfirst1);
    double p[3];
    double dx = llast1[0]-lfirst1[0];
    double dy = llast1[1]-lfirst1[1];
    double dz = llast1[2]-lfirst1[2];

    p[0] = dx/dz;
    p[1] = dy/dz;
    p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
    ts1.SetP(p);

    rb::TrackSegment ts2 = rb::TrackSegment();
    for (auto p : spv)
      if (p.Station() == 2 || p.Station() == 3 || p.Station() == 4)
        ts2.Add(p);
    ts2.SetVtx(lfirst2);
    dx = llast2[0]-lfirst2[0];
    dy = llast2[1]-lfirst2[1];
    dz = llast2[2]-lfirst2[2];

    p[0] = dx/dz;
    p[1] = dy/dz;
    p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
    ts2.SetP(p);

    rb::TrackSegment ts3 = rb::TrackSegment();
    for (auto p : spv)
      if (p.Station() == 5 || p.Station() == 6 || p.Station() == 7)
        ts3.Add(p);
    ts3.SetVtx(lfirst3);
    dx = llast3[0]-lfirst3[0];
    dy = llast3[1]-lfirst3[1];
    dz = llast3[2]-lfirst3[2];

    p[0] = dx/dz;
    p[1] = dy/dz;
    p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
    ts3.SetP(p);

    double ts2_dot_ts3 = ts2.P()[0]*ts3.P()[0]+ts2.P()[1]*ts3.P()[1]+ts2.P()[2]*ts3.P()[2];
    double ts2_mag = sqrt(ts2.P()[0]*ts2.P()[0]+ts2.P()[1]*ts2.P()[1]+ts2.P()[2]*ts2.P()[2]);
    double ts3_mag = sqrt(ts3.P()[0]*ts3.P()[0]+ts3.P()[1]*ts3.P()[1]+ts3.P()[2]*ts3.P()[2]);
    double recoBendAngle = TMath::ACos(ts2_dot_ts3/(ts2_mag*ts3_mag));
    double recop = recoFcn.getMomentum(recoBendAngle);

    //change track segments
    double realp1[3];
    realp1[2] = recop*ts1.P()[2];
    realp1[0] = ts1.P()[0]*realp1[2];
    realp1[1] = ts1.P()[1]*realp1[2];
    ts1.SetP(realp1);
    tsv.push_back(ts1);

    double realp2[3];
    realp2[2] = recop*ts2.P()[2];
    realp2[0] = ts2.P()[0]*realp2[2];
    realp2[1] = ts2.P()[1]*realp2[2];
    ts2.SetP(realp2);
    tsv.push_back(ts2);

    double realp3[3];
    realp3[2] = recop*ts3.P()[2];
    realp3[0] = ts3.P()[0]*realp3[2];
    realp3[1] = ts3.P()[1]*realp3[2];
    ts3.SetP(realp3);
    tsv.push_back(ts3);

    double recopz = ts2.P()[2];
    double recopx = ts2.P()[0];
    double recopy = ts2.P()[1];

    sectrkp[0] = recopx;
    sectrkp[1] = recopy;
    sectrkp[2] = recopz;

    TVector3 a(lfirst1[0],lfirst1[1],lfirst1[2]);
    TVector3 b(llast1[0],llast1[1],llast1[2]);
    TVector3 c(lfirst2[0],lfirst2[1],lfirst2[2]);
    TVector3 d(llast2[0],llast2[1],llast2[2]);
    double l0t[3];
    double l1t[3];
    recoFcn.ClosestApproach(a,b,c,d,sectrkvtx,l0t,l1t,"TrackSegment");

    return tsv;

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
