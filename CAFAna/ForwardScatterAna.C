#include "StandardRecord/StandardRecord.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"

#include "CAFAna/Selection.h"
#include "CAFAna/SignalDef.h"

enum kHist1DType {
  kNSelect,
  kNSignal,
  kNSignalSelect,
  kPurity,
  kNHist1DTypes  
};

std::vector<std::string> gHistoName = {"NSelect","Efficiency","Purity"};
std::vector<std::string> gHistoTitle = {"NSelect","Efficiency","Purity"};

void MakeAnaHistos(std::vector<TH1F> & hv)
{
  for (int i=0; i<kNHist1DTypes; ++i) {
    TH1F h1(gHistoName[i].c_str(),gHistoTitle[i].c_str(),gNbins[i],gNXMin[i],gNXMax[i]);
    hv.push_back(h1);
}

void Ana120Gev()
{
  // Define input caf file here
  TFile *file = new TFile("emphdata_v03.caf.root", "read");

  // Get the event record tree
  TTree *tree = (TTree*)file->Get("recTree");

  // We must create a StandardRecord object and set the branch address
  // using our tree.
  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  // Create all of the histograms we will ever need, ha ha ha ha 
  std::vector <TH1F> HistoVec;
  MakeAnaHistos(HistoVec);

  // Loop over our tree and each time we call GetEntries, the data
  // in the tree is copied to rec.
bool isMC = false;

for(int i = 0; i < tree->GetEntries(); ++i)
  {
      tree->GetEntry(i);

      isMC = rec->IsMC;

      // fill NSignal history 
      if (isMC && IsSignal(rec->truth))
          HistoVec[kNSignal].Fill(TrueTheta2(rec->truth) );

      if (IsGoodBeammTrack(rec->beamtrk) &&
          HasGoodChi2(rec->trk[0]) && (rec->trk.ntrks > 1) && HasOkMomentum(rec->trk[0]) &&
          HasOkARICHPID(rec->trk[0]) && HasOkVertex(rec->trk[0].vtx) ) 

        HistoVec[kNSelect].Fill(rec->trk[0].theta2); 
        if (isMC) {
          if (IsSignal(rec->truth) ) {
            HistoVec[kNPurity].Fill(rec->trk[0].theta2);
            HistoVec[kNSignalSelect].Fill(TrueTheta2(rec->truth));        
        }
      }

  } // end loop over entries

  if (isMC) {
    HistoVec[kNPurity].Divide(HistoVec[kNSelect]);
    HistoVec[kNSignalSelect].Divide(kNSignal);
  }

  // now write all histograms out to a Root file so we can make pretty plots
