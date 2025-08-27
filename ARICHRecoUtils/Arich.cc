////////////////////////////////////////////////////////////////////////
///// \brief    ARICH class
///// \author  mdallolio
///// \date
//////////////////////////////////////////////////////////////////////////
#include "ARICHRecoUtils/Arich.h"
#include <memory>

namespace arichreco {

  // ARICH constructor
  Arich::Arich(arichreco::Detector* detector, double refr1, double refr2, double aeroP1, double aeroP2, double thick1, double thick2)
    : thickness1(thick1), thickness2(thick2), aeroPos1(aeroP1), aeroPos2(aeroP2),
      aerogel1(std::make_unique<arichreco::Aerogel>(refr1, thick1, aeroP1)),
      aerogel2(std::make_unique<arichreco::Aerogel>(refr2, thick2, aeroP2)),
      detector(detector)
  {
    detectorDist = detector->getDist();
    aerogel1->setDownIndex(refr2);
    aerogel2->setUpIndex(refr1);
  }

  // destructor: only deletes detector (aerogels are managed by unique_ptr)
  Arich::~Arich() {
    delete detector;
  }

  // Calculate mean distribution of photons over detector for given beta
  TH2D Arich::calculatePdf(arichreco::particleInfoStruct params, char* histName) {
    constexpr bool SCAT = true; // Set to true to apply scattering (currently enabled, could configure)
    constexpr int nEvents = 1000; // Number of events to simulate; should make this configurable
    std::unique_ptr<arichreco::Beam> beam = std::make_unique<arichreco::Beam>(params.pos, params.dir, params.beta);
    std::unique_ptr<TH2D> tempphotonHist(detector->makeDetectorHist(histName, histName));
    TH2D photonHist;
    for (int i = 0; i < nEvents; ++i) {
      std::unique_ptr<arichreco::Particle> pa(beam->generateParticle());
      std::vector<arichreco::Photon*> photons = aerogel1->generatePhotons(pa.get(), detector);
      if (SCAT) aerogel1->applyPhotonScatters(photons);
      aerogel1->exitAerogel(photons, true, aerogel2->getRefractiveIndex());
      pa->travelZDist(aeroPos2 - aeroPos1);
      std::vector<arichreco::Photon*> photons2 = aerogel2->generatePhotons(pa.get(), detector);
      photons.insert(photons.end(), photons2.begin(), photons2.end());
      if (SCAT) aerogel2->applyPhotonScatters(photons);
      aerogel2->exitAerogel(photons, true, 1.0);
      detector->projectPhotons(tempphotonHist.get(), photons);
      for (auto* ph : photons) {
        delete ph;
      }
    }
    tempphotonHist->Scale(detector->getFillFactor() / nEvents);
    photonHist = *tempphotonHist;
    return photonHist;
  }

} 
