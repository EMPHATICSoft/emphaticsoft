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
#include "TRandom3.h"
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
#include "Alignment/MilleRecord.h"
//#include "millepede_ii/Mille.h"
#include "Alignment/GSLStudies.h"
#include "Alignment/MilleRun.h"
#include "Alignment/MilleResult.h"
#include "Geometry/ModAlign/ModGDML.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  struct resultAlignmentStation { // volatile data, to hold and dump to a CSV file the result of a bunch of simulation. 
    double genStx_, alStvalx_, alSterrx_; // in mm 
    double genSty_, alStvaly_, alSterry_; 
    double genStphi_, alStvalphi_, alSterrphi_; // in mr
  };
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
    const bool fRandomizePulls;  
    const bool fRunPede; // at the end job, we run the Millipede-II algorithm, to align.. 
    const bool fGSLStudies; // at the end job, we run the Millipede-II algorithm, to align.. 
    const bool fFixStationRotations; 
    const int fFixAllStationPosButOne;
    const int fFixAllStationPosRotButOne;
    const int fFixAllFreeOneRotStation;
    const int fModeRunPede; 
    const double fScaleErrorsForPede; 
    const double fXCenterSel, fYCenterSel, fXWidthSel, fYWidthSel; // selection on regions, expect biases.. 
    const double fOneOverSqrt12, fOneOverSqrt2;
    double fNumAnomalous;
    int fNumUsedEvts; // Number of available (one and only one cluster per sensor) 
    int fNumMeasure; // Number of available (one and only one cluster per sensor) 
    const std::string fTokenCSV; // for book-keeping..
    const std::string fFNameMille;
    
    art::Handle< std::vector<rb::SSDCluster> > fClustH;
    std::map<int, std::vector<rb::SSDCluster>::const_iterator > fClustMap;
    std::map<int, int > fClustNum;
    std::vector<double> fEffErrAtStation;    
    //
    // To smooth out the pull distribution, by +- 1/2 a strip.. 
    //    
    TRandom3*   myRandForPulls;

    //
    // Diagnostics, studies.. 
    // 
    std::ofstream fClustAllDist; //  Inclusive, all      
    std::ofstream fClustSelDist; //     
    std::ofstream fPullDist, fPullDist2; //
    std::string fFileNameSumRes;
    std::ofstream fMilleResults; // only one line per run of this module (and associated module, see fcl-job file.      
    std::string fFileNameGSLStuRes;
    std::ofstream fGSLStuRes; // as above only one line per run of this module, final result of linear fits., shifts, slope Orthocoord.       
 
    //Millepede stuff
    //  Mille* m; Obsolete.. 
    
    align::MilleRecords fDataMillePulls; 
    
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
    fRandomizePulls(pset.get<bool> ("RandomizePulls")),
    fXCenterSel (pset.get< double >("XCenterSel")),
    fYCenterSel (pset.get< double >("YCenterSel")),
    fXWidthSel (pset.get< double >("XWidthSel")),
    fYWidthSel (pset.get< double >("YWidthSel")),
    fOneOverSqrt12(1.0/std::sqrt(12.)), fOneOverSqrt2(1.0/std::sqrt(2.)),
    fNumAnomalous(0), fNumUsedEvts(0), fNumMeasure(0),
    fRunPede   (pset.get< bool > ("RunPede")),
    fGSLStudies   (pset.get< bool > ("GSLStudies")),
    fFixStationRotations (pset.get<bool> ("FixStationRotations")), 
    fFixAllFreeOneRotStation (pset.get<int> ("FixAllFreeOneRotStation")), 
    fModeRunPede (pset.get<int>("ModeRunPede")),
    fScaleErrorsForPede (pset.get<double>("ScaleErrorsForPede")),
    fFixAllStationPosButOne (pset.get<int> ("FixAllStationPosButOne")), 
    fFixAllStationPosRotButOne (pset.get<int> ("FixAllStationPosRotButOne")), 
    fTokenCSV (pset.get<std::string> ("TokenCSV")),
    fFNameMille(std::string("m0043_") + fTokenCSV + std::string(".bin"))
    {
    
      if (fRandomizePulls) myRandForPulls = new TRandom3(123456); 
        // fixed seed, even on the grid.. Used only to certify the use of Millipede, for geantinos and/or muons.. 
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
    if (fSkipUW && (!geo->IsMovedByStations())) {
      std::cerr << " SingleTrackAlignmentV2::beginRun, inconsistent option, geometry is such that W planes are rotated.. Fatal " << std::endl; 
      exit(2);
    }
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
    std::ostringstream runStrStr; runStrStr << run.run();
    std::string fOpt123("");
    if (fUse123) fOpt123 += std::string("_Opt123");  
    if (fUse456) fOpt123 += std::string("_Opt456");  
    if (fUse7890) fOpt123 += std::string("_Opt7890");
    if (fUse123 &&  fUse456 && fUse7890) fOpt123 = std::string("_AllEvts");
    if (fDoSelectionStudies && (!fClustAllDist.is_open())) {
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
      fClustSelDist << std::endl; fPullDist << " lastRRVal";
      fPullDist << std::endl; 
      //
//      std::string fileNamePulls2("./SingleTrackAlignment_Pulls2_"); 
//      fileNamePulls2 += runStrStr.str() + fOpt123 + std::string("_") + fTokenCSV + std::string("_v1.txt");
//      fPullDist2.open(fileNamePulls2);
//      fPullDist2 << " spill evt kSensor phim phimT xl yl dd dddx dddy dddphi " << std::endl;
    }

//
// Summary files
//
     if (fRunPede) { 
       fFileNameSumRes = std::string("./SingleTrackAlignment_MilleRes_");
       fFileNameSumRes += runStrStr.str() + fOpt123 + std::string("_") + fTokenCSV + std::string("_v1.txt");
       std::ifstream fInTmp(fFileNameSumRes.c_str());
       if (!fInTmp.good()) { // assume it does not exist.. 
	  fMilleResults.open(fFileNameSumRes.c_str());
	  fMilleResults << " nRec nRej chiSq ";
	  for (short kSt=1; kSt != 8; kSt++) {
	    if (kSt == 4) continue;
	    fMilleResults << " genSt" << kSt << "X alSt" << kSt << "valX alSt" << kSt << "errX";
	    fMilleResults << " genSt" << kSt << "Y alSt" << kSt << "valY alSt" << kSt << "errY";
	    fMilleResults << " genSt" << kSt << "Phi alSt" << kSt << "valPhi alSt" << kSt << "errPhi";
	  }
	  fMilleResults << std::endl;
	  fMilleResults.close();
       } else { fInTmp.close(); }  
     }
     if (fGSLStudies) { 
       fFileNameGSLStuRes = std::string("./SingleTrackAlignment_GSLStuPhi_");
       fFileNameGSLStuRes += runStrStr.str() + fOpt123 + std::string("_") + fTokenCSV + std::string("_v1.txt");
       std::ifstream fInTmp(fFileNameGSLStuRes.c_str());
       if (!fInTmp.good()) { // assume it does not exist.. 
	  fGSLStuRes.open(fFileNameGSLStuRes.c_str());
	  for (short iSt = 1; iSt != 8; iSt++) {
	    if (iSt == 4) continue;
	    fGSLStuRes << " StGen" << iSt;
	    if (iSt < 5) { 
	      fGSLStuRes << " XShift"  << iSt << " XShift" << iSt << "err";
	      fGSLStuRes << " XSlope"  << iSt << " XSLope" << iSt << "err";
	      fGSLStuRes << " YShift"  << iSt << " YShift" << iSt << "err";
	      fGSLStuRes << " YSlope"  << iSt << " YSLope" << iSt << "err";
	    } else { 
	      for (short kS = 0; kS !=2; kS++) { 
	        fGSLStuRes << " XShift"  << iSt << "_" << kS << " XShift" << iSt << "_" << kS  << "err";
	        fGSLStuRes << " XSlope"  << iSt << "_" << kS << " XSLope" << iSt << "_" << kS  << "err";
	        fGSLStuRes << " YShift"  << iSt << "_" << kS << " YShift" << iSt << "_" << kS  << "err";
	        fGSLStuRes << " YSlope"  << iSt << "_" << kS << " YSLope" << iSt << "_" << kS  << "err";
	      }
	  }
	}
	fGSLStuRes << std::endl;
	fGSLStuRes.close();
	std::cerr << " ... Check file name fFileNameGSLStuRes " << fFileNameGSLStuRes <<std::endl;
       } else { fInTmp.close(); }  
     }

  }

  //......................................................................
   
  void emph::SingleTrackAlignmentV2::beginJob()
  {
    std::cerr<<"Starting SingleTrackAlignment.... "<<std::endl;
    // We will do this later.. Such that we can fix the shift better.. 
//    m = new Mille(fFNameMille.c_str(),true,true);
    
  }
 
  //......................................................................
  
  void emph::SingleTrackAlignmentV2::endJob()
  {
//    delete m;

    std::cout<<"SingleTrackAlignment: Number of events used = "<< fNumUsedEvts << std::endl;
    if (fDoSelectionStudies) {
      fClustAllDist.close(); fClustSelDist.close(); fPullDist.close(); 
      // .close() ; //     
    }
    if (fRunPede) { 
      align::MilleRun mRun(geo->IsMovedByStations(), fModeRunPede, fTokenCSV);
      mRun.SetScaleErrors(fScaleErrorsForPede); 
      mRun.doIt(fDataMillePulls, fFixStationRotations,  fFixAllStationPosButOne, fFixAllStationPosRotButOne, 
               fFixAllFreeOneRotStation, fModeRunPede );  // ModeRunPede 0 means no calculation and shifting of the mean pull
	       // 1, we compute the mean puls, and correct the data for it.. 
     // 
     // analysis, compare with 
     //
     std::cerr << " Number of Millepede result tables is " << mRun.numResults() << std::endl;
     // 
     // Valid only if by Station.. 
     //
     
     
     if (mRun.numResults() == 0) {
       fMilleResults.open(fFileNameSumRes.c_str(), std::ios_base::app); 
       fMilleResults << " -1 0 9.0e9 ";  
       for (short kSt=1; kSt != 8; kSt++) {
	    if (kSt == 4) continue;
	    for (short ii=0; ii !=9; ii++) fMilleResults << " 9.e9";
       }
       fMilleResults << std::endl;
       fMilleResults.close(); 
       return ;
     }// Take always the last ... if move/rotate one station at time
     for (auto it =  mRun.CBeginFinalResultSt(); it !=  mRun.CEndFinalResultSt(); it++) {
       if (it->station_ != (short) fFixAllStationPosRotButOne) continue;
       if (it->glVar_ == std::string("DeltaX")) { 
         std::cerr << " for station " <<  fFixAllStationPosRotButOne << " DeltaX = " << it->val_ << " +- " << it->err_ << std::endl;
	 const double delta = it->val_ -  geo->MovedStationByX();
	 const double nSigma = delta/it->err_; 
	 std::cerr << " ...So, aligment delta, X direction " << delta << " +- " << it->err_ << " [mm] or " << nSigma << " sigma.. " << std::endl;
       }
       if (it->glVar_ == std::string("DeltaY")) { 
         std::cerr << " for station " <<  fFixAllStationPosRotButOne << " DeltaY = " << it->val_ << " +- " << it->err_ << std::endl;
	 const double delta = it->val_ -  geo->MovedStationByY();
	 const double nSigma = delta/it->err_; 
	 std::cerr << " ...So, aligment delta, Y direction " << delta << " +- " << it->err_ << " [mm] or " << nSigma << " sigma " << std::endl;
       }
       if (it->glVar_ == std::string("DeltaPhi")) { 
         std::cerr << " for station " <<  fFixAllStationPosRotButOne << " DeltaPhi = " << 1.0e3*it->val_ 
	           << " [mr] +- " << 1.0e3*it->err_ << std::endl;
	 const double delta = it->val_ -  (M_PI * geo->RotatedStationBydPhi())/180.; // in radian
	 const double nSigma = delta/it->err_; 
	 std::cerr << " ...So, aligment delta, Phi direction " << 1.0e3*delta 
	           << " +- " << 1.0e3*it->err_ << " [mr] or " << nSigma << " sigma " << std::endl;
       }
       
     } 
     // again, for the summary result file.. 
     std::cerr << " .... Opening file " << fFileNameSumRes << " appending.. " << std::endl;
     std::cerr << " .... Check fFixAllStationPosRotButOne " << fFixAllStationPosRotButOne << std::endl;
     fMilleResults.open(fFileNameSumRes.c_str(), std::ios_base::app); 
     fMilleResults << " " << mRun.NumRecords() << " " << mRun.NumRejected() << " " << mRun.Chi2Norm();  
     double gen, val, err; 
     std::cerr << " ....  Number of fitted parameters " << mRun.numResults() << std::endl;
     std::cerr << " check the geo pointer, by fetching the GDML file name " << geo->Geo()->GDMLFile() << std::endl;
     for (auto it =  mRun.CBeginFinalResultSt(); it !=  mRun.CEndFinalResultSt(); it++) {
//        std::cerr << " .... Check again fFixAllStationPosRotButOne " << fFixAllStationPosRotButOne << std::endl;
       if (fFixAllStationPosRotButOne != -1) {
         if (it->station_ != (short) fFixAllStationPosRotButOne) {
           gen = 0.; val = 0.; err = 9.0e9;
	 } else { // we rely on consistent use of fHicl parameters here...(Rotation and move for a given station. 
	   val = it->val_; err= it->err_;
           if (it->glVar_ == std::string("DeltaX")) gen = geo->MovedStationByX(); 
           if (it->glVar_ == std::string("DeltaY")) gen = geo->MovedStationByY();  
           if (it->glVar_ == std::string("DeltaPhi")) gen = geo->RotatedStationBydPhi(); 
	 }
       } else { 
	 val = it->val_; err= it->err_;
//	 std::cerr << " ... Fetching info for stations " << it->station_ << std::endl;
         if (it->glVar_ == std::string("DeltaX")) gen = geo->MovedStationByX(it->station_); 
         if (it->glVar_ == std::string("DeltaY")) gen = geo->MovedStationByY(it->station_);  
         if (it->glVar_ == std::string("DeltaPhi")) gen = geo->RotatedStationBydPhi(it->station_); 
       }
//       std::cerr << " ... ... writing to Mille Res, glVariable name " << it->glVar_ << " value " << val <<  std::endl;
       fMilleResults << " " << gen << " " << val << " " << err;
      }
      fMilleResults << std::endl;
      fMilleResults.close(); 
      
   } // running pede.. 
   std::cerr << " ... emph::SingleTrackAlignmentV2::endJob.. Ready to the simple Linear regression fits. " << std::endl;
   if (fGSLStudies) {
     std::cerr << " ... doing it... on  " << fDataMillePulls.numRecords() << " num Records " << std::endl;
     emph::align::GSLStudies myStu(true, 0, fTokenCSV);
     myStu.SetDebugOn();
//      myStu.StudyByStationXY(fDataMillePulls, 1); // for now, debugging.. 
//      myStu.StudyByStationXY(fDataMillePulls, 5, 1); // for now, debugging..
     auto theModDetGeo = dgm->Map();
     rb::LineSegment aLs; 
     std::cerr << " Opening file " << fFileNameGSLStuRes << std::endl;      
     fGSLStuRes.open(fFileNameGSLStuRes.c_str(), std::ios_base::app);
     if (! fGSLStuRes.good()) {
       std::cerr << " Not a good status ???? quit here!! " << std::endl; exit(2);
     }
     for (short iSt = 1; iSt != 8; iSt++) { 
       if (iSt == 4) continue;
       short nSensor = (iSt < 4) ? 1 : 2;
       for (short kS = 0; kS != nSensor; kS++) { 
         myStu.StudyByStationXY(fDataMillePulls, iSt, kS);
	 for (short jC = 0 ; jC != 2; jC++) {
	   int jPlaneX = (iSt < 4) ? 2 : 0;
	   if (iSt == 1) jPlaneX = 1; 
	   theModDetGeo->StationSensorPlaneToLineSegment((int) iSt, (int) kS, (int) jPlaneX, aLs, 360.);
	   auto X0Ls = aLs.X0(); auto X1Ls = aLs.X1(); auto dXLs = X1Ls - X0Ls;
	   double valGenAbs = std::atan2(dXLs.Y(), dXLs.X()); 
	   double valGen = 0.; 
	   if (std::abs(valGenAbs - M_PI/2.) < 0.1) valGen = valGenAbs - M_PI/2.;
	   if (std::abs(valGenAbs + M_PI/2.) < 0.1) valGen = valGenAbs + M_PI/2.; 
	   if (!myStu.gotResult(iSt, jC, kS)) fGSLStuRes << " " << 1.0e3*valGen << " 9.0e9 9.0e9 9.0e9 9.0e9 ";
	   else { 
	     auto itFinal = myStu.finalResult(iSt, jC, kS);
	     if ((jC == 0) && (kS == 0)) fGSLStuRes << " " << 1.0e3*valGen;
	     fGSLStuRes << " " << itFinal->shift_ << " " << std::sqrt(itFinal->covMat_[0]);
	     fGSLStuRes << " " << 1.0e3*itFinal->slope_ << " " << 1.0e3*std::sqrt(itFinal->covMat_[2]);
	   }
	 }
       }
     }
     fGSLStuRes << std::endl;
     fGSLStuRes .close(); 
  } // GSL Studies. 
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
       if (debugNow) {
            std::cerr << " SingleTrackAlignmentV2::setTrackParams, at station 0, view " <<
                                  clustXY0.View() << " Strip " << clustXY0.WgtAvgStrip() << " x = " << fTrackX0 << " y = " << fTrackY0 << std::endl;
       }
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
       if (clustXY4.View() == emph::geo::X_VIEW) { 
         //
	 // Note the readout for station 5, X view is done in reverse order from station 0 - to 4. 
	 // So, in the Monte-Carlo, at least, due to the digitization, there is shift of one strip.. 
	 // Not sure on what's happening on date..
	 //
         //xSt4 = 0.5*(X0.X() + X1.X()) - 0.060; 
	 xSt4 = 0.5*(X0.X() + X1.X());
	 zStX4= X0.Z(); 
       }
       else if (clustXY4.View() == emph::geo::Y_VIEW) { ySt4 = 0.5*(X0.Y() + X1.Y()); zStY4= X0.Z(); }
       else {
         std::cerr << " .... inconsistent view for cluster(s) in Station 4 " << clustXY4 << std::endl;
         std::cerr << " .... Fatal, quit here and now " << std::endl; exit(2);
       }
       if (debugNow) {
          std::cerr << " .... At Station 4, view " << clustXY4.View() << " Strip " << clustXY4.WgtAvgStrip() 
	            << " xSt4 " << xSt4 << " y " << ySt4 << " at z = " << zStY4 << std::endl;
       }                             
     }
     fTrackSlxUpstr = (xSt4 - fTrackX0)/(zStX0 - zStX4); 
     fTrackSly =  (ySt4 - fTrackY0)/(zStY0 - zStY4); 
     if (fMagCenter > 0.) {
       fTrackSlxDwnstr = fTrackSlxUpstr + fEffMagnetKick;
     } else {
       fTrackSlxDwnstr = fTrackSlxUpstr;
     } 
     if (debugNow) std::cerr << " ... end of SingleTrackAlignmentV2::setTrackParams, X slope, upstream  " 
                             << 1.0e3*fTrackSlxUpstr << " Dwnstr " << 1.0e3*fTrackSlxDwnstr 
			     << " Y slope " << 1.0e3*fTrackSly << " [mr] " <<  std::endl << std::endl;
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
    std::array<float, 3> globalParamsDeriv; 
    if (debugNow) std::cerr << " SingleTrackAlignmentV2::FillPulls, starting  with " << fNumMeasure  << " measurements "  << std::endl;
    double lastRRVal = 0;
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
      if (debugNow) std::cerr << " check Geometry pointer...Refenrece geometry now  " << std::endl;
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
      double dd = -1.0*(l1[0] - l2[0])*std::sin(phim) + (l1[1] - l2[1])*std::cos(phim);
      // correcting for the rotation of 180.0 in the geometry, 
      if (itCl->View() == emph::geo::X_VIEW)  dd *= -1.0;
      // Randomize, such that, for Geantinos, we get a smooth chi-sq. distribution. 
      if (fRandomizePulls) { 
        const double rr = (-0.060) + 0.060*myRandForPulls->Uniform(); // I think it generated a flat distribuiton between 0.  and 1.0 
	lastRRVal = rr;
        dd += rr;
      }
      const double err =  fEffErrAtStation[iStation]; // add here error from multiple scattering in quadrature. 
      if (debugNow) {
	  std::cerr << " for Sensor Index " << itM->first << " phim " << phim << " pull " << dd << " +- " << err << std::endl;
      }
      int iiiSt = static_cast<int>(iStation)*10;
      if (itCl->View() == emph::geo::W_VIEW) { 
        // flip the sign of sin(phi) ? 
//        globalParamsDeriv[0] = static_cast<float>( std::sin(phim)); Nop, 
// Original , as should be.. 
        globalParamsDeriv[0] = static_cast<float>( -1.0*std::sin(phim));
        globalParamsDeriv[1] = static_cast<float>(1.0*std::cos(phim));
        globalParamsDeriv[2] = static_cast<float>(-1.*std::cos(phim) * (xLocal) - 1.*std::sin(phim) * yLocal); // as in the original.. 
      
      } else { 
//        globalParamsDeriv[0] = static_cast<float>( -1.0*std::sin(phim)); 
        globalParamsDeriv[0] = static_cast<float>( std::sin(phim)); // flip the sifn of x ? 
        globalParamsDeriv[1] = static_cast<float>(std::cos(phim));
//      globalParamsDeriv[2] = static_cast<float>(-1.*std::cos(phim) * (-1.0*xLocal) - 1.*std::sin(phim) * yLocal); // as in the original..
      // but phi is incorrect..  X -> -x but, abs value correct.. 
        globalParamsDeriv[2] = static_cast<float>(-1.*std::cos(phim) * (xLocal) - 1.*std::sin(phim) * yLocal); // as in the original.. 
      }
      if (fPullDist2.is_open()) {
      // 
      // We debug the W contribution to the chi-square.. Something is wrong 
      // the calculation of the derivatives.. 
      // Compute the true phim, run the code with tiny rotation, than with sizable rotation.. 
      //  Compute the derivative numerically, based on these two runs.. 
      //     
        rb::LineSegment aLsTrue;
	if (debugNow) std::cerr << " check Geometry pointer...True (e.g. modified by this module)  geometry now  " << std::endl;
        dgm->Map()->SSDClusterToLineSegment(clust, aLsTrue);
        TVector3 lsCTrue(aLsTrue.X0().X(), aLsTrue.X0().Y(), aLsTrue.X0().Z());
        TVector3 lsDTrue(aLsTrue.X1().X(), aLsTrue.X1().Y(), aLsTrue.X1().Z());
        TVector3 dirStripTrue = lsDTrue - lsCTrue; 
	const double phimTrue= std::atan2(dirStripTrue.Y(), dirStripTrue.X());  
     	fPullDist2 << " " << fSubrun << " " << fEvtNum << " " <<  kSensor << " " << phim << " " << phimTrue
	           << " " << xLocal << " " << yLocal << " " << dd;
	for (size_t k=0; k !=3; k++) fPullDist2 << " " << globalParamsDeriv[k];
	fPullDist2  << std::endl;
      } 
      if (debugNow) {
	  std::cerr << " labels  " << itM->first << " phim " << phim << " pull " << dd << " +- " << err << std::endl;
       }
      if (dd > 8.99e9) {
         std::cerr << "  Logic error in FillPulls.. dd too big  " << dd << " fatal " << std::endl; exit(2);
      }
      const float ddFloat = static_cast<float>(dd);
      const float errFloat = static_cast<float>(err);
      float orthoCoord = 0;
      if (clust.View() == emph::geo::X_VIEW) orthoCoord = yLocal;
      if (clust.View() == emph::geo::Y_VIEW) orthoCoord = xLocal;
      const short aSpill = (short) fSubrun;
      align::MilleRecord mr(fEvtNum, aSpill, 
                          static_cast<short>(iStation), static_cast<short>(clust.Plane()), static_cast<short>(clust.Sensor()), 
                          globalParamsDeriv, orthoCoord, ddFloat, errFloat);
      fDataMillePulls.add(mr);
//      m->mille(1, &lcd, 3, &globalParamsDeriv[0], ltmp, ddFloat, errFloat); // Works fine for X and Y view, phim = 0. 
//       m->mille(3, &globalParamsDeriv[0], 3, &globalParamsDeriv[0], &ltmp[0], ddFloat, errFloat); // local and global derivative could be identical..???
      pulls[kSensor] = dd;
      chiSq += (dd*dd)/(err*err);
      if (debugNow) std::cerr << " At Station " << iStation << " View " << itCl->View() << " pull " 
                               << dd << " chiSq Contrib " <<  (dd*dd)/(err*err) << std::endl << std::endl;
// 
// Debugging the anomalous chiSq contrib. To be commented out when fix.. 
//
      if ((dd*dd)/(err*err) > 0.1*DBL_MAX) { // turn this off.. 
         fNumAnomalous++; 
         std::cerr << " Anomalous chiSq contrib, assuming perfect alignment and Geantino " << std::endl;
	 std::cerr << " At Station " << iStation << " View " << itCl->View() << " sensor " << clust.Sensor() 
	           << " strip " << itCl->WgtAvgStrip() << " xLocal " << xLocal << " y " << yLocal << " pull " 
                               << dd << " err " << err << " chiSq Contrib " <<  (dd*dd)/(err*err) << std::endl;
         std::cerr << " Strip line segment, C x " << aLs.X0().X() << " y " <<  aLs.X0().Y() << " Z " << aLs.X0().Z() << std::endl;
         std::cerr << " Strip line segment, D x " << aLs.X1().X() << " y " <<  aLs.X1().Y() << " Z " << aLs.X1().Z() << std::endl;
	 this->setTrackParams(true);  
            if (fNumAnomalous > 40) {  std::cerr << " quit after 40 anomalous pulls " << std::endl; exit(2); } 
       }			       
       kSensor++;
     }
     if (fPullDist.is_open()) {
     	fPullDist << " " << fSubrun << " " << fEvtNum << " " <<  fTrackX0 << " " << fTrackY0 
        	<< " " << fTrackSlxUpstr << " " << fTrackSly << " " << chiSq << " " << (fNumMeasure-4);
        for (size_t k=0; k != fClustMap.size(); k++)  fPullDist << " " << pulls[k];
	fPullDist << " " << lastRRVal; 
        fPullDist << std::endl;  
     }
//    m->end();
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
