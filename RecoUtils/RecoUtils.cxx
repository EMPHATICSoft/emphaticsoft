////////////////////////////////////////////////////////////////////////
/// \brief   Class for reconstruction functions
/// \author  Robert Chirco
/// \date    10/30/23
/// \song    High speed calm air tonight by ML Buch
////////////////////////////////////////////////////////////////////////

#include "RecoUtils/RecoUtils.h"
#include "Simulation/SSDHit.h"
#include "RecoBase/TrackSegment.h"

#include <vector>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "TVector3.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

namespace ru {
  
  //----------------------------------------------------------------------
  
  RecoUtils::RecoUtils() :
     fEvtNum(-1)
  {
  }
  
  //------------------------------------------------------------

  RecoUtils::RecoUtils(int num)
  {
     fEvtNum = num;
  }

  //------------------------------------------------------------

  void RecoUtils::ClosestApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double F[3], double l1[3], double l2[3], const char* type, bool verbose){

     double r12 = (B-A).Dot(B-A);
     double r22 = (D-C).Dot(D-C);

     double d4321 = (D-C).Dot(B-A);
     double d3121 = (C-A).Dot(B-A);
     double d4331 = (D-C).Dot(C-A);

     double s = (-d4321*d4331 + d3121*r22) / (r12*r22 - d4321*d4321);
     double t = (d4321*d3121 - d4331*r12) / (r12*r22 - d4321*d4321);

     //std::cout<<"s: "<<s<<std::endl;
     //std::cout<<"t: "<<t<<std::endl;

     if (strcmp(type,"SSD") == 0){
       if ( s >= 0 && s <= 1 && t >=0 && t <= 1){
         //std::cout<<"Closest approach all good :)"<<std::endl;
         for (int i=0; i<3; i++){
           l1[i] = A(i) + s*(B(i) - A(i));
           l2[i] = C(i) + t*(D(i) - C(i));
         }
         //std::cout<<"CA CHECK (L1)...x: "<<L1[0]<<"   y: "<<L1[1]<<"   z: "<<L1[2]<<std::endl;
         //std::cout<<"CA CHECK (L2)...x: "<<L2[0]<<"   y: "<<L2[1]<<"   z: "<<L2[2]<<std::endl;
       }
       else{
         //this should be very rare
         if (verbose){
           std::cout<<"Closest approach calculation exception @ event "<<fEvtNum<<std::endl;
           std::cout<<"A: ("<<A(0)<<","<<A(1)<<","<<A(2)<<")"<<std::endl;
           std::cout<<"B: ("<<B(0)<<","<<B(1)<<","<<B(2)<<")"<<std::endl;
           std::cout<<"C: ("<<C(0)<<","<<C(1)<<","<<C(2)<<")"<<std::endl;
           std::cout<<"D: ("<<D(0)<<","<<D(1)<<","<<D(2)<<")"<<std::endl;
           std::cout<<"How do line segments AB and CD look if you draw them in the beam view (i.e. the same plane)?"<<std::endl;
           std::cout<<"And don't worry! A hit is still created, but the line segments (probably) come close to intersecting...but don't"<<std::endl;
	 }

         double sbound[2] = {0.,1.};
	 double tbound[2] = {0.,1.};

         ClampedApproach(A,B,C,D,l1,l2,sbound,tbound,type,verbose);
       }
     }
     else{ //i.e. "TrackSegment"

       double sl1 = (D(2) + 10. - A(2))/(B(2) - A(2));
       double tl2 = (A(2) - 10. - C(2))/(D(2) - C(2));

       if ( s >= 0 && s <= sl1 && t >=tl2 && t <= 1){
         //std::cout<<"Normal"<<std::endl;
         for (int i=0; i<3; i++){
           l1[i] = A(i) + s*(B(i) - A(i));
           l2[i] = C(i) + t*(D(i) - C(i));
         }
       }
       else{
         double sbound[2] = {0.,sl1};  
         double tbound[2] = {tl2,1.};

	 TVector3 Aext;
	 TVector3 Dext;

         for (int i=0; i<3; i++){
	   Dext(i) = A(i) + sl1*(B(i) - A(i));
	   Aext(i) = C(i) + tl2*(D(i) - C(i));
	 }	
	 //std::cout<<"Clamped"<<std::endl;
	 ClampedApproach(Aext,B,C,Dext,l1,l2,sbound,tbound,type,verbose);
       }
     }

     //set point of closest approach
     for (int i=0; i<3; i++){
       F[i] = (l1[i] + l2[i])/2.;
     }

  }

  //------------------------------------------------------------

  void RecoUtils::ClampedApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double l1[3], double l2[3], double sbound[2], double tbound[2], const char* type, bool verbose){

     double r12 = (B-A).Dot(B-A);
     double r22 = (D-C).Dot(D-C);

     double d3121 = (C-A).Dot(B-A);
     double d4331 = (D-C).Dot(C-A);

     TVector3 l1p3;
     TVector3 l1p4;
     TVector3 l2p1;
     TVector3 l2p2;

     double d4121 = (D-A).Dot(B-A);
     double d4332 = (D-C).Dot(C-B);

     double s_l1p3 = d3121/r12;
     double s_l1p4 = d4121/r12;
     double t_l2p1 = -d4331/r22;
     double t_l2p2 = -d4332/r22;

     s_l1p3 = std::clamp(s_l1p3,sbound[0],sbound[1]);
     s_l1p4 = std::clamp(s_l1p4,sbound[0],sbound[1]);
     t_l2p1 = std::clamp(t_l2p1,tbound[0],tbound[1]);
     t_l2p2 = std::clamp(t_l2p2,tbound[0],tbound[1]);

     double d_l1p3;
     double d_l1p4;
     double d_l2p1;
     double d_l2p2;

     for (int i=0; i<3; i++){
       l1p3(i) = A(i) + s_l1p3*(B(i) - A(i));
       l1p4(i) = A(i) + s_l1p4*(B(i) - A(i));
       l2p1(i) = C(i) + t_l2p1*(D(i) - C(i));
       l2p2(i) = C(i) + t_l2p2*(D(i) - C(i));
     }

     //this is squared distance
     d_l1p3 = (C-l1p3).Dot(C-l1p3);
     d_l1p4 = (D-l1p4).Dot(D-l1p4);
     d_l2p1 = (A-l2p1).Dot(A-l2p1);
     d_l2p2 = (B-l2p2).Dot(B-l2p2);

     if (strcmp(type,"SSD") == 0){
       if (d_l1p3 < d_l1p4){
         for (int i=0; i<3; i++) { l1[i] = l1p3(i); }
       }
       else{
         for (int i=0; i<3; i++) { l1[i] = l1p4(i); }
       }
       if (d_l2p1 < d_l2p2){
         for (int i=0; i<3; i++) { l2[i] = l2p1(i); }
       }
       else{
         for (int i=0; i<3; i++) { l2[i] = l2p2(i); }
       }
     }
     else{
       // for TrackSegment you want to compare the minimum fo l1p3 and l2p2 
       // beause p2 and p3 are the points closest to the "intersection"
       if (d_l1p4 < d_l2p1){
         for (int i=0; i<3; i++) { l1[i] = l1p4(i); }
	 for (int i=0; i<3; i++) { l2[i] = D(i); if (verbose) std::cout<<"CLAMPED EXCEPTION D @ "<<D(2)<<std::endl; }
       }
       else{
         for (int i=0; i<3; i++) { l2[i] = l2p1(i); }
         for (int i=0; i<3; i++) { l1[i] = A(i); if (verbose) std::cout<<"CLAMPED EXCEPTION A @ "<<A(2)<<std::endl;}
       }
     }
  }

  //------------------------------------------------------------

  void RecoUtils::findLine(std::vector<std::vector<double>> v, double lfirst[3], double llast[3])
  {
          //a line can be parameterized by L = A + tN
          //A is a point that lies on the line
          //N is a normalized direction vector
          //t is a real number

	  int N = v.size();

          double mean[3] = {0., 0., 0.};
          double corr[3][3] = {0.};
          for(auto p : v)
          {
             //construct A which is the mean value of all points
             mean[0] += p[0];
             mean[1] += p[1];
             mean[2] += p[2];
             //construct correlation matrix
             for(int i = 0; i < 3; i++){
                for(int j = i; j < 3; j++){
                   corr[i][j] += p[i] * p[j];
                }
             }
          }
          for (int i = 0; i < 3; i++){
              mean[i] /= N;
              for(int j = i; j < 3; j++){
                 corr[i][j] /= N;
              }
          }
          //construct covariance matrix
          double cov_arr[] = { corr[0][0] - mean[0] * mean[0], corr[0][1] - mean[0] * mean[1], corr[0][2] - mean[0] * mean[2],
                               corr[0][1] - mean[0] * mean[1], corr[1][1] - mean[1] * mean[1], corr[1][2] - mean[1] * mean[2],
                               corr[0][2] - mean[0] * mean[2], corr[1][2] - mean[2] * mean[1], corr[2][2] - mean[2] * mean[2] };
          TMatrixDSym cov(3,cov_arr);
          TMatrixDSymEigen cov_e(cov);
          //find N by solving the eigenproblem for the covariance matrix
          TVectorD eig = cov_e.GetEigenValues();
          TMatrixD eigv = cov_e.GetEigenVectors();

          //take the eigenvector corresponding to the largest eigenvalue,
          //corresponding to the solution N
          double eig_max;
          eig_max = std::max(eig[0], std::max(eig[1], eig[2]));

          //std::cout<<"eig"<<std::endl;
          //eig.Print();
          //std::cout<<"eigmax: "<<eig_max<<std::endl;

          int el = -1;
          
          for (int i = 0; i < 3; i++){
             if (eig[i] == eig_max){ el = i; break; }
          }
          if (el == -1) {
            MF_LOG_ERROR("RecoUtils") << "Error: eig_max not found in eig array!";
            return;  // or throw an exception or handle this case appropriately
          }
      
          double n[3];
          n[0] = eigv[0][el];
          n[1] = eigv[1][el];
          n[2] = eigv[2][el];

          //we can create any point L on the line by varying t

          //create endpoints at first and last station z-position
          //find t where z = v[0][2] then z = [v.size()-1][2]
          double tfirst; double tlast;

          tfirst = (v[0][2] - mean[2])/n[2];
          tlast  = (v[v.size()-1][2] - mean[2])/n[2];

          lfirst[0] = mean[0] + tfirst*n[0];
          lfirst[1] = mean[1] + tfirst*n[1];
          lfirst[2] = v[0][2];

          llast[0] = mean[0] + tlast*n[0];
          llast[1] = mean[1] + tlast*n[1];
          llast[2] = v[v.size()-1][2];

  } 
  
  //------------------------------------------------------------

  double RecoUtils::findRecoAngle(double p1[3], double p2[3], double p3[3], double p4[3])
  {
     //create line in cartesian coordinates
     double a1 = p2[0] - p1[0]; double a2 = p4[0] - p3[0];
     double b1 = p2[1] - p1[1]; double b2 = p4[1] - p3[1];
     double c1 = p2[2] - p1[2]; double c2 = p4[2] - p3[2];

     TVector3 m1(a1,b1,c1);
     TVector3 m2(a2,b2,c2);

     double theta_rad = m1.Angle(m2);

     return theta_rad;
  }

  //------------------------------------------------------------

  double RecoUtils::findTruthAngle(std::vector<sim::SSDHit> sim_i, std::vector<sim::SSDHit> sim_f){
     double p_ix=0.; double p_iy=0.; double p_iz=0.;
     double p_fx=0.; double p_fy=0.; double p_fz=0.;

     for (auto i : sim_i){
         p_ix += i.GetPx();
         p_iy += i.GetPy();
         p_iz += i.GetPz();
     }
     p_ix /= sim_i.size();
     p_iy /= sim_i.size();
     p_iz /= sim_i.size();

     for (auto f : sim_f){
         p_fx += f.GetPx();
         p_fy += f.GetPy();
         p_fz += f.GetPz();
     }
     p_fx /= sim_f.size();
     p_fy /= sim_f.size();
     p_fz /= sim_f.size();

     TVector3 p_i(p_ix,p_iy,p_iz);
     TVector3 p_f(p_fx,p_fy,p_fz);

     double theta_rad = p_i.Angle(p_f);

     return theta_rad;

  }

  //------------------------------------------------------------
  
  double RecoUtils::getMomentum(double theta){

     //convert from rad to mrad
     theta = theta*1000.;

     double pz = 64.649/theta; 
     //double pz = 69.2004/theta - 2.8854/theta/theta;
     return pz;	
  }

  //------------------------------------------------------------

   void RecoUtils::findTrackIntersection(rb::TrackSegment trk1, rb::TrackSegment trk2, double point[3]){

     TVector3 p1(trk1.P()[0],trk1.P()[1],trk1.P()[2]);
     TVector3 p2(trk2.P()[0],trk2.P()[1],trk2.P()[2]);
     TVector3 a = p1.Cross(p2);
     double dot = a.Dot(a);

     if (dot == 0) std::cout<<"Parallel"<<std::endl;

     TVector3 ab(trk2.Vtx()[0]-trk1.Vtx()[0],trk2.Vtx()[1]-trk1.Vtx()[1],trk2.Vtx()[2]-trk1.Vtx()[2]);
 
     TVector3 b = ab.Cross(p2);

     double t = b.Dot(a) / dot;

     point[0] = trk1.Vtx()[0] + (t*p1(0));
     point[1] = trk1.Vtx()[1] + (t*p1(1));
     point[2] = trk1.Vtx()[2] + (t*p1(2));
  }

  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const RecoUtils& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Event Number = "     << std::setw(5) << std::right << h.GetEvtNum();

    return o;
  }

  //------------------------------------------------------------

 
} // end namespace ru
//////////////////////////////////////////////////////////////////////////////
