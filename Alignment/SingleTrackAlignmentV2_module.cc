////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct single-particle tracks for 
///  the purpose of Aligning the SSD.
///       
/// \author  $Author: Paul Lebrun $ lebrun@fnal.gov
///
///  This art module is of the "Analyzer" type, it produces no
///  art recod.  Besides some ASCII test files for diagnostics, 
///  it produces a binary file, to be dtreat outside the scope 
/// of art by the Millipede executable. 
///  
/// The track expressed in a reference befine by Station 0, as 
/// this is the origin of our coordinate system, and by 
/// station 4.  In principle, to get a Cartesian (there by orthognal), 
/// only one coordinate is sufficient to fix that coordinate system,
/// as the other constraints come from the orthogonality condition. 
/// One goal of this modul is to study tyhe biases if we define the track 
/// fixing both the X and Y slopes of the track, using the 
/// coordinates of Station 0 and 4, in both planes.  
///
/// Unlike the version written by Robert Chirco, we assemble the 
/// Aligner track in this module, and we do use 
/// the Alignment file.  The Geometry Modifier does this...
/// The geometry needs to be modified, if we a-priori know 
/// Here, we compute the pulls solely based on the geometry.. 
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>
#include <fstream>
#include <cstdlib>
#include <cmath>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraph2D.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "TMath.h"
//#include "TGeoMatrix.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// EMPHATICSoft includes
#include "Align/service/AlignService.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RecoBase/SSDCluster.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/Track.h"
#include "RecoUtils/RecoUtils.h"
#include "Simulation/SSDHit.h"
#include "Simulation/Particle.h"
#include "TrackReco/SingleTrackAlgo.h"

#include "millepede_ii/Mille.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class SingleTrackAlignmentV2 : public art::EDAnalyzer {
  public:
    explicit SingleTrackAlignmentV2(fhicl::ParameterSet const& pset); 
    // Required! explicit tag tells the compiler this is not a copy constructor
    ~SingleTrackAlignmentV2() {};
    
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(const art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

  private:
  
    art::ServiceHandle<emph::geo::GeometryService> geo;
    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

    int         fRun,fSubrun;
    int         fEvtNum;

    size_t         fNStations;
    size_t         fNPlanes;
    size_t         fNXYPlanes;
    size_t         fNUWPlanes;
    
    // geometry parameters.. 
    
    double fTargetz;
    double fMagnetusz;
    double fMagnetdsz;
    double fMagCenter; 
    double fEffMagnetKick; // At 120 GeV, sudden kick approximation. 
    // Track parameters; 
    double fTrackX0, fTrackY0, fTrackSlxUpstr, fTrackSlxDwnstr, fTrackSly;
    //
    //fcl parameters
    std::string fClusterLabel;
    //
    // P.L. studies.. Jan 2026.. We might be using a bias sample to initiate the alignment. 
    //  Are our alignment parameters values stable against these selection criteria?  
    //
    const bool fDoSelectionStudies; // by default, false  // sufffix fSel for all the variables used in this study. (or prefix...) 
    const bool fUse123; // Using the event with last digit being 1, 2,  or 3 (default) 
    const bool fUse456; // Using the event with last digit being 4, 5 ,6  (default) 
    const bool fUse7890; // Using the event with last digit being 7, 8, or 9 (default) 
    const int fMinSensorOAOCl; // Minimum number of sensors with one and only hit to trigger an entry in the Millipede II data set.
    const bool fSkipUW; // skipping the confirm planes, if we align by stations.  
    const bool fRunPede; // at the end job, we run the Millipede-II algorithm, to align.. 
    const bool fFixStationRotations; 
    const int fFixAllStationPosButOne;
    const int fFixAllStationPosRotButOne;
    const int fFixAllFreeOneRotStation;
    const double fXCenterSel, fYCenterSel, fXWidthSel, fYWidthSel; // selection on regions, expect biases.. 
    const double fOneOverSqrt12, fOneOverSqrt2;
    int fNumUsedEvts; // Number of available (one and only one cluster per sensor) 
    int fNumMeasure; // Number of available (one and only one cluster per sensor) 
    const std::string fTokenCSV; // for book-keeping..
    const std::string fFNameMille;
    
    art::Handle< std::vector<rb::SSDCluster> > fClustH;
    std::map<int, std::vector<rb::SSDCluster>::const_iterator > fClustMap;
    std::map<int, int > fClustNum;
    std::vector<double> fEffErrAtStation;
    //
    // Diagnostics, studies.. 
    // 
    std::ofstream fClustAllDist; //  Inclusive, all      
    std::ofstream fClustSelDist; //     
    std::ofstream fPullDist; //     
 
    //Millepede stuff
    Mille* m;
    //
    // Internals 
    void fillClustMap();  
    void setTrackParams(bool debugNow = false);
    void FillPulls(bool dbg=false);
    // 
    void ClusterStudy(int stage); 

 };

  //.......................................................................
  
  emph::SingleTrackAlignmentV2::SingleTrackAlignmentV2(fhicl::ParameterSet const& pset)
    : EDAnalyzer{pset},
    fRun(0),fSubrun(0), fEvtNum(-1),  fNStations(-1), fNPlanes(-1), 
    fTargetz(-9999.), fMagnetusz(-9999.), fMagCenter(-9999.), 
    fEffMagnetKick (pset.get< double >("EffectiveMagnetKick")),
    fTrackX0(DBL_MAX), fTrackY0(DBL_MAX), fTrackSlxUpstr(DBL_MIN), 
    fTrackSlxDwnstr(DBL_MIN), fTrackSly(DBL_MIN), 
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fDoSelectionStudies (pset.get< bool >("DoSelectionStudies")),
    fUse123 (pset.get< bool >("Use123")),
    fUse456 (pset.get< bool >("Use456")),
    fUse7890 (pset.get< bool >("Use7890")),
    fMinSensorOAOCl (pset.get <int> ("MinSensorOAOCl")),
    fSkipUW (pset.get <bool> ("SkipUW")),
    fXCenterSel (pset.get< double >("XCenterSel")),
    fYCenterSel (pset.get< double >("YCenterSel")),
    fXWidthSel (pset.get< double >("XWidthSel")),
    fYWidthSel (pset.get< double >("YWidthSel")),
    fOneOverSqrt12(1.0/std::sqrt(12.)), fOneOverSqrt2(1.0/std::sqrt(2.)),
    fNumUsedEvts(0), fNumMeasure(0),
    fRunPede   (pset.get< bool > ("RunPede")),
    fFixStationRotations (pset.get<bool> ("FixStationRotations")), 
    fFixAllFreeOneRotStation (pset.get<int> ("FixAllFreeOneRotStation")), 
    fFixAllStationPosButOne (pset.get<int> ("FixAllStationPosButOne")), 
    fFixAllStationPosRotButOne (pset.get<int> ("FixAllStationPosRotButOne")), 
    fTokenCSV (pset.get<std::string> ("TokenCSV")),
    fFNameMille(std::string("m0043_") + fTokenCSV + std::string(".bin"))
    {
    }
  
  //......................................................................
  
//  SingleTrackAlignmentV2::~SingleTrackAlignmentV2()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................
  
  void SingleTrackAlignmentV2::beginRun(const art::Run& run)
  {
    auto emgeo = geo->Geo();
    fNStations = emgeo->NSSDStations();
    fNPlanes = emgeo->NSSDPlanes();
    int numSensorTotal = emgeo->NSSDs();
    std::cerr << " SingleTrackAlignmentV2::beginRun, numStations " << fNStations 
              << " Planes " << fNPlanes << " sensors " << numSensorTotal << std::endl;
    if (emgeo->GetTarget()) fTargetz = emgeo->GetTarget()->Pos()(2);
    else fTargetz = 380.5;    
    if (emgeo->MagnetLoad()) { 
      fMagnetusz = emgeo->MagnetUSZPos();
      fMagnetdsz = emgeo->MagnetDSZPos();
      fMagCenter = 0.5*(fMagnetusz + fMagnetdsz); 
    } else { fMagCenter = -9999.; }
    int l=0;
    for (int ii=0; ii<(int)fNStations; ii++){
      auto stationTmp = emgeo->GetSSDStation(ii);
      for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
        auto planeTmp = emgeo->GetSSDStation(ii)->GetPlane(jj);
	if ((planeTmp->View() == emph::geo::X_VIEW) || (planeTmp->View() == emph::geo::Y_VIEW)) fNXYPlanes++; 
	if ((planeTmp->View() == emph::geo::U_VIEW) || (planeTmp->View() == emph::geo::W_VIEW)) fNUWPlanes++; 
        for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
	  // Recap the view... 
	  std::cerr << " Station " << ii << " Plane " << jj << " has view ";
	  if (planeTmp->View() == emph::geo::X_VIEW) std::cerr << "X"; 
	  if (planeTmp->View() == emph::geo::Y_VIEW) std::cerr << "Y"; 
	  if (planeTmp->View() == emph::geo::U_VIEW) std::cerr << "U"; 
	  if (planeTmp->View() == emph::geo::W_VIEW) std::cerr << "W";
	  std::cerr << std::endl; 
        }
      }
    }
    //
    // Set the expected uncertainties at 120 GeV.. ! must be scaled by momentum!...
    // it includes the  position error (SSD pitch /sqrt(12), multiple scattering and 
    // magnetic field fringe fields. 
    //
    //  Obtained running the simulation.. 
    //
    fEffErrAtStation = {fOneOverSqrt12, 0.021, 0.035, 0.050, 0.064, 0.075, 0.083, 0.108, 0.129};
    // 
    if (fDoSelectionStudies && (!fClustAllDist.is_open())) {
      std::ostringstream runStrStr; runStrStr << run.run();
      std::string fOpt123("");
      if (fUse123) fOpt123 += std::string("_Opt123");  
      if (fUse456) fOpt123 += std::string("_Opt456");  
      if (fUse7890) fOpt123 += std::string("_Opt7890");
      if (fUse123 &&  fUse456 && fUse7890) fOpt123 = std::string("_AllEvts");
      std::string fileNameClustAll("./SingleTrackAlignment_ClustAll_"); 
      fileNameClustAll += runStrStr.str() + fOpt123 + std::string("_") + fTokenCSV + std::string("_v1.txt");
      fClustAllDist.open(fileNameClustAll.c_str()); fClustAllDist << " spill evt station plane sensor strip ";
      fClustAllDist << std::endl;   
      std::string fileNameClustSel("./SingleTrackAlignment_ClustSel_"); 
      fileNameClustSel += runStrStr.str() + fOpt123 + std::string("_") + fTokenCSV + std::string("_v1.txt");
      fClustSelDist.open(fileNameClustSel.c_str()); fClustSelDist << " spill evt ";
      std::string fileNamePulls("./SingleTrackAlignment_Pulls_"); 
      fileNamePulls += runStrStr.str() + fOpt123 + std::string("_") + fTokenCSV + std::string("_v1.txt");
      fPullDist.open(fileNamePulls.c_str()); fPullDist << " spill evt x0 y0 slx sly chiSq ndgf ";
      for (int ii=0; ii<(int)fNStations; ii++){
        for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
          for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){	  
            fClustSelDist << " ClSt" << ii << "Pl" << jj << "S" << kk;
            fPullDist << " PuSt" << ii << "Pl" << jj << "S" << kk;
	  }
	}
      }
      fClustSelDist << std::endl; fPullDist << std::endl; 
      //
    }


  }

  //......................................................................
   
  void emph::SingleTrackAlignmentV2::beginJob()
  {
    std::cerr<<"Starting SingleTrackAlignment.... "<<std::endl;
    m = new Mille(fFNameMille.c_str(),true,true);
    
  }
 
  //......................................................................
  
  void emph::SingleTrackAlignmentV2::endJob()
  {
    delete m;

    std::cout<<"SingleTrackAlignment: Number of events used = "<< fNumUsedEvts << std::endl;
    if (fDoSelectionStudies) {
      fClustAllDist.close(); fClustSelDist.close(); fPullDist.close(); //     
    }
    if (fRunPede) { 
          // Run pede..
      const char* env_PEDELib = std::getenv("MILLEPEDE_II_LIB"); // assume the find_package worked.. 
      std::string pathToPede(env_PEDELib); 
      std::string libStr("lib"); std::string binStr("bin");
      size_t iPosLib = pathToPede.find("lib");
      pathToPede.replace(iPosLib, 3, binStr, 0, 3);
      pathToPede += std::string("/pede");
      // 
      const char* env_SteerAll = std::getenv("CETPKG_SOURCE"); // assume the find_package worked.. 
      std::string pathToSteerAll(env_SteerAll);  
      pathToSteerAll += std::string("/Alignment/mp2/steer_3GlPPerStation.txt");
      std::string cmdCp01("cp ");  cmdCp01 += pathToSteerAll + std::string(" .");
      std::system(cmdCp01.c_str());
      std::string aNewPathToSteerAll("steer_"); aNewPathToSteerAll += fTokenCSV + std::string(".txt");
      std::string cmdMv01("mv steer_3GlPPerStation.txt ");  cmdMv01 +=  aNewPathToSteerAll;
      std::system(cmdMv01.c_str());
      //
      // Edit this file, change the name of the biinary file,  
      //
      std::string cmdSed01("sed -i  s/m004.bin/"); cmdSed01 += fFNameMille + std::string("/ "); 
      cmdSed01 += aNewPathToSteerAll;
      std::system(cmdSed01.c_str());
      bool introFixing = false;
      if (fFixStationRotations) { // valid only for station rotations..  
        introFixing = true;
//	std::string cmdSed02("sed -i '$a !fixing the station rotation angles '" ); cmdSed02 += aNewPathToSteerAll;
//	std::system(cmdSed02.c_str());
// Commented out, not how to introduce meta character in sed.. Not a regex expert.. 
	std::string cmdSed03("sed -i '$a parameter' " ); cmdSed03 += aNewPathToSteerAll;
	std::system(cmdSed03.c_str());
        for (int ii=1; ii<(int)fNStations; ii++){
          if (ii == 4) continue;
	  int paramNum = 10*ii + 3; // I set this up... 
	  std::ostringstream  staStrStr; staStrStr << " " << paramNum << "  0.   -1. ' ";
	  std::string staStr(staStrStr.str()); 
	  std::string cmdSed04x("sed -i '$a " ); cmdSed04x += staStr + aNewPathToSteerAll;
	  std::system(cmdSed04x.c_str());
        }
      } 
      if (fFixAllStationPosButOne != -1) { // valid only for station rotations.. 
        if (!introFixing) { 
	  std::string cmdSed03("sed -i '$a parameter' " ); cmdSed03 += aNewPathToSteerAll;
	  std::system(cmdSed03.c_str());
	  introFixing = true;
	}
        for (int ii=1; ii<(int)fNStations; ii++){
          if (ii == 4) continue;
	  if (fFixAllStationPosButOne == ii) continue;
	  for (int kDir=1; kDir !=3; kDir++) { 
	    int paramNum = 10*ii + kDir; // I set this up... 
	    std::ostringstream  staStrStr; staStrStr << " " << paramNum << "  0.   -1. ' ";
	    std::string staStr(staStrStr.str()); 
	    std::string cmdSed04x("sed -i '$a " ); cmdSed04x += staStr + aNewPathToSteerAll;
	    std::system(cmdSed04x.c_str());
	  }
        }
      } 
      if (fFixAllStationPosRotButOne != -1) { // valid only for station rotations.. 
        if (!introFixing) { 
	  std::string cmdSed03("sed -i '$a parameter' " ); cmdSed03 += aNewPathToSteerAll;
	  std::system(cmdSed03.c_str());
	  introFixing = true;
	}
        for (int ii=1; ii<(int)fNStations; ii++){
          if (ii == 4) continue;
	  if (fFixAllStationPosRotButOne == ii) continue;
	  for (int kDir=1; kDir !=4; kDir++) { 
	    int paramNum = 10*ii + kDir; // I set this up... 
	    std::ostringstream  staStrStr; staStrStr << " " << paramNum << "  0.   -1. ' ";
	    std::string staStr(staStrStr.str()); 
	    std::string cmdSed04x("sed -i '$a " ); cmdSed04x += staStr + aNewPathToSteerAll;
	    std::system(cmdSed04x.c_str());
	  }
        }
      } 
      if (fFixAllFreeOneRotStation != -1) { // valid only for station rotations.. 
        if (!introFixing) { 
	  std::string cmdSed03("sed -i '$a parameter' " ); cmdSed03 += aNewPathToSteerAll;
	  std::system(cmdSed03.c_str());
	  introFixing = true;
	}
        for (int ii=1; ii<(int)fNStations; ii++){
          if (ii == 4) continue;
	  for (int kDir=1; kDir !=4; kDir++) { 
	    int paramNum = 10*ii + kDir; // I set this up... 
	    if ((kDir == 3) && (fFixAllFreeOneRotStation == ii)) continue;
	    std::ostringstream  staStrStr; staStrStr << " " << paramNum << "  0.   -1. ' ";
	    std::string staStr(staStrStr.str()); 
	    std::string cmdSed04x("sed -i '$a " ); cmdSed04x += staStr + aNewPathToSteerAll;
	    std::system(cmdSed04x.c_str());
	  }
        }
      } 
      
      // 
      // 
      // Edit, depending on what we try.. 
      //
      std::string cmdPede(pathToPede); cmdPede += std::string(" ") + aNewPathToSteerAll;
      std::cerr << " .... Now trying to run pede, cmd is " << cmdPede << std::endl;
      int returnCodePede = std::system(cmdPede.c_str());
      std::cerr << " ... back emph::SingleTrackAlignmentV2::endJob, return code from pede " << returnCodePede << std::endl;
      // 
      // if we have a result file , rename it. 
      //
      std::ifstream fRes("./millepede.res");
      if (fRes.good()) { 
        std::string cmdMv1("mv ./millepede.res millepede_");
	cmdMv1 +=  fTokenCSV + std::string(".res");
	std::system(cmdMv1.c_str());
      }
      //
      // Create a new geometry file...
      //
    }  

  }

  //......................................................................
  void emph::SingleTrackAlignmentV2::setTrackParams(bool debugNow) {
  
     fTrackX0 = DBL_MAX; fTrackY0 = DBL_MAX;
     fTrackSlxUpstr = DBL_MIN; fTrackSlxDwnstr =  DBL_MIN; fTrackSly = DBL_MIN; 
     // x0, y0 at station 0 
     
     auto emgeo = geo->GeoRef(); // We use the reference map, in this case.. 
     rb::LineSegment aLs;
     double zStX0, zStY0, zStX4, zStY4; 
     for (int iPl0 = 0; iPl0 != 2; iPl0++) {  
       auto itM = fClustMap.find(100*iPl0); auto itCl = itM->second; 
     // this method is private and is called after selection.. So, this pointer should be O.K. 
       if (itCl == fClustH->cend()) {
         std::cerr << " SingleTrackAlignmentV2::setTrackParams wrong logic, no good data at Station 0 .. Fatal, quit here and now " << std::endl;
	 exit(2);
       }  
       rb::SSDCluster clustXY0 = (*itCl); // unneccessary deep copy.. 
       
       dgm->MapRef()->SSDClusterToLineSegment(clustXY0, aLs);
       auto X0 = aLs.X0(); auto X1 = aLs.X1();
       if (clustXY0.View() == 1) { fTrackX0 = 0.5*(X0.X() + X1.X());  zStX0= X0.Z(); }
       // Assume no tilts!!! 
       else if (clustXY0.View() == 2) { fTrackY0 = 0.5*(X0.Y() + X1.Y()); zStY0= X0.Z(); } 
       else {
         std::cerr << " SingleTrackAlignmentV2::setTrackParams, inconsistent view for cluster(s) in Station 0 " << clustXY0 << std::endl;
         std::cerr << " .... Fatal, quit here and now " << std::endl; exit(2);
       }
       if (debugNow) std::cerr << " SingleTrackAlignmentV2::setTrackParams, view " <<
                                  clustXY0.View() << " x at station 0 = " << fTrackX0 << " y = " << fTrackY0 << std::endl;
     }
     double xSt4 = DBL_MAX; double ySt4 = DBL_MAX; 
     for (int iPl0 = 0; iPl0 != 2; iPl0++) {  
       auto itM = fClustMap.find(4000 + 100*iPl0); auto itCl = itM->second; 
       if (itCl == fClustH->cend()) {
         std::cerr << " SingleTrackAlignmentV2::setTrackParams wrong logic, no good data at Station 4 .. Fatal, quit here and now " << std::endl;
	 exit(2);
       }  
       rb::SSDCluster clustXY4 = (*itCl); // unneccessary deep copy.. Code clarity, I guess... 
       dgm->MapRef()->SSDClusterToLineSegment(clustXY4, aLs);
       auto X0 = aLs.X0(); auto X1 = aLs.X1();
       if (clustXY4.View() == emph::geo::X_VIEW) { xSt4 = 0.5*(X0.X() + X1.X()); zStX4= X0.Z(); }
       else if (clustXY4.View() == emph::geo::Y_VIEW) { ySt4 = 0.5*(X0.Y() + X1.Y()); zStY4= X0.Z(); }
       else {
         std::cerr << " SingleTrackAlignmentV2::setTrackParams, inconsistent view for cluster(s) in Station 4 " << clustXY4 << std::endl;
         std::cerr << " .... Fatal, quit here and now " << std::endl; exit(2);
       }
     }
     fTrackSlxUpstr = (xSt4 - fTrackX0)/(zStX0 - zStX4); 
     fTrackSly =  (ySt4 - fTrackY0)/(zStY0 - zStY4); 
     if (fMagCenter > 0.) {
       fTrackSlxDwnstr = fTrackSlxUpstr + fEffMagnetKick;
     } else {
       fTrackSlxDwnstr = fTrackSlxUpstr;
     } 
     if (debugNow) std::cerr << " SingleTrackAlignmentV2::setTrackParams, X slope, upstream  " 
                             << 1.0e3*fTrackSlxUpstr << " Dwnstr " << 1.0e3*fTrackSlxDwnstr << " Y slope " << fTrackSly << std::endl;
  }

  //......................................................................
  
  void emph::SingleTrackAlignmentV2::FillPulls(bool debugNow)
  {
    auto emgeo = geo->GeoRef();
    ru::RecoUtils rUtil = ru::RecoUtils(fEvtNum);
    std::vector<float> pulls(fClustMap.size(), 9.e9); 
    rb::LineSegment aLs;
    TVector3 trA, trB;
    size_t kSensor = 0; double chiSq = 0.;
    size_t numGlobalParams = (fNStations - 2)*3; 
    // the offset in the transverse plane, and a rotation angle for a given station. 
    std::vector<float> globalParamsDeriv(3, 0.); 
    if (debugNow) std::cerr << " SingleTrackAlignmentV2::FillPulls, starting  with " << fNumMeasure  << " measurements "  << std::endl;
    const float lcd = 1.; // Up to a sign !!!! One measurement per sensor..Ignore the quadratic terms..
//    const float lcd = 2.;
// No effect..      
//    const float lcd = 0.; // No local derivatives..   
    for (auto itM = fClustMap.cbegin(); itM != fClustMap.cend(); itM++) { 
      if (itM->second == fClustH->cend()) { kSensor++; continue; } // No data for this sensor. 
      const size_t iStation = static_cast<size_t>((itM->first)/1000);
      if (iStation == 0) { kSensor++; continue; } // Non-Controversial, fixing the origin of our coordinate system..   
      if (iStation == 4) { kSensor++; continue; } // Controversial, We should do this for one plane only, not the whole station
//      const size_t jPlane = static_cast<size_t> ((itM->first - 1000*iStation )/100);
      if (fSkipUW && 
          ((itM->second->View() == emph::geo::W_VIEW) || (itM->second->View() == emph::geo::U_VIEW))) { kSensor++; continue; }
      // Our coordinate system will no longer be orthogonal, if the X-View, Y-View sensor are not stricly perpendicular to 
      // to each others.  
      // Measurement characteristic. 
      auto itCl = itM->second;
      rb::SSDCluster clust = (*itCl); // Deep copy, for code clarity.. (need to convert the iterator to a C-style pointer, in any case. 
      dgm->MapRef()->SSDClusterToLineSegment(clust, aLs);
      const double zMeas = aLs.X0().Z();
      TVector3 lsC(aLs.X0().X(), aLs.X0().Y(), zMeas);
      TVector3 lsD(aLs.X1().X(), aLs.X1().Y(), zMeas);
      TVector3 dirStrip = lsD - lsC; const double sensorz = 0.5*(lsC[2] + lsD[2]); 
      if (debugNow) {
        std::cerr << " At sensor index " << itM->first << " Station " << iStation << " View " << itM->second->View() << std::endl;
        std::cerr << " Strip line segment, C x " << aLs.X0().X() << " y " <<  aLs.X0().Y() << " Z " << aLs.X0().Z() << std::endl;
        std::cerr << " Strip line segment, D x " << aLs.X1().X() << " y " <<  aLs.X1().Y() << " Z " << aLs.X1().Z() << std::endl;
      }
      // Model data.. 
      double slopeX;
      double xCoordB, yCoordB; 
      double xLocal, yLocal; 
      if (itM->first < 4999) { // Upstream Station are order by Station number, which are order by Z position
        trA = TVector3(fTrackX0, fTrackY0, 0.);
        xCoordB = fTrackX0 + fTrackSlxUpstr*fMagCenter; 
        yCoordB = fTrackY0 + fTrackSly*fMagCenter; 
        trB = TVector3(xCoordB, yCoordB, fMagCenter);
        slopeX = fTrackSlxUpstr; 
        xLocal = fTrackX0 + fTrackSlxUpstr*zMeas; 
        yLocal = fTrackY0 + fTrackSly*zMeas; 
      } else  {
        trA = TVector3(fTrackX0 + fTrackSlxUpstr*fMagCenter, fTrackY0 + fTrackSly*fMagCenter, fMagCenter);  
        xCoordB = fTrackX0 + fTrackSlxUpstr*fMagCenter + fTrackSlxDwnstr*fMagCenter; 
        yCoordB = fTrackY0 + fTrackSly*2.0*fMagCenter; 
        trB = TVector3(xCoordB, yCoordB, 2.0*fMagCenter); 
        slopeX = fTrackSlxDwnstr ;  
        xLocal = fTrackX0 + fTrackSlxUpstr*fMagCenter + fTrackSlxDwnstr*(zMeas-fMagCenter) ; 
        yLocal = fTrackY0 + fTrackSly*zMeas; 
      }
      if (debugNow) std::cerr << " .... Linear model give xLocal " << xLocal << " y " << yLocal << std::endl;
      // the above is approximate, if tilt are large, probably wrong.. 
      double F[3]; double l1[3]; double l2[3]; std::string typeCA("SSD"); 
      rUtil.ClosestApproach(trA, trB, lsC, lsD, F, l1, l2, typeCA.c_str(), debugNow);
      if (debugNow) {
        std::cerr << " After Closest Approach, l1 x " << l1[0] << " y " <<  l1[1] << " Z " << l1[2] << std::endl;
        std::cerr << "      ................   l2 x " << l2[0] << " y " <<  l2[1] << " Z " << l2[2] << std::endl;
      }
      double phim = std::atan2(dirStrip.Y(),dirStrip.X()); 
      if (phim < 0.) phim += 2.0*M_PI; 
      const double dd = -1.0*(l1[0] - l2[0])*std::sin(phim) + (l1[1] - l2[1])*std::cos(phim);
      const double err =  fEffErrAtStation[iStation]; // add here error from multiple scattering in quadrature. 
      if (debugNow) {
	  std::cerr << " for Sensor Index " << itM->first << " phim " << phim << " pull " << dd << " +- " << err << std::endl;
      }
      int iiiSt = static_cast<int>(iStation)*10;
      
      if (itCl->View() == emph::geo::X_VIEW) {
        globalParamsDeriv[0] = 1.0;
	globalParamsDeriv[1] = phim;
	globalParamsDeriv[2] = -yLocal;
      } 
      if (itCl->View() == emph::geo::Y_VIEW) { 
        globalParamsDeriv[1] = 1.0;
	globalParamsDeriv[0] = phim;
	globalParamsDeriv[2] = -xLocal; 
      }
      if (itCl->View() == emph::geo::W_VIEW) { // To be checked...!!!! 
        globalParamsDeriv[0] = fOneOverSqrt2;
	globalParamsDeriv[1] = -fOneOverSqrt2;
	globalParamsDeriv[2] = fOneOverSqrt2 * ( yCoordB - xCoordB); // Not sure...
      } 
       int ltmp[3] = {iiiSt+1, iiiSt+2, iiiSt+3}; // organized by coordinate plane, or View.. 
//       m->mille(1, &lcd, 3, &globalParamsDeriv[0], ltmp, dd,err); // Works fine for X and Y view, phim = 0. 
       m->mille(3, &globalParamsDeriv[0], 3, &globalParamsDeriv[0], ltmp, dd,err); // local and global derivative could be identical..???
       pulls[kSensor] = dd;
       chiSq += (dd*dd)/(err*err);
       if (debugNow) std::cerr << " At Station " << iStation << " View " << itCl->View() << " pull " 
                               << dd << " chiSq Contrib " <<  (dd*dd)/(err*err) << std::endl << std::endl;;
       kSensor++;
     }
     if (fPullDist.is_open()) {
     	fPullDist << " " << fSubrun << " " << fEvtNum << " " <<  fTrackX0 << " " << fTrackY0 
        	<< " " << fTrackSlxUpstr << " " << fTrackSly << " " << chiSq << " " << (fNumMeasure-4);
        for (size_t k=0; k != fClustMap.size(); k++)  fPullDist << " " << pulls[k];
        fPullDist << std::endl;  
     }
    m->end();
  }
  //......................................................................

  void emph::SingleTrackAlignmentV2::ClusterStudy(int stage) 
  {
    if (fEvtNum < 5) {
       std::cerr << " SingleTrackAlignmentV2::clusterStudy, stage " << stage  
                               << " evt " << fEvtNum << " spill " << fSubrun << std::endl;
       std::cerr << " Size of clustH " << fClustH->size() << std::endl;
    }		       
    std::ostringstream headEvt; headEvt << " " << fSubrun << " " << fEvtNum ;
    if (stage == 0) { // inclusve, distribution of clusters.. 
      for (auto itC = fClustH->cbegin(); itC != fClustH->cend(); itC++) {
         fClustAllDist << headEvt.str() << " " << itC->Station() << " " << itC->Plane() 
	              << " " << itC->Sensor() << " " << itC->WgtAvgStrip() << std::endl;
     }
    } else if (stage == 1) { 
      fClustSelDist << headEvt.str();
      for (auto itCM = fClustMap.cbegin(); itCM != fClustMap.cend(); itCM++) {
        if (itCM->second == fClustH->cend()) fClustSelDist << " -9999.";
	else fClustSelDist << " " << itCM->second->WgtAvgStrip();
      }
      fClustSelDist << std::endl; 
    }  
 }
  
 void emph::SingleTrackAlignmentV2::fillClustMap() { // Also number of cluster per sensors
 
     auto emgeo = geo->Geo();
     int nSensors = emgeo->NSSDs();
     if (fClustMap.size() != nSensors) { // once per job... 
       for (int ii=0; ii<(int)fNStations; ii++){
         for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
           for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
	     int index = 1000*ii + 100*jj + 10*kk;
	     fClustMap.insert(std::pair<int, std::vector<rb::SSDCluster>::const_iterator>(index, fClustH->cend()));
	     fClustNum.insert(std::pair<int, int>(index, 0));
	   }
	 }
       }
     }
     // intialize.. 
     for (auto ic = fClustMap.begin(); ic != fClustMap.end(); ic++) ic->second = fClustH->cend();
     for (auto ic = fClustNum.begin(); ic != fClustNum.end(); ic++) ic->second = 0;
     // Now fill 
     // fill.. Only if there is one cluster in the sensor.. 
     fNumMeasure = 0;
     for (auto iTCl= fClustH->cbegin(); iTCl != fClustH->cend(); iTCl++) {
       int index = 1000*iTCl->Station() + 100*iTCl->Plane() + 10*iTCl->Sensor();
       std::map<int, int>::iterator itMapNum = fClustNum.find(index);
       if (itMapNum == fClustNum.end()) continue; // Should not happen, the geometry changed in mid run... 
       fNumMeasure++;
       itMapNum->second++;
     }
     for (auto iTCl= fClustH->cbegin(); iTCl != fClustH->cend(); iTCl++) {
       int index = 1000*iTCl->Station() + 100*iTCl->Plane() + 10*iTCl->Sensor();
       std::map<int, int>::const_iterator itMapNum = fClustNum.find(index);
       if (itMapNum == fClustNum.cend()) continue; // Should not happen, the geometry changed in mid run...
       if (itMapNum->second != 1) continue; // Maximum one cluster per plane.  
       std::map<int, std::vector<rb::SSDCluster>::const_iterator >::iterator itMapPtr= fClustMap.find(index);
       itMapPtr->second = iTCl;
     }
}

  //......................................................................

  void emph::SingleTrackAlignmentV2::analyze(const art::Event& evt)
  {

    auto emgeo = geo->Geo();
    
    fRun = evt.run();
    fSubrun = evt.subRun();
    fEvtNum = evt.id().event();
    bool debugNow = fEvtNum < 5;
    
    // if data fcl
    std::string digitStr = std::to_string(fEvtNum);
    if (debugNow) std::cerr << " SingleTrackAlignmentV2::produce, at spill " << fSubrun << " event " << digitStr << std::endl;
    bool useEvent = false;
    if (fUse123) { 
      if ((digitStr.back() == '1') || (digitStr.back() == '2') || (digitStr.back() == '3')) useEvent = true;
      }
    if (fUse456) {
      if ((digitStr.back() == '4') || (digitStr.back() == '5') || (digitStr.back() == '6')) useEvent = true;
      }
    if (fUse7890) {
      if ((digitStr.back() == '7') || (digitStr.back() == '8') || (digitStr.back() == '9') || (digitStr.back() == '0')) 
        useEvent = true;
    } 
    // we therefor allow to use some 30%, 60% or 100% of the sample, as these flags fUse* flags can be either true or false. 
                                                 
    bool gotHandleClusters = evt.getByLabel(fClusterLabel, fClustH);
    if (!gotHandleClusters) { 
       if (debugNow) std::cerr << " SingleTrackAlignmentV2::analyze, failed to get the art handle for cluster.. " << std::endl;   
       return;
    } 
    if (fClustH->empty()) {
    	if (debugNow) std::cerr <<  " .... !! This event has the d/s clusters  but is empty... " << std::endl;
	return;
    }

    if (fDoSelectionStudies) this->ClusterStudy(0); 

    if (!useEvent) return;
       if (debugNow) std::cerr << " .... Using this event, # " << fEvtNum << std::endl;
       
    this->fillClustMap();
    //
    // Require one and only one hit in Station 0 and 4 
    //
    for (int iSt=0; iSt < 5; iSt += 4) { 
      for (int iPl = 0; iPl !=2; iPl++) {
        int index = 1000*iSt + 100*iPl;
        auto itMCl = fClustMap.find(index); 
        auto itCl = itMCl->second;
        if (itCl == fClustH->cend()) {
          if (debugNow) std::cerr << " .... No SSD cluster in Station " << iSt <<", plane " << iPl << "  skip ... " << std::endl;
          return;
        }
        if (fClustNum.find(index)->second > 1) {
          if (debugNow) std::cerr << " .... Too many hits  " << iSt <<", plane " << iPl << "  skip ... " << std::endl;
          return;
        }
      }
    }
    // 
    if (fNumMeasure < fMinSensorOAOCl) {
        if (debugNow) std::cerr << " .... Not enough sensors with one & only 1 cluster, # measurements  " 
	                        << fNumMeasure << " Minimum is " << fMinSensorOAOCl << " skip ... " << std::endl;
        return;
    } else {
      if (debugNow) std::cerr << " .... We have enough sensors with one & only 1 cluster, nSensorOK  " << fNumMeasure << "  keep going " << std::endl;
    }
    
    fNumUsedEvts++;
     
    if (fDoSelectionStudies) this->ClusterStudy(1); 
    
    this->setTrackParams(debugNow); 

   //  Now compute the pulls, and stash the info in the Millipede file. 
    
    this->FillPulls(debugNow);
    if (debugNow) std::cerr << " .... Done for this event.. nSensorOK  " <<  fNumMeasure << std::endl << std::endl;
    return;
  }

} // end namespace emph
DEFINE_ART_MODULE(emph::SingleTrackAlignmentV2)
