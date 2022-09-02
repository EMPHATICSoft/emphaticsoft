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





vector<double> yalign(vector<double> residuals,double size){
   
   //Input: vector with current SSD offsets
   //Input: size of histograms to be filled with residuals

   //Output: vector with new residuals

  

   int two_hits = 0;
   int one_hit = 0;

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
   residual_array[0] = new TH1F("h0","First Station Y-SSD",num_bins,-size,size);
   residual_array[1] = new TH1F("h1","Second Station Y-SSD",num_bins,-size,size);
   residual_array[2] = new TH1F("h2","Third Station Y-SSD",num_bins,-size,size);
   residual_array[3] = new TH1F("h3","Fourth Station Y-SSD",num_bins,-size,size);
   residual_array[4] = new TH1F("h4","Fith Station Upper Y-SSD",num_bins,-size,size);
   residual_array[5] = new TH1F("h5","Fith Station Lower Y-SSD",num_bins,-size,size);
   residual_array[6] = new TH1F("h6","Sixth Station Upper Y-SSD",num_bins,-size,size);
   residual_array[7] = new TH1F("h7","Sixth Station Lower Y-SSD",num_bins,-size,size);
	

   
   //loops over single event and fills vector
   for(int j=0; j<accepted_tracks.size(); j++){

      vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
      vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};
      vector<int> xcounts = {0,0,0,0,0,0,0,0};
   
	   for(int i=bounds[accepted_tracks[j]]+1;i<=bounds[accepted_tracks[j]+1];i++){
		   t1->GetEntry(i);
		      
				//Filling vectors for xz-plot
				
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
	         xcounts[5]++;
	      }else if(fer ==2 && module==1){
	         xcounts[4]++;
	         xcounts[5]++;
	      }else if(fer ==3 && module==0){
	         xcounts[6]++;
	         xcounts[7]++;
	      }else if(fer ==3 && module==1){
	         xcounts[6]++;
	         xcounts[7]++;
	      }
	      //Filling vectors for yz-plot
	      else if(fer ==0 && module==0){
	         ypos[0].push_back((row-320)*ssdPitch-residuals[0]);
	         zpos[0].push_back(station0_zpos);
	      }else if(fer ==0 && module==2){
	         ypos[1].push_back((row-320)*ssdPitch-residuals[1]);
	         zpos[1].push_back(station1_zpos);      
	      }else if(fer ==1 && module==0){
	         ypos[2].push_back((row-320)*ssdPitch-residuals[2]);
	         zpos[2].push_back(station2_zpos);      
	      }else if(fer ==1 && module==2){
	         ypos[3].push_back((row-320)*ssdPitch-residuals[3]);
	         zpos[3].push_back(station3_zpos);      
	      }else if(fer ==2 && module==2){
	         ypos[4].push_back((row-640)*ssdPitch-residuals[4]);
	         zpos[4].push_back(station4_zpos);      
	      }else if(fer ==2 && module==3){
	         ypos[5].push_back(-(row-640)*ssdPitch-residuals[5]);
	         zpos[5].push_back(station4_zpos);      
	      }else if(fer ==3 && module==2){
	         ypos[6].push_back((row-640)*ssdPitch-residuals[6]);
	         zpos[6].push_back(station5_zpos);      
	      }else if(fer ==3 && module==3){
	         ypos[7].push_back(-(row-640)*ssdPitch-residuals[7]);
	         zpos[7].push_back(station5_zpos);
	      }  
	   }


	   // graph of event
	   int count = 0; 
	   for(int i=0;i<ypos.size();i++){
	     if(ypos[i].size()!=0 && xcounts[i]!=0)  count ++;  
	     if(ypos[i].size()==2) two_hits++;
	     else if(ypos[i].size()==1) one_hit++;

	  }
	   
      cout<<endl<<count<<endl;
      Double_t ylist[count], zlist[count];


	   int num = 0;
	   for(int i=0;i<ypos.size();i++){
	      if(ypos[i].size() ==0 || xcounts[i]==0)continue;
		   ylist[num] = getAverage(ypos[i]);
		   zlist[num] = getAverage(zpos[i]);



	      num++;	
		}
	  
		
	 
	   TGraph* event = new TGraph(count,zlist,ylist);
	   TF1 *fit = new TF1("fit","[1]*x+[0]",-200,1200);
	   event->Fit(fit,"Q"); 

	   //cout<<endl;
      //for(int i=0; i<count; i++)cout<<ylist[i]<<", ";
      //cout<<endl;
     
		for(int i=0;i<ypos.size();i++){
		   if(ypos[i].size() ==0 || xcounts[i]==0)continue;
		   double delta_y = (getAverage(ypos[i])-fit->Eval(zpos[i][0],0,0));//calculate difference between fitted and actual data
			residual_array[i]->Fill(delta_y*sqrt(12)/ssdPitch);
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

void yplot(vector<double> residuals, int event){

   //Input: vector of offsets and starting event
   //Plots yz-positions for six events

   vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};//store x,z positions
  
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   int start = -1;
   //loops over a single track and fills vectors
   int numgraphs = 6;
   for(int j = 0; j<numgraphs; j++){
      start ++;
	   //for(int i=bounds[accepted_tracks[event+j]]+1;i<=bounds[accepted_tracks[event+j]+1];i++){
      for(int i=bounds[accepted_tracks[event+j]]+1;i<=bounds[accepted_tracks[event+j]+1];i++){
			t1->GetEntry(i);
	      	
			//Filling vectors for yz-plot
			if(fer ==0 && module==0){
				ypos[start].push_back((row-320)*ssdPitch-residuals[0]);
				zpos[start].push_back(station0_zpos);
			}else if(fer ==0 && module==2){
				ypos[start].push_back((row-320)*ssdPitch-residuals[1]);
				zpos[start].push_back(station1_zpos);
			}else if(fer ==1 && module==0){
				ypos[start].push_back((row-320)*ssdPitch-residuals[2]);
				zpos[start].push_back(station2_zpos);
			}else if(fer ==1 && module==2){
				ypos[start].push_back((row-320)*ssdPitch-residuals[3]);
				zpos[start].push_back(station3_zpos);
			}else if(fer ==2 && module==2){
				ypos[start].push_back((row-640)*ssdPitch-residuals[4]);
				zpos[start].push_back(station4_zpos);
			}else if(fer ==2 && module==3){
				ypos[start].push_back(-(row-640)*ssdPitch-residuals[5]);
				zpos[start].push_back(station4_zpos);
			}else if(fer ==3 && module==2){
				ypos[start].push_back((row-640)*ssdPitch-residuals[6]);
				zpos[start].push_back(station5_zpos);
			}else if(fer ==3 && module==3){
				ypos[start].push_back(-(row-640)*ssdPitch-residuals[7]);
				zpos[start].push_back(station5_zpos);
			}	
		
	   }
	}

       
      TGraph** yzPlot_array = new TGraph*[numgraphs]; 
       TF1** fit_array = new TF1*[numgraphs];
       for (int i=0;i<numgraphs;i++) { 
	     if(ypos[i].size()!=0){
		     Double_t ylist[ypos[i].size()], zlist[ypos[i].size()];
		     for(int j = 0; j<ypos[i].size(); j++){
			   ylist[j] = ypos[i][j];
			   zlist[j] = zpos[i][j];
			   }

		     yzPlot_array[i] = new TGraph(ypos[i].size(),zlist,ylist);
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

   //TLatex *tx0 = new TLatex(725,30,"#scale[0.6]{Magnetic Field Region}");
   //TArrow *ar0 = new TArrow(800,29,540,0,0.005,"|>");
   //ar0->SetAngle(40);
   //ar0->SetLineWidth(1);

   //TLatex *tx1 = new TLatex(460,-35,"#scale[0.6]{Magnet}");
   TLatex *tx2 = new TLatex(460,-43,"#scale[0.6]{Magnet}");
   

   //Drawing plots
   TCanvas *ce4 = new TCanvas("ce4","ce4",1500,700);
   ce4->Divide(3,2);
   for(int i=0;i<numgraphs;i++){
	if(ypos[i].size()!=0){
	   yzPlot_array[i]->SetTitle(Form("Event %d yz-Hits",accepted_tracks[event+i]));
     	yzPlot_array[i]->GetYaxis()->SetTitle("y-position mm");
     	yzPlot_array[i]->GetXaxis()->SetTitle("z-position mm");
		yzPlot_array[i]->GetYaxis()->SetRangeUser(-45,45);
    	yzPlot_array[i]->GetXaxis()->SetRangeUser(-200,1300);
		yzPlot_array[i]->SetMarkerColor(4);
      yzPlot_array[i]->SetMarkerStyle(21);
      ce4->cd(i+1);
   		
		yzPlot_array[i]->Draw("AP");
		field->Draw("same");
		b0->Draw("same");
		b1->Draw("same");
		b2->Draw("same");
		b3->Draw("same");
		b4->Draw("same");
		b5->Draw("same");
		tx2->Draw("same");
		TF1 *fit1 = new TF1("fit1","[0]*x+[1]",-200,1300);
		//TF1 *fit2 = new TF1("fit2","[0]*x+[1]",380,1300);
		yzPlot_array[i]->Fit(fit1);
		//yzPlot_array[i]->Fit(fit2,"R+");
   		
		}
	}

/*
   TCanvas *ce0 = new TCanvas("ce0","ce0",300,500);
   for(int i=0;i<numgraphs;i++){
	if(xpos[i].size()!=0){
	        
   	if(i==0)xzPlot_array[i]->Draw("AP*");
		else if(i>0)xzPlot_array[i]->Draw("same*");
		fit_array[i]->Draw("same");
		
		}
	}
*/

}


//.........................................................................

//.........................................................................

void just_tracks(vector<double> residuals, int event){

   //Input: vector of offsets and starting event
   //Plots yz-positions for six events

   vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};//store x,z positions
  
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   int start = -1;
   //loops over a single track and fills vectors
   int numgraphs = 6;
   for(int j = 0; j<numgraphs; j++){
           start ++;
	   for(int i=bounds[accepted_tracks[event+j]]+1;i<=bounds[accepted_tracks[event+j]+1];i++){
		t1->GetEntry(i);
      	
		//Filling vectors for yz-plot
		if(fer ==0 && module==0){
			ypos[start].push_back((row-320)*ssdPitch-residuals[0]);
			zpos[start].push_back(station0_zpos);
		}else if(fer ==0 && module==2){
			ypos[start].push_back((row-320)*ssdPitch-residuals[1]);
			zpos[start].push_back(station1_zpos);
		}else if(fer ==1 && module==0){
			ypos[start].push_back((row-320)*ssdPitch-residuals[2]);
			zpos[start].push_back(station2_zpos);
		}else if(fer ==1 && module==2){
			ypos[start].push_back((row-320)*ssdPitch-residuals[3]);
			zpos[start].push_back(station3_zpos);
		}else if(fer ==2 && module==2){
			ypos[start].push_back((row-640)*ssdPitch-residuals[4]);
			zpos[start].push_back(station4_zpos);
		}else if(fer ==2 && module==3){
			ypos[start].push_back(-(row-640)*ssdPitch-residuals[5]);
			zpos[start].push_back(station4_zpos);
		}else if(fer ==3 && module==2){
			ypos[start].push_back((row-640)*ssdPitch-residuals[6]);
			zpos[start].push_back(station5_zpos);
		}else if(fer ==3 && module==3){
			ypos[start].push_back(-(row-640)*ssdPitch-residuals[7]);
			zpos[start].push_back(station5_zpos);
		}	
	
	   }
	}

       
      TGraph** yzPlot_array = new TGraph*[numgraphs]; 
       TF1** fit_array = new TF1*[numgraphs];
       for (int i=0;i<numgraphs;i++) { 
	     if(ypos[i].size()!=0){
		     Double_t ylist[ypos[i].size()], zlist[ypos[i].size()];
		     for(int j = 0; j<ypos[i].size(); j++){
			   ylist[j] = ypos[i][j];
			   zlist[j] = zpos[i][j];
			   }

		     yzPlot_array[i] = new TGraph(ypos[i].size(),zlist,ylist);
	     	     }
             }


   //Adding some geometry detail to the plots
   TBox *field = new TBox(420,20,600,-20); //field region
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

   TLatex *tx0 = new TLatex(725,30,"#scale[0.6]{Magnetic Field Region}");
   TArrow *ar0 = new TArrow(800,29,540,0,0.005,"|>");
   ar0->SetAngle(40);
   ar0->SetLineWidth(1);

   TLatex *tx1 = new TLatex(460,-35,"#scale[0.6]{Magnet}");
   TLatex *tx2 = new TLatex(460,35,"#scale[0.6]{Magnet}");
   

   //Drawing plots
   TCanvas *ce4 = new TCanvas("ce4","ce4",1500,700);
   ce4->Divide(3,2);
   for(int i=0;i<numgraphs;i++){
	if(ypos[i].size()!=0){
	   yzPlot_array[i]->SetTitle(Form("Event %d yz-Hits",accepted_tracks[event+i]));
     	yzPlot_array[i]->GetYaxis()->SetTitle("y-position mm");
     	yzPlot_array[i]->GetXaxis()->SetTitle("z-position mm");
		yzPlot_array[i]->GetYaxis()->SetRangeUser(-45,45);
    	yzPlot_array[i]->GetXaxis()->SetRangeUser(-200,1300);
		yzPlot_array[i]->SetMarkerColor(4);
      yzPlot_array[i]->SetMarkerStyle(21);
      ce4->cd(i+1);
   		
		yzPlot_array[i]->Draw("AP");
		
		//TF1 *fit1 = new TF1("fit1","[0]*x+[1]",-200,400);//630
		TF1 *fit2 = new TF1("fit2","[0]*x+[1]",-200,1300);
		
		//yzPlot_array[i]->Fit(fit1,"R");
		//fit_array[i] = fit1;
	
		}
	}

/*
   TCanvas *ce0 = new TCanvas("ce0","ce0",300,500);
   for(int i=0;i<numgraphs;i++){
	if(xpos[i].size()!=0){
	        
   	if(i==0)xzPlot_array[i]->Draw("AP*");
		else if(i>0)xzPlot_array[i]->Draw("same*");
		fit_array[i]->Draw("same");
		
		}
	}
*/

}

//.........................................................................

void multiple_tracks(vector<double> residuals, int event){

   //Input: vector of offsets and starting event
   //Plots yz-positions for six events

   vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
   vector<vector<double>> zpos = {{},{},{},{},{},{},{},{}};//store x,z positions
  
   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   int start = -1;
   //loops over a single track and fills vectors
   int numgraphs = 6;
   for(int j = 0; j<numgraphs; j++){
           start ++;
	   for(int i=bounds[accepted_tracks[event+j]]+1;i<=bounds[accepted_tracks[event+j]+1];i++){
		t1->GetEntry(i);
      	
		//Filling vectors for yz-plot
		if(fer ==0 && module==0){
			ypos[start].push_back((row-320)*ssdPitch-residuals[0]);
			zpos[start].push_back(station0_zpos);
		}else if(fer ==0 && module==2){
			ypos[start].push_back((row-320)*ssdPitch-residuals[1]);
			zpos[start].push_back(station1_zpos);
		}else if(fer ==1 && module==0){
			ypos[start].push_back((row-320)*ssdPitch-residuals[2]);
			zpos[start].push_back(station2_zpos);
		}else if(fer ==1 && module==2){
			ypos[start].push_back((row-320)*ssdPitch-residuals[3]);
			zpos[start].push_back(station3_zpos);
		}else if(fer ==2 && module==2){
			ypos[start].push_back((row-640)*ssdPitch-residuals[4]);
			zpos[start].push_back(station4_zpos);
		}else if(fer ==2 && module==3){
			ypos[start].push_back(-(row-640)*ssdPitch-residuals[5]);
			zpos[start].push_back(station4_zpos);
		}else if(fer ==3 && module==2){
			ypos[start].push_back((row-640)*ssdPitch-residuals[6]);
			zpos[start].push_back(station5_zpos);
		}else if(fer ==3 && module==3){
			ypos[start].push_back(-(row-640)*ssdPitch-residuals[7]);
			zpos[start].push_back(station5_zpos);
		}	
	
	   }
	}


    TGraph** yzPlot_array = new TGraph*[numgraphs]; 
    TF1** fit_array = new TF1*[numgraphs];
    for (int i=0;i<numgraphs;i++) { 
	    if(ypos[i].size()!=0){
		    Double_t ylist[ypos[i].size()], zlist[ypos[i].size()];
		    for(int j = 0; j<ypos[i].size(); j++){
			   ylist[j] = ypos[i][j];
			   zlist[j] = zpos[i][j];
			   }

		    yzPlot_array[i] = new TGraph(ypos[i].size(),zlist,ylist);
		    yzPlot_array[i]->SetTitle("Multiple Tracks yz-Plane");
		    yzPlot_array[i]->GetYaxis()->SetRangeUser(-45,45);
		    yzPlot_array[i]->GetYaxis()->SetTitle("y-position mm");
     	    yzPlot_array[i]->GetXaxis()->SetTitle("z-position mm");  
		    TF1 *fit1 = new TF1("fit1","[0]*x+[1]",-200,1300);
		    yzPlot_array[i]->Fit(fit1,"Q");
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
   //TLatex *tx2 = new TLatex(460,35,"#scale[0.6]{Magnet}");
   
  
   TCanvas *ce0 = new TCanvas("ce0","ce0",700,500);
	
   for(int i=0;i<numgraphs;i++){
	if(ypos[i].size()!=0){
	    
   	if(i==0)yzPlot_array[i]->Draw("AP*");
		else if(i>0)yzPlot_array[i]->Draw("same*");
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

/*
void yhist(vector<double> residuals){

   //Input: vector with ssd offsets
   ///Plots a histogram of the beam profile

   //Read in data from SSD.root tree 
   TFile *f = new TFile(infile);
   TTree *t1 = (TTree*)f->Get("SSDtree");
   int fer, module, row;
   t1->SetBranchAddress("fer",&fer);
   t1->SetBranchAddress("module",&module);
   t1->SetBranchAddress("row",&row);
   auto h1 = new TH2F("h1",Form("Spill %d: yz-Hits",spill),150,-200,1200,1500,-45,45);
   auto h2 = new TH1F("h2","help", 1500,-45,45);

//loops over single event and fills vector
	for(int i=0; i<t1->GetEntries(); i++){
		t1->GetEntry(i);
	      	
		//Filling vectors for yz-plot
		if(fer ==0 && module==0){
			h1->Fill(station0_zpos,(row-320)*ssdPitch-residuals[0]);
		}else if(fer ==0 && module==2){
			h1->Fill(station1_zpos,(row-320)*ssdPitch-residuals[1]);
		}else if(fer ==1 && module==0){
			h1->Fill(station2_zpos,(row-320)*ssdPitch-residuals[2]);
		}else if(fer ==1 && module==2){
			h1->Fill(station3_zpos,(row-320)*ssdPitch-residuals[3]);	
		}else if(fer ==2 && module==2){
			h1->Fill(station4_zpos,(row-640)*ssdPitch-residuals[4]);
	                h2->Fill((row-640)*ssdPitch-residuals[4]);		
		}else if(fer ==2 && module==3){
			h1->Fill(station4_zpos,-(row-640)*ssdPitch-residuals[5]);
	                h2->Fill(-(row-640)*ssdPitch-residuals[5]);		
		}else if(fer ==3 && module==2){
			h1->Fill(station5_zpos,(row-640)*ssdPitch-residuals[6]);
			h2->Fill((row-640)*ssdPitch-residuals[6]);		
		}else if(fer ==3 && module==3){
			h1->Fill(station5_zpos,-(row-640)*ssdPitch-residuals[7]);
			h2->Fill(-(row-640)*ssdPitch-residuals[7]);
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
   //h2->Draw();
}
*/

//.........................................................................

double with_fit(vector<double>residuals, int event){

    //Input: vector of offsets and starting event
    // Returns chi squared of a single track

   vector<vector<double>> ypos = {{},{},{},{},{},{},{},{}};
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
      	
		//Filling vectors for yz-plot
		if(fer ==0 && module==0){
			ypos[0].push_back((row-320)*ssdPitch-residuals[0]);
			zpos[0].push_back(station0_zpos);
		}else if(fer ==0 && module==2){
			ypos[1].push_back((row-320)*ssdPitch-residuals[1]);
			zpos[1].push_back(station1_zpos);		
		}else if(fer ==1 && module==0){
			ypos[2].push_back((row-320)*ssdPitch-residuals[2]);
			zpos[2].push_back(station2_zpos);		
		}else if(fer ==1 && module==2){
			ypos[3].push_back((row-320)*ssdPitch-residuals[3]);
			zpos[3].push_back(station3_zpos);		
		}else if(fer ==2 && module==2){
			ypos[4].push_back((row-640)*ssdPitch-residuals[4]);
			zpos[4].push_back(station4_zpos);		
		}else if(fer ==2 && module==3){
			ypos[5].push_back(-(row-640)*ssdPitch-residuals[5]);
			zpos[5].push_back(station4_zpos);		
		}else if(fer ==3 && module==2){
			ypos[6].push_back((row-640)*ssdPitch-residuals[6]);
			zpos[6].push_back(station5_zpos);		
		}else if(fer ==3 && module==3){
			ypos[7].push_back(-(row-640)*ssdPitch-residuals[7]);
			zpos[7].push_back(station5_zpos);
		}	
	}
  

  if(ypos.size()==0) return 0;
  //Plotting yz-graph

  // graph of event
  int count = 0;	
  for(int i=0;i<zpos.size();i++)  if(zpos[i].size()!=0 && ypos[i].size()!=0)  count ++;			

  Double_t ylist[count], zlist[count];
  int num = 0;
  for(int i=0;i<zpos.size();i++){
	if(zpos[i].size()!=0 && ypos[i].size()!=0){
		ylist[num] = getAverage(ypos[i]);
		zlist[num] = zpos[i][0];
	        num++;
		}		
			
	}
  
  TGraph* yzPlot = new TGraph(count,zlist,ylist);
  TF1 *fit = new TF1("fit","[0]*x+[1]",-200,1200);
  yzPlot->Fit(fit,"Q");
  TFitResultPtr r = yzPlot->Fit(fit,"S");
  double chi2 = r->Chi2();
  f->Close();
  return chi2;


	
}

//.........................................................................


vector<vector<double>> alignments;
void chi(){

   auto c = new TCanvas("c","c");
   TH1* h1 = new TH1I("h1", "#chi^{2} with alignment", 104, 0, 50);
   //TH1* h2 = new TH1I("h2", "#chi^{2} without alignment", 104, 0,100000);
 
   for(int i=0; i<accepted_tracks.size(); i++){
      cout<<i<<", ";
   	double with = with_fit({ -1.5472465, -0.42643085, 1.1843462, 2.0211066, 0.35674985, -1.9408237, 4.7775777, 2.5560878 },accepted_tracks[i]);
	   //double without =with_fit({0,0,0,0,0,0,0,0},accepted_tracks[i]);
        
	   h1->Fill(12*with/0.0036);
	   //h2->Fill(12*without/0.0036);  
     }
  

  THStack* hstack = new THStack("hstack", "Chi Squared Before and After Alignment");

  
  h1->SetLineColor(kRed);
  h1->SetFillStyle(3354);
  h1->SetLineWidth(2);
  h1->Draw();

/*
  h2->SetLineColor(kBlue);
  h2->SetFillStyle(3354);
  h2->SetLineWidth(2);



  hstack->Add(h1);
  hstack->Add(h2);
  hstack->Draw();
  hstack->GetXaxis()->SetTitle("Chi Squared");
  hstack->GetYaxis()->SetTitle("counts");
  gPad->BuildLegend(0.75,0.75,0.95,0.95,"");
  */

}


vector<double> looping(vector<double>residuals, double width, int times){
    vector<double> alignment = yalign(residuals,width);
    

    for(int i = 0; i<times; i++){
	   vector<double> new_alignment = yalign(alignment,width);
      for(int j=0;j<alignment.size();j++)alignment[j] = new_alignment[j];
        
    }
    for(int i=0;i<alignment.size();i++)cout<<alignment[i]<<" ";
    cout<<endl;
    return alignment;
}


void alignmenty(){
   
  //yhist();
  /*
  alignments.push_back(initial_yalign({0,0,0,0,0,0,0,0}));
  
  for(int i=0;i<6;i++){
	alignments.push_back(yalign(alignments[i]));
	}
  */	 
   
}



//final { -1.3499483, -0.35833732, 1.0266207, 1.7363814, -0.61060676, -2.9070570, 3.6750252, 1.4502444 }






