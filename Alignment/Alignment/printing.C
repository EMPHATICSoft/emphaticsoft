#include<iostream>
#include<vector>

void print(vector <double> a) {

   cout<<"{";
   for(int i=0; i < a.size(); i++) cout << a.at(i) << ", ";
   cout<<"}";
}

//....................................................................


template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}



//.........................................................................


double angle(double x,double y){
        double result;
	if(x>=0 && y>=0) result = atan(x/y);
	else if(x<0) result = atan(x/y)*(180/3.1415)+180;
	else if(x>=0 && y<0) result = atan(x/y)*(180/3.1415)+360;
	return result;
}

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
		h1->Fill(station0_zpos+station_thickness,(row-320)*ssdPitch-residuals[1]);
	}else if(fer ==1 && module==0){
		h1->Fill(station1_zpos,(row-320)*ssdPitch-residuals[2]);
	}else if(fer ==1 && module==2){
		h1->Fill(station1_zpos+station_thickness,(row-320)*ssdPitch-residuals[3]);	
	}else if(fer ==2 && module==2){
		h1->Fill(station2_zpos,(row-640)*ssdPitch-residuals[4]);
                h2->Fill((row-640)*ssdPitch-residuals[4]);		
	}else if(fer ==2 && module==3){
		h1->Fill(station2_zpos,-(row-640)*ssdPitch-residuals[5]);
                h2->Fill(-(row-640)*ssdPitch-residuals[5]);		
	}else if(fer ==3 && module==2){
		h1->Fill(station2_zpos+station_thickness,(row-640)*ssdPitch-residuals[6]);
		h2->Fill((row-640)*ssdPitch-residuals[6]);		
	}else if(fer ==3 && module==3){
		h1->Fill(station2_zpos+station_thickness,-(row-640)*ssdPitch-residuals[7]);
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

