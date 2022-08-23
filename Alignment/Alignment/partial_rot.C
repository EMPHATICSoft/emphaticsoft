#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"
#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <string>
#include <algorithm> 
#include <stdio.h>
#include <string.h> 
#include <numeric>
#include "track.C"


double running_chi = 0;


double chi_squared(vector<double> Variables, int event){
   cout<<running_chi<<endl;
   for(int i=0; i< Variables.size();i++)cout<<Variables[i]<<", ";
   //cout<<endl;

   double xtot_shift = Variables[8];
   double ytot_shift = Variables[9];
   //Input: vector of offsets and starting event
   // Returns chi squared of a single track

   // Variables[0] first  x-station rotation
   // Variables[1] first  y-station rotation
   // Variables[2] second x-station rotation
   // Variables[3] second y-station rotation
   // Variables[4] third  x-station rotation
   // Variables[5] third  y-station rotation
   // Variables[6] fourth x-station rotation
   // Variables[7] fourth y-station rotation
   // Variables[8] x-total shift
   // Variables[9] y-total shift
 


   vector<vector<double>> ypos = {{},{},{},{},{},{}};
   vector<vector<double>> yzpos = {{},{},{},{},{},{}};
   vector<vector<double>> xpos = {{},{},{},{},{},{}};
   vector<vector<double>> xzpos = {{},{},{},{},{},{}};
 
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   
  for(int i=bounds[event]+1;i<=bounds[event+1];i++){
      t1->GetEntry(i);

      //Filling vectors for xz-plot
      if(fer ==0 && module==1){
         xpos[0].push_back((-row+320)*ssdPitch-xresiduals[0]-xtot_shift);
         xzpos[0].push_back(station0_zpos);
      }else if(fer ==0 && module==3){
         xpos[1].push_back((-row+320)*ssdPitch-xresiduals[1]-xtot_shift);
         xzpos[1].push_back(station0_zpos+station_thickness);
      }else if(fer ==1 && module==1){
         xpos[2].push_back((-row+320)*ssdPitch-xresiduals[2]-xtot_shift);
         xzpos[2].push_back(station1_zpos);
      }else if(fer ==1 && module==3){
         xpos[3].push_back((-row+320)*ssdPitch-xresiduals[3]-xtot_shift);
         xzpos[3].push_back(station1_zpos+station_thickness);
      }else if(fer ==2 && module==0){
         xpos[4].push_back((row)*ssdPitch-xresiduals[4]-xtot_shift);
         xzpos[4].push_back(station2_zpos);
      }else if(fer ==2 && module==1){
         xpos[4].push_back((-row)*ssdPitch-xresiduals[5]-xtot_shift);
         xzpos[4].push_back(station2_zpos);
      }else if(fer ==3 && module==0){
         xpos[5].push_back((-row+640)*ssdPitch-xresiduals[6]-xtot_shift);
         xzpos[5].push_back(station2_zpos+station2_thickness);
      }else if(fer ==3 && module==1){
         xpos[5].push_back((row-640)*ssdPitch-xresiduals[7]-xtot_shift);
         xzpos[5].push_back(station2_zpos+station2_thickness);

      //Filling vectors for yz-plot
      }else if(fer ==0 && module==0){
         ypos[0].push_back((row-320)*ssdPitch-yresiduals[0]-ytot_shift);
         yzpos[0].push_back(station0_zpos);
      }else if(fer ==0 && module==2){
         ypos[1].push_back((row-320)*ssdPitch-yresiduals[1]-ytot_shift);
         yzpos[1].push_back(station0_zpos+station_thickness);     
      }else if(fer ==1 && module==0){
         ypos[2].push_back((row-320)*ssdPitch-yresiduals[2]-ytot_shift);
         yzpos[2].push_back(station1_zpos);     
      }else if(fer ==1 && module==2){
         ypos[3].push_back((row-320)*ssdPitch-yresiduals[3]-ytot_shift);
         yzpos[3].push_back(station1_zpos+station_thickness);     
      }else if(fer ==2 && module==2){
         ypos[4].push_back((row-640)*ssdPitch-yresiduals[4]-ytot_shift);
         yzpos[4].push_back(station2_zpos);     
      }else if(fer ==2 && module==3){
         ypos[4].push_back(-(row-640)*ssdPitch-yresiduals[5]-ytot_shift);
         yzpos[4].push_back(station2_zpos);     
      }else if(fer ==3 && module==2){
         ypos[5].push_back((row-640)*ssdPitch-yresiduals[6]-ytot_shift);
         yzpos[5].push_back(station2_zpos+station2_thickness);     
      }else if(fer ==3 && module==3){
         ypos[5].push_back(-(row-640)*ssdPitch-yresiduals[7]-ytot_shift);
         yzpos[5].push_back(station2_zpos+station2_thickness);
         
      }


   }
  
  
   // get x,y,z positions
   int count = 0; 
   for(int i=0;i<xpos.size();i++)  if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0)  count ++;
   //cout<<endl<<count<<endl;
   Double_t xwithout_rot[count], ywithout_rot[count], zlist[count],xfinal[count],yfinal[count];
   int num = 0;
   

   //apply the total station rotations
   for(int i=0;i<xpos.size();i++){
      if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0){
         if(yzpos[i][0]!=xzpos[i][0])continue;
         double x = getAverage(xpos[i]);
         double y = getAverage(ypos[i]);
         xwithout_rot[num] = x;
         ywithout_rot[num] = y;
         xfinal[num] = x;
         yfinal[num] = y;
         zlist[num] = getAverage(xzpos[i]);
         num++;

      }        
   }
   


   //Rotation of the first 8 SSDs
   //Rotating the first 4 stations

   for(int i=0;i<4;i++){
      double xc = 0 - xresiduals[i]-xtot_shift;
      double yc = 0 - yresiduals[i]-ytot_shift;
      double x_angle = Variables[2*i];
      double y_angle = Variables[2*i+1];
      xfinal[i] = (xc + (xwithout_rot[i]-xc)/cos(x_angle) - yc*tan(x_angle) - ((ywithout_rot[i]-yc)/cos(y_angle))*tan(x_angle))/(1+tan(x_angle)*tan(y_angle));
      yfinal[i] = (yc + (ywithout_rot[i]-yc)/cos(y_angle) + xc*tan(y_angle) + ((xwithout_rot[i]-xc)/cos(x_angle))*tan(y_angle))/(1+tan(x_angle)*tan(y_angle));
   }    

   
   for(int i=0;i<2;i++){
      if(xpos[i+4][0] > xcenters[i]-xtot_shift && ypos[i+4][0] > ycenters[i]-ytot_shift){
         //first quandrant
         double xc = 19.2 - xresiduals[5+2*i]-xtot_shift;
         double yc = 19.2 - yresiduals[5+2*i]-ytot_shift;
         double x_angle = Variables[8+4*i];
         double y_angle = Variables[9+4*i];
         xfinal[i+4] = (xc + (xwithout_rot[i+4]-xc)/cos(x_angle) - yc*tan(x_angle) - ((ywithout_rot[i+4]-yc)/cos(y_angle))*tan(x_angle))/(1+tan(x_angle)*tan(y_angle));
         yfinal[i+4] = (yc + (ywithout_rot[i+4]-yc)/cos(y_angle) + xc*tan(y_angle) + ((xwithout_rot[i+4]-xc)/cos(x_angle))*tan(y_angle))/(1+tan(x_angle)*tan(y_angle));
     
         
      }else if(xpos[i+4][0] < xcenters[i]-xtot_shift && ypos[i+4][0] > ycenters[i]-ytot_shift){
         //second quandrant
         double xc = -19.2 - xresiduals[4+2*i]-xtot_shift;
         double yc = 19.2 - yresiduals[5+2*i]-ytot_shift;
         double x_angle = Variables[10+4*i];
         double y_angle = Variables[9+4*i];
         xfinal[i+4] = (xc + (xwithout_rot[i+4]-xc)/cos(x_angle) - yc*tan(x_angle) - ((ywithout_rot[i+4]-yc)/cos(y_angle))*tan(x_angle))/(1+tan(x_angle)*tan(y_angle));
         yfinal[i+4] = (yc + (ywithout_rot[i+4]-yc)/cos(y_angle) + xc*tan(y_angle) + ((xwithout_rot[i+4]-xc)/cos(x_angle))*tan(y_angle))/(1+tan(x_angle)*tan(y_angle));
     
      }else if(xpos[i+4][0] < xcenters[i]-xtot_shift && ypos[i+4][0] < ycenters[i]-ytot_shift){
         //third quandrant
         double xc = -19.2 - xresiduals[4+2*i]-xtot_shift;
         double yc = -19.2 - yresiduals[4+2*i]-ytot_shift;
         double x_angle = Variables[10+4*i];
         double y_angle = Variables[11+4*i];
         xfinal[i+4] = (xc + (xwithout_rot[i+4]-xc)/cos(x_angle) - yc*tan(x_angle) - ((ywithout_rot[i+4]-yc)/cos(y_angle))*tan(x_angle))/(1+tan(x_angle)*tan(y_angle));
         yfinal[i+4] = (yc + (ywithout_rot[i+4]-yc)/cos(y_angle) + xc*tan(y_angle) + ((xwithout_rot[i+4]-xc)/cos(x_angle))*tan(y_angle))/(1+tan(x_angle)*tan(y_angle));
          
      }else if(xpos[i+4][0] > xcenters[i]-xtot_shift && ypos[i+4][0] < ycenters[i]-ytot_shift){
         //fourth quandrant
         double xc =  19.2 - xresiduals[5+2*i]-xtot_shift;
         double yc = -19.2 - yresiduals[4+2*i]-ytot_shift;
         double x_angle = Variables[8+4*i];
         double y_angle = Variables[11+4*i];
         xfinal[i+4] = (xc + (xwithout_rot[i+4]-xc)/cos(x_angle) - yc*tan(x_angle) - ((ywithout_rot[i+4]-yc)/cos(y_angle))*tan(x_angle))/(1+tan(x_angle)*tan(y_angle));
         yfinal[i+4] = (yc + (ywithout_rot[i+4]-yc)/cos(y_angle) + xc*tan(y_angle) + ((xwithout_rot[i+4]-xc)/cos(x_angle))*tan(y_angle))/(1+tan(x_angle)*tan(y_angle));
     
      }
   }

   

   //do fitting and get chi squared for one event
   TGraph* x_event = new TGraph(count,zlist,xfinal);
   TGraph* y_event = new TGraph(count,zlist,yfinal);
   TF1 *x_fit = new TF1("x_fit","[1]*x+[0]",-200,1200);
   TF1 *y_fit = new TF1("y_fit","[3]*x+[2]",-200,1200);

   //get chi squared from fit
   TFitResultPtr rx = x_event->Fit(x_fit,"S");
   TFitResultPtr ry = y_event->Fit(y_fit,"S");
   double chi2x = rx->Chi2();
   double chi2y = ry->Chi2();
   f->Close();
   cout<<  (chi2x+chi2y);
   return (chi2x+chi2y);

}


//***********************************************************************

 
double Total_chi_squared(const double *Variables)
{
  const Double_t theta0  = Variables[0];
  const Double_t theta1  = Variables[1];
  const Double_t theta2  = Variables[2];
  const Double_t theta3  = Variables[3];
  const Double_t theta4  = Variables[4];
  const Double_t theta5  = Variables[5];
  const Double_t theta6  = Variables[6];
  const Double_t theta7  = Variables[7];
  const Double_t xtot_shift = Variables[8];
  const Double_t ytot_shift = Variables[9];
 

  double total = 0;
  for(int i=0;i<200;i++){
     double chi2 = chi_squared({ theta0, theta1, theta2,  theta3,  theta4,  theta5,  theta6, theta7, xtot_shift, ytot_shift} ,accepted_tracks[i]);
     total += chi2;
  }
  return total;
}



int partial_rot(const char * minName = "Minuit2",const char *algoName = "" , int randomSeed = -9999)
{
 
   ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

   // set tolerance , etc...
   min->SetMaxFunctionCalls(10000); // for Minuit/Minuit2 
   min->SetMaxIterations(1000);  // for GSL 
   min->SetTolerance(0.001);
   min->SetPrintLevel(1);

   // create funciton wrapper for minmizer
   // a IMultiGenFunction type 
   ROOT::Math::Functor f(&Total_chi_squared,10); 
   double step = 0.00005;
   // starting point
    
   double variable[10] = {0   ,  -0.0308491  ,  0.00150159  ,  -0.0273688  ,  0.00356327  ,  -0.0192158  ,  0.00319301  ,  -0.0139853  ,  3.40833  ,  3.86717 };
   
 
   min->SetFunction(f);
 
   // Set the free variables to be minimized!
   min->SetFixedVariable(0,"theta0",variable[0]);
   min->SetVariable(1,"theta1",variable[1], step);
   min->SetVariable(2,"theta2",variable[2], step);
   min->SetVariable(3,"theta3",variable[3], step);
   min->SetVariable(4,"theta4",variable[4], step);
   min->SetVariable(5,"theta5",variable[5], step);
   min->SetVariable(6,"theta6",variable[6], step);
   min->SetVariable(7,"theta7",variable[7], step);
 
   min->SetVariable(8,"xtot_shift",variable[8], step);
   min->SetVariable(9,"ytot_shift",variable[9], step);
  
   // do the minimization
   min->Minimize(); 
   
 
   return 0;
}
