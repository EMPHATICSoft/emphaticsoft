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

int spill = 20;




vector<double> xalign(vector<double> residuals, double size){
   

   //Input: vector with current offsets
   //Input: size of histograms to be filled with residuals

   //Output: vector with new residuals


   int two_hits = 0;
   int one_hit = 0;

   //Input: vector with SSD offsets
   //Returns a vector with new offsets
  
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   vector<double> new_residuals;
   //Histstograms filled with residuals
   TH1F** residual_array = new TH1F*[8]; 
   int num_bins = floor(1.3*size/0.06);
   residual_array[0] = new TH1F("h0","First Station X-SSD",num_bins,-size,size);
   residual_array[1] = new TH1F("h1","Second Station X-SSD",num_bins,-size,size);
   residual_array[2] = new TH1F("h2","Third Station X-SSD",num_bins,-size,size);
   residual_array[3] = new TH1F("h3","Fourth Station X-SSD",num_bins,-size,size);
   residual_array[4] = new TH1F("h4","Fith Station Upper X-SSD",num_bins,-size,size);
   residual_array[5] = new TH1F("h5","Fith Station Lower X-SSD",num_bins,-size,size);
   residual_array[6] = new TH1F("h6","Sixth Station Upper X-SSD",num_bins,-size,size);
   residual_array[7] = new TH1F("h7","Sixth Station Lower X-SSD",num_bins,-size,size);
	

   
   //loops over single event and fills vector
   for(int j=0; j<accepted_tracks.size(); j++){

      vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
      vector<vector<double>> yzpos = {{},{},{},{},{},{},{},{}};
      vector<vector<double>> xpos = {{},{},{},{},{},{},{},{}};
      vector<vector<double>> xzpos = {{},{},{},{},{},{},{},{}};
   
   
   for(int i=bounds[accepted_tracks[j]]+1;i<=bounds[accepted_tracks[j]+1];i++){
	   t1->GetEntry(i);
	      
			//Filling vectors for xz-plot
			if(fer ==0 && module==1){
				xpos[0].push_back((-row+320)*ssdPitch-residuals[0]);
				xzpos[0].push_back(station0_zpos);
			}else if(fer ==0 && module==3){
				xpos[1].push_back((-row+320)*ssdPitch-residuals[1]);
				xzpos[1].push_back(station0_zpos+station_thickness);
			}else if(fer ==1 && module==1){
				xpos[2].push_back((-row+320)*ssdPitch-residuals[2]);
				xzpos[2].push_back(station1_zpos);
			}else if(fer ==1 && module==3){
				xpos[3].push_back((-row+320)*ssdPitch-residuals[3]);
				xzpos[3].push_back(station1_zpos+station_thickness);
			}else if(fer ==2 && module==0){
				xpos[4].push_back((row)*ssdPitch-residuals[4]);
				xzpos[4].push_back(station2_zpos);
			}else if(fer ==2 && module==1){
				xpos[5].push_back((-row)*ssdPitch-residuals[5]);
				xzpos[5].push_back(station2_zpos);
			}else if(fer ==3 && module==0){
				xpos[6].push_back((-row+640)*ssdPitch-residuals[6]);
				xzpos[6].push_back(station2_zpos+station2_thickness);
			}else if(fer ==3 && module==1){
				xpos[7].push_back((row-640)*ssdPitch-residuals[7]);
				xzpos[7].push_back(station2_zpos+station2_thickness);
			
         // y information
		   }else if(fer ==0 && module==0){
	         ypos[0].push_back((row-320)*ssdPitch-residuals[0]);
	         yzpos[0].push_back(station0_zpos);
	      }else if(fer ==0 && module==2){
	         ypos[1].push_back((row-320)*ssdPitch-residuals[1]);
	         yzpos[1].push_back(station0_zpos+station_thickness);      
	      }else if(fer ==1 && module==0){
	         ypos[2].push_back((row-320)*ssdPitch-residuals[2]);
	         yzpos[2].push_back(station1_zpos);      
	      }else if(fer ==1 && module==2){
	         ypos[3].push_back((row-320)*ssdPitch-residuals[3]);
	         yzpos[3].push_back(station1_zpos+station_thickness);      
	      }else if(fer ==2 && module==2){
	         ypos[4].push_back((row-640)*ssdPitch-residuals[4]);
	         yzpos[4].push_back(station2_zpos);      
	      }else if(fer ==2 && module==3){
	         ypos[5].push_back(-(row-640)*ssdPitch-residuals[5]);
	         yzpos[5].push_back(station2_zpos);     
	      }else if(fer ==3 && module==2){
	         ypos[6].push_back((row-640)*ssdPitch-residuals[6]);
	         yzpos[6].push_back(station2_zpos+station2_thickness);    
	      }else if(fer ==3 && module==3){
	         ypos[7].push_back(-(row-640)*ssdPitch-residuals[7]);
	         yzpos[7].push_back(station2_zpos+station2_thickness);
	      }  	
	
	
   }
  


	for(int i=0;i<ypos.size();i++){
     //if(ypos[i].size()!=0 && xcounts[i]!=0)  count ++;  
     if(xpos[i].size()==2) two_hits++;
     else if(xpos[i].size()==1) one_hit++;
	  }
  
   // graph of event
   int count = 0;	
   for(int i=0;i<xzpos.size();i++)  if(xpos[i].size()!=0 && xzpos[i].size()!=0)  count ++;			
   
   Double_t xlist[count], zlist[count];
   int num = 0;
   for(int i=0;i<xzpos.size();i++){
 	   if(xpos[i].size()!=0 && xzpos[i].size()!=0){
 	   	//if(yzpos[i][0]!=xzpos[i][0])continue;
		   xlist[num] = getAverage(xpos[i]);
		   zlist[num] = getAverage(xzpos[i]);
	      num++;
		}		
			
	}
	
  
  TGraph* event = new TGraph(count,zlist,xlist);
  TF1 *fit_entire = new TF1("fit_entire","[1]*x+[0]",-200,1200);
  event->Fit(fit_entire,"Q"); 
  for(int i = 0; i<xzpos.size();i++){
	if(xpos[i].size() !=0 && xzpos[i].size() !=0){
		double delta_x = (getAverage(xpos[i])-fit_entire->Eval(xzpos[i][0],0,0));//calculate difference between fitted and actual data
		residual_array[i]->Fill(delta_x*sqrt(12)/ssdPitch);
		}
	}

}	
   
  
   TCanvas *ce4 = new TCanvas("ce4","ce4",1300,1300);
   ce4->Divide(4,2);
   for(int i=0;i<8;i++){
        cout<<residual_array[i]->GetMean()<<"   "<<residual_array[i]->GetStdDev()<<endl;
        new_residuals.push_back(residuals[i]+(residual_array[i]->GetMean())*ssdPitch/sqrt(12)); //Fill vector with offsets	
        residual_array[i]->GetYaxis()->SetTitle("counts");
        residual_array[i]->GetXaxis()->SetTitle("residual");
        ce4->cd(i+1);
   	  residual_array[i]->Draw();
	}


  cout<< endl<<"one hit: "<<one_hit<<". Two hits: "<< two_hits<<endl;
  return new_residuals; 
}


//.........................................................................


void xhist(vector<double> residuals){

   //Input: vector with ssd offsets
   ///Plots a histogram of the beam profile

   //Read in data from SSD.root tree 
   auto c = new TCanvas("c","c");
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   auto h1 = new TH2F("h1","xz-Hits",150,-200,1200,300,-45,45);
   

   //loops over single event and fills vector
   for(int i=0; i<t1->GetEntries(); i++){
	t1->GetEntry(i);
      	
	//Filling vectors for xz-plot
	if(fer ==0 && module==1){
		h1->Fill(station0_zpos,(-row+320)*ssdPitch-residuals[0]);
	}else if(fer ==0 && module==3){
		h1->Fill(station0_zpos+station_thickness,(-row+320)*ssdPitch-residuals[1]);
	}else if(fer ==1 && module==1){
		h1->Fill(station1_zpos,(-row+320)*ssdPitch-residuals[2]);
	}else if(fer ==1 && module==3){
		h1->Fill(station1_zpos+station_thickness,(-row+320)*ssdPitch-residuals[3]);	
	}else if(fer ==2 && module==0){
		h1->Fill(station2_zpos,(row)*ssdPitch-residuals[4]);		
	}else if(fer ==2 && module==1){
		h1->Fill(station2_zpos,(-row)*ssdPitch-residuals[5]);		
	}else if(fer ==3 && module==0){
		h1->Fill(station2_zpos+station2_thickness,(-row+640)*ssdPitch-residuals[6]);		
	}else if(fer ==3 && module==1){
		h1->Fill(station2_zpos+station2_thickness,(row-640)*ssdPitch-residuals[7]);
		
	}	
	
   }
  
   
   //cout<<"hello";
   h1->SetBarWidth(11);
   h1->SetFillStyle(0);
   h1->SetFillColor(kGray);
   h1->SetLineColor(kBlue);
   h1->GetYaxis()->SetTitle("x-position mm");
   h1->GetXaxis()->SetTitle("z-position mm");
   h1->SetStats(0);
   h1->Draw("violiny(112000000)");

}


//.........................................................................

void xplot(vector<double> residuals, int event){

   //Input: vector of offsets and starting event
   //Plots xz-positions for six events

   vector<vector<double>> xpos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};//store x,z positions
   Double_t ez[6] = {0,0,0,0,0,0};
   Double_t ex[6] = {0.06,0.06,0.06,0.06,0.06,0.06};


   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   
   //loops over a single track and fills vectors
   int numgraphs = 6;
   for(int j = 0; j<numgraphs; j++){
           
	   for(int i=bounds[accepted_tracks[event+j]]+1;i<=bounds[accepted_tracks[event+j]+1];i++){
		t1->GetEntry(i);
      	
			//Filling vectors for xz-plot
			if(fer ==0 && module==1){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[0]);
				zpos[j].push_back(station0_zpos);
			}else if(fer ==0 && module==3){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[1]);
				zpos[j].push_back(station0_zpos+station_thickness);
			}else if(fer ==1 && module==1){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[2]);
				zpos[j].push_back(station1_zpos);
			}else if(fer ==1 && module==3){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[3]);
				zpos[j].push_back(station1_zpos+station_thickness);
			}else if(fer ==2 && module==0){
				xpos[j].push_back((row)*ssdPitch-residuals[4]);
				zpos[j].push_back(station2_zpos);
			}else if(fer ==2 && module==1){
				xpos[j].push_back((-row)*ssdPitch-residuals[5]);
				zpos[j].push_back(station2_zpos);
			}else if(fer ==3 && module==0){
				xpos[j].push_back((-row+640)*ssdPitch-residuals[6]);
				zpos[j].push_back(station2_zpos+station2_thickness);
			}else if(fer ==3 && module==1){
				xpos[j].push_back((row-640)*ssdPitch-residuals[7]);
				zpos[j].push_back(station2_zpos+station2_thickness);
			}	
	
	   }
	}
       TGraph** xzPlot_array = new TGraph*[numgraphs]; 
       TF1** fit_array = new TF1*[numgraphs];
       for (int i=0;i<numgraphs;i++) { 
	     if(xpos[i].size()!=0){
		     Double_t xlist[xpos[i].size()], zlist[xpos[i].size()];
		     for(int j = 0; j<xpos[i].size(); j++){

			   xlist[j] = xpos[i][j];
			   zlist[j] = zpos[i][j];
			   }

		     xzPlot_array[i] = new TGraph(xpos[i].size(),zlist,xlist);
	     	     }
             }



   //Adding some geometry detail to the plots
   TBox *deadRegion = new TBox(station2_zpos+station2_thickness,-128*ssdPitch-residuals[7],station2_zpos+station2_thickness+10,-256*ssdPitch-residuals[7]);
   TBox *field = new TBox(420,45,600,-45); //field region
   TBox *b0 = new TBox (440,-23,490,-45); //lower 1
   TBox *b1 = new TBox (440,23,490,45); //upper 1   
   TBox *b2 = new TBox (490,-31,540,-45); //lower 2   
   TBox *b3 = new TBox (490,31,540,45); //upper 2   
   TBox *b4 = new TBox (540,-40,590,-45); //lower 3  
   TBox *b5 = new TBox (540,40,590,45); //upper 3 
   deadRegion->SetFillColor(2);  
   field->SetFillStyle(0);    
   b0->SetFillColor(16);
   b1->SetFillColor(16);
   b2->SetFillColor(16);
   b3->SetFillColor(16);
   b4->SetFillColor(16);
   b5->SetFillColor(16);

   TLatex *tx1 = new TLatex(460,-43.5,"#scale[0.6]{Magnet}");
   
   //Drawing Plots
   TCanvas *ce4 = new TCanvas("ce4","ce4",1500,700);
   ce4->Divide(3,2);
   for(int i=0;i<numgraphs;i++){
	if(xpos[i].size()!=0){
	   xzPlot_array[i]->SetTitle(Form("Event %d xz-Hits",accepted_tracks[event+i]));
      xzPlot_array[i]->GetYaxis()->SetTitle("x-position mm");
      xzPlot_array[i]->GetXaxis()->SetTitle("z-position mm");
		xzPlot_array[i]->SetMarkerColor(4);  
		xzPlot_array[i]->SetMarkerStyle(21);
      ce4->cd(i+1);
      xzPlot_array[i]->GetXaxis()->SetRangeUser(-180,1300);
		xzPlot_array[i]->GetYaxis()->SetRangeUser(-45,45);
		xzPlot_array[i]->Draw("AP");
      deadRegion->Draw("same");
		field->Draw("same");
		b0->Draw("same");
		b1->Draw("same");
		b2->Draw("same");
		b3->Draw("same");
		b4->Draw("same");
		b5->Draw("same");
		tx1->Draw("same");
		
		TF1 *fit1 = new TF1("fit1","[0]*x+[1]",-250,700);
		TF1 *fit2 = new TF1("fit2","[0]*x+[1]",380,1300);
		xzPlot_array[i]->Fit(fit1,"R");
		xzPlot_array[i]->Fit(fit2,"R+");

      ce4->Update();	
		}
	}
}



void p_calc(vector<double> residuals){

   //Input: vector with SSD offsets
   //Returns a vector with new offsets
  
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
  
   
   TH1F* momenta = new TH1F("moment","-8 GeV/c Pion: Estimated Momentum Distribution",100,-12,0);
   //loops over single event and fills vector
   for(int j=0; j<accepted_tracks.size(); j++){

     vector<vector<double>> xpos = {{},{},{},{},{},{},{},{}};
     vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};
   
	   
	   for(int i=bounds[accepted_tracks[j]]+1;i<=bounds[accepted_tracks[j]+1];i++){
		   t1->GetEntry(i);
		      
			//Filling vectors for xz-plot
			if(fer ==0 && module==1){
				xpos[0].push_back((-row+320)*ssdPitch-residuals[0]);
				zpos[0].push_back(station0_zpos);
			}else if(fer ==0 && module==3){
				xpos[1].push_back((-row+320)*ssdPitch-residuals[1]);
				zpos[1].push_back(station0_zpos+station_thickness);
			}else if(fer ==1 && module==1){
				xpos[2].push_back((-row+320)*ssdPitch-residuals[2]);
				zpos[2].push_back(station1_zpos);
			}else if(fer ==1 && module==3){
				xpos[3].push_back((-row+320)*ssdPitch-residuals[3]);
				zpos[3].push_back(station1_zpos+station_thickness);
			}else if(fer ==2 && module==0){
				xpos[4].push_back((row)*ssdPitch-residuals[4]);
				zpos[4].push_back(station2_zpos);
			}else if(fer ==2 && module==1){
				xpos[5].push_back((-row)*ssdPitch-residuals[5]);
				zpos[5].push_back(station2_zpos);
			}else if(fer ==3 && module==0){
				xpos[6].push_back((-row+640)*ssdPitch-residuals[6]);
				zpos[6].push_back(station2_zpos+station2_thickness);
			}else if(fer ==3 && module==1){
				xpos[7].push_back((row-640)*ssdPitch-residuals[7]);
				zpos[7].push_back(station2_zpos+station2_thickness);
			}	
		
		
	   }
  


	  
	   // graph of event
	   int count = 0;	
	   for(int i=0;i<zpos.size();i++)  if(zpos[i].size()!=0 && xpos[i].size()!=0)  count ++;	

	   Double_t xlist[count], zlist[count];
	   int num = 0;
	   for(int i=0;i<zpos.size();i++){
	 	   if(zpos[i].size()!=0 && xpos[i].size()!=0){
			   xlist[num] = getAverage(xpos[i]);
			   zlist[num] = zpos[i][0];
		      num++;
			}		
				
		}
	    TGraph* event = new TGraph(count,zlist,xlist);
	    TF1 *fit1 = new TF1("fit1","[0]*x+[1]",-200,700);
		TF1 *fit2 = new TF1("fit2","[0]*x+[1]",380,1300);
		event->Fit(fit1,"R");
		event->Fit(fit2,"R+");

	    double s1 = fit1->GetParameter(0);
	    double s2 = fit2->GetParameter(0);
        momenta->Fill(-1*(0.3*0.075)/(sin(0.5*atan(-s1)+0.5*atan(s2))));
	}
    momenta->Draw();
}



//.........................................................................


void multiple_tracks(vector<double> residuals, int event){

   //Input: vector of offsets and starting event
   //Plots xz-positions for six events

   vector<vector<double>> xpos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};//store x,z positions
   Double_t ez[6] = {0,0,0,0,0,0};
   Double_t ex[6] = {0.06,0.06,0.06,0.06,0.06,0.06};


   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   
   //loops over a single track and fills vectors
   int numgraphs = 6;
   for(int j = 0; j<numgraphs; j++){
           
	   for(int i=bounds[accepted_tracks[event+j]]+1;i<=bounds[accepted_tracks[event+j]+1];i++){
		t1->GetEntry(i);
      	
			//Filling vectors for xz-plot
			if(fer ==0 && module==1){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[0]);
				zpos[j].push_back(station0_zpos);
			}else if(fer ==0 && module==3){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[1]);
				zpos[j].push_back(station0_zpos+station_thickness);
			}else if(fer ==1 && module==1){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[2]);
				zpos[j].push_back(station1_zpos);
			}else if(fer ==1 && module==3){
				xpos[j].push_back((-row+320)*ssdPitch-residuals[3]);
				zpos[j].push_back(station1_zpos+station_thickness);
			}else if(fer ==2 && module==0){
				xpos[j].push_back((row)*ssdPitch-residuals[4]);
				zpos[j].push_back(station2_zpos);
			}else if(fer ==2 && module==1){
				xpos[j].push_back((-row)*ssdPitch-residuals[5]);
				zpos[j].push_back(station2_zpos);
			}else if(fer ==3 && module==0){
				xpos[j].push_back((-row+640)*ssdPitch-residuals[6]);
				zpos[j].push_back(station2_zpos+station2_thickness);
			}else if(fer ==3 && module==1){
				xpos[j].push_back((row-640)*ssdPitch-residuals[7]);
				zpos[j].push_back(station2_zpos+station2_thickness);
			}	
	
	   }
	}


   TGraph** xzPlot_array = new TGraph*[numgraphs]; 
   TF1** fit_array = new TF1*[numgraphs];
   for (int i=0;i<numgraphs;i++) { 
      if(xpos[i].size()!=0){
  	      Double_t xlist[xpos[i].size()], zlist[xpos[i].size()];
		   for(int j = 0; j<xpos[i].size(); j++){
			   xlist[j] = xpos[i][j];
			   zlist[j] = zpos[i][j];
			   }

		   xzPlot_array[i] = new TGraph(xpos[i].size(),zlist,xlist);
		   xzPlot_array[i]->SetTitle("Multiple Tracks xz-Plane");
		   xzPlot_array[i]->GetYaxis()->SetRangeUser(-45,45);
		   xzPlot_array[i]->GetYaxis()->SetTitle("x-position mm");
     	   xzPlot_array[i]->GetXaxis()->SetTitle("z-position mm");  
		   TF1 *fit1 = new TF1("fit1","[0]*x+[1]",-200,1300);
		   xzPlot_array[i]->Fit(fit1,"Q");
		   fit_array[i] = fit1;
	   }
   }



   

   //Adding some geometry detail to the plots
   TBox *field = new TBox(420,45,600,-45); //field region
   TBox *b0 = new TBox (440,-23,490,-45); //lower 1
   TBox *b1 = new TBox (440,23,490,45); //upper 1   
   TBox *b2 = new TBox (490,-31,540,-45); //lower 2   
   TBox *b3 = new TBox (490,31,540,45); //upper 2   
   TBox *b4 = new TBox (540,-40,590,-45); //lower 3  
   TBox *b5 = new TBox (540,40,590,45); //upper 3   
   field->SetFillStyle(0);    
   b0->SetFillColor(16);
   b1->SetFillColor(16);
   b2->SetFillColor(16);
   b3->SetFillColor(16);
   b4->SetFillColor(16);
   b5->SetFillColor(16);


   TLatex *tx1 = new TLatex(460,-43.5,"#scale[0.6]{Magnet}");
   
   TCanvas *ce0 = new TCanvas("ce0","ce0",700,500);
	
   for(int i=0;i<numgraphs;i++){
	if(xpos[i].size()!=0){
	    
   	if(i==0)xzPlot_array[i]->Draw("AP*");
		else if(i>0)xzPlot_array[i]->Draw("same*");
		
		fit_array[i]->Draw("same");
		field->Draw("same");
		b0->Draw("same");
		b1->Draw("same");
		b2->Draw("same");
		b3->Draw("same");
		b4->Draw("same");
		b5->Draw("same");
		
		tx1->Draw("same");
		
		}
	}
   
}

//.........................................................................


double with_fit(vector<double>residuals, int event){

    //Input: vector of offsets and starting event
    // Returns chi squared of a single track

   vector<vector<double>> xpos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};//store x,z positions
   

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
		xpos[0].push_back((-row+320)*ssdPitch-residuals[0]);
		zpos[0].push_back(station0_zpos);
	}else if(fer ==0 && module==3){
		xpos[1].push_back((-row+320)*ssdPitch-residuals[1]);
		zpos[1].push_back(station0_zpos+station_thickness);
	}else if(fer ==1 && module==1){
		xpos[2].push_back((-row+320)*ssdPitch-residuals[2]);
		zpos[2].push_back(station1_zpos);
	}else if(fer ==1 && module==3){
		xpos[3].push_back((-row+320)*ssdPitch-residuals[3]);
		zpos[3].push_back(station1_zpos+station_thickness);
	}else if(fer ==2 && module==0){
		xpos[4].push_back((row)*ssdPitch-residuals[4]);
		zpos[4].push_back(station2_zpos);
	}else if(fer ==2 && module==1){
		xpos[5].push_back((-row)*ssdPitch-residuals[5]);
		zpos[5].push_back(station2_zpos);
	}else if(fer ==3 && module==0){
		xpos[6].push_back((-row+640)*ssdPitch-residuals[6]);
		zpos[6].push_back(station2_zpos+station2_thickness);
	}else if(fer ==3 && module==1){
		xpos[7].push_back((row-640)*ssdPitch-residuals[7]);
		zpos[7].push_back(station2_zpos+station2_thickness);
	}		



   }
  

  if(xpos.size()==0) return 0;
  //Plotting xz-graph

  // graph of event
  int count = 0;	
  for(int i=0;i<zpos.size();i++)  if(zpos[i].size()!=0 && xpos[i].size()!=0)  count ++;			

  Double_t xlist[count], zlist[count];
  int num = 0;
  for(int i=0;i<zpos.size();i++){
	if(zpos[i].size()!=0 && xpos[i].size()!=0){
		xlist[num] = getAverage(xpos[i]);
		zlist[num] = zpos[i][0];
	        num++;
		}		
			
	}
  
  TGraph* xzPlot = new TGraph(count,zlist,xlist);
  TF1 *fit = new TF1("fit","[0]*x+[1]",-200,1200);
  xzPlot->Fit(fit,"Q");
  TFitResultPtr r = xzPlot->Fit(fit,"S");
  double chi2 = r->Chi2();
  f->Close();
  return chi2;


	
}


//.........................................................................

void chi(){

   auto c = new TCanvas("c","c");
   TH1* h1 = new TH1I("h1", "#chi^{2} with alignment", 104, 0, 10000);
   TH1* h2 = new TH1I("h2", "#chi^{2} without alignment", 104, 0,10000);
 
   for(int i=0; i<accepted_tracks.size(); i++){
        cout<<i<<", ";
   	double with = with_fit({ -0.49763154, -0.22506982, 0.41498188, 0.45220126, -1.2129257, 1.0351785, -1.9216890, 0.081811431 },accepted_tracks[i]);
	double without =with_fit({0,0,0,0,0,0,0,0},accepted_tracks[i]);
        
	h1->Fill(12*with/0.0036);
	h2->Fill(12*without/0.0036);
        
   }
  

  THStack* hstack = new THStack("hstack", "Chi Squared Before and After Alignment");
  h1->GetYaxis()->SetTitle("counts");
  h1->GetXaxis()->SetTitle("#chi^{2}");
  //TCanvas *ce2 = new TCanvas("ce2");
  h1->SetLineColor(kRed);
  h1->SetFillStyle(3354);
  h1->SetLineWidth(2);
  //h1->Draw();


  TCanvas *ce3 = new TCanvas("ce3");
  h2->SetLineColor(kBlue);
  h2->SetFillStyle(3354);
  h2->SetLineWidth(2);
  //h2->Draw();



  hstack->Add(h2);
  hstack->Add(h1);
  hstack->Draw();
  hstack->GetXaxis()->SetTitle("Chi Squared");
  hstack->GetYaxis()->SetTitle("counts");
  gPad->BuildLegend(0.75,0.75,0.95,0.95,"");
  //hstack->Draw();

}


vector<vector<double>> alignments;

vector<double> looping(vector<double>residuals, double with, int times){
    vector<double> alignment = xalign(residuals,with);
    

    for(int i = 0; i<times; i++){
	vector<double> new_alignment = xalign(alignment,with);
       // for(int i=0;i<8;i++)cout<<new_alignment[i]<<endl;

        for(int j=0;j<alignment.size();j++){
	    alignment[j] = new_alignment[j];
	}
        
    }
    return alignment;
}



void alignmentx(){
   
 
}




//Final ALignment values: { -0.46621757, -0.18866671, 0.45762286, 0.49498593, -1.2534144, 1.1220332, -1.9807021, 0.18154533 }





