////////////////////////////////////////////////////////////////////////
///// \brief   Definition of ARICH
///// \author  mdallolio
///// \date
//////////////////////////////////////////////////////////////////////////
#ifndef ARICH_INCLUDE
#define ARICH_INCLUDE

#include "stdlib.h"
#include <iostream>
#include <chrono>
#include <memory>

#include <TROOT.h>
#include <TStyle.h>
#include "TMath.h"
#include "TH2D.h"
#include "TFile.h"
#include "TRandom3.h"
#include "TVector3.h"

#include "ARICHRecoUtils/Beam.h"
#include "ARICHRecoUtils/Aerogel.h"
#include "ARICHRecoUtils/Particle.h"
#include "ARICHRecoUtils/Photon.h"
#include "ARICHRecoUtils/Detector.h"
#include "ARICHRecoUtils/Utility.h"

namespace arichreco{
    
  struct particleInfoStruct {
    // Input parameters to a simple fast simulation
    TVector3 pos;
    TVector3 dir;
    double beta;
    std::string name;

    // Default constructor: initializes everything to safe defaults
    particleInfoStruct()
      : pos(0,0,0), dir(0,0,1), beta(0), name("None") {}

    // Parameterized constructor for convenience
    particleInfoStruct(const TVector3& p, const TVector3& d, double b, const std::string& n)
      : pos(p), dir(d), beta(b), name(n) {}

    void reset() {
      pos.SetXYZ(0,0,0);
      dir.SetXYZ(0,0,1);
      beta = 0;
      name = "None";
    }
  };

  struct photonStruct {
    // Initial direction and position
    double dirxi;
    double diryi;
    double dirzi;
    double posxi;
    double posyi;
    double poszi;
    // Direction and position upon exit of aerogel
    double dirxe;
    double dirye;
    double dirze;
    double posxe;
    double posye;
    double posze;
    // Wavelength
    double wav;
    // Parent particle
    int paid;
    // Number of scatters
    int numscat;
  };

  struct particleStruct {
    double dirx;
    double diry;
    double dirz;
    double posx;
    double posy;
    double posz;
    double beta;
    int id;
  };
  
  class Arich {

    private:
      // static constexpr double aeroPos1 = 0.; // positions of upstream edges of aerogel layers
      // static constexpr double aeroPos2 = 2.;
      // static constexpr double thickness1 = 2.0; // thickness of aerogel layers
      // static constexpr double thickness2 = 2.0;
      // static constexpr double n1 = 1.035; //1.0352; // upstream aerogel index of refraction
      // static constexpr double n2 = 1.045; //1.0452; // downstream aerogel index of refraction
      //static constexpr double detectorDist = 21.0; // dist to detector plane
      double detectorDist;
      double thickness1;
      double thickness2;
      double aeroPos1;
      double aeroPos2;

      std::unique_ptr<arichreco::Aerogel> aerogel1;
      std::unique_ptr<arichreco::Aerogel> aerogel2;
      arichreco::Detector* detector;

      // double integrateAndDrawEllipse(arichreco::particleInfoStruct params, TH2Poly* photonHist, TPad* pad);

    public:
      Arich(arichreco::Detector* detector, double refr1, double refr2, double aeroP1, double aeroP2, double thick1, double thick2);
      virtual ~Arich();
      TH2D calculatePdf(arichreco::particleInfoStruct params, char* histName= (char*) "photonHist");
      //TH2Poly* generateEvent(arichreco::particleInfoStruct params, bool save=true, std::string histName="generatedEvent", std::string outputDir="./output");
      //TH2Poly* simulateBeam(arichreco::particleInfoStruct params, std::string outputDir="simulatedBeam");
  };
} // namespace arichreco
#endif
