////////////////////////////////////////////////////////////////////////
/////// \brief   Definition of AEROGEL class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#ifndef AEROGEL_INCLUDE
#define AEROGEL_INCLUDE

#include <fstream>

//Root
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"
#include "ARICHRecoUtils/Beam.h"
#include "ARICHRecoUtils/Photon.h"
#include "ARICHRecoUtils/Particle.h"
#include "ARICHRecoUtils/Detector.h"


namespace arichreco{

        class Aerogel {
        private:
          double refracIndex=1.0;
          double thickness=0.0;
          double height=30.0;
          double width=30.0;
          double zPos=0.0;
          double chAngle = 0.0;
          double upIndex = 1.0; //Upstream refractive index
          double downIndex = 1.0; //Downstream refractive
          TF1 *wavPdf;
          TF1 *scatAngleFunc;
          std::shared_ptr<TRandom3> randomGenerate;
          std::vector<double> interactionLengths;

          static std::vector<double> readInteractionLength(double);
          std::vector<double> getRandomTheta(int);
          double calcdNdX(double beta);
          double getRandomWav();
          double getRandomScatAngle();
          void refractPhoton(arichreco::Photon*);
          void applyPhotonScatter(arichreco::Photon*);
          double getIntLengthForWav(double wav);
          double getRandomIntDistance(double wav);

        public:
          Aerogel(double refractiveIndex, double thickness, double zPos);
          double calcChAngle(double beta);
          double getRefractiveIndex();
          double getThickness();
          double getZPos();
          void setUpIndex(double n);
          void setDownIndex(double n);

          int calcNumPhotons(double particleDist, double beta);
          void exitAerogel(Photon* ph,  bool refract = true, double n2 = 1.0);
          void exitAerogel(std::vector<arichreco::Photon*> ph, bool refract = true, double n2 = 1.0);
          bool isInAerogel(TVector3);
          void applyPhotonScatters(std::vector<arichreco::Photon*>);
          std::vector<Photon*> generatePhotons(arichreco::Particle*, arichreco::Detector*);
          double getDistInGel(arichreco::Particle*);

        };
    }
#endif
