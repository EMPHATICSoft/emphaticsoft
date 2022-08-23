#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <string>
#include <algorithm> 
#include <stdio.h>
#include <string.h> 
#include <numeric>
#include "printing.C"


char infile[ ] = "data/599/SSD1.root"; //choose data file to read
double station0_zpos = -170; //ssd station positions 
double station1_zpos = -46;
double station2_zpos = 194;
double station3_zpos = 194 + 124;
double station4_zpos = 768;
double station5_zpos = 768 + 226;
double ssdPitch = 0.06;
double constr = 0.07;


// offsets of ssd stations in mm. 
vector<double> xresiduals =  { -0.54759960, -0.25566729, 0.49941560, 0.54903480, -1.9639748, 0.46639482, -2.2882809, -0.088365250 };     //300, { -0.48217263, -0.21014961, 0.50731036, 0.53800422, -2.0419823, 0.37994331, -2.4031378, -0.21027787 };
vector<double> yresiduals =   { -1.2834529, -0.27548232, 1.1979157, 1.9309231, -0.81951882, -3.1079977, 3.7253454, 1.5296846 };          //{ 0.30217235, 1.1305285, 2.2554511, 2.8074963, -0.60025912, -2.8860684, 3.5960561, 1.3911136 };//300 { -1.4549245, -0.24419160, 1.6180551, 2.5482706, 0.52150012, -1.7596827, 5.4426099, 3.2489963 };
vector<double> xcenters={xresiduals[4]/2+xresiduals[5]/2,xresiduals[6]/2+xresiduals[7]/2};
vector<double> ycenters={yresiduals[4]/2+yresiduals[5]/2,yresiduals[6]/2+yresiduals[7]/2};


//.........................................................................

vector<int> find_bounds()
{
   
   //Return Vector with starting index of each event

   //Read in data from sssd.root file
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   vector<int> result = {0};
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   
   //read all entries and fill the histograms
   for (int i = 0; i<t1->GetEntries(); i++) {
     t1->GetEntry(i);
     	int first_fer = fer; 
     t1->GetEntry(i+1);
        int second_fer = fer;
    if(first_fer == 3 && second_fer ==0){
            result.push_back(i);	    
            }
   }   
   return result;
}

vector<int> bounds = find_bounds(); 


//.........................................................................


vector<int> find_accepted_tracks(){

   //Returns vector with index of accepted tracks
   //Only accepts tracks with hits in each ssd and "reasonable" chi squared
 

   /*
   //beam profiles
   TH2F** beam_array = new TH2F*[6];
   double sizes = 30;
   int bin = 100;
   beam_array[0] = new TH2F("h7","First Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[1] = new TH2F("h8","Second Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[2] = new TH2F("h9","Third Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[3] = new TH2F("h10","Fourth Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[4] = new TH2F("h11","Fith Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[5] = new TH2F("h12","Fith Station",bin,-sizes,sizes,bin,-sizes,sizes);
   */
   
   //Returns a vector with single particle events
   vector<int> result;
  
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   

   //loops over single event and fills vector
   for(int j=0; j<bounds.size(); j++){

      vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
      vector<vector<double>> yzpos = {{},{},{},{},{},{},{},{}};
   
      vector<vector<double>> xpos = {{},{},{},{},{},{},{},{}};
      vector<vector<double>> xzpos = {{},{},{},{},{},{},{},{}};


      for(int i=bounds[j]+1;i<=bounds[j+1];i++){
   	t1->GetEntry(i);

   	//Filling vectors for xz-plot
      if(fer ==0 && module==1){
         xpos[0].push_back((-row+320)*ssdPitch-xresiduals[0]);
         xzpos[0].push_back(station0_zpos);
      }else if(fer ==0 && module==3){
         xpos[1].push_back((-row+320)*ssdPitch-xresiduals[1]);
         xzpos[1].push_back(station1_zpos);
      }else if(fer ==1 && module==1){
         xpos[2].push_back((-row+320)*ssdPitch-xresiduals[2]);
         xzpos[2].push_back(station2_zpos);
      }else if(fer ==1 && module==3){
         xpos[3].push_back((-row+320)*ssdPitch-xresiduals[3]);
         xzpos[3].push_back(station3_zpos);
      }else if(fer ==2 && module==0){
         xpos[4].push_back((row)*ssdPitch-xresiduals[4]);
         xzpos[4].push_back(station4_zpos);
      }else if(fer ==2 && module==1){
         xpos[5].push_back((-row)*ssdPitch-xresiduals[5]);
         xzpos[5].push_back(station4_zpos);
      }else if(fer ==3 && module==0){
         xpos[6].push_back((-row+640)*ssdPitch-xresiduals[6]);
         xzpos[6].push_back(station5_zpos);
      }else if(fer ==3 && module==1){
         xpos[7].push_back((row-640)*ssdPitch-xresiduals[7]);
         xzpos[7].push_back(station5_zpos);
      
   	//Filling vectors for yz-plot
      }else if(fer ==0 && module==0){
         ypos[0].push_back((row-320)*ssdPitch-yresiduals[0]);
         yzpos[0].push_back(station0_zpos);
      }else if(fer ==0 && module==2){
         ypos[1].push_back((row-320)*ssdPitch-yresiduals[1]);
         yzpos[1].push_back(station1_zpos);      
      }else if(fer ==1 && module==0){
         ypos[2].push_back((row-320)*ssdPitch-yresiduals[2]);
         yzpos[2].push_back(station2_zpos);      
      }else if(fer ==1 && module==2){
         ypos[3].push_back((row-320)*ssdPitch-yresiduals[3]);
         yzpos[3].push_back(station3_zpos);      
      }else if(fer ==2 && module==2){
         ypos[4].push_back((row-640)*ssdPitch-yresiduals[4]);
         yzpos[4].push_back(station4_zpos);      
      }else if(fer ==2 && module==3){
         ypos[5].push_back(-(row-640)*ssdPitch-yresiduals[5]);
         yzpos[5].push_back(station4_zpos);      
      }else if(fer ==3 && module==2){
         ypos[6].push_back((row-640)*ssdPitch-yresiduals[6]);
         yzpos[6].push_back(station5_zpos);      
      }else if(fer ==3 && module==3){
         ypos[7].push_back(-(row-640)*ssdPitch-yresiduals[7]);
         yzpos[7].push_back(station5_zpos);
      }  
	
   }
  
   //Only keep events with single track
   if(  //ypart
        (yzpos[0].size()==1 || (yzpos[0].size()==2 && abs(ypos[0][0]-ypos[0][1])<constr ))
    &&  (yzpos[1].size()==1 || (yzpos[1].size()==2 && abs(ypos[1][0]-ypos[1][1])<constr ))
    &&  (yzpos[2].size()==1 || (yzpos[2].size()==2 && abs(ypos[2][0]-ypos[2][1])<constr ))
    &&  (yzpos[3].size()==1 || (yzpos[3].size()==2 && abs(ypos[3][0]-ypos[3][1])<constr ))
    &&(((yzpos[4].size()==1 || (yzpos[4].size()==2 && abs(ypos[4][0]-ypos[4][1])<constr )) && yzpos[5].size()==0)
    ||(( yzpos[5].size()==1 || (yzpos[5].size()==2 && abs(ypos[5][0]-ypos[5][1])<constr )) && yzpos[4].size()==0))
    &&(((yzpos[6].size()==1 || (yzpos[6].size()==2 && abs(ypos[6][0]-ypos[6][1])<constr )) && yzpos[7].size()==0)
    ||(( yzpos[7].size()==1 || (yzpos[7].size()==2 && abs(ypos[7][0]-ypos[7][1])<constr )) && yzpos[6].size()==0))
     
    //xpart
    &&  (xzpos[0].size()==1 || (xzpos[0].size()==2 && abs(xpos[0][0]-xpos[0][1])<constr ))
    &&  (xzpos[1].size()==1 || (xzpos[1].size()==2 && abs(xpos[1][0]-xpos[1][1])<constr ))
    &&  (xzpos[2].size()==1 || (xzpos[2].size()==2 && abs(xpos[2][0]-xpos[2][1])<constr ))
    &&  (xzpos[3].size()==1 || (xzpos[3].size()==2 && abs(xpos[3][0]-xpos[3][1])<constr ))
    &&(((xzpos[4].size()==1 || (xzpos[4].size()==2 && abs(xpos[4][0]-xpos[4][1])<constr )) && xzpos[5].size()==0)
    ||(( xzpos[5].size()==1 || (xzpos[5].size()==2 && abs(xpos[5][0]-xpos[5][1])<constr )) && xzpos[4].size()==0))
    &&(((xzpos[6].size()==1 || (xzpos[6].size()==2 && abs(xpos[6][0]-xpos[6][1])<constr )) && xzpos[7].size()==0)
    ||(( xzpos[7].size()==0 ||   xzpos[7].size()==1|| (xzpos[7].size()==2 && abs(xpos[7][0]-xpos[7][1])<constr )) && xzpos[6].size()==0))

    
    ){


      

      /*
      int num = 0;
      for(int i=0;i<4;i++){
         if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0){
            beam_array[i]->Fill(getAverage(xpos[i]),getAverage(ypos[i]));
            num++;
         }        
      }
      

      if(xpos[4].size()!=0 and ypos[4].size()!=0)beam_array[4]->Fill(getAverage(xpos[4]),getAverage(ypos[4]));
      else if(xpos[5].size()!=0 and ypos[5].size()!=0)beam_array[4]->Fill(getAverage(xpos[5]),getAverage(ypos[5]));
      else if(xpos[5].size()!=0 and ypos[4].size()!=0)beam_array[4]->Fill(getAverage(xpos[5]),getAverage(ypos[4]));
      else if(xpos[4].size()!=0 and ypos[5].size()!=0)beam_array[4]->Fill(getAverage(xpos[4]),getAverage(ypos[5]));

      if(xpos[6].size()!=0 and ypos[6].size()!=0)beam_array[5]->Fill(getAverage(xpos[6]),getAverage(ypos[6]));
      else if(xpos[7].size()!=0 and ypos[7].size()!=0)beam_array[5]->Fill(getAverage(xpos[7]),getAverage(ypos[7]));
      else if(xpos[7].size()!=0 and ypos[6].size()!=0)beam_array[5]->Fill(getAverage(xpos[7]),getAverage(ypos[6]));
      else if(xpos[6].size()!=0 and ypos[7].size()!=0)beam_array[5]->Fill(getAverage(xpos[6]),getAverage(ypos[7]));
      */



      // Only take events with reasonably small chi squared
      int xcount = 0;
      int ycount = 0; 
      for(int i=0;i<yzpos.size();i++)  if( ypos[i].size() != 0)  ycount ++; 
      for(int i=0;i<xzpos.size();i++)  if( xpos[i].size() != 0)  xcount ++;  
      Double_t xlist[xcount], ylist[ycount],xzlist[xcount], yzlist[ycount];

      int numy = 0;
      for(int i=0;i<ypos.size();i++){
         if( yzpos[i].size()!=0 ){
            ylist[numy] = getAverage(ypos[i]);
            yzlist[numy] = getAverage(yzpos[i]);
            numy++;
         }      
      }
      int numx = 0;
      for(int i=0;i<xpos.size();i++){
         if( xzpos[i].size()!=0 ){
            xlist[numx] = getAverage(xpos[i]);
            xzlist[numx] = getAverage(xzpos[i]);
            numx++;
         }      
      }
 
      
      TGraph* yevent = new TGraph(ycount,yzlist,ylist);
      TGraph* xevent = new TGraph(xcount,xzlist,xlist);

     
     
     //drop events when predicted hit is in dead region but we still have a hit

      TF1 *first_x_points = new TF1("first_x_points","[1]*x+[0]",-200,400);//fit a function to the first five points
      xevent->Fit(first_x_points,"R"); //Use range option to only fit first five points
      double x_predicted = first_x_points->Eval(station5_zpos,0,0); //get the expexted value of x at the final station
      //cout<<endl<<x_predicted;
      
      
      if( ((x_predicted > (-256*ssdPitch-xresiduals[7])) 
      && (x_predicted < (-128*ssdPitch-xresiduals[7]))) 
      && (xpos[6].size() + xpos[7].size() != 0) ){
         cout<<"in dead region"<< xcount<<endl;

         continue;
         }
      
     //drop events when predicted hit is outside dead region and we don't have a hit
     if( ((x_predicted < (-256*ssdPitch-xresiduals[7])) 
      || (x_predicted > (-128*ssdPitch-xresiduals[7])))
      && (xpos[6].size() + xpos[7].size() == 0) ){
           cout<<"not in dead region "<< xcount<<endl;
           continue;
           }
      cout<<endl<<xcount<<", "<<ycount<<endl;
      

      TF1 *yfit = new TF1("yfit","[1]*x+[0]",-200,1200);
      TF1 *xfit = new TF1("xfit","[3]*x+[2]",-200,1200);

      TFitResultPtr ry = yevent->Fit(yfit,"S");
      TFitResultPtr rx = xevent->Fit(xfit,"S");

      double chi2y = ry->Chi2();
      double chi2x = rx->Chi2();
      

	   if(chi2y<0.04 && chi2x<0.04)result.push_back(j);

	   }
   }


  /*
   TCanvas *c1 = new TCanvas("c1","c1",1400,700);
   c1->Divide(3,2);
   for(int i=0;i<6;i++){
       
        beam_array[i]->GetYaxis()->SetTitle("y-position mm");
        beam_array[i]->GetXaxis()->SetTitle("x-position mm");
        c1->cd(i+1);
        beam_array[i]->Draw("COLZ");
   }
   */

   return result; 
}

vector<int> accepted_tracks = find_accepted_tracks();



//.........................................................................

void rotation_performance(){
   
   //Plots residuals with and without rotations

   vector<double> Variables = {0.000498613, 0.00643335, -0.000972057, 0.00489587, -0.00508142, 0.00343643, -0.00805295, 0.00475778, -0.0207308, 0.00688021, -0.0204942, 0.0052927, -0.0326327, -0.0104844, -0.0292073, -0.00816859, -2.71399, -0.43999};
   double xtot_shift = Variables[16];
   double ytot_shift = Variables[17];

   double chix_with =0;
   double chiy_with =0;
   double chix_without =0;
   double chiy_without =0;
   



   //Histstograms filled with residuals with and without rotations
   TH1F** residualx_array = new TH1F*[6]; 
   TH1F** residualy_array = new TH1F*[6];
   TH1F** baselinex_array = new TH1F*[6];
   TH1F** baseliney_array = new TH1F*[6];
   double size = 15;
   int bin = 100;

   //residuals for stations before and after roations
   baselinex_array[0] = new TH1F("b0","First Station Baseline x",bin,-size,size);
   baselinex_array[1] = new TH1F("b1","Second Station Baseline x",bin,-size,size);
   baselinex_array[2] = new TH1F("b2","Third Station Baseline x",bin,-size,size);
   baselinex_array[3] = new TH1F("b3","Fourth Station Baseline x",bin,-size,size);
   baselinex_array[4] = new TH1F("b4","Fith Station Baseline x",bin,-size,size);
   baselinex_array[5] = new TH1F("b5","Fith Station Baseline x",bin,-size,size);

   baseliney_array[0] = new TH1F("b6","First Station Baseline y",bin,-size,size);
   baseliney_array[1] = new TH1F("b7","Second Station Baseline y",bin,-size,size);
   baseliney_array[2] = new TH1F("b8","Third Station Baseline y",bin,-size,size);
   baseliney_array[3] = new TH1F("b9","Fourth Station Baseline y",bin,-size,size);
   baseliney_array[4] = new TH1F("b10","Fith Station Baseline y",bin,-size,size);
   baseliney_array[5] = new TH1F("b11","Fith Station Baseline y",bin,-size,size);

   residualx_array[0] = new TH1F("r0","First Station x",bin,-size,size);
   residualx_array[1] = new TH1F("r1","Second Station x",bin,-size,size);
   residualx_array[2] = new TH1F("r2","Third Station x",bin,-size,size);
   residualx_array[3] = new TH1F("r3","Fourth Station x",bin,-size,size);
   residualx_array[4] = new TH1F("r4","Fith Station x",bin,-size,size);
   residualx_array[5] = new TH1F("r5","Fith Station x",bin,-size,size);

   residualy_array[0] = new TH1F("r6","First Station y",bin,-size,size);
   residualy_array[1] = new TH1F("r7","Second Station y",bin,-size,size);
   residualy_array[2] = new TH1F("r8","Third Station y",bin,-size,size);
   residualy_array[3] = new TH1F("r9","Fourth Station y",bin,-size,size);
   residualy_array[4] = new TH1F("r10","Fith Station y",bin,-size,size);
   residualy_array[5] = new TH1F("r11","Fith Station y",bin,-size,size);


   //beam profiles
   TH2F** beam_array = new TH2F*[6];
   double width = 30;
   beam_array[0] = new TH2F("h7","First Station",bin,-width,width,bin,-width,width);
   beam_array[1] = new TH2F("h8","Second Station",bin,-width,width,bin,-width,width);
   beam_array[2] = new TH2F("h9","Third Station",bin,-width,width,bin,-width,width);
   beam_array[3] = new TH2F("h10","Fourth Station",bin,-width,width,bin,-width,width);
   beam_array[4] = new TH2F("h11","Fith Station",bin,-width,width,bin,-width,width);
   beam_array[5] = new TH2F("h12","Fith Station",bin,-width,width,bin,-width,width);


   //explanation of double peaks
   TH1F *region1 = new TH1F("region1","region1",bin,-size,size);
   TH1F *region2 = new TH1F("region2","region2",bin,-size,size);
   TH1F *region3 = new TH1F("region3","region3",bin,-size,size);
   THStack *hs = new THStack("hs","");

   TH1F *yslopes_without = new TH1F("y_slopes_without","y_slopes_without",300,-0.0035,-0.0015);
   TH1F *yslopes_with = new TH1F("yslopes_with","y_slopes_with",300,-0.0035,-0.0015);

   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
  
   //loops over multiple events and fills vector
   for(int j=0; j<accepted_tracks.size(); j++){

      vector<vector<double>> ypos = {{},{},{},{},{},{}};
      vector<vector<double>> yzpos = {{},{},{},{},{},{}};
      vector<vector<double>> xpos = {{},{},{},{},{},{}};
      vector<vector<double>> xzpos = {{},{},{},{},{},{}};


      for(int i=bounds[accepted_tracks[j]]+1;i<=bounds[accepted_tracks[j]+1];i++){
			t1->GetEntry(i);

         //Filling vectors for xz-plot
         if(fer ==0 && module==1){
            xpos[0].push_back((-row+320)*ssdPitch-xresiduals[0]-xtot_shift);
            xzpos[0].push_back(station0_zpos);
         }else if(fer ==0 && module==3){
            xpos[1].push_back((-row+320)*ssdPitch-xresiduals[1]-xtot_shift);
            xzpos[1].push_back(station1_zpos);
         }else if(fer ==1 && module==1){
            xpos[2].push_back((-row+320)*ssdPitch-xresiduals[2]-xtot_shift);
            xzpos[2].push_back(station2_zpos);
         }else if(fer ==1 && module==3){
            xpos[3].push_back((-row+320)*ssdPitch-xresiduals[3]-xtot_shift);
            xzpos[3].push_back(station3_zpos);
         }else if(fer ==2 && module==0){
            xpos[4].push_back((row)*ssdPitch-xresiduals[4]-xtot_shift);
            xzpos[4].push_back(station4_zpos);
         }else if(fer ==2 && module==1){
            xpos[4].push_back((-row)*ssdPitch-xresiduals[5]-xtot_shift);
            xzpos[4].push_back(station4_zpos);
         }else if(fer ==3 && module==0){
            xpos[5].push_back((-row+640)*ssdPitch-xresiduals[6]-xtot_shift);
            xzpos[5].push_back(station5_zpos);
         }else if(fer ==3 && module==1){
            xpos[5].push_back((row-640)*ssdPitch-xresiduals[7]-xtot_shift);
            xzpos[5].push_back(station5_zpos);

         //Filling vectors for yz-plot
         }else if(fer ==0 && module==0){
            ypos[0].push_back((row-320)*ssdPitch-yresiduals[0]-ytot_shift);
            yzpos[0].push_back(station0_zpos);
         }else if(fer ==0 && module==2){
            ypos[1].push_back((row-320)*ssdPitch-yresiduals[1]-ytot_shift);
            yzpos[1].push_back(station1_zpos);     
         }else if(fer ==1 && module==0){
            ypos[2].push_back((row-320)*ssdPitch-yresiduals[2]-ytot_shift);
            yzpos[2].push_back(station2_zpos);     
         }else if(fer ==1 && module==2){
            ypos[3].push_back((row-320)*ssdPitch-yresiduals[3]-ytot_shift);
            yzpos[3].push_back(station3_zpos);     
         }else if(fer ==2 && module==2){
            ypos[4].push_back((row-640)*ssdPitch-yresiduals[4]-ytot_shift);
            yzpos[4].push_back(station4_zpos);     
         }else if(fer ==2 && module==3){
            ypos[4].push_back(-(row-640)*ssdPitch-yresiduals[5]-ytot_shift);
            yzpos[4].push_back(station4_zpos);     
         }else if(fer ==3 && module==2){
            ypos[5].push_back((row-640)*ssdPitch-yresiduals[6]-ytot_shift);
            yzpos[5].push_back(station5_zpos);     
         }else if(fer ==3 && module==3){
            ypos[5].push_back(-(row-640)*ssdPitch-yresiduals[7]-ytot_shift);
            yzpos[5].push_back(station5_zpos);
            
         }


      }
     
  
   
   // get x,y,z positions
   int count = 0; 
   for(int i=0;i<xpos.size();i++)  if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0)  count ++;
   //cout<<endl<<count<<endl;
   Double_t xwithout_rot[count], ywithout_rot[count], zfinal[count],xfinal[count],yfinal[count];
   int num = 0;
   

   //Fill lists to be used for plotting
   for(int i=0;i<xpos.size();i++){
      if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0){
         if(yzpos[i][0]!=xzpos[i][0])continue;
         double x = getAverage(xpos[i]);
         double y = getAverage(ypos[i]);
         xwithout_rot[num] = x;
         ywithout_rot[num] = y;
         xfinal[num] = x;
         yfinal[num] = y;
         zfinal[num] = getAverage(yzpos[i]);
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


   //Rotate SSDs in the last 2 stations
   //Only rotate last station if there are a total of 6 hits 
   int last_station = 0;
   if(count==6)last_station++;
   for(int i=0;i<1+last_station;i++){
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


   TGraph* x_event = new TGraph(count,zfinal,xfinal);
	TGraph* y_event = new TGraph(count,zfinal,yfinal);
   TGraph* xwithout_rot_event = new TGraph(count,zfinal,xwithout_rot);
   TGraph* ywithout_rot_event = new TGraph(count,zfinal,ywithout_rot);
	

   TF1 *x_fit = new TF1("x_fit","[1]*x+[0]",-200,1200);
   TF1 *y_fit = new TF1("y_fit","[1]*x+[0]",-200,1200);

   TF1 *xwithout_rot_fit = new TF1("xwithout_rot_fit","[1]*x+[0]",-200,1200);
   TF1 *ywithout_rot_fit = new TF1("ywithout_rot_fit","[1]*x+[0]",-200,1200);

   x_event->Fit(x_fit,"Q");
   y_event->Fit(y_fit,"Q"); //fit single event

   xwithout_rot_event->Fit(xwithout_rot_fit,"Q"); 
   ywithout_rot_event->Fit(ywithout_rot_fit,"Q"); 
   
   yslopes_without->Fill(ywithout_rot_fit->GetParameter(1));
   yslopes_with->Fill(y_fit->GetParameter(1));

   int counting=0;
   for(int i = 0; i<xzpos.size();i++){
        if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0){
           
		     double x_exp = x_fit->Eval(zfinal[counting],0,0);
           double y_exp = y_fit->Eval(zfinal[counting],0,0);

           double xwithout_rot_exp = xwithout_rot_fit->Eval(zfinal[counting],0,0);
		     double ywithout_rot_exp = ywithout_rot_fit->Eval(zfinal[counting],0,0);

           double x_obs = xfinal[counting];
           double y_obs = yfinal[counting];

		     double xwithout_rot_obs = xwithout_rot[counting];
           double ywithout_rot_obs = ywithout_rot[counting];

           counting ++;

           if(xpos[i].size()==2){
              residualx_array[i]->Fill((x_exp-x_obs)*sqrt(12)/(0.06*0.42));
              baselinex_array[i]->Fill((xwithout_rot_exp-xwithout_rot_obs)*sqrt(12)/(0.06*0.42));
            }else if(xpos[i].size()==1){
              residualx_array[i]->Fill((x_exp-x_obs)*sqrt(12)/(0.06*0.58));
              baselinex_array[i]->Fill((xwithout_rot_exp-xwithout_rot_obs)*sqrt(12)/(0.06*0.58));
            }

            if(xpos[i].size()==2){
              residualy_array[i]->Fill((y_exp-y_obs)*sqrt(12)/(0.06*0.42));
              baseliney_array[i]->Fill((ywithout_rot_exp-ywithout_rot_obs)*sqrt(12)/(0.06*0.42));
            }else if(xpos[i].size()==1){
              residualx_array[i]->Fill((y_exp-y_obs)*sqrt(12)/(0.06*0.58));
              baselinex_array[i]->Fill((ywithout_rot_exp-ywithout_rot_obs)*sqrt(12)/(0.06*0.58));
            }

           //if(i==5 && getAverage(xpos[i])> 2.288280) region3->Fill((ywithout_rot_exp-ywithout_rot_obs)*sqrt(12)/0.06);
           //else if(i==5 && getAverage(xpos[i]) > -15.0 && getAverage(xpos[i]) < 2.2882809) region2->Fill((ywithout_rot_exp-ywithout_rot_obs)*sqrt(12)/0.06);
           //else if(i==5 && getAverage(xpos[i]) < -15.0 ) region1->Fill((ywithout_rot_exp-ywithout_rot_obs)*sqrt(12)/0.06);

           chix_with += pow((x_exp-x_obs)*sqrt(12)/0.06,2);
           chiy_with += pow((y_exp-y_obs)*sqrt(12)/0.06,2);
           chix_without += pow((xwithout_rot_exp-xwithout_rot_obs)*sqrt(12)/0.06,2);
           chiy_without += pow((ywithout_rot_exp-ywithout_rot_obs)*sqrt(12)/0.06,2);

	        }

       }


      for(int i=0;i<count;i++)beam_array[i]->Fill(xfinal[i],yfinal[i]);               

   }
   

   //beam profile after rotations
   TCanvas *c10 = new TCanvas("c10","c10",1400,700);
   c10->Divide(3,2);
   for(int i=0;i<6;i++){
       
        beam_array[i]->GetYaxis()->SetTitle("y-position mm");
        beam_array[i]->GetXaxis()->SetTitle("x-position mm");
        c10->cd(i+1);
        beam_array[i]->Draw("COLZ");
   }


   
   



   //All plotting and statistics for x
   TCanvas *ce4 = new TCanvas("ce4","ce4",1400,700);
   ce4->Divide(3,4);
   
   cout<<endl<<"x with rotations"<<endl;
   for(int i=0;i<6;i++){
      cout<<residualx_array[i]->GetMean()<<"   "<<residualx_array[i]->GetStdDev()<<endl;
      ce4->cd(i+1);
      residualx_array[i]->GetYaxis()->SetTitle("counts");
      residualx_array[i]->GetXaxis()->SetTitle("residual");
   	residualx_array[i]->Draw();
	}

   cout<<endl<<"x without rotations"<<endl;
   for(int i=0;i<6;i++){
      cout<<baselinex_array[i]->GetMean()<<"   "<<baselinex_array[i]->GetStdDev()<<endl;
      ce4->cd(i+7);
      baselinex_array[i]->GetYaxis()->SetTitle("counts");
      baselinex_array[i]->GetXaxis()->SetTitle("residual");
      baselinex_array[i]->Draw();
   }


   //All plotting and statistics for y
   TCanvas *c1 = new TCanvas("c1","c1",1400,700);
   c1->Divide(3,4);
   cout<<endl<<"y with rotations"<<endl;
   for(int i=0;i<6;i++){
      cout<<residualy_array[i]->GetMean()<<"   "<<residualy_array[i]->GetStdDev()<<endl;
      c1->cd(i+1);
      residualy_array[i]->GetYaxis()->SetTitle("counts");
      residualy_array[i]->GetXaxis()->SetTitle("residual");
      residualy_array[i]->Draw();
   }
   cout<<endl<<"y without rotations"<<endl;
   for(int i=0;i<6;i++){
      cout<<baseliney_array[i]->GetMean()<<"   "<<baseliney_array[i]->GetStdDev()<<endl;
      c1->cd(i+7);
      baseliney_array[i]->GetYaxis()->SetTitle("counts");
      baseliney_array[i]->GetXaxis()->SetTitle("residual");
      baseliney_array[i]->Draw();
   }

  cout<<endl;
  cout<<" x without rotations: "<<chix_without<<endl;
  cout<<" x with rotations: "<<chix_with<<endl;
  cout<<" y without rotations: "<<chiy_without<<endl;
  cout<<" y with rotations: "<<chiy_with<<endl;

  /*
  region2->SetLineColor(kRed);
  region1->SetLineColor(kGreen);
  region3->SetLineColor(kBlue);
  region1->GetYaxis()->SetTitle("counts");
  region1->GetXaxis()->SetTitle("residual");

  hs->Add(region1);
  hs->Add(region2);
  hs->Add(region3);
  TCanvas *c2 = new TCanvas("c2","c2",700,700);
  hs->Draw();
  gPad->BuildLegend(0.75,0.75,0.95,0.95,"");
  */

  //distribution of slopes
  TCanvas *c2 = new TCanvas("c2","c2",700,700);
  yslopes_without->Draw();
  TCanvas *c3 = new TCanvas("c3","c3",700,700);
  yslopes_with->Draw();

}


//.........................................................................

void beam_profile(){
   
   //Plots 2d-histogram for each ssd station 
   //Only shows hits from accepted tracks

   //beam profiles
   TH2F** beam_array = new TH2F*[6];
   double sizes = 30;
   int bin = 100;
   beam_array[0] = new TH2F("h7","First Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[1] = new TH2F("h8","Second Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[2] = new TH2F("h9","Third Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[3] = new TH2F("h10","Fourth Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[4] = new TH2F("h11","Fith Station",bin,-sizes,sizes,bin,-sizes,sizes);
   beam_array[5] = new TH2F("h12","Fith Station",bin,-sizes,sizes,bin,-sizes,sizes);


   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   vector<double> new_residuals;
   //Histstograms filled with residuals
   TH1F** residual_array = new TH1F*[6]; 
   double size = 0.3;
   int bins = 100;
   residual_array[0] = new TH1F("h0","First Station",bins,-size,size);
   residual_array[1] = new TH1F("h1","Second Station",bins,-size,size);
   residual_array[2] = new TH1F("h2","Third Station",bins,-size,size);
   residual_array[3] = new TH1F("h3","Fourth Station",bins,-size,size);
   residual_array[4] = new TH1F("h4","Fith Station",bins,-size,size);
   residual_array[5] = new TH1F("h5","Fith Station",bins,-size,size);
   //residual_array[6] = new TH1F("h6","Sixth Station Lower X-SSD",bins,-size,size);
   //residual_array[7] = new TH1F("h7","Sixth Station Upper X-SSD",bins,-size,size);

   //loops over single event and fills vector
   for(int j=0; j<accepted_tracks.size(); j++){

      vector<vector<double>> ypos = {{},{},{},{},{},{}};
      vector<vector<double>> yzpos = {{},{},{},{},{},{}};
      vector<vector<double>> xpos = {{},{},{},{},{},{}};
      vector<vector<double>> xzpos = {{},{},{},{},{},{}};


      for(int i=bounds[accepted_tracks[j]]+1;i<=bounds[accepted_tracks[j]+1];i++){
      	t1->GetEntry(i);

      	//Filling vectors for xz-plot
         if(fer ==0 && module==1){
            xpos[0].push_back((-row+320)*ssdPitch-xresiduals[0]);
            xzpos[0].push_back(station0_zpos);
         }else if(fer ==0 && module==3){
            xpos[1].push_back((-row+320)*ssdPitch-xresiduals[1]);
            xzpos[1].push_back(station1_zpos);
         }else if(fer ==1 && module==1){
            xpos[2].push_back((-row+320)*ssdPitch-xresiduals[2]);
            xzpos[2].push_back(station2_zpos);
         }else if(fer ==1 && module==3){
            xpos[3].push_back((-row+320)*ssdPitch-xresiduals[3]);
            xzpos[3].push_back(station3_zpos);
         }else if(fer ==2 && module==0){
            xpos[4].push_back((row)*ssdPitch-xresiduals[4]);
            xzpos[4].push_back(station4_zpos);
         }else if(fer ==2 && module==1){
            xpos[4].push_back((-row)*ssdPitch-xresiduals[5]);
            xzpos[4].push_back(station4_zpos);
         }else if(fer ==3 && module==0){
            xpos[5].push_back((-row+640)*ssdPitch-xresiduals[6]);
            xzpos[5].push_back(station5_zpos);
         }else if(fer ==3 && module==1){
            xpos[5].push_back((row-640)*ssdPitch-xresiduals[7]);
            xzpos[5].push_back(station5_zpos);  

      	//Filling vectors for yz-plot
      	}else if(fer ==0 && module==0){
      		ypos[0].push_back((row-320)*ssdPitch-yresiduals[0]);
      		yzpos[0].push_back(station0_zpos);
      	}else if(fer ==0 && module==2){
      		ypos[1].push_back((row-320)*ssdPitch-yresiduals[1]);
      		yzpos[1].push_back(station1_zpos);		
      	}else if(fer ==1 && module==0){
      		ypos[2].push_back((row-320)*ssdPitch-yresiduals[2]);
      		yzpos[2].push_back(station2_zpos);		
      	}else if(fer ==1 && module==2){
      		ypos[3].push_back((row-320)*ssdPitch-yresiduals[3]);
      		yzpos[3].push_back(station3_zpos);		
      	}else if(fer ==2 && module==2){
      		ypos[4].push_back((row-640)*ssdPitch-yresiduals[4]);
      		yzpos[4].push_back(station4_zpos);		
      	}else if(fer ==2 && module==3){
      		ypos[4].push_back(-(row-640)*ssdPitch-yresiduals[5]);
      		yzpos[4].push_back(station4_zpos);		
      	}else if(fer ==3 && module==2){
      		ypos[5].push_back((row-640)*ssdPitch-yresiduals[6]);
      		yzpos[5].push_back(station5_zpos);		
      	}else if(fer ==3 && module==3){
      		ypos[5].push_back(-(row-640)*ssdPitch-yresiduals[7]);
      		yzpos[5].push_back(station5_zpos);
      		
      	}
      	
	
      }
  
  
      // graph of x_event
      int count = 0;	
      for(int i=0;i<xpos.size();i++)  if(xpos[i].size()!=0 && xzpos[i].size()!=0 && ypos[i].size()!=0 && yzpos[i].size()!=0)  count ++;
      //Double_t ylist[count], zlist[count],zlist[count];
      //cout<<endl<<count<<endl;
      //if(count!=6)continue;
      int num = 0;
      for(int i=0;i<xzpos.size();i++){
         if(xpos[i].size()==0 || ypos[i].size()==0)continue;
      	beam_array[i]->Fill(getAverage(xpos[i]),getAverage(ypos[i]));
      	num++;					
      }
      
      cout<<endl<<num<<endl;

   }
   
   
   TCanvas *c1 = new TCanvas("c1","c1",1400,700);
   c1->Divide(3,2);
   for(int i=0;i<6;i++){
       
        beam_array[i]->GetYaxis()->SetTitle("y-position mm");
        beam_array[i]->GetXaxis()->SetTitle("x-position mm");
        c1->cd(i+1);
   	  beam_array[i]->Draw("COLZ");
	}
  
}



void main_alignment(){
 
}

