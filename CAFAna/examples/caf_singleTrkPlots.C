#include "StandardRecord/StandardRecord.h"
#include "StandardRecord/SRVector3D.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"

void angDiff(std::vector<caf::SRTrueSSDHits> truthv, caf::SRVector3D& p, double ang[2]);
void findDistFixedZ(std::vector<caf::SRTrueSSDHits> truthv, caf::SRTrackSegment& seg);
void getPosFromSeg(caf::SRTrackSegment& seg,double t);
TH1F* hAngDiffx[8];
TH1F* hAngDiffy[8];
TH1F* hFixedZ_DX[8][3];
TH1F* hFixedZ_DY[8][3];
TH2F* hRecoMinusTrueVsTrueX[8][3];
TH2F* hRecoMinusTrueVsTrueY[8][3];

void caf_singleTrkPlots(std::string fname)
{
  // Define input caf file here
  TFile *file = new TFile(fname.c_str(), "read");

  // Get the event record tree
  TTree *tree = (TTree*)file->Get("recTree");

  // We must create a StandardRecord object and set the branch address
  // using our tree.
  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  // Initalize a simple histogram
  TH1I* hNTrkSeg = new TH1I("nTrkSeg", "", 20,0,20);
  TH1D* hScattAngleRes = new TH1D("hScattAngleRes","Scattering Angle Resolution;#theta_{reco}-#theta_{true} ", 100,-0.005,0.005);
  TH1D* hBendAngleRes = new TH1D("hBendAngleRes","Bending Angle Resolution;#theta_{reco}-#theta_{true}", 100,-0.0025,0.0025);
  TH2F* hSSDHit[8][3];

  TH1F* hRes_x[8];
  TH1F* hRes_y[8];;
  char *hresx = new char[8];
  char *hresy = new char[8];
  for (int i=0; i<8; i++){
    sprintf(hresx,"hRes_x_%d",i);
    hRes_x[i] = new TH1F(hresx,hresx,100,-0.1,0.1);
    sprintf(hresy,"hRes_y_%d",i);
    hRes_y[i] = new TH1F(hresy,hresy,100,-0.1,0.1);
  }
  char *htruth = new char[12];
  for (int i=0; i<8; i++){
    for (int j=0; j<3; j++){
      sprintf(htruth,"hSSDHit_%d_%d",i,j);
      hSSDHit[i][j] = new TH2F(htruth,htruth,1000,-50.,50.,1000,-50.,50.);
    }
  }
  TH1F* hPulls[8];
  char *hpulls = new char[9];
  for (int i=0; i<8; i++){
    sprintf(hpulls,"hPulls_%d",i);
    hPulls[i] = new TH1F(hpulls,hpulls,100,0.,0.2);
  }
  TH1F* hChi2_seg1 = new TH1F("hChi2_seg1","hChi2_seg1",100,0.,30.);
  TH1F* hChi2_seg2 = new TH1F("hChi2_seg2","hChi2_seg2",100,0.,100.);
  TH1F* hChi2_seg3 = new TH1F("hChi2_seg3","hChi2_seg3",100,0.,1000.);

  TH1F* hAngDiffx_seg1 = new TH1F("hAngDiffx_seg1","hAngDiffx_seg1",100,-0.1,0.1);
  TH1F* hAngDiffx_seg2 = new TH1F("hAngDiffx_seg2","hAngDiffx_seg2",100,-0.1,0.1);
  TH1F* hAngDiffx_seg3 = new TH1F("hAngDiffx_seg3","hAngDiffx_seg3",100,-0.1,0.1);
  TH1F* hAngDiffy_seg1 = new TH1F("hAngDiffy_seg1","hAngDiffy_seg1",100,-0.1,0.1);
  TH1F* hAngDiffy_seg2 = new TH1F("hAngDiffy_seg2","hAngDiffy_seg2",100,-0.1,0.1);
  TH1F* hAngDiffy_seg3 = new TH1F("hAngDiffy_seg3","hAngDiffy_seg3",100,-0.1,0.1);

  char *hangx = new char[11];
  char *hangy = new char[11];
  for (int i=0; i<8; i++){ 
    sprintf(hangx,"hAngDiffx_%d",i);
    sprintf(hangy,"hAngDiffy_%d",i);
    hAngDiffx[i] = new TH1F(hangx,hangx,100,-0.005,0.005);
    hAngDiffy[i] = new TH1F(hangy,hangy,100,-0.005,0.005);
  }  
  char *hdx = new char[13];
  char *hdy = new char[13];
  for (int i=0; i<8; i++){
    for (int j=0; j<3; j++){
      sprintf(hdx,"hFixedZ_DX_%d_%d",i,j);
      sprintf(hdy,"hFixedZ_DY_%d_%d",i,j);
      hFixedZ_DX[i][j] = new TH1F(hdx,hdx,200,-0.3,0.3);
      hFixedZ_DY[i][j] = new TH1F(hdy,hdy,200,-0.3,0.3);
     }
  }
  char *h2dx = new char[25];
  char *h2dy = new char[25];
  for (int i=0; i<8; i++){
    for (int j=0; j<3; j++){
      sprintf(h2dx,"hRecoMinusTrueVsTrueX_%d_%d",i,j);
      sprintf(h2dy,"hRecoMinusTrueVsTrueY_%d_%d",i,j);
      hRecoMinusTrueVsTrueX[i][j] = new TH2F(h2dx,h2dx,40,-20,20,200,-0.3,0.3);
      hRecoMinusTrueVsTrueY[i][j] = new TH2F(h2dy,h2dy,40,-20,20,200,-0.3,0.3);
    }
  }
  TH2F* hBendingXYat4 = new TH2F("hBendingXYat4","hBendingXYat4",40,-20,20,40,-20,20); 
  
  TH1F* hBending = new TH1F("hBending","hBending",200,0.,0.05);
  TH1F* hScattering = new TH1F("hScattering","hScattering",200,0.,0.03);

  TGraph* gRecoHits_xz[100];
  TGraph* gRecoHits_yz[100];
  TGraph* gSimHits_xz[100];
  TGraph* gSimHits_yz[100];
  TMultiGraph* gHits_xz[100];
  TMultiGraph* gHits_yz[100];
  std::vector<TMultiGraph*> gHits_xz_pass;
  std::vector<TMultiGraph*> gHits_yz_pass;

  TGraph* gTruthHits_xz[100]; TGraph* gTruthHits_yz[100];
  TGraph* gLine_xz1[100]; TGraph* gLine_yz1[100];
  TGraph* gLine_xz2[100]; TGraph* gLine_yz2[100];
  TGraph* gLine_xz3[100]; TGraph* gLine_yz3[100];
  TMultiGraph* gLines_xz[100];
  TMultiGraph* gLines_yz[100];
  std::vector<TMultiGraph*> gLines_xz_pass;
  std::vector<TMultiGraph*> gLines_yz_pass;

  //simulated hits (avg position)
  double xsim[8] = {999,999,999,999,999,999,999,999};
  double ysim[8] = {999,999,999,999,999,999,999,999};
  double zsim[8] = {999,999,999,999,999,999,999,999};

  //reconstructed hits
  double xreco[8] = {999,999,999,999,999,999,999,999};
  double yreco[8] = {999,999,999,999,999,999,999,999};
  double zreco[8] = {999,999,999,999,999,999,999,999};

  //truth info for lines
  std::vector<caf::SRTrueSSDHits> truthhit_track1;
  std::vector<caf::SRTrueSSDHits> truthhit_track2;
  std::vector<caf::SRTrueSSDHits> truthhit_track3;
  std::vector<std::vector<caf::SRTrueSSDHits>> alltruthtracks;
  alltruthtracks.push_back(truthhit_track1);
  alltruthtracks.push_back(truthhit_track2);
  alltruthtracks.push_back(truthhit_track3);

  double recoAngle, trueAngle, angleDiff;
  double xres, yres;
  double targetpos = 380.5;
  double magnetusz = 903.47;
  double magnetdsz = 1071.82;
  caf::SRVector3D truep0(0,0,0);
  caf::SRVector3D truep1(0,0,0);
  caf::SRVector3D truep2(0,0,0);

  std::vector<double> xtruth;
  std::vector<double> ytruth;
  std::vector<double> ztruth;

  TH2F* hBendVsTrueRAt4 = new TH2F("hBendVsTrueRAt4","hBendVsTrueRAt4",60,0,30,200,0.,0.05);
  TH2F* hBendVsTrueRAt5 = new TH2F("hBendVsTrueRAt5","hBendVsTrueRAt5",60,0,30,200,0.,0.05);

  TH2F* hBendVsRecoRAt4 = new TH2F("hBendVsRecoRAt4","hBendVsRecoRAt4",60,0,30,200,0.,0.05);
  TH2F* hBendVsRecoRAt5 = new TH2F("hBendVsRecoRAt5","hBendVsRecoRAt5",60,0,30,200,0.,0.05);

  TH2F* hBendVsTrueOmegaAt4 = new TH2F("hBendVsTrueOmegaAt4","hBendVsTrueOmegaAt4",100,0,0.01,200,0.03,0.04);
  TH2F* hBendVsTrueOmegaAt5 = new TH2F("hBendVsTrueOmegaAt5","hBendVsTrueOmegaAt5",200,0.025,0.045,200,0.03,0.04);

  TH2F* hBendVsRecoOmegaAt4 = new TH2F("hBendVsRecoOmegaAt4","hBendVsRecoOmegaAt4",100,0,0.01,200,0.03,0.04);
  TH2F* hBendVsRecoOmegaAt5 = new TH2F("hBendVsRecoOmegaAt5","hBendVsRecoOmegaAt5",200,0.025,0.045,200,0.03,0.04); 

  int ncl = 0;

  // Loop over our tree and each time we call GetEntries, the data
  // in the tree is copied to rec.
  for(int i = 0; i < tree->GetEntries(); ++i)
  {
      tree->GetEntry(i);
      // Spit out Run number for the first entry only
      if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;
            
      // Find how many track segments there are
      int ntrkseg = int(rec->sgmnts.seg.size());
      hNTrkSeg->Fill(ntrkseg);

      int nspcpts = int(rec->spcpts.sp.size());
      int nclusters = int(rec->cluster.clust.size());

      //criteria
      bool goodCluster = false;
      bool goodEvent = false;
      bool goodHit = false;
      if (nspcpts > 0) {goodCluster = true; ncl++;}
      if (goodCluster && rec->truth.truehits.truehits.size() == 20) goodEvent = true;
      if (goodCluster){
        for (size_t i=0; i<nspcpts; i++){
          caf::SRSpacePoint& s = rec->spcpts.sp[i];
          xreco[s.station] = s.x[0];
          yreco[s.station] = s.x[1];
          zreco[s.station] = s.x[2];
        }
      }
      bool good4 = false;
      bool good5 = false;
      if (goodEvent){
        for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
          caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
          hSSDHit[h.GetStation][h.GetPlane]->Fill(h.GetX,h.GetY);

          if (h.GetZ < targetpos) truthhit_track1.push_back(h);
          if (h.GetZ > targetpos && h.GetZ < magnetusz) truthhit_track2.push_back(h);
          if (h.GetZ > magnetdsz) truthhit_track3.push_back(h);

          if (h.GetStation == 4 && h.GetPlane == 1){
            if (abs(h.GetX) < 15 && abs(h.GetY) < 15) good4 = true;
          }
	  if (h.GetStation == 5 && h.GetPlane == 0){
            if (abs(h.GetX) < 15 && abs(h.GetY) < 15) good5 = true;
          }
	  if (good4 && good5) goodHit = true;
          //goodHit = true;

	  //xtruth.push_back(h.GetX);
	  //ytruth.push_back(h.GetY);
	  //ztruth.push_back(h.GetZ);
        }
        for (size_t k=0; k<8; k++){
          double xsum = 0.; double ysum = 0.; double zsum = 0.;
          int xc=0; int yc=0; int zc=0;

          for (size_t l=0; l<rec->truth.truehits.truehits.size(); l++){
            caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[l];
            if ((int)k == h.GetStation) {
              xsum += h.GetX;
              ysum += h.GetY;
              zsum += h.GetZ;
              xc++; yc++; zc++;
            }
          }
          xsim[k] = xsum/(double)xc;
          ysim[k] = ysum/(double)yc;
          zsim[k] = zsum/(double)zc;

        }
        if (goodHit){
          for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
            caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
            xtruth.push_back(h.GetX);
            ytruth.push_back(h.GetY);
            ztruth.push_back(h.GetZ);
          }
        // only look at events with three track segments
        //if (ntrkseg == 3) {
        // first fill scattering angle resolution plot
	caf::SRVector3D& p0 = rec->sgmnts.seg[0].mom;
	caf::SRVector3D& p1 = rec->sgmnts.seg[1].mom;
	recoAngle = TMath::ACos(p0.Dot(p1)/(p0.Mag()*p1.Mag()));
	hScattering->Fill(recoAngle);

	int nGot=0;
	for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
	  caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	  if (h.GetStation == 1 && h.GetPlane == 1) {
	    truep0.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (h.GetStation == 2 && h.GetPlane == 0) {
	    truep1.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (nGot == 2) break;
	}

	if (nGot == 2) {
	  trueAngle = TMath::ACos(truep0.Dot(truep1)/(truep0.Mag()*truep1.Mag()));
	  angleDiff = recoAngle - trueAngle;
	  hScattAngleRes->Fill(angleDiff);
	}

	// now fill bending angle resolution plot
	caf::SRVector3D& p2 = rec->sgmnts.seg[2].mom;
	recoAngle = TMath::ACos(p1.Dot(p2)/(p1.Mag()*p2.Mag()));
        hBending->Fill(recoAngle);

	nGot=0;
	for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
	  caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	  if (h.GetStation == 3 && h.GetPlane == 2) {
	    truep1.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (h.GetStation == 6 && h.GetPlane == 0) {
	    truep2.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (nGot == 2) break;
	}
	if (nGot == 2) {
	  trueAngle = TMath::ACos(truep1.Dot(truep2)/(truep1.Mag()*truep2.Mag()));
	  angleDiff = recoAngle - trueAngle;
	  hBendAngleRes->Fill(angleDiff);
	}
	for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
          caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	  //double rho = sqrt(h.GetX*h.GetX + h.GetY*h.GetY);
          if (h.GetStation == 4 && h.GetPlane == 1){
	    hBendingXYat4->Fill(h.GetX,h.GetY,trueAngle);
	    double r = sqrt(h.GetX*h.GetX + h.GetY*h.GetY);
	    //double omega = TMath::ATan2(sqrt(h.GetPx*h.GetPx+h.GetPy*h.GetPy),h.GetPz);
	    double omega = sqrt(h.GetPx*h.GetPx+h.GetPy*h.GetPy)/h.GetPz;

	    hBendVsTrueRAt4->Fill(r,trueAngle);
            hBendVsTrueOmegaAt4->Fill(omega,trueAngle);
	  }
          if (h.GetStation == 5 && h.GetPlane == 0){
            double r = sqrt(h.GetX*h.GetX + h.GetY*h.GetY);
            double omega = sqrt(h.GetPx*h.GetPx+h.GetPy*h.GetPy)/h.GetPz;

            hBendVsTrueRAt5->Fill(r,trueAngle);
            hBendVsTrueOmegaAt5->Fill(omega,trueAngle);
	  }
          //if (h.GetZ == magnetdsz) hBendVsTrueRhoDownstream->Fill(trueAngle,rho);
	}
	// bendvsreco stuff
	// get track xyz at 4 and 5
	double t4 = (zreco[4] - rec->sgmnts.seg[1].vtx[2])/rec->sgmnts.seg[1].mom.z;
        double x4 = rec->sgmnts.seg[1].vtx[0]+t4*rec->sgmnts.seg[1].mom.x;
        double y4 = rec->sgmnts.seg[1].vtx[1]+t4*rec->sgmnts.seg[1].mom.y;
	double r4 = sqrt(x4*x4 + y4*y4);
	double omega4 = sqrt(rec->sgmnts.seg[1].mom.x*rec->sgmnts.seg[1].mom.x+rec->sgmnts.seg[1].mom.y*rec->sgmnts.seg[1].mom.y)/rec->sgmnts.seg[1].mom.z;
	hBendVsRecoRAt4->Fill(r4,recoAngle);
	hBendVsRecoOmegaAt4->Fill(omega4,recoAngle);

	double t5 = (zreco[5] - rec->sgmnts.seg[2].vtx[2])/rec->sgmnts.seg[2].mom.z;
        double x5 = rec->sgmnts.seg[2].vtx[0]+t5*rec->sgmnts.seg[2].mom.x;
        double y5 = rec->sgmnts.seg[2].vtx[1]+t5*rec->sgmnts.seg[2].mom.y;
        double r5 = sqrt(x5*x5 + y5*y5);
        double omega5 = sqrt(rec->sgmnts.seg[2].mom.x*rec->sgmnts.seg[2].mom.x+rec->sgmnts.seg[2].mom.y*rec->sgmnts.seg[2].mom.y)/rec->sgmnts.seg[2].mom.z;
        hBendVsRecoRAt5->Fill(r5,recoAngle);
        hBendVsRecoOmegaAt5->Fill(omega5,recoAngle);

	//double r_at4 = sqrt( xreco[4]*xreco[4] + yreco[4]*yreco[4] );
	//double r_at5 = sqrt( xreco[5]*xreco[5] + yreco[5]*yreco[5] );
	//hBendVsRecoRAt4->Fill(r_at4,recoAngle);
	//hBendVsRecoRAt5->Fill(r_at5,recoAngle);

	double err = 0.06/std::sqrt(12.);
	double chi2 = 0;
        // findDist
        for (auto i : truthhit_track1) {
          caf::SRVector3D m = rec->sgmnts.seg[0].mom;
	  caf::SRVector3D p(i.GetX,i.GetY,i.GetZ); 
          caf::SRVector3D ap;
          for (int j=0; j<3; j++){
            double a = rec->sgmnts.seg[0].vtx[j];
            if (j==0) ap.SetX(-1*(p.X()-a));
            if (j==1) ap.SetY(-1*(p.Y()-a));
	    if (j==2) ap.SetZ(-1*(p.Z()-a));
          }
	  caf::SRVector3D vtemp = ap.Cross(m);
	  double d = vtemp.Mag() / m.Mag();
	  hPulls[i.GetStation]->Fill(d);
          chi2 += std::pow((d/err),2);
	}
	hChi2_seg1->Fill(chi2);
	chi2 = 0;
	for (auto i : truthhit_track2) {
          caf::SRVector3D m = rec->sgmnts.seg[1].mom;
          caf::SRVector3D p(i.GetX,i.GetY,i.GetZ);
          caf::SRVector3D ap;
          for (int j=0; j<3; j++){
            double a = rec->sgmnts.seg[1].vtx[j];
            if (j==0) ap.SetX(-1*(p.X()-a));
            if (j==1) ap.SetY(-1*(p.Y()-a));
            if (j==2) ap.SetZ(-1*(p.Z()-a));
          }
          caf::SRVector3D vtemp = ap.Cross(m);
          double d = vtemp.Mag() / m.Mag();
          hPulls[i.GetStation]->Fill(d);
          chi2 += std::pow((d/err),2);
	}
        hChi2_seg2->Fill(chi2);
        chi2 = 0;
	for (auto i : truthhit_track3) {
          caf::SRVector3D m = rec->sgmnts.seg[2].mom;
          caf::SRVector3D p(i.GetX,i.GetY,i.GetZ);
          caf::SRVector3D ap;
          for (int j=0; j<3; j++){
            double a = rec->sgmnts.seg[2].vtx[j];
            if (j==0) ap.SetX(p.X()-a);
            if (j==1) ap.SetY(p.Y()-a);
            if (j==2) ap.SetZ(p.Z()-a);
          }
          caf::SRVector3D vtemp = ap.Cross(m);
          double d = vtemp.Mag() / m.Mag();
          hPulls[i.GetStation]->Fill(d);
          chi2 += std::pow((d/err),2);
        }
        hChi2_seg3->Fill(chi2);
        chi2 = 0;

	// angular difference
        double ad1[2]; double ad2[2]; double ad3[2];
	angDiff(truthhit_track1,p0,ad1);
        hAngDiffx_seg1->Fill(ad1[0]); hAngDiffy_seg1->Fill(ad1[1]);
	angDiff(truthhit_track2,p1,ad2);
        hAngDiffx_seg2->Fill(ad2[0]); hAngDiffy_seg2->Fill(ad2[1]);
        angDiff(truthhit_track3,p2,ad3);
        hAngDiffx_seg3->Fill(ad3[0]); hAngDiffy_seg3->Fill(ad3[1]);

        // findDistFixedZ
        findDistFixedZ(truthhit_track1,rec->sgmnts.seg[0]);
        findDistFixedZ(truthhit_track2,rec->sgmnts.seg[1]);
        findDistFixedZ(truthhit_track3,rec->sgmnts.seg[2]);
      } //goodHit
    } //goodEvent
    if (goodCluster && goodEvent){
      // clear reco and sim vectors for next entry
      for (size_t i=0; i<8; i++){
        if (xsim[i] == 999) std::cout<<"A hit is missing in station "<<i<<std::endl;
        if (ysim[i] == 999) std::cout<<"A hit is missing in station "<<i<<std::endl;
        if (zsim[i] == 999) std::cout<<"A hit is missing in station "<<i<<std::endl;
      }
      for (size_t i=0; i<8; i++){
        xres = xreco[i] - xsim[i];
        yres = yreco[i] - ysim[i];
        hRes_x[i]->Fill(xres);
        hRes_y[i]->Fill(yres);
      }
    }
    if (goodEvent && goodHit){
      // draw hits
      if (i < 100){
        char *Ghevt = new char[16];
        char *Gxzsim = new char[12];
        char *Gyzsim = new char[12];
        if ( std::find(std::begin(xreco), std::end(xreco), 999) == std::end(xreco)){
          sprintf(Ghevt,"gRecoHits_xz_e%d",i);
          sprintf(Gxzsim,"gHits_xz_e%d",i);

          gHits_xz[i] = new TMultiGraph(Gxzsim,Gxzsim);
	  gSimHits_xz[i] = new TGraph(8,&zsim[0],&xsim[0]);
	  gRecoHits_xz[i] = new TGraph(8,zreco,xreco);
	  gHits_xz[i]->Add(gRecoHits_xz[i]);
          gHits_xz[i]->Add(gSimHits_xz[i]);
	
	  gHits_xz[i]->GetYaxis()->SetRangeUser(-50,50);
          gHits_xz[i]->GetXaxis()->SetLimits(-50,1900);
          gSimHits_xz[i]->SetMarkerStyle(3);
          gSimHits_xz[i]->SetMarkerSize(2);
          gSimHits_xz[i]->SetMarkerColor(kRed);
          gSimHits_xz[i]->SetLineColor(kRed);

          gRecoHits_xz[i]->SetMarkerStyle(3);
          gRecoHits_xz[i]->SetMarkerSize(2);

	  gHits_xz_pass.push_back(gHits_xz[i]);
        }
	if ( std::find(std::begin(yreco), std::end(yreco), 999) == std::end(yreco)){
          sprintf(Ghevt,"gRecoHits_yz_e%d",i);
          sprintf(Gyzsim,"gHits_yz_e%d",i);

          gHits_yz[i] = new TMultiGraph(Gyzsim,Gxzsim);
          gSimHits_yz[i] = new TGraph(8,&zsim[0],&ysim[0]);
          gRecoHits_yz[i] = new TGraph(8,zreco,yreco);
          gHits_yz[i]->Add(gRecoHits_yz[i]);
          gHits_yz[i]->Add(gSimHits_yz[i]);

          gHits_yz[i]->GetYaxis()->SetRangeUser(-50,50);
          gHits_yz[i]->GetXaxis()->SetLimits(-50,1900);
          gSimHits_yz[i]->SetMarkerStyle(3);
          gSimHits_yz[i]->SetMarkerSize(2);
          gSimHits_yz[i]->SetMarkerColor(kRed);
          gSimHits_yz[i]->SetLineColor(kRed);

          gRecoHits_yz[i]->SetMarkerStyle(3);
          gRecoHits_yz[i]->SetMarkerSize(2);

          gHits_yz_pass.push_back(gHits_yz[i]);
        }
        char *Gxz = new char[13];
        char *Gyz = new char[13];
        sprintf(Gxz,"gLines_xz_e%d",i);
        sprintf(Gyz,"gLines_yz_e%d",i);

	// seg 1
	double t1 = (zreco[1] - rec->sgmnts.seg[0].vtx[2])/rec->sgmnts.seg[0].mom.z;
        double end1x = rec->sgmnts.seg[0].vtx[0]+t1*rec->sgmnts.seg[0].mom.x;
        double end1y = rec->sgmnts.seg[0].vtx[1]+t1*rec->sgmnts.seg[0].mom.y;
	double l1x[2] = {rec->sgmnts.seg[0].vtx[0],end1x};
	double l1y[2] = {rec->sgmnts.seg[0].vtx[1],end1y};
	double l1z[2] = {rec->sgmnts.seg[0].vtx[2],zreco[1]};

        // seg 2
        double t2 = (zreco[4] - rec->sgmnts.seg[1].vtx[2])/rec->sgmnts.seg[1].mom.z;
        double end2x = rec->sgmnts.seg[1].vtx[0]+t2*rec->sgmnts.seg[1].mom.x;
        double end2y = rec->sgmnts.seg[1].vtx[1]+t2*rec->sgmnts.seg[1].mom.y;
        double l2x[2] = {rec->sgmnts.seg[1].vtx[0],end2x};
        double l2y[2] = {rec->sgmnts.seg[1].vtx[1],end2y};
        double l2z[2] = {rec->sgmnts.seg[1].vtx[2],zreco[4]};

        // seg 3
        double t3 = (zreco[7] - rec->sgmnts.seg[2].vtx[2])/rec->sgmnts.seg[2].mom.z;
        double end3x = rec->sgmnts.seg[2].vtx[0]+t3*rec->sgmnts.seg[2].mom.x;
        double end3y = rec->sgmnts.seg[2].vtx[1]+t3*rec->sgmnts.seg[2].mom.y;
        double l3x[2] = {rec->sgmnts.seg[2].vtx[0],end3x};
        double l3y[2] = {rec->sgmnts.seg[2].vtx[1],end3y};
        double l3z[2] = {rec->sgmnts.seg[2].vtx[2],zreco[7]};

	gLines_xz[i] = new TMultiGraph(Gxz,Gxz);
        gLines_yz[i] = new TMultiGraph(Gyz,Gyz);

	gLine_xz1[i] = new TGraph(2,&l1z[0],&l1x[0]);
        gLine_xz2[i] = new TGraph(2,&l2z[0],&l2x[0]);
        gLine_xz3[i] = new TGraph(2,&l3z[0],&l3x[0]);
        gLine_yz1[i] = new TGraph(2,&l1z[0],&l1y[0]);
        gLine_yz2[i] = new TGraph(2,&l2z[0],&l2y[0]);
        gLine_yz3[i] = new TGraph(2,&l3z[0],&l3y[0]);

        gRecoHits_xz[i] = new TGraph(8,zreco,xreco);
        gRecoHits_yz[i] = new TGraph(8,zreco,yreco);

        gTruthHits_xz[i] = new TGraph(20,&ztruth[0],&xtruth[0]);
        gTruthHits_yz[i] = new TGraph(20,&ztruth[0],&ytruth[0]);

        gLines_xz[i]->Add(gRecoHits_xz[i]);
        gRecoHits_xz[i]->SetMarkerStyle(3);
        gRecoHits_xz[i]->SetMarkerSize(2);
        gRecoHits_xz[i]->SetMarkerColor(kBlack);
        gRecoHits_xz[i]->SetLineColor(0);

        gLines_yz[i]->Add(gRecoHits_yz[i]);
        gRecoHits_yz[i]->SetMarkerStyle(3);
        gRecoHits_yz[i]->SetMarkerSize(2);
        gRecoHits_yz[i]->SetMarkerColor(kBlack);
        gRecoHits_yz[i]->SetLineColor(0);

        gLines_xz[i]->Add(gTruthHits_xz[i]);
        gTruthHits_xz[i]->SetMarkerStyle(3);
        gTruthHits_xz[i]->SetMarkerSize(2);
        gTruthHits_xz[i]->SetMarkerColor(kViolet);
        gTruthHits_xz[i]->SetLineColor(0);

        gLines_yz[i]->Add(gTruthHits_yz[i]);
        gTruthHits_yz[i]->SetMarkerStyle(3);
        gTruthHits_yz[i]->SetMarkerSize(2);
        gTruthHits_yz[i]->SetMarkerColor(kViolet);
        gTruthHits_yz[i]->SetLineColor(0);

        gLines_xz[i]->Add(gLine_xz1[i]);
        gLines_xz[i]->GetYaxis()->SetRangeUser(-50,50);
        gLines_xz[i]->GetXaxis()->SetLimits(-50,1900);
        gLine_xz1[i]->SetMarkerStyle(3);
        gLine_xz1[i]->SetMarkerSize(2);
        gLine_xz1[i]->SetMarkerColor(kRed);
        gLine_xz1[i]->SetLineColor(kRed);

        gLines_xz[i]->Add(gLine_xz2[i]);
        gLines_xz[i]->GetYaxis()->SetRangeUser(-50,50);
        gLines_xz[i]->GetXaxis()->SetLimits(-50,1900);
        gLine_xz2[i]->SetMarkerStyle(3);
        gLine_xz2[i]->SetMarkerSize(2);
        gLine_xz2[i]->SetMarkerColor(kBlue);
        gLine_xz2[i]->SetLineColor(kBlue);

        gLines_xz[i]->Add(gLine_xz3[i]);
        gLines_xz[i]->GetYaxis()->SetRangeUser(-50,50);
        gLines_xz[i]->GetXaxis()->SetLimits(-50,1900);
        gLine_xz3[i]->SetMarkerStyle(3);
        gLine_xz3[i]->SetMarkerSize(2);
        gLine_xz3[i]->SetMarkerColor(kGreen);
        gLine_xz3[i]->SetLineColor(kGreen);

        gLines_yz[i]->Add(gLine_yz1[i]);
        gLines_yz[i]->GetYaxis()->SetRangeUser(-50,50);
        gLines_yz[i]->GetXaxis()->SetLimits(-50,1900);
        gLine_yz1[i]->SetMarkerStyle(3);
        gLine_yz1[i]->SetMarkerSize(2);
        gLine_yz1[i]->SetMarkerColor(kRed);
        gLine_yz1[i]->SetLineColor(kRed);

        gLines_yz[i]->Add(gLine_yz2[i]);
        gLines_yz[i]->GetYaxis()->SetRangeUser(-50,50);
        gLines_yz[i]->GetXaxis()->SetLimits(-50,1900);
        gLine_yz2[i]->SetMarkerStyle(3);
        gLine_yz2[i]->SetMarkerSize(2);
        gLine_yz2[i]->SetMarkerColor(kBlue);
        gLine_yz2[i]->SetLineColor(kBlue);

        gLines_yz[i]->Add(gLine_yz3[i]);
        gLines_yz[i]->GetYaxis()->SetRangeUser(-50,50);
        gLines_yz[i]->GetXaxis()->SetLimits(-50,1900);
        gLine_yz3[i]->SetMarkerStyle(3);
        gLine_yz3[i]->SetMarkerSize(2);
        gLine_yz3[i]->SetMarkerColor(kGreen);
        gLine_yz3[i]->SetLineColor(kGreen);

        gLines_xz_pass.push_back(gLines_xz[i]);
        gLines_yz_pass.push_back(gLines_yz[i]);
      }
    }
    for (size_t i=0; i<8; i++){
      xreco[i] = 999; xsim[i] = 999;
      yreco[i] = 999; ysim[i] = 999;
      zreco[i] = 999; zsim[i] = 999;
    }
    truthhit_track1.clear();
    truthhit_track2.clear();
    truthhit_track3.clear();

    xtruth.clear();
    ytruth.clear();
    ztruth.clear();
  //} //3 track segments
  } // end loop over entries

  std::cout<<"n good clusters: "<<ncl<<std::endl; 
 
  // Make canvases and draw histograms on them
  // Easier to manipulate how we want things too look
  // Can also now do more histogram and canvas manipulation
  // This is as basic as it comes for now
  TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  hNTrkSeg->SetLineWidth(2);
  //hNTrkSeg->Draw();
  c1->Print("hNTrkSeg.png");

  hScattAngleRes->SetLineWidth(2);
  //hScattAngleRes->Draw();
  c1->Print("ScattAngleRes.png");

  hBendAngleRes->SetLineWidth(2);
  //hBendAngleRes->Draw();
  c1->Print("BendAngleRes.png");

  //make new output root file?
  TFile* caf_out = new TFile("caf_out.root","RECREATE");
  hScattAngleRes->Write();
  hBendAngleRes->Write();
  for (int i=0; i<8; i++){
    for (int j=0; j<3; j++){
      hSSDHit[i][j]->Write();
    }
  }
  for (int i=0; i<8; i++){
    hRes_x[i]->Write();
    hRes_y[i]->Write();
  }
  for (int i=0; i<8; i++){
    hPulls[i]->Write();
  }
  hChi2_seg1->Write();
  hChi2_seg2->Write();
  hChi2_seg3->Write();

  for (int i=0; i<8; i++){
    hAngDiffx[i]->Write();
    hAngDiffy[i]->Write();
  }
  hAngDiffx_seg1->Write();
  hAngDiffy_seg1->Write();
  hAngDiffx_seg2->Write();
  hAngDiffy_seg2->Write();
  hAngDiffx_seg3->Write();
  hAngDiffy_seg3->Write();

  for (int i=0; i<8; i++){
    for (int j=0; j<3; j++){
      hFixedZ_DX[i][j]->Write();
      hFixedZ_DY[i][j]->Write();
    }
  }
  for (int i=0; i<8; i++){
    for (int j=0; j<3; j++){
      hRecoMinusTrueVsTrueX[i][j]->Write();
      hRecoMinusTrueVsTrueY[i][j]->Write();
    }
  }
  hBendingXYat4->Write();
  hBending->Write();
  hScattering->Write();

  hBendVsTrueRAt4->Write();
  hBendVsTrueRAt5->Write();
  hBendVsRecoRAt4->Write();
  hBendVsRecoRAt5->Write();
  hBendVsTrueOmegaAt4->Write();
  hBendVsTrueOmegaAt5->Write();
  hBendVsRecoOmegaAt4->Write();
  hBendVsRecoOmegaAt5->Write();

  //for (int i=0; i<100; i++){
  //  if (gHits_xz[i]->GetListOfGraphs()) std::cout<<"true"<<std::endl; //gHits_xz[i]->Write();
  //}
  for (size_t i=0; i<gHits_xz_pass.size(); i++){
      gHits_xz_pass[i]->Write();
      gHits_yz_pass[i]->Write();
      gLines_xz_pass[i]->Write();
      gLines_yz_pass[i]->Write();
  }

  caf_out->Close();   

}
void angDiff(std::vector<caf::SRTrueSSDHits> truthv, caf::SRVector3D& p, double ang[2]){
   double thetax_sim = 0;
   double thetay_sim = 0;

   double thetax_re = TMath::ATan2(p.x,p.z);
   double thetay_re = TMath::ATan2(p.y,p.z);

   for (auto i : truthv) {
     double thetax_i = TMath::ATan2(i.GetPx,i.GetPz);
     double thetay_i = TMath::ATan2(i.GetPy,i.GetPz);
     thetax_sim += thetax_i;
     thetay_sim += thetay_i;

     hAngDiffx[i.GetStation]->Fill(thetax_re - thetax_i);
     hAngDiffy[i.GetStation]->Fill(thetay_re - thetay_i);
   }
   thetax_sim /= truthv.size();
   thetay_sim /= truthv.size();

   // reconstructed - avg simulated angle
   double angdiffx = thetax_re - thetax_sim;
   double angdiffy = thetay_re - thetay_sim;

   ang[0] = angdiffx;
   ang[1] = angdiffy;
}
void findDistFixedZ(std::vector<caf::SRTrueSSDHits> truthv, caf::SRTrackSegment& seg){
   for (auto i : truthv){
     //get point a, which we take as the vertex/start of the track
     //this can be any point on the line though
     double t = ( i.GetZ - seg.vtx[2] )/seg.mom.z;
     double x = seg.vtx[0] + t*seg.mom.x;
     double y = seg.vtx[1] + t*seg.mom.y;

     hFixedZ_DX[i.GetStation][i.GetPlane]->Fill(x-i.GetX);
     hFixedZ_DY[i.GetStation][i.GetPlane]->Fill(y-i.GetY);

     hRecoMinusTrueVsTrueX[i.GetStation][i.GetPlane]->Fill(i.GetX,x-i.GetX);
     hRecoMinusTrueVsTrueY[i.GetStation][i.GetPlane]->Fill(i.GetY,y-i.GetY);
   }
}
