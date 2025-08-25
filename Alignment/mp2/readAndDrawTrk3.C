#include "TString.h"
#include "TH1.h"
#include "TFile.h"
#include "TGraph.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TObjArray.h"
#include "TMultiGraph.h"
#include "TColor.h"
#include "TH1F.h"
#include "TMarker.h"
#include "TExec.h"
#include "TLatex.h"

using namespace std;

std::vector<Double_t> zpos;
std::vector<std::pair<std::string,std::string>> pair_1c;
std::vector<std::string> names;
std::vector<std::vector<std::vector<std::vector<TGraphErrors>>>> runsx, runsy, runsz, runsphi;
std::vector<std::vector<std::vector<std::vector<TGraph>>>> runsxd, runsyd, runszd, runsphid;
int ni;

std::vector<Color_t> pal = {kRed,kOrange-3,kGreen,kBlue,kViolet,kViolet+3};
std::vector<Color_t> pal8 = {kRed,kOrange-3,kYellow+1,kGreen+1,kTeal,kBlue-2,kViolet,kPink+1};
int findStation(int n);
void makeMarkers(std::vector<string> n0, vector<vector<vector<Double_t>>> &t, vector<vector<vector<Double_t>>> &p, vector<vector<vector<Double_t>>> &tz, Double_t z, vector<vector<vector<int>>> &s, float pssd);
void DrawText();
float getParam(std::vector<int> id, const char* entry2);
void readMe(const char* entry, const char* entry2);
void readAndDrawTrk3(const char* list, const char* ssdlist){

  std::vector<Double_t> zpos28 = { -6.95, 0.3, 274.05, 281.3, 494.05, 506.75, 511.3, 608.05,
                              620.75, 625.3, 839.05, 846.3, 1139.43, 1140.63, 1146.68,
                              1146.68, 1156.68, 1156.68, 1464.87, 1466.07, 1472.12,
                              1472.12, 1482.12, 1482.12, 1737.87, 1739.07, 1745.12, 1745.12 };

  zpos = { -6.95, 0.3, 274.05, 281.3, 494.05, 506.75, 511.3, 608.05,
                              620.75, 625.3, 839.05, 846.3, 1139.43, 1146.68,
                              1156.68, 1464.87, 1472.12,
                              1482.12, 1737.87, 1745.12 };
  std::vector<Double_t> z;

  ni = 0;

  std::string line;

  pair_1c = {
  //  {"0","y"}, {"10", "x"},
    {"100", "y"}, {"110", "x"},
    {"200", "u"}, {"210", "y"}, {"220", "x"}, 
    {"300", "u"}, {"310", "y"}, {"320", "x"},
    {"400", "y"}, {"410", "x"},
    {"500", "x"}, {"501", "x"}, {"510", "y"}, {"511", "y"}, {"520", "u"}, {"521", "u"},
    {"600", "x"}, {"601", "x"}, {"610", "y"}, {"611", "y"}, {"620", "u"}, {"621", "u"}, 
    {"700", "x"}, {"701", "x"}, {"710", "y"}, {"711", "y"} 
    };

  ifstream l;
  l.open(list);
  std::string entry;

  while (l.good())
  {
    while (getline(l, entry)){
      auto pos = entry.find("res");
      auto len = entry.length();
      auto und = entry.find(".txt");
      auto nn = entry.substr(pos+4,len-und);
      names.push_back(nn);
      ifstream m;
      m.open(ssdlist);
      std::string entry2;

      while (m.good()){
	while (getline(m,entry2)){ 
	  if (entry2.find(nn) != std::string::npos){ 
	    readMe(entry.c_str(), entry2.c_str());
	    ni++;
	  }
	}
      }
      m.close();
    }
  }
  l.close();

  std::vector<Double_t> xline = {-200,2200};
  std::vector<Double_t> yline = {0,0};
  TGraph *zero = new TGraph(2,xline.data(),yline.data());
  zero->SetLineStyle(2);
  zero->SetLineColor(16);

  auto c1 = new TCanvas("c1","A Simple Graph with error bars",1200,600);
  gStyle->SetLegendTextSize(0.035);

/*
  auto legend = new TLegend(0.15,0.6,0.37,0.9); //0.7,0.48,0.9);
  legend->SetHeader("Beam Momentum [GeV/c]","C");

  for (int i=0; i<tgphil.size(); i++){
    for (int j=0; j<tgphil[i].size(); j++){
      TGraphErrors* g = &tgphil[i][j];
      std::string namemarker = names[j];
      if (i==0) namemarker += " for X-plane";
      if (i==1) namemarker += " for Y-plane";
      if (i==2) namemarker += " for U-plane";
      legend->AddEntry(g,namemarker.c_str(),"lep"); //lep
    }
  }
  zero->Draw();
  legend->Draw();
  gPad->Modified();
  gPad->Update(); 
*/

  auto c2 = new TCanvas("c2","A Simple Graph with error bars",1200,600);
  gStyle->SetLegendTextSize(0.035);
  c2->cd();

  int n = names.size();
  auto hg = new TH1F("hg","hg",n,0,n);
  for (Int_t i = 0; i < n; i++) {
      hg->GetXaxis()->SetBinLabel(i + 1, names[i].c_str());
  }

  TMultiGraph *mgxr = new TMultiGraph("MultiGraph","X Shifts");
  std::vector<TGraphErrors> eachx;
  TMultiGraph *mgxrs = new TMultiGraph("MultiGraph","X Shifts (Errors x10)");
  std::vector<std::vector<std::vector<TMultiGraph*>>> mgxrv;
  mgxrv.resize(3);
  for (int i=0; i<mgxrv.size(); i++) mgxrv[i].resize(2);

  auto legend2 = new TLegend(0.15,0.69,0.55,0.89); //0.7,0.48,0.9);
  legend2->SetHeader("","C"); //"Stations and Sensors","C");
  legend2->SetNColumns(4);
  legend2->SetBorderSize(0);
  for (int i=0; i<11; i++){
      std::string namemarker = "";
      if (i<8) {
        namemarker += Form("Station %i",i);
        TLegendEntry* l = legend2->AddEntry(mgxr,namemarker.c_str(),"l"); //lep
        l->SetLineColor(pal8[i]);
      }
      if (i>=8) {
        if (i==8) namemarker += "X-plane";
        if (i==9) namemarker += "Y-plane";
        if (i==10) namemarker += "U-plane";
	TLegendEntry* l = legend2->AddEntry(mgxr,namemarker.c_str(),"p");
        l->SetMarkerStyle(24+i-8);
      }
  }

  for (int plane=0; plane<3; plane++){ // XYU
    for (int sensor=0; sensor<2; sensor++){ // 0 or 1
      for (int station=0; station<8; station++){ // 0 to 7
        TMultiGraph *mgxrv_js = new TMultiGraph("MultiGraph","X Shifts");
        TMultiGraph *mgdummy = new TMultiGraph("MultiGraph","X Shifts");
        for (int i=0; i<runsx[plane][sensor][station].size(); i++){
          TGraphErrors* g = &runsx[plane][sensor][station][i];
          if (g->GetN() != 0){
            //g->SetMarkerColor(pal8[station]);      
	    //g->SetLineColor(pal8[station]);
            g->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
            g->SetMarkerSize(2);
            mgxr->Add(g);
            mgxrv_js->Add(g);

            TGraph* gd = &runsxd[plane][sensor][station][i];
	    mgdummy->Add(g);
            //mgdummy->Add(gd);
          }
        }
        mgxrv[plane][sensor].push_back(mgxrv_js);
        if (mgdummy->GetListOfGraphs() != NULL){
          TF1 *f1 = new TF1("f1", "[0]", 0, 10);
          mgdummy->Fit("f1","Q");   
          auto *params = f1->GetParameters();
          auto *errors = f1->GetParErrors();
          std::cout<<"x shifts "<<plane<<station<<": "<<errors[0]<<std::endl;

          auto ge = new TGraphErrors(1);
          ge->SetPoint(0,(double)station,params[0]);
          ge->SetPointError(0,0.,errors[0]);
          //ge->SetMarkerColor(pal8[station]);
          //ge->SetLineColor(pal8[station]);
          ge->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
          ge->SetMarkerSize(2);
          eachx.push_back(*ge); 

          ge->Draw("AP");
          ge->SetTitle("X Shifts Fit to a Constant");
          ge->GetXaxis()->SetTitle("Station");
          ge->GetYaxis()->SetTitle("Shifts [mm]");
          ge->GetXaxis()->SetLimits(-1.,8.);
	  std::string planetype = "";
          if (plane == 0) planetype = "x";
          if (plane == 1) planetype = "y";
          if (plane == 2) planetype = "u";
	  //c2->SaveAs(Form("xshifts_avg_%i%s%i.png",station,planetype.c_str(),sensor));
        }
      }
    }
  }
  for (int i=0; i<eachx.size(); i++){
    TGraphErrors* gc = &eachx[i];
    mgxrs->Add(gc);
  }

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TExec *ex = new TExec("ex","DrawText();");
        mgxrv[plane][sensor][station]->GetListOfFunctions()->Add(ex);
        mgxrv[plane][sensor][station]->Draw("AP");
        mgxrv[plane][sensor][station]->GetXaxis()->SetLabelSize(0);
        mgxrv[plane][sensor][station]->GetXaxis()->SetTickLength(0);
        mgxrv[plane][sensor][station]->GetXaxis()->SetTitle("Run Number");
        mgxrv[plane][sensor][station]->GetXaxis()->SetTitleOffset(1.5);
        mgxrv[plane][sensor][station]->GetYaxis()->SetTitle("Shifts [mm]");
        mgxrv[plane][sensor][station]->GetYaxis()->SetLabelSize(0.04);
        zero->Draw();
        //legend2->Draw();
        gPad->Modified();
        gPad->Update();
        std::string planetype = "";
        if (plane == 0) planetype = "x";
        if (plane == 1) planetype = "y";
        if (plane == 2) planetype = "u";
        c2->SaveAs(Form("xshifts_runs_%i%s%i.png",station,planetype.c_str(),sensor));
        //c2->SaveAs(Form("xshifts_runs_%i%i%i.png",station,plane,sensor));
      }
    }
  }

  TExec *ex = new TExec("ex","DrawText();");

  TMultiGraph *mgyr = new TMultiGraph("MultiGraph","Y Shifts");
  std::vector<TGraphErrors> eachy;
  TMultiGraph *mgyrs = new TMultiGraph("MultiGraph","Y Shifts (Errors x10)");
  std::vector<std::vector<std::vector<TMultiGraph*>>> mgyrv;
  mgyrv.resize(3);
  for (int i=0; i<mgyrv.size(); i++) mgyrv[i].resize(2);

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TMultiGraph *mgyrv_js = new TMultiGraph("MultiGraph","Y Shifts");
        TMultiGraph *mgdummy = new TMultiGraph("MultiGraph","Y Shifts");
        for (int i=0; i<runsy[plane][sensor][station].size(); i++){
          TGraphErrors* g = &runsy[plane][sensor][station][i];
          if (g->GetN() != 0){
            //g->SetMarkerColor(pal8[station]);
            //g->SetLineColor(pal8[station]);
            g->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
            g->SetMarkerSize(2);
            mgyr->Add(g);
            mgyrv_js->Add(g);

            TGraph* gd = &runsyd[plane][sensor][station][i];
            mgdummy->Add(g);
            //mgdummy->Add(gd);
          }
        }
        mgyrv[plane][sensor].push_back(mgyrv_js);
        if (mgdummy->GetListOfGraphs() != NULL){
          TF1 *f1 = new TF1("f1", "[0]", 0, 10);
          mgdummy->Fit("f1","Q");
          auto *params = f1->GetParameters();
          auto *errors = f1->GetParErrors();
          std::cout<<"y shifts "<<plane<<station<<": "<<errors[0]<<std::endl;

          auto ge = new TGraphErrors(1);
          ge->SetPoint(0,(double)station,params[0]);
          ge->SetPointError(0,0.,errors[0]);
          //ge->SetMarkerColor(pal8[station]);
          //ge->SetLineColor(pal8[station]);
          ge->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
          ge->SetMarkerSize(2);
          eachy.push_back(*ge);

	  ge->Draw("AP");
	  ge->SetTitle("Y Shifts Fit to a Constant");
          ge->GetXaxis()->SetTitle("Station");
          ge->GetYaxis()->SetTitle("Shifts [mm]");
          ge->GetXaxis()->SetLimits(-1.,8.);
          std::string planetype = "";
          if (plane == 0) planetype = "x";
          if (plane == 1) planetype = "y";
          if (plane == 2) planetype = "u";
          //c2->SaveAs(Form("yshifts_avg_%i%s%i.png",station,planetype.c_str(),sensor));
        }
      }
    }
  }
  for (int i=0; i<eachy.size(); i++){
    TGraphErrors* gc = &eachy[i];
    mgyrs->Add(gc);
  }

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TExec *ex = new TExec("ex","DrawText();");
        mgyrv[plane][sensor][station]->GetListOfFunctions()->Add(ex);
        mgyrv[plane][sensor][station]->Draw("AP");
        mgyrv[plane][sensor][station]->GetXaxis()->SetLabelSize(0);
        mgyrv[plane][sensor][station]->GetXaxis()->SetTickLength(0);
        mgyrv[plane][sensor][station]->GetXaxis()->SetTitle("Run Number");
        mgyrv[plane][sensor][station]->GetYaxis()->SetTitle("Shifts [mm]");
        mgyrv[plane][sensor][station]->GetXaxis()->SetTitleOffset(1.5);
        mgyrv[plane][sensor][station]->GetYaxis()->SetLabelSize(0.04);
        zero->Draw();
        //legend2->Draw();
        gPad->Modified();
        gPad->Update();
        std::string planetype = "";
        if (plane == 0) planetype = "x";
        if (plane == 1) planetype = "y";
        if (plane == 2) planetype = "u";
        c2->SaveAs(Form("yshifts_runs_%i%s%i.png",station,planetype.c_str(),sensor));
      }
    }
  }

  TMultiGraph *mgzr = new TMultiGraph("MultiGraph","Z Shifts");
  std::vector<TGraphErrors> eachz;
  TMultiGraph *mgzrs = new TMultiGraph("MultiGraph","Z Shifts (Errors x10)");
  std::vector<std::vector<std::vector<TMultiGraph*>>> mgzrv;
  mgzrv.resize(3);
  for (int i=0; i<mgzrv.size(); i++) mgzrv[i].resize(2);

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TMultiGraph *mgzrv_js = new TMultiGraph("MultiGraph","Z Shifts");
        TMultiGraph *mgdummy = new TMultiGraph("MultiGraph","Z Shifts");
        for (int i=0; i<runsz[plane][sensor][station].size(); i++){
          TGraphErrors* g = &runsz[plane][sensor][station][i];
          if (g->GetN() != 0){
            //g->SetMarkerColor(pal8[station]);
            //g->SetLineColor(pal8[station]);
            g->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
            g->SetMarkerSize(2);
            mgzr->Add(g);
            mgzrv_js->Add(g);

            TGraph* gd = &runszd[plane][sensor][station][i];
            mgdummy->Add(g);
            //mgdummy->Add(gd);
          }
        }
        mgzrv[plane][sensor].push_back(mgzrv_js);
        if (mgdummy->GetListOfGraphs() != NULL){
          TF1 *f1 = new TF1("f1", "[0]", 0, 10);
          mgdummy->Fit("f1","Q");
          auto *params = f1->GetParameters();
          auto *errors = f1->GetParErrors();
          std::cout<<"z shifts "<<plane<<station<<": "<<errors[0]<<std::endl;

          auto ge = new TGraphErrors(1);
          ge->SetPoint(0,(double)station,params[0]);
          ge->SetPointError(0,0.,errors[0]);
          //ge->SetMarkerColor(pal8[s]);
          //ge->SetLineColor(pal8[s]);
          ge->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
          ge->SetMarkerSize(2);
          eachz.push_back(*ge);

          ge->Draw("AP");
          ge->SetTitle("Z Shifts Fit to a Constant");
          ge->GetXaxis()->SetTitle("Station");
          ge->GetYaxis()->SetTitle("Shifts [mm]");
          ge->GetXaxis()->SetLimits(-1.,8.);
          std::string planetype = "";
          if (plane == 0) planetype = "x";
          if (plane == 1) planetype = "y";
          if (plane == 2) planetype = "u";
          //c2->SaveAs(Form("zshifts_avg_%i%s%i.png",station,planetype.c_str(),sensor));
        }
      }
    }
  }
  for (int i=0; i<eachz.size(); i++){
    TGraphErrors* gc = &eachz[i];
    mgzrs->Add(gc);
  }

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TExec *ex = new TExec("ex","DrawText();");
        mgzrv[plane][sensor][station]->GetListOfFunctions()->Add(ex);
        mgzrv[plane][sensor][station]->Draw("AP");
        mgzrv[plane][sensor][station]->GetXaxis()->SetLabelSize(0);
        mgzrv[plane][sensor][station]->GetXaxis()->SetTickLength(0);
        mgzrv[plane][sensor][station]->GetXaxis()->SetTitle("Run Number");
        mgzrv[plane][sensor][station]->GetYaxis()->SetTitle("Shifts [mm]");
        mgzrv[plane][sensor][station]->GetXaxis()->SetTitleOffset(1.5);
        mgzrv[plane][sensor][station]->GetYaxis()->SetLabelSize(0.04);
        zero->Draw();
        //legend2->Draw();
        gPad->Modified();
        gPad->Update();
        std::string planetype = "";
        if (plane == 0) planetype = "x";
        if (plane == 1) planetype = "y";
        if (plane == 2) planetype = "u";
        c2->SaveAs(Form("zshifts_runs_%i%s%i.png",station,planetype.c_str(),sensor));
      }
    }
  }

  TMultiGraph *mgphir = new TMultiGraph("MultiGraph","#phi Shifts");
  std::vector<TGraphErrors> eachphi;
  TMultiGraph *mgphirs = new TMultiGraph("MultiGraph","#phi Shifts (Errors x10)");
  std::vector<std::vector<std::vector<TMultiGraph*>>> mgphirv;
  mgphirv.resize(3);
  for (int i=0; i<mgphirv.size(); i++) mgphirv[i].resize(2);

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TMultiGraph *mgphirv_js = new TMultiGraph("MultiGraph","#phi Shifts");
        TMultiGraph *mgdummy = new TMultiGraph("MultiGraph","#phi Shifts");
        for (int i=0; i<runsphi[plane][sensor][station].size(); i++){
          TGraphErrors* g = &runsphi[plane][sensor][station][i];
          if (g->GetN() != 0){
            //g->SetMarkerColor(pal8[station]);
            //g->SetLineColor(pal8[station]);
            g->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
            g->SetMarkerSize(2);
            mgphir->Add(g);
            mgphirv_js->Add(g);

            TGraph* gd = &runsphid[plane][sensor][station][i];
            mgdummy->Add(g);
            //mgdummy->Add(gd);
          }
        }
        mgphirv[plane][sensor].push_back(mgphirv_js);
        if (mgdummy->GetListOfGraphs() != NULL){
          TF1 *f1 = new TF1("f1", "[0]", 0, 10);
          mgdummy->Fit("f1","Q");
          auto *params = f1->GetParameters();
          auto *errors = f1->GetParErrors();
  	  std::cout<<"phi shifts "<<plane<<station<<": "<<errors[0]<<std::endl;	

          auto ge = new TGraphErrors(1);
          ge->SetPoint(0,(double)station,params[0]);
          ge->SetPointError(0,0.,errors[0]);
          //ge->SetMarkerColor(pal8[station]);
          //ge->SetLineColor(pal8[station]);
          ge->SetMarkerStyle(24+plane); //x-plane is circle, y-plane is square, u-plane is triangle
          ge->SetMarkerSize(2);
          eachphi.push_back(*ge);

          ge->Draw("AP");
          ge->SetTitle("#phi Shifts Fit to a Constant");
          ge->GetXaxis()->SetTitle("Station");
          ge->GetYaxis()->SetTitle("Shifts [mrad]");
          ge->GetXaxis()->SetLimits(-1.,8.);
          std::string planetype = "";
          if (plane == 0) planetype = "x";
          if (plane == 1) planetype = "y";
          if (plane == 2) planetype = "u";
          //c2->SaveAs(Form("phishifts_avg_%i%s%i.png",station,planetype.c_str(),sensor));
        }
      }
    }
  }
  for (int i=0; i<eachphi.size(); i++){
    TGraphErrors* gc = &eachphi[i];
    mgphirs->Add(gc);
  }

  for (int plane=0; plane<3; plane++){
    for (int sensor=0; sensor<2; sensor++){
      for (int station=0; station<8; station++){
        TExec *ex = new TExec("ex","DrawText();");
        mgphirv[plane][sensor][station]->GetListOfFunctions()->Add(ex);
        mgphirv[plane][sensor][station]->Draw("AP");
        mgphirv[plane][sensor][station]->GetXaxis()->SetLabelSize(0);
        mgphirv[plane][sensor][station]->GetXaxis()->SetTickLength(0);
        mgphirv[plane][sensor][station]->GetXaxis()->SetTitle("Run Number");
        mgphirv[plane][sensor][station]->GetYaxis()->SetTitle("Shifts [mrad]");
        mgphirv[plane][sensor][station]->GetXaxis()->SetTitleOffset(1.5);
        mgphirv[plane][sensor][station]->GetYaxis()->SetLabelSize(0.04);
        zero->Draw();
        //legend2->Draw();
        gPad->Modified();
        gPad->Update();
        std::string planetype = "";
        if (plane == 0) planetype = "x";
        if (plane == 1) planetype = "y";
        if (plane == 2) planetype = "u";
        c2->SaveAs(Form("phishifts_runs_%i%s%i.png",station,planetype.c_str(),sensor));
      }
    }
  }

}
float getParam(std::vector<int> id, const char* entry2){
  ifstream f;
  f.open(entry2);

  std::string line;

  float finalparam = 999.;

  while (f.good())
  {
    std::getline(f, line); //skip first line i.e. the header
    while (getline(f, line)){
      std::stringstream ss(line);
      std::string element;
      std::vector<std::string> row;
      std::vector<string> n;

      while (std::getline(ss, element, ' ')) {
        if (!element.empty()) row.push_back(element);
      }
      if (row.size() > 0){
        for (const auto &elem : row) {
          n.push_back(elem);
        }
        if ( std::stoi(n[0]) == id[0] && std::stoi(n[1]) == id[1] && std::stoi(n[2]) == id[2] ){
	 if (id[3] == 1) finalparam = std::stof(n[3]);
         if (id[3] == 2) finalparam = std::stof(n[4]);
         if (id[3] == 3) finalparam = std::stof(n[5]);
         if (id[3] == 4) finalparam = std::stof(n[6]);
        }
      }
    }    
  }
  f.close();
  return finalparam;
}

void readMe(const char* entry, const char* entry2){
  ifstream f;
  f.open(entry);

  std::string line;

  vector<vector<vector<Double_t>>> xl, yl, zl, phil;
  vector<vector<vector<Double_t>>> xlz, ylz, zlz, philz;

  vector<vector<vector<Double_t>>> pxl, pyl, pzl, pphil;

  vector<vector<vector<int>>> sx,sy,sz,sphi;

  // 0 = x, 1 = y, 2 = u

  xl.resize(3);
  yl.resize(3);
  zl.resize(3);
  phil.resize(3);
  for (auto i=0; i<xl.size(); i++) xl[i].resize(2);
  for (auto i=0; i<yl.size(); i++) yl[i].resize(2);
  for (auto i=0; i<zl.size(); i++) zl[i].resize(2);
  for (auto i=0; i<phil.size(); i++) phil[i].resize(2);

  xlz.resize(3);
  ylz.resize(3);
  zlz.resize(3);
  philz.resize(3);
  for (auto i=0; i<xlz.size(); i++) xlz[i].resize(2);
  for (auto i=0; i<ylz.size(); i++) ylz[i].resize(2);
  for (auto i=0; i<zlz.size(); i++) zlz[i].resize(2);
  for (auto i=0; i<philz.size(); i++) philz[i].resize(2);

  pxl.resize(3);
  pyl.resize(3);
  pzl.resize(3);
  pphil.resize(3);
  for (auto i=0; i<pxl.size(); i++) pxl[i].resize(2);
  for (auto i=0; i<pyl.size(); i++) pyl[i].resize(2);
  for (auto i=0; i<pzl.size(); i++) pzl[i].resize(2);
  for (auto i=0; i<pphil.size(); i++) pphil[i].resize(2);

  sx.resize(3);
  sy.resize(3);
  sz.resize(3);
  sphi.resize(3);

  for (auto i=0; i<sx.size(); i++)  sx[i].resize(2);
  for (auto i=0; i<sy.size(); i++)  sy[i].resize(2);
  for (auto i=0; i<sz.size(); i++)  sz[i].resize(2);
  for (auto i=0; i<sphi.size(); i++)  sphi[i].resize(2);

  runsx.resize(3);
  runsy.resize(3);
  runsz.resize(3);
  runsphi.resize(3);

  for (auto i=0; i<runsx.size(); i++) {
    runsx[i].resize(2);
    for (auto j=0; j<runsx[i].size(); j++){
      runsx[i][j].resize(8);
    }
  }
  for (auto i=0; i<runsy.size(); i++) {
    runsy[i].resize(2);
    for (auto j=0; j<runsy[i].size(); j++){
      runsy[i][j].resize(8);
    }
  }
  for (auto i=0; i<runsz.size(); i++) {
    runsz[i].resize(2);
    for (auto j=0; j<runsz[i].size(); j++){
      runsz[i][j].resize(8);
    }
  }
  for (auto i=0; i<runsphi.size(); i++) {
    runsphi[i].resize(2);
    for (auto j=0; j<runsphi[i].size(); j++){
      runsphi[i][j].resize(8);
    }
  }

  runsxd.resize(3);
  runsyd.resize(3);
  runszd.resize(3);
  runsphid.resize(3);

  for (auto i=0; i<runsxd.size(); i++) {
    runsxd[i].resize(2);
    for (auto j=0; j<runsxd[i].size(); j++){
      runsxd[i][j].resize(8);
    }
  }
  for (auto i=0; i<runsyd.size(); i++) {
    runsyd[i].resize(2);
    for (auto j=0; j<runsyd[i].size(); j++){
      runsyd[i][j].resize(8);
    }
  }
  for (auto i=0; i<runszd.size(); i++) {
    runszd[i].resize(2);
    for (auto j=0; j<runszd[i].size(); j++){
      runszd[i][j].resize(8);
    }
  }
  for (auto i=0; i<runsphid.size(); i++) {
    runsphid[i].resize(2);
    for (auto j=0; j<runsphid[i].size(); j++){
      runsphid[i][j].resize(8);
    }
  }

  int ln = 0;
  int lc = 0;
  int nz = 0;
  int inc = 0;
  while (f.good())
  {
    while (getline(f, line)){
        std::stringstream ss(line);
        std::string element;
        std::vector<std::string> row;
        std::vector<string> n;

        while (std::getline(ss, element, ' ')) {
          if (!element.empty()) row.push_back(element);
        }
        if (row.size() == 6){
          for (const auto &elem : row) {
            n.push_back(elem);
          }
          int s = findStation(std::stoi(n[0]));
	  std::vector<int> id;
          if (n[0].size() == 4){
	    for (char c : n[0]) {
              int cint = c - '0' ;
	      id.push_back(cint);
            }
	  }
	  else{
	    id.push_back(0);
	    for (char c : n[0]) {
              int cint = c - '0' ;
              id.push_back(cint);
            }
          }
	  if (n[0].back() == '4'){
	    float p4 = getParam(id,entry2);
 	    p4 *= 1000*TMath::Pi()/180.; //degrees to mrad 
	    makeMarkers(n,phil,pphil,philz,zpos[inc-1],sphi,p4);
	  }
          if (n[0].back() == '3'){
            float p3 = getParam(id,entry2);
            makeMarkers(n,zl,pzl,zlz,zpos[inc-1],sz,p3);
          }
          if (n[0].back() == '2'){
            float p2 = getParam(id,entry2);
            makeMarkers(n,yl,pyl,ylz,zpos[inc-1],sy,p2);
          }
	  if (n[0].back() == '1'){
            float p1 = getParam(id,entry2);
            makeMarkers(n,xl,pxl,xlz,zpos[inc-1],sx,p1);
	  }
        if (lc % 4 == 0) inc++;
        lc++;
        ln++;
      }
    }
    f.close();

    for (int i=0; i<3; i++){
      for (int m=0; m<2; m++){
        for (Int_t k = 0; k < pxl[i][m].size(); k++){
          auto rx = new TGraphErrors(1);
          rx->SetPoint(0, ni, pxl[i][m][k]); // Set The point itself
          rx->SetPointError(0, 0., xl[i][m][k]);
          runsx[i][m][sx[i][m][k]].push_back(*rx);

          auto rxd = new TGraph(1);
          rxd->SetPoint(0, ni, pxl[i][m][k]); // Set The point itself
          runsxd[i][m][sx[i][m][k]].push_back(*rxd);
        }
      }
    }

    for (int i=0; i<3; i++){
      for (int m=0; m<2; m++){
        for (Int_t k = 0; k < pyl[i][m].size(); k++){
          auto ry = new TGraphErrors(1);
          ry->SetPoint(0, ni, pyl[i][m][k]); // Set The point itself
          ry->SetPointError(0, 0., yl[i][m][k]);
          runsy[i][m][sy[i][m][k]].push_back(*ry);

          auto ryd = new TGraph(1);
          ryd->SetPoint(0, ni, pyl[i][m][k]); // Set The point itself
          runsyd[i][m][sy[i][m][k]].push_back(*ryd);
        }
      }
    }
  
    for (int i=0; i<3; i++){
      for (int m=0; m<2; m++){
        for (Int_t k = 0; k < pzl[i][m].size(); k++){
          auto rz = new TGraphErrors(1);
          rz->SetPoint(0, ni, pzl[i][m][k]); // Set The point itself
          rz->SetPointError(0, 0., zl[i][m][k]);
          runsz[i][m][sz[i][m][k]].push_back(*rz);

          auto rzd = new TGraph(1);
          rzd->SetPoint(0, ni, pzl[i][m][k]); // Set The point itself
          runszd[i][m][sz[i][m][k]].push_back(*rzd);
        }
      }
    }

    for (int i=0; i<3; i++){
      for (int m=0; m<2; m++){
        for (Int_t k = 0; k < pphil[i][m].size(); k++){
          auto rphi = new TGraphErrors(1);
          rphi->SetPoint(0, ni, pphil[i][m][k]); // Set The point itself
          rphi->SetPointError(0, 0., phil[i][m][k]*1000);
          runsphi[i][m][sphi[i][m][k]].push_back(*rphi);

          auto rphid = new TGraph(1);
          rphid->SetPoint(0, ni, pphil[i][m][k]); // Set The point itself      
          runsphid[i][m][sphi[i][m][k]].push_back(*rphid);
        }
      }
    }
  }
}
int findStation(int n){
  int station = 10;
  if (n < 1000) station = 0;
  else if (n < 2000) station = 1;
  else if (n < 3000) station = 2;
  else if (n < 4000) station = 3;
  else if (n < 5000) station = 4;
  else if (n < 6000) station = 5;
  else if (n < 7000) station = 6;
  else if (n < 8000) station = 7;
  else station = 10;
  return station;
}
void makeMarkers(std::vector<string> n0, vector<vector<vector<Double_t>>> &t, vector<vector<vector<Double_t>>> &p, vector<vector<vector<Double_t>>> &tz, Double_t z, vector<vector<vector<int>>> &s, float pssd){
  for (int i=0; i<pair_1c.size(); i++){
    int sensor = 0;
    if (n0[0].length() == 4){
      if (n0[0].find(pair_1c[i].first) != std::string::npos){
        sensor = n0[0][2] - '0';
        if (pair_1c[i].second == "x"){
          t[0][sensor].push_back(std::stof(n0[4]));
          p[0][sensor].push_back(pssd);
	  tz[0][sensor].push_back(z);
          s[0][sensor].push_back(findStation(std::stoi(n0[0])));
        }
        if (pair_1c[i].second == "y"){
          t[1][sensor].push_back(std::stof(n0[4]));
          p[1][sensor].push_back(pssd);
          tz[1][sensor].push_back(z);
          s[1][sensor].push_back(findStation(std::stoi(n0[0])));
        }
        if (pair_1c[i].second == "u"){
	  t[2][sensor].push_back(std::stof(n0[4]));
          p[2][sensor].push_back(pssd);
          tz[2][sensor].push_back(z);
          s[2][sensor].push_back(findStation(std::stoi(n0[0])));
        }
      }
    }
    if (n0[0].length() == 3){
      sensor = n0[0][1] - '0';
      t[0][sensor].push_back(std::stof(n0[4]));
      p[0][sensor].push_back(pssd);
      tz[0][sensor].push_back(z);
      s[0][sensor].push_back(findStation(std::stoi(n0[0])));
      break;
    }
    if (n0[0].length() == 1){
      sensor = 0;	
      t[1][sensor].push_back(std::stof(n0[4]));
      p[1][sensor].push_back(pssd);
      tz[1][sensor].push_back(z);
      s[1][sensor].push_back(findStation(std::stoi(n0[0]))); 
      break;
    }
  }
}
void DrawText()
{
   Int_t i,n;
   Double_t x,y;
   TLatex *t;
   TLine *l;

   TMultiGraph *mg = (TMultiGraph*)gPad->GetListOfPrimitives()->FindObject("MultiGraph");
   TList* grlist;
   grlist = mg->GetListOfGraphs();
   TIter next(grlist);
   TObject *obj;
   TGraph *g;

   double ymin = mg->GetHistogram()->GetMinimum();
   double ymax= mg->GetHistogram()->GetMaximum();
   double dy = (ymax-ymin);

   while ((obj = next())) {
     g=(TGraph*)obj;
        
     n = g->GetN();
     for (i=0; i<n; i++) {
       g->GetPoint(i,x,y);
       t = new TLatex(x, ymin-0.045*dy, names[(int)x].c_str());
       //t->SetTextSize(0.025);  //(0.035);
       t->SetTextSize(0.04);
       t->SetTextFont(42);
       t->SetTextAlign(21);
       //t->SetTextAngle(60);
       t->Paint();
     }
   }
}
int main(int argc, const char* argv[]){
  readAndDrawTrk3(argv[1], argv[2]);
  return 0;
}
