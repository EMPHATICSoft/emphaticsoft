////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to understand our SSD clusters,, in particular, the relative alignment 
///          between views.  
/// \author  $Author: lebrun $
/*
 We should probably look at 120 GeV first.. 
  Note from Teresa, Thursday, Dec 1 
   to Jack Mirabito, and others, since this is probably useful for everyone looking at the SSDs) 120 GeV runs with probably usable SSD data: 
               1039, 1040, 1042, 1043, 1054, 1055, (Al brick in front of magnet: 1058, 1060, 1062).
The issue with all of these runs was that we saw significantly fewer (~3-6k) triggers on the SSDs than the other detectors, 
so the event matching is in question. It's possible all of the data is usable, 
but until we investigate the event matching we won't know. 
Either way, if you're only looking at individual detectors they should be okay
*/
////////////////////////////////////////////////////////////////////////
// C/C++ includes
// C++ includes
#include <cstddef>
#include <memory>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <climits>
#include <cfloat>
//
// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
//
// simulation results per say.. stuff
// 
#include "RunHistory/RunHistory.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "RecoBase/SSDCluster.h"
#include "SSDReco/SSDHotChannelList.h"
//

namespace emph {
  class StudyOneSSDClusters;
}
  //
    
    class emph::StudyOneSSDClusters : public art::EDAnalyzer {
    
    public:
      
      struct myLinFitResult { 
        public:
	  int ndgf; // number of degrees of freedom Other data member are self explicit.
	  double offset;
	  double slope;
	  double sigmaOffset;
	  double sigmaSlope;
	  double covOffsetSlope; 
	  double chiSq;
	  std::vector<double> resids; // The residuals
      };
           
      explicit StudyOneSSDClusters(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void analyze(const art::Event& evt);
      
  // Plugins should not be copied or assigned.
      StudyOneSSDClusters(StudyOneSSDClusters const&) = delete;
      StudyOneSSDClusters(StudyOneSSDClusters&&) = delete;
      StudyOneSSDClusters& operator=(StudyOneSSDClusters const&) = delete;
      StudyOneSSDClusters& operator=(StudyOneSSDClusters&&) = delete;


      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const&);
//      void endRun(art::Run const&); // not needed.. 
//      void endSubRun(art::SubRun const&);
      void endJob();

   private:
//
// Some utilities.. 
//      
      bool fFilesAreOpen;
      std::string fTokenJob;
      std::string fSSDClsLabel;
      bool fDumpClusters;    
      bool fSelectHotChannels;    
      bool fAlignX;    
      bool fAlignY;    
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      double fPitch;
//
// access to the geometry.   
//
      runhist::RunHistory *fRunHistory;
      emph::geo::Geometry *fEmgeo;
      std::vector<double> fZlocXPlanes;
      std::vector<double> fZlocYPlanes;
      std::vector<double> fZlocUPlanes;
      std::vector<double> fZlocVPlanes;
      std::map<int, char> fXYUVLabels; // key on 10*stations + sensor. 
// 
// Container for the hot channels. 
// 
//
// access to input data..   
//
      std::vector<art::Ptr<rb::SSDCluster> > fSSDclPtrs; // This is what I got from art, see analyze method. 
      std::vector<rb::SSDCluster> fSSDcls; // we will do a deep copy, as my first attempt at using the above vector failed.. 
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr;

//
// Hot channel Analysis      
//
   std::vector<emph::ssdr::SSDHotChannelList> fHotChans;      
//
// CSV tuple output..
// 
      std::ofstream fFOutA1X, fFOutA1Y, fFOutA1U, fFOutA1V;
      void openOutputCsvFiles();
      
      void dumpXYCls();
      void fillHotChannels();
      
      char getView(std::vector<rb::SSDCluster>::const_iterator itCl) const; 
      
      void  fitLin(const char dirPlane, const std::vector<double> &t, const std::vector<double> &sigT, myLinFitResult &fitRes ) const ; 
      
    }; 
    
// .....................................................................................
    emph::StudyOneSSDClusters::StudyOneSSDClusters(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fSSDClsLabel("?"),
    fDumpClusters(false), fSelectHotChannels(false),     
    fAlignX(false), fAlignY(false),    
    fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0) , fPitch(0.06),
     fRunHistory(nullptr), fEmgeo(nullptr), 
     fZlocXPlanes(0), fZlocYPlanes(0), fZlocUPlanes(0), fZlocVPlanes(0)
    {
       std::cerr << " Constructing StudyOneSSDClusters " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;
    }
    
    void emph::StudyOneSSDClusters::reconfigure(const fhicl::ParameterSet& pset)
    {
      std::cerr << " emph::StudyOneSSDClusters::reconfigure ... " <<std::endl;
      fSSDClsLabel = pset.get<std::string>("SSDClsLabel");   
      std::cerr << "  ... fSSDClsLabel " << fSSDClsLabel << std::endl;   
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fDumpClusters = pset.get<bool>("dumpClusters", false);
      fSelectHotChannels = pset.get<bool>("selectHotChannels", false);
      fAlignX = pset.get<bool>("alignX", false);
      fAlignY = pset.get<bool>("alignY", false);
      if ((!fDumpClusters) && (!fSelectHotChannels) && (!fAlignX) && (!fAlignX)) { 
        std::cerr << " .... Nothing to do !!! Therefore, quit here and now  " << std::endl; exit(2);
      }
      if (fSelectHotChannels) {
        for (int kSt = 0; kSt != 6; kSt++) { 
	  for (int kSe = 0; kSe != 6; kSe++) { 
             emph::ssdr::SSDHotChannelList aHl(fRun, kSt, kSe);
	     fHotChans.push_back(aHl);
	  }
	}
      }
      std::cerr << " .... O.K. keep going .. " << std::endl;
    }
    void emph::StudyOneSSDClusters::beginRun(art::Run const &run)
    {
      // Assume th same geometry for all sub runs (this is called for every subruns, it turns out.. ) 
      if (fXYUVLabels.size() != 0) return;
      std::cerr << " StudyOneSSDClusters::beginRun, run " << run.id() << std::endl;
      fRunHistory = new runhist::RunHistory(run.run());
      fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile()); 
      for (int kSt = 0; kSt != 6; kSt++) {
        for(int kSs =0; kSs !=6; kSs++) {
            fXYUVLabels.emplace(std::make_pair(10*kSt + kSs, '?'));    
	}
      }
//
      std::vector<double> XlocXPlanes, YlocYPlanes; // labeled by view 
      for (int k=0; k != fEmgeo->NSSDStations(); k++) {
        emph::geo::SSDStation aStation = fEmgeo->GetSSDStation(k);
        TVector3 posSt = aStation.Pos();
	std::cerr << " .... For Station by index " <<  k  << " Stations name is " << aStation.Name() 
	          << " Number of planes " << aStation.NSSDs() << std::endl;
        for (int kk=0; kk != aStation.NSSDs(); kk++) {
          emph::geo::Detector aPlane = aStation.GetSSD(kk);
	  char viewChar = '?';
	  if ((std::abs(aPlane.Rot() - 270.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() - 90.*M_PI/180.) < 0.1) || 
	     (std::abs(aPlane.Rot() + 270.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() + 90.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos();
	    fZlocXPlanes.push_back(pos[2] + posSt[2]);
	    XlocXPlanes.push_back(pos[0] + posSt[0]);
	    viewChar = 'X';
	  }
	  if ((std::abs(aPlane.Rot()) < 0.1) || (std::abs(aPlane.Rot() - 180.*M_PI/180.) < 0.1)) {
	    TVector3 pos = aPlane.Pos();
	    fZlocYPlanes.push_back(pos[2] + posSt[2]);
	    YlocYPlanes.push_back(pos[1] + posSt[1]);
	    viewChar = 'Y';
	  }
	  if ((std::abs(aPlane.Rot() - 315.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() - 45.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos();
	    fZlocUPlanes.push_back(pos[2] + posSt[2]);
	    viewChar = 'U';
	  }
	  if ((std::abs(aPlane.Rot() - 225.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() + 45.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos();
	    fZlocUPlanes.push_back(pos[2] + posSt[2]);
	    viewChar = 'V';
	  }
	  fXYUVLabels[10*k + kk] = viewChar;
	  std::cerr << " .... .... For Sensor by index " <<  kk  << " Sensor name is " << aPlane.Name() 
	            << " view " << viewChar << "  view, check  " << fXYUVLabels.at(10*k +kk) << std::endl;
        } // on SS Detector planes of segment of a plane.   
      } // on index k, SSD Stations.
      if (fZlocXPlanes.size() < 5)  { std::cerr << " Not enough X planes to do this study, quit here and now " << std::endl; exit(2); }
      std::cerr << " Number of SSD X planes (vertical strips) " << fZlocXPlanes.size() << std::endl; 
      std::cerr << " Z locations ";
      for (size_t i=0; i != fZlocXPlanes.size(); i++) std::cerr << " " << fZlocXPlanes[i] << ",";
      std::cerr << std::endl << " X locations ";
      for (size_t i=0; i != XlocXPlanes.size(); i++) std::cerr << " " << XlocXPlanes[i] << ",";
      std::cerr << std::endl << " Number of SSD Y planes (Horizontal strips) " << fZlocYPlanes.size() << std::endl;
      std::cerr << " Z locations ";
      for (size_t i=0; i != fZlocYPlanes.size(); i++) std::cerr << " " << fZlocYPlanes[i] << ",";
      std::cerr << std::endl << " Y location " ;
      for (size_t i=0; i != YlocYPlanes.size(); i++) std::cerr << " " << YlocYPlanes[i] << ",";
      std::cerr  << std::endl << " ------------- End of StudyOneSSDClusters::beginRun ------------------" << std::endl << std::endl;
    }
    
    void emph::StudyOneSSDClusters::beginJob()
    {
    }
    void emph::StudyOneSSDClusters::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " StudyOneSSDClusters::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      if (fDumpClusters) { 
        std::string headerS(" subRun evt station nCl iCl wgtAvgStrip wgtRmsStrip avgADC ");
        std::ostringstream fNameDumpClustStrStr; fNameDumpClustStrStr << "./SSDClusterTuple_V1_" << fRun << "_" << fTokenJob;
        std::string fNameDumpClustStr(fNameDumpClustStrStr.str());
        std::string fNameDumpClusXStr(fNameDumpClustStr); fNameDumpClusXStr += "_X.txt";
        fFOutA1X.open(fNameDumpClusXStr.c_str());
        fFOutA1X << headerS << std::endl;
        std::string fNameDumpClusYStr(fNameDumpClustStr); fNameDumpClusYStr += "_Y.txt";
        fFOutA1Y.open(fNameDumpClusYStr.c_str());
        fFOutA1Y << headerS << std::endl;
      }
      fFilesAreOpen = true;
    }
    
    void emph::StudyOneSSDClusters::endJob() {
      
      std::cerr << " StudyOneSSDClusters::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      if (fDumpClusters) {
        fFOutA1X.close(); fFOutA1Y.close();
      }
      if (fSelectHotChannels) {
        for (std::vector<emph::ssdr::SSDHotChannelList>::iterator itHl = fHotChans.begin(); itHl != fHotChans.end(); itHl++) 
          itHl->tallyIt(5.0);
        std::string dirName("./"); // in my build directory, 
        for (std::vector<emph::ssdr::SSDHotChannelList>::const_iterator itHl = fHotChans.cbegin(); itHl != fHotChans.cend(); itHl++) 
          itHl->dumpItToFile(dirName);
      }
    }
    
    void emph::StudyOneSSDClusters::dumpXYCls() {      
     // first count the number of hits for the X stations and Y station.. Will do the U and V laters.. 
     std::vector<int> numClsX(6, 0);
     std::vector<int> numClsY(6, 0);
     if (fNEvents < 20) {
       std::cerr << " emph::StudyOneSSDClusters::dumpXYCls, number of cluster for evt " << fEvtNum <<  "  is " << fSSDClsPtr->size() << std::endl;
     }
     
     for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aStation = itCl->Station(); // iterator to pointer to the cluster. 
         char aView = this->getView(itCl);
	if (aView == 'X')  numClsX[aStation]++;
	if (aView == 'Y')  numClsY[aStation]++;
      }
      for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aSensor = itCl->Sensor();
        int aStation = itCl->Station();
        char aView =  fXYUVLabels.at(10*aStation+aSensor); // we have done the check above.. 
	int nn = 0;
	if (aView == 'X') nn = numClsX[aStation]; 
	if (aView == 'Y') nn = numClsY[aStation];
	if ((aView != 'X') && (aView != 'Y')) continue;
        std::ostringstream aLineStrStr; 
//        std::string headerS(" subRun evt station nCl iCl wgtAvgStrip wgtRmsStrip avgADC ");
	aLineStrStr << " " << fSubRun << " " << fEvtNum << " " << itCl->Station();
	aLineStrStr << " " << nn << " " << itCl->ID() << " " << itCl->WgtAvgStrip() << " " 
	            << itCl->WgtRmsStrip() << " " << itCl->AvgADC();
        std::string aLineStr(aLineStrStr.str()); 
	if (aView == 'X')  fFOutA1X << aLineStr << std::endl;  
	if (aView == 'Y')  fFOutA1Y << aLineStr << std::endl;  
      }
    }
    void emph::StudyOneSSDClusters::fillHotChannels() {      
      for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aSensor = itCl->Sensor();
        int aStation = itCl->Station();
        for (std::vector<emph::ssdr::SSDHotChannelList>::iterator itHl = fHotChans.begin(); itHl != fHotChans.end(); itHl++) {
	  if ((itHl->Station() == aStation) && (itHl->Sensor() == aSensor)) itHl->fillHit(itCl);
        }
      }
    }
    void emph::StudyOneSSDClusters::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      
//      std::cerr << " StudyOneSSDClusters::analyze , event " << fEvtNum << " and do not much  " <<   std::endl; 
      
    //
    // Get the data. 
      auto hdlCls = evt.getHandle<std::vector<rb::SSDCluster>>(fSSDClsLabel);
      art::fill_ptr_vector(fSSDclPtrs, hdlCls);
      if (fSSDclPtrs.size() == 0) return; // nothing to do, no data. 
      //
      // This code fragment is simply for debugging.. And is confusing.. 
//      if (fSSDclPtrs.size() != 1) { 
//        std::cerr << " Number of pointers to a given SSDCluster, " << fSSDclPtrs.size() << std::endl;
//      }

//      std::vector<art::Ptr<rb::SSDCluster> >::const_iterator aPtrClIt = fSSDclPtrs.cbegin();
//      art::Ptr<rb::SSDCluster> aPtrCl = *aPtrClIt;
//      std::cerr << " Station for the first cluster  " << aPtrCl->Station() << std::endl;
      // 
      // These will fail, as the sStion number is clearly bogus... for many clusters.  
      // old, deprecated interface, with a deep copy.. 
      //
      evt.getByLabel (fSSDClsLabel, fSSDClsPtr);
      fSSDcls = (*fSSDClsPtr); // deep copy here. Probably can be avoided.. 
      if (fNEvents < 50){
         std::cerr << " Number SSDClusters, deprecated interface (with deep copy) " << fSSDcls.size() << std::endl;
        if (fSSDcls.size() != 0) {
          std::vector<rb::SSDCluster>::const_iterator itClsTest = fSSDcls.cbegin(); 
          std::cerr << " Station for the first cluster, old interface   " << itClsTest->Station() << std::endl;
          std::vector<rb::SSDCluster>::const_iterator itClsTest2 = fSSDClsPtr->cbegin(); 
          std::cerr << " Station for the first cluster, old interface no deep copy   " << itClsTest2->Station() << std::endl;
	  
        }
      } 
      if (fDumpClusters) this->dumpXYCls();
      if (fSelectHotChannels) this->fillHotChannels(); 
    } // end of Analyze, event by events.  
   
   char emph::StudyOneSSDClusters::getView(std::vector<rb::SSDCluster>::const_iterator itCl) const {
   
       int aSensor = itCl->Sensor();
       if ((aSensor < 0 ) || (aSensor >= 6) ) {
         std::cerr << " StudyOneSSDClusters::getView, invalid sensor number " << aSensor << " fatal, quit!" << std::endl; 
	 std::cerr <<  " Faulty cluster " << std::endl;
	 std::cerr << (*itCl) << std::endl;	 
	 exit(2);
       } 
       int aStation = itCl->Station();
       if ((aStation < 0 ) || (aStation >= 6) ) {
         std::cerr << " StudyOneSSDClusters::getView, invalid station number " << aStation << " fatal, quit!" << std::endl;
	 std::cerr <<  " Faulty cluster " << std::endl;
	 std::cerr << (*itCl) << std::endl;
	 exit(2);
	 
       }
       char aView =  fXYUVLabels.at(10*aStation+aSensor);
       if (aView  == '?') {
          std::cerr << " StudyOneSSDClusters::getView, invalid sensor/station view, station " 
	             << aStation << " sensor " << aSensor <<  " fatal, quit!" << std::endl; 
		     
	 std::cerr <<  " Faulty cluster " << std::endl;
	 std::cerr << (*itCl) << std::endl;
		     
	 exit(2);
       }
       return aView;
   }
   void  emph::StudyOneSSDClusters::fitLin(const char dirPlane, const std::vector<double> &ts, const std::vector<double> &sigTs, 
                                    myLinFitResult &fitRes ) const  {
     //
     // See good old Numerical Recipes, chapter 15.2, subroutine fit. (yeah, a subroutine!) 
     //
     if (ts.size() != sigTs.size()) {
       std::cerr << " StudyOneSSDClusters::fitLin mismatch of length between measurement values ( " 
                  << ts.size() << " )  and uncertainties ( " << sigTs.size() << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
     }
     std::vector<double> xs;
     switch (dirPlane) { 
       case 'X' : 
         xs = fZlocXPlanes;
	 break;
       case 'Y' : 
          xs = fZlocYPlanes;
	 break;
       case 'U' : 
         xs = fZlocUPlanes;
	 break;
       case 'V' : 
         xs = fZlocVPlanes;
	 break;
       default : 
       std::cerr << " StudyOneSSDClusters::fitLin Unrecognize plane direction " << dirPlane << " Fatal, stop here, there I said it  " << std::endl;
		  exit(2);  
     }
     if (ts.size() != xs.size()) {
       std::cerr << " StudyOneSSDClusters::fitLin mismatch of length between measurement values ( " 
                  << ts.size() << " )  Z positions of planes ( " << xs.size() << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
     }
     fitRes.ndgf = static_cast<int> (ts.size() - 2);
     std::vector<double> ws(ts);
     double sx = 0.; double sy = 0.; double ss = 0.;
     for (size_t k=0; k != ts.size(); k++) {
       ws[k] = 1.0/(sigTs[k]*sigTs[k]);
       sx += xs[k]*ws[k]; sy += ts[k]*ws[k]; ss += ws[k];
     }
     const double sxoss = sx/ss;
     double b = 0.; double st2 = 0.;
     for (size_t k=0; k != ts.size(); k++) {
       const double tmpT = (xs[k] - sxoss)/sigTs[k];
       st2 += tmpT*tmpT;
       b += tmpT*ts[k];
     }
/*     
	  double sigmaSlope;
	  double covOffsetSlope;
	  double chiSq;
	  std::vector<double> resids; // The residuals
*/
     fitRes.slope = b/st2;
     fitRes.offset = (sy - sx*fitRes.slope)/ss;
     fitRes.sigmaOffset = std::sqrt((1. + (sx*sx)/(ss*st2))/ss);   
     fitRes.sigmaSlope = std::sqrt(1./st2);
     fitRes.covOffsetSlope = -1.0 * sx/(ss*st2);
     fitRes.chiSq = 0.;
     fitRes.resids.clear();
     for(size_t k=0; k != ts.size(); k++) {
      const double rr = ts[k] - (fitRes.offset + fitRes.slope*xs[k]);
      fitRes.resids.push_back(rr);
      fitRes.chiSq += (rr*rr)*ws[k];
     }
     
     std::cerr << " StudyOneSSDClusters::fitLin, done, offset " << fitRes.offset << " +- " 
               <<  fitRes.sigmaOffset << " but need checking, so, quit now for good  " << std::endl; 
     exit(2);
     
   } // end of fitLin..  
DEFINE_ART_MODULE(emph::StudyOneSSDClusters)
