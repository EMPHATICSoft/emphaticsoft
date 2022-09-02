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
#include "main_alignment.C"




double chi_squared(Double_t shift0, Double_t shift1,Double_t shift2,Double_t shift3,Double_t shift4,Double_t shift5,Double_t shift6,Double_t shift7,int event){
   cout<< "{  "<<shift0 <<",  "<< shift1<<",  "<< shift2<<",  "<< shift3<<",  "<<shift4<<",  "<<shift5<<",  "<<shift6<<",  "<<shift7<<"}"<<endl;
   cout<<"event: "<<event<<endl;
   //Input: vector of offsets and starting event
   // Returns chi squared of a single track

   vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};
   vector<int> xcounts = {0,0,0,0,0,0};

   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   
   //loops over a single track and fills vectors
   for(int i=1+bounds[event];i<=bounds[event+1];i++){
      t1->GetEntry(i);
      

      //Filling vectors for xz-plot
      if(fer ==0 && module==1){
         xcounts[0]++;
      }else if(fer ==0 && module==3){
         xcounts[1]++;
      }else if(fer ==1 && module==1){
         xcounts[2]++;
      }else if(fer ==1 && module==3){
         xcounts[3]++;
      }else if(fer ==2 && module==0){
         xcounts[4]++;
      }else if(fer ==2 && module==1){
         xcounts[4]++;
      }else if(fer ==3 && module==0){
         xcounts[5]++;
      }else if(fer ==3 && module==1){
         xcounts[5]++;
      }
      //Filling vectors for yz-plot
      else if(fer ==0 && module==0){
         ypos[0].push_back((row-320)*ssdPitch-yresiduals[0]-shift0);
         zpos[0].push_back(station0_zpos);
      }else if(fer ==0 && module==2){
         ypos[1].push_back((row-320)*ssdPitch-yresiduals[1]-shift1);
         zpos[1].push_back(station1_zpos);      
      }else if(fer ==1 && module==0){
         ypos[2].push_back((row-320)*ssdPitch-yresiduals[2]-shift2);
         zpos[2].push_back(station2_zpos);      
      }else if(fer ==1 && module==2){
         ypos[3].push_back((row-320)*ssdPitch-yresiduals[3]-shift3);
         zpos[3].push_back(station3_zpos);      
      }else if(fer ==2 && module==2){
         ypos[4].push_back((row-640)*ssdPitch-yresiduals[4]-shift4);
         zpos[4].push_back(station4_zpos);      
      }else if(fer ==2 && module==3){
         ypos[5].push_back(-(row-640)*ssdPitch-yresiduals[5]-shift5);
         zpos[5].push_back(station4_zpos);      
      }else if(fer ==3 && module==2){
         ypos[6].push_back((row-640)*ssdPitch-yresiduals[6]-shift6);
         zpos[6].push_back(station5_zpos);      
      }else if(fer ==3 && module==3){
         ypos[7].push_back(-(row-640)*ssdPitch-yresiduals[7]-shift7);
         zpos[7].push_back(station5_zpos);
      }  
   }
   // graph of event
   int count = 0; 
   for(int i=0;i<4;i++)  if(ypos[i].size()!=0 && xcounts[i]!=0)  count ++;  
   for(int i=0;i<4;i++) if(ypos[4+i].size()!=0 && xcounts[4+floor(i/1.9)]==0) count++;      

   Double_t ylist[count], zlist[count];
   int num = 0;
   for(int i=0;i<zpos.size();i++){
      if(zpos[i].size()!=0 && ypos[i].size()!=0){
         ylist[num] = getAverage(ypos[i]);
         zlist[num] = getAverage(zpos[i]);
         num++;
         }     
         
      }
  
   TGraph* yzPlot = new TGraph(count,zlist,ylist);

  
   TF1 *fit = new TF1("fit","[1]*x+[0]",-200,1200);
   //yzPlot->Fit(fit,"Q"); //fit single event
 
   TFitResultPtr r = yzPlot->Fit(fit,"S");
   double chi2 = r->Chi2();
   f->Close();
   return chi2;

}





 
double Total_chi_squared(const double *shifts )
{
  const Double_t shift0 = shifts[0];
  const Double_t shift1 = shifts[1];
  const Double_t shift2 = shifts[2];
  const Double_t shift3 = shifts[3];
  const Double_t shift4 = shifts[4];
  const Double_t shift5 = shifts[5];
  const Double_t shift6 = shifts[6];
  const Double_t shift7 = shifts[7];
  double total = 0;
  for(int i=0;i<accepted_tracks.size();i++){
     double chi=chi_squared(shift0,shift1,shift2,shift3,shift4,shift5,shift6,shift7,accepted_tracks[i]);
     total+=chi;
  }
  return total;
}



int yMinimization(const char * minName = "Minuit2",
                          const char *algoName = "" , 
                          int randomSeed = -1)
{
  
   ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

   // set tolerance , etc...
   min->SetMaxFunctionCalls(10000); // for Minuit/Minuit2 
   min->SetMaxIterations(1000);  // for GSL 
   min->SetTolerance(0.0001);
   min->SetPrintLevel(1);

   // create funciton wrapper for minmizer
   // a IMultiGenFunction type 
   ROOT::Math::Functor f(&Total_chi_squared,8); 
   double step = 0.0001;
   // starting point
    
   double variable[8] = {  -1.46018,  -0.90429,  0.174014,  0.729234,  2.67872,  2.83762,  3.73049,  3.85003};//{ -1.4601788, -0.24812365, 1.6201245, 2.5574596, 0.54495041, -1.7392500, 5.4337097, 3.2299324 };
   
 
   min->SetFunction(f);
 
   // Set the free variables to be minimized!
   min->SetFixedVariable(0,"shift0",variable[0]);
   min->SetVariable(1,"shift1",variable[1], step);
   min->SetVariable(2,"shift2",variable[2], step);
   min->SetVariable(3,"shift3",variable[3], step);
   min->SetVariable(4,"shift4",variable[4], step);
   min->SetVariable(5,"shift5",variable[5], step);
   min->SetVariable(6,"shift6",variable[6], step);
   min->SetVariable(7,"shift7",variable[7], step);

   // do the minimization
   min->Minimize(); 
   
   
   
   return 0;
}

